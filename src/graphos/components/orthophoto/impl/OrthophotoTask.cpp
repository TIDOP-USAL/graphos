/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "OrthophotoTask.h"

/* GRAPHOS */
#include "graphos/components/orthophoto/impl/Orthoimage.h"
#include "graphos/core/image.h"
#include "graphos/components/orthophoto/impl/OrthoimageTask.h"

/* TidopLib */
#include <tidop/core/messages.h>
#include <tidop/core/exception.h>
#include <tidop/core/progress.h>
#include <tidop/core/chrono.h>
#include <tidop/geospatial/crs.h>
#include <tidop/vect/vectreader.h>
#include <tidop/graphic/layer.h>
#include <tidop/graphic/datamodel.h>
#include <tidop/graphic/entities/polygon.h>
#include <tidop/img/formats.h>
#include <tidop/img/metadata.h>
#include <tidop/geospatial/crstransf.h>

/* OpenCV */
#include <opencv2/stitching.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>

#include <QDateTime>

//#define FAST_ORTHO 1

namespace graphos
{

constexpr double exposure_compensator_factor = 0.1;
constexpr int ortho_tile_w = 600;
constexpr int ortho_tile_h = 600;
constexpr int ortho_overlap = 50;

static std::shared_ptr<tl::GPolygon> bestImage(const tl::Point<double> &pt, std::shared_ptr<tl::GLayer> layer)
{
    std::shared_ptr<tl::GPolygon> footprint_image;

    std::map<double, std::shared_ptr<tl::GPolygon>> entities;

    for (const auto &entity : *layer) {
        tl::GraphicEntity::Type type = entity->type();
        if (type == tl::GraphicEntity::Type::polygon_2d) {

            std::shared_ptr<tl::GPolygon> polygon = std::dynamic_pointer_cast<tl::GPolygon>(entity);
            if (polygon->isInner(pt)) {
                tl::Point<double> center = polygon->window().center();
                double distance = tl::distance(center, pt);
                entities[distance] = polygon;
            }

        } else {
            tl::Message::error("It is not a footprint file");
            break;
        }

    }

    if (!entities.empty()) {
        footprint_image = entities.begin()->second;
    }

    return footprint_image;
}

static void findOptimalFootprint(const tl::Path &footprint_file,
                          std::vector<tl::WindowD> &grid,
                          const tl::Path &optimal_footprint_path,
                          const tl::Crs &crs)
{
    std::map<std::string, std::shared_ptr<tl::GPolygon>> clean_footprint;
    ///....
    std::vector<std::pair<std::string, std::shared_ptr<tl::GPolygon>>> grid_images;
    ///....

    std::unique_ptr<tl::VectorReader> vectorReader = tl::VectorReaderFactory::create(footprint_file);
    vectorReader->open();
    if (vectorReader->isOpen()) {

        if (vectorReader->layersCount() >= 1) {

            std::map<double, std::shared_ptr<tl::GPolygon>> entities;
            std::shared_ptr<tl::GLayer> layer = vectorReader->read(0);

            for (size_t i = 0; i < grid.size(); i++) {

                /// Busqueda de imagen mas centrada
                std::shared_ptr<tl::GPolygon> polygon = bestImage(grid[i].center(), layer);
                if (polygon) {
                    std::shared_ptr<tl::TableRegister> data = polygon->data();
                    std::string ortho_to_compensate = data->value(0);
                    clean_footprint[ortho_to_compensate] = polygon;

                    ///....
                    std::shared_ptr<tl::GPolygon> polygon = std::make_shared<tl::GPolygon>();
                    auto x_min = grid[i].pt1.x;
                    auto y_min = grid[i].pt1.y;
                    auto x_max = grid[i].pt2.x;
                    auto y_max = grid[i].pt2.y;

                    polygon->push_back(tl::Point<double>(x_min, y_min));
                    polygon->push_back(tl::Point<double>(x_max, y_min));
                    polygon->push_back(tl::Point<double>(x_max, y_max));
                    polygon->push_back(tl::Point<double>(x_min, y_max));

                    grid_images.push_back({ortho_to_compensate, polygon});
                    ///....
                }

            }

            vectorReader->close();

        }

        tl::Message::info("Optimal footprint. {} retained images", clean_footprint.size());
        {
            std::unique_ptr<tl::VectorWriter> vector_writer = tl::VectorWriterFactory::create(optimal_footprint_path);
            vector_writer->open();
            if (!vector_writer->isOpen()) throw std::runtime_error("Vector open error");
            vector_writer->create();
            vector_writer->setCRS(crs.toWktFormat());

            std::shared_ptr<tl::TableField> field(new tl::TableField("image",
                                                  tl::TableField::Type::STRING,
                                                  254));
            std::vector<std::shared_ptr<tl::TableField>> fields;
            fields.push_back(field);

            tl::GLayer layer;
            layer.setName("footprint");
            layer.addDataField(field);

            for (const auto &footprint : clean_footprint) {
                std::shared_ptr<tl::TableRegister> data(new tl::TableRegister(fields));
                data->setValue(0, footprint.first);
                layer.push_back(footprint.second);
            }

            vector_writer->write(layer);

            vector_writer->close();
        }

        /// Write grid
        {

            auto grid_path = optimal_footprint_path;
            grid_path.replaceBaseName("grid");
            std::unique_ptr<tl::VectorWriter> grid_writer = tl::VectorWriterFactory::create(grid_path);
            grid_writer->open();
            if (!grid_writer->isOpen()) throw std::runtime_error("Vector open error");
            grid_writer->create();
            grid_writer->setCRS(crs.toWktFormat());

            std::shared_ptr<tl::TableField> field_image(new tl::TableField("image",
                                                        tl::TableField::Type::STRING,
                                                        254));
            std::vector<std::shared_ptr<tl::TableField>> fields;
            fields.push_back(field_image);

            tl::GLayer layer;
            layer.setName("grid");
            layer.addDataField(field_image);
            int i = 0;
            for (const auto &grid_image : grid_images) {
                std::string image_name = grid_image.first;
                auto polygon = grid_image.second;
                std::shared_ptr<tl::TableRegister> data(new tl::TableRegister(fields));
                data->setValue(0, image_name);
                polygon->setData(data);
                layer.push_back(polygon);
            }

            grid_writer->write(layer);

            grid_writer->close();
        }
        ////
    }
}

static cv::Mat combineImages(const std::vector<cv::Mat> &images)
{
    cv::Mat result = images[0].clone(); // Clona la imagen base como resultado

    for (size_t i = 1; i < images.size(); ++i) {
        cv::Mat maskResult;
        // Crear máscara para los píxeles negros en la imagen resultante
        cv::inRange(result, cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), maskResult);
        if (cv::countNonZero(maskResult) == 0)
            break;

        cv::Mat maskImage;
        cv::inRange(images[i], cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), maskImage);
        cv::bitwise_not(maskImage, maskImage);

        cv::Mat mask;
        cv::bitwise_and(maskResult, maskImage, mask);

        // Dilatación de la máscara para cubrir bordes y suavizar uniones
        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
        cv::dilate(mask, mask, element);


        // Solo copiar los píxeles de la imagen actual donde result es negro
        images[i].copyTo(result, mask);

        // Relleno de pixeles negros
        cv::Mat blackPixelMask = createBlackPixelMask(result, 1024);
        //cv::Mat black_pixel_mask = createBlackPixelMask(result, 512);
        cv::inpaint(result, blackPixelMask, result, 3, cv::INPAINT_TELEA);
        //cv::inpaint(result, black_pixel_mask, result, 5, cv::INPAINT_NS);
    }

    return result;
}

//static cv::Mat combineImagesSmart(const std::vector<cv::Mat> &images,
//                           const std::vector<double> &distances,
//                           float weight_color = 0.7f,
//                           float weight_distance = 0.3f)
//{
//    if (images.empty()) return cv::Mat();
//
//    size_t images_size = images.size();
//
//    TL_ASSERT(images_size == distances.size(), "Number of images and distances must match");
//
//    int rows = images[0].rows;
//    int cols = images[0].cols;
//
//    // Precompute valid masks (non-black)
//    std::vector<cv::Mat> valid_masks(images_size);
//    for (int i = 0; i < images_size; ++i) {
//        cv::inRange(images[i], cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), valid_masks[i]);
//        cv::bitwise_not(valid_masks[i], valid_masks[i]); // ahora 255 = válido
//    }
//
//    // Convert images to Lab float32 for perceptual distance
//    std::vector<cv::Mat> lab_images(images_size);
//    for (size_t i = 0; i < images_size; ++i) {
//        images[i].convertTo(lab_images[i], CV_32F, 1.0 / 255.0); // [0,1] float
//        cv::cvtColor(lab_images[i], lab_images[i], cv::COLOR_BGR2Lab); // Lab float (L in 0-100 scaled ~0-100 but here 0-1 scaled)
//    }
//
//    // Normalize distances to [0,1] (smaller = better)
//    double min_distance = std::numeric_limits<double>::max();
//    double max_distance = std::numeric_limits<double>::lowest();
//
//    for (const auto &d : distances) {
//        if (d < min_distance) min_distance = d;
//        if (d > max_distance) max_distance = d;
//    }
//
//    double distance_range = (max_distance > min_distance) ? (max_distance - min_distance) : 1.0;
//
//    cv::Mat result(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0));
//    cv::Mat assigned(rows, cols, CV_8U, cv::Scalar(0)); // 1 si ya asignado
//
//    // mediana por canal (robusto frente a outliers)
//    auto median_of = [](std::vector<float> &v) -> float {
//        size_t n = v.size();
//        size_t mid = n / 2;
//        std::nth_element(v.begin(), v.begin() + mid, v.end());
//        float med = v[mid];
//        if (n % 2 == 0) {
//            // promedio del par medio
//            float left = *std::max_element(v.begin(), v.begin() + mid);
//            med = 0.5f * (med + left);
//        }
//        return med;
//        };
//
//    // Para cada píxel calcular mediana robusta y elegir la mejor muestra
//    tl::parallel_for(0, rows, [&](size_t y) {
//
//        std::vector<float> L;
//        std::vector<float> A;
//        std::vector<float> B;
//        std::vector<float> color_distances;
//        L.reserve(images_size);
//        A.reserve(images_size);
//        B.reserve(images_size);
//        color_distances.reserve(images_size);
//
//        for (int x = 0; x < cols; ++x) {
//
//            L.clear(); 
//            A.clear(); 
//            B.clear();
//
//            // Reunir muestras válidas en Lab
//            for (int i = 0; i < images_size; ++i) {
//                if (valid_masks[i].at<uchar>(static_cast<int>(y), x)) {
//                    cv::Vec3f lab = lab_images[i].at<cv::Vec3f>(static_cast<int>(y), x);
//                    L.push_back(lab[0]);
//                    A.push_back(lab[1]);
//                    B.push_back(lab[2]);
//                }
//            }
//
//            if (L.empty()) {
//                continue;
//            }
//
//            std::vector<float> tmpL = L, tmpA = A, tmpB = B;
//            float median_l = median_of(tmpL);
//            float median_a = median_of(tmpA);
//            float median_b = median_of(tmpB);
//
//            // calcular distancia fotométrica de cada imagen válida al mediano
//            color_distances.clear();
//            color_distances.resize(images_size, std::numeric_limits<float>::infinity());
//
//            // volver a iterar para calcular score por imagen (necesitamos índice original)
//            int idxValid = 0;
//            for (int i = 0; i < images_size; ++i) {
//                if (!valid_masks[i].at<uchar>(static_cast<int>(y), x)) continue;
//                cv::Vec3f lab = lab_images[i].at<cv::Vec3f>(static_cast<int>(y), x);
//                float dL = lab[0] - median_l;
//                float dA = lab[1] - median_a;
//                float dB = lab[2] - median_b;
//                float distance_color = std::sqrt(dL * dL + dA * dA + dB * dB); // Euclid Lab
//                color_distances[i] = distance_color;
//                ++idxValid;
//            }
//
//            // normalize color distances among valid samples to [0,1]
//            float minC = std::numeric_limits<float>::infinity();
//            float maxC = std::numeric_limits<float>::lowest();
//            for (int i = 0; i < images_size; ++i) if (color_distances[i] < std::numeric_limits<float>::infinity()) {
//                if (color_distances[i] < minC) minC = color_distances[i];
//                if (color_distances[i] > maxC) maxC = color_distances[i];
//            }
//            float rangeC = (maxC > minC) ? (maxC - minC) : 1.0f;
//
//            // seleccionar la mejor muestra minimizando score = w_color * color_norm + w_dist * dist_norm
//            float bestScore = std::numeric_limits<float>::infinity();
//            int bestIdx = -1;
//            for (int i = 0; i < images_size; ++i) {
//                if (color_distances[i] == std::numeric_limits<float>::infinity()) continue;
//                float colorNorm = (color_distances[i] - minC) / rangeC; // 0..1
//                float distNorm = static_cast<float>((distances[i] - min_distance) / distance_range);
//                float score = weight_color * colorNorm + weight_distance * distNorm;
//                if (score < bestScore) {
//                    bestScore = score;
//                    bestIdx = i;
//                }
//            }
//
//            if (bestIdx >= 0) {
//                //// asignar píxel desde la mejor imagen
//                //result.at<cv::Vec3b>(static_cast<int>(y), x) = images[bestIdx].at<cv::Vec3b>(static_cast<int>(y), x);
//                //assigned.at<uchar>(static_cast<int>(y), x) = 255;
//                    
//                // --- Interpolación local (suavizado adaptativo) ---
//                cv::Vec3f sumColor(0, 0, 0);
//                float sumWeight = 0.f;
//
//                const int radius = 1; // ventana 3x3
//                const float sigma_s = 1.0f; // peso espacial
//                const float sigma_c = 0.1f; // peso color
//
//                const cv::Mat &img = lab_images[bestIdx];
//                cv::Vec3f center = img.at<cv::Vec3f>(static_cast<int>(y), x);
//
//                for (int dy = -radius; dy <= radius; ++dy) {
//                    int yy = static_cast<int>(y) + dy;
//                    if (yy < 0 || yy >= rows) continue;
//                    for (int dx = -radius; dx <= radius; ++dx) {
//                        int xx = x + dx;
//                        if (xx < 0 || xx >= cols) continue;
//                        if (!valid_masks[bestIdx].at<uchar>(yy, xx)) continue;
//
//                        cv::Vec3f val = img.at<cv::Vec3f>(yy, xx);
//
//                        float ds = std::sqrt(float(dx * dx + dy * dy));
//                        float dc = std::sqrt((val[0] - center[0]) * (val[0] - center[0]) +
//                            (val[1] - center[1]) * (val[1] - center[1]) +
//                            (val[2] - center[2]) * (val[2] - center[2]));
//
//                        float ws = std::exp(-0.5f * (ds * ds) / (sigma_s * sigma_s));
//                        float wc = std::exp(-0.5f * (dc * dc) / (sigma_c * sigma_c));
//                        float w = ws * wc;
//
//                        sumColor += val * w;
//                        sumWeight += w;
//                    }
//                }
//
//                cv::Vec3f labSmoothed = (sumWeight > 0.f) ? (sumColor / sumWeight) : center;
//
//                // volver a BGR
//                cv::Mat labPix(1, 1, CV_32FC3, labSmoothed);
//                cv::Mat bgrPix;
//                cv::cvtColor(labPix, bgrPix, cv::COLOR_Lab2BGR);
//                cv::Vec3f bgr = bgrPix.at<cv::Vec3f>(0, 0) * 255.0f;
//                result.at<cv::Vec3b>(static_cast<int>(y), x) = cv::Vec3b(
//                    cv::saturate_cast<uchar>(bgr[0]),
//                    cv::saturate_cast<uchar>(bgr[1]),
//                    cv::saturate_cast<uchar>(bgr[2])
//                );
//                assigned.at<uchar>(static_cast<int>(y), x) = 255;
//            }
//        }
//    });
//
//    // Post-procesado
//    cv::Mat assigned_copy = assigned.clone();
//    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
//
//    cv::Mat dil;
//    cv::dilate(assigned_copy, dil, kernel);
//
//    // Por cada píxel no asignado, si en el dilatado hay vecinos asignados, copiamos desde la imagen preferida por color/dist
//    tl::parallel_for(0, rows, [&](size_t y) {
//        for (int x = 0; x < cols; ++x) {
//            if (assigned.at<uchar>(static_cast<int>(y), x)) continue;
//            if (!dil.at<uchar>(static_cast<int>(y), x)) continue;
//
//            // Buscar el mejor candidato entre imágenes que tengan píxel válido y que en sus vecinos haya asignación
//            float bestScore = std::numeric_limits<float>::infinity();
//            int bestIdx = -1;
//            for (int i = 0; i < images_size; ++i) {
//                if (!valid_masks[i].at<uchar>(static_cast<int>(y), x)) continue;
//
//                float distNorm = static_cast<float>((distances[i] - min_distance) / distance_range);
//                float score = weight_distance * distNorm;
//                if (score < bestScore) {
//                    bestScore = score;
//                    bestIdx = i;
//                }
//            }
//            if (bestIdx >= 0) {
//                result.at<cv::Vec3b>(static_cast<int>(y), x) = images[bestIdx].at<cv::Vec3b>(static_cast<int>(y), x);
//            }
//        }
//    });
//
//    // Opcional: suavizado local para evitar bordes duros
//    //cv::GaussianBlur(result, result, cv::Size(0, 0), 0.6);
//
//    //Filtro de realce (unsharp mask)
//    //cv::Mat sharp, blurred;
//    //cv::GaussianBlur(result, blurred, cv::Size(0, 0), 1.0);
//    //cv::addWeighted(result, 1.5, blurred, -0.5, 0, sharp);
//    //result = sharp;
//
//    //cv::bilateralFilter(result, 
//
//    return result;
//}
//static cv::Mat combineImagesSmart(const std::vector<cv::Mat> &images,
//                                  const std::vector<double> &distances,
//                                  float weight_color = 0.7f,
//                                  float weight_distance = 0.3f)
//{
//    if (images.empty()) return cv::Mat();
//
//    size_t images_size = images.size();
//
//    TL_ASSERT(images_size == distances.size(), "Number of images and distances must match");
//
//    int rows = images[0].rows;
//    int cols = images[0].cols;
//
//    // Precompute valid masks (non-black)
//    std::vector<cv::Mat> valid_masks(images_size);
//    for (int i = 0; i < images_size; ++i) {
//        cv::inRange(images[i], cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), valid_masks[i]);
//        cv::bitwise_not(valid_masks[i], valid_masks[i]);
//    }
//
//    // Convert images to Lab float32 for perceptual distance
//    std::vector<cv::Mat> lab_images(images_size);
//    for (size_t i = 0; i < images_size; ++i) {
//        images[i].convertTo(lab_images[i], CV_32F, 1.0 / 255.0);
//        cv::cvtColor(lab_images[i], lab_images[i], cv::COLOR_BGR2Lab);
//    }
//
//    // Normalize distances to [0,1] (smaller = better)
//    double min_distance = std::numeric_limits<double>::max();
//    double max_distance = std::numeric_limits<double>::lowest();
//
//    for (const auto &d : distances) {
//        if (d < min_distance) min_distance = d;
//        if (d > max_distance) max_distance = d;
//    }
//
//    double distance_range = (max_distance > min_distance) ? (max_distance - min_distance) : 1.0;
//
//    // Precompute normalized distances
//    std::vector<float> normalized_distances(images_size);
//    for (size_t i = 0; i < images_size; ++i)
//        normalized_distances[i] = static_cast<float>((distances[i] - min_distance) / distance_range);
//
//    cv::Mat result(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0));
//    cv::Mat assigned(rows, cols, CV_8U, cv::Scalar(0));
//
//    // mediana por canal (robusto frente a outliers)
//    auto median_of = [](std::vector<float> &v) -> float {
//
//        size_t n = v.size();
//        size_t mid = n / 2;
//        std::nth_element(v.begin(), v.begin() + mid, v.end());
//        float med = v[mid];
//        if (n % 2 == 0) {
//            // promedio del par medio
//            float left = *std::max_element(v.begin(), v.begin() + mid);
//            med = 0.5f * (med + left);
//        }
//
//        return med;
//    };
//
//    // Para cada píxel calcular mediana robusta y elegir la mejor muestra
//    tl::parallel_for(0, rows, [&](size_t y) {
//
//        std::vector<float> L;
//        std::vector<float> A;
//        std::vector<float> B;
//        std::vector<float> color_distances;
//        L.reserve(images_size);
//        A.reserve(images_size);
//        B.reserve(images_size);
//        color_distances.reserve(images_size);
//
//        for (int x = 0; x < cols; ++x) {
//
//            L.clear();
//            A.clear();
//            B.clear();
//
//            for (int i = 0; i < images_size; ++i) {
//                if (valid_masks[i].at<uchar>(static_cast<int>(y), x)) {
//                    cv::Vec3f lab = lab_images[i].at<cv::Vec3f>(static_cast<int>(y), x);
//                    L.push_back(lab[0]);
//                    A.push_back(lab[1]);
//                    B.push_back(lab[2]);
//                }
//            }
//
//            if (L.empty()) {
//                continue;
//            }
//
//            std::vector<float> tmpL = L, tmpA = A, tmpB = B;
//            float median_l = median_of(tmpL);
//            float median_a = median_of(tmpA);
//            float median_b = median_of(tmpB);
//
//            // Distancia fotométrica de cada imagen válida al mediano
//            color_distances.clear();
//            color_distances.resize(images_size, std::numeric_limits<float>::infinity());
//
//            // Calcular score por imagen
//            int idxValid = 0;
//            for (int i = 0; i < images_size; ++i) {
//                if (!valid_masks[i].at<uchar>(static_cast<int>(y), x)) continue;
//                cv::Vec3f lab = lab_images[i].at<cv::Vec3f>(static_cast<int>(y), x);
//                float dL = lab[0] - median_l;
//                float dA = lab[1] - median_a;
//                float dB = lab[2] - median_b;
//                float distance_color = std::sqrt(dL * dL + dA * dA + dB * dB);
//                color_distances[i] = distance_color;
//                ++idxValid;
//            }
//
//            // normalize color distances among valid samples to [0,1]
//            float minC = std::numeric_limits<float>::infinity();
//            float maxC = std::numeric_limits<float>::lowest();
//            for (int i = 0; i < images_size; ++i) if (color_distances[i] < std::numeric_limits<float>::infinity()) {
//                if (color_distances[i] < minC) minC = color_distances[i];
//                if (color_distances[i] > maxC) maxC = color_distances[i];
//            }
//            float rangeC = (maxC > minC) ? (maxC - minC) : 1.0f;
//
//            // seleccionar la mejor muestra minimizando score = w_color * color_norm + w_dist * dist_norm
//            float bestScore = std::numeric_limits<float>::infinity();
//            int bestIdx = -1;
//            for (int i = 0; i < images_size; ++i) {
//                if (color_distances[i] == std::numeric_limits<float>::infinity()) continue;
//                float colorNorm = (color_distances[i] - minC) / rangeC; // 0..1
//                float distNorm = normalized_distances[i];
//                float score = weight_color * colorNorm + weight_distance * distNorm;
//                if (score < bestScore) {
//                    bestScore = score;
//                    bestIdx = i;
//                }
//            }
//
//            if (bestIdx >= 0) {
//                // Interpolación local bilateral en LAB (ventana 3x3)
//                // Parámetros (ajustables)
//                const int radius = 1; // ventana 3x3
//                const float sigma_s = 1.0f; // espacio (pixels)
//                const float sigma_c = 0.05f; // color (Lab) — ajuste fino: depende del escalado de lab_images
//
//                // central en LAB
//                cv::Vec3f centerLab = lab_images[bestIdx].at<cv::Vec3f>(static_cast<int>(y), x);
//
//                cv::Vec3f sumLab(0.f, 0.f, 0.f);
//                float sumW = 0.f;
//
//                // recorrer vecinos
//                for (int dy = -radius; dy <= radius; ++dy) {
//                    int yy = static_cast<int>(y) + dy;
//                    if (yy < 0 || yy >= rows) continue;
//                    for (int dx = -radius; dx <= radius; ++dx) {
//                        int xx = x + dx;
//                        if (xx < 0 || xx >= cols) continue;
//                        // usar solo si el vecino es válido en la misma imagen
//                        if (!valid_masks[bestIdx].at<uchar>(yy, xx)) continue;
//
//                        cv::Vec3f labNeighbor = lab_images[bestIdx].at<cv::Vec3f>(yy, xx);
//
//                        float dsq = static_cast<float>(dx * dx + dy * dy);
//                        float dcolor = (labNeighbor[0] - centerLab[0]) * (labNeighbor[0] - centerLab[0]) +
//                            (labNeighbor[1] - centerLab[1]) * (labNeighbor[1] - centerLab[1]) +
//                            (labNeighbor[2] - centerLab[2]) * (labNeighbor[2] - centerLab[2]);
//
//                        float w_space = std::exp(-0.5f * dsq / (sigma_s * sigma_s));
//                        float w_color = std::exp(-0.5f * dcolor / (sigma_c * sigma_c));
//                        float w = w_space * w_color;
//
//                        sumLab[0] += labNeighbor[0] * w;
//                        sumLab[1] += labNeighbor[1] * w;
//                        sumLab[2] += labNeighbor[2] * w;
//                        sumW += w;
//                    }
//                }
//
//                cv::Vec3f labSmoothed = (sumW > 0.f) ? (sumLab / sumW) : centerLab;
//
//                // Convertir LAB (float) a BGR 8-bit
//                // cv::cvtColor espera la misma escala usada originalmente para lab_images,
//                // por eso usamos un Mat 1x1 y luego reescalamos si es necesario.
//                cv::Mat labPix(1, 1, CV_32FC3);
//                labPix.at<cv::Vec3f>(0, 0) = labSmoothed;
//                cv::Mat bgrPix;
//                cv::cvtColor(labPix, bgrPix, cv::COLOR_Lab2BGR); // produce float BGR en 0..1 (si la entrada fue en esa escala)
//                cv::Vec3f bgrf = bgrPix.at<cv::Vec3f>(0, 0) * 255.0f;
//
//                cv::Vec3b finalBgr;
//                finalBgr[0] = cv::saturate_cast<uchar>(bgrf[0]);
//                finalBgr[1] = cv::saturate_cast<uchar>(bgrf[1]);
//                finalBgr[2] = cv::saturate_cast<uchar>(bgrf[2]);
//
//                result.at<cv::Vec3b>(static_cast<int>(y), x) = finalBgr;
//                assigned.at<uchar>(static_cast<int>(y), x) = 255;
//            }
//        }
//        });
//
//    // Post-procesado
//    cv::Mat assigned_copy = assigned.clone();
//    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
//
//    cv::Mat dil;
//    cv::dilate(assigned_copy, dil, kernel);
//
//    // Por cada píxel no asignado, si en el dilatado hay vecinos asignados, copiamos desde la imagen preferida por color/dist
//    tl::parallel_for(0, rows, [&](size_t y) {
//        for (int x = 0; x < cols; ++x) {
//            if (assigned.at<uchar>(static_cast<int>(y), x)) continue;
//            if (!dil.at<uchar>(static_cast<int>(y), x)) continue;
//
//            // Buscar el mejor candidato entre imágenes que tengan píxel válido y que en sus vecinos haya asignación
//            float bestScore = std::numeric_limits<float>::infinity();
//            int bestIdx = -1;
//            for (int i = 0; i < images_size; ++i) {
//                if (!valid_masks[i].at<uchar>(static_cast<int>(y), x)) continue;
//
//                float distNorm = normalized_distances[i];
//                float score = weight_distance * distNorm;
//                if (score < bestScore) {
//                    bestScore = score;
//                    bestIdx = i;
//                }
//            }
//            if (bestIdx >= 0) {
//                result.at<cv::Vec3b>(static_cast<int>(y), x) = images[bestIdx].at<cv::Vec3b>(static_cast<int>(y), x);
//            }
//        }
//        });
//
//    // suavizado global ligero
//    // cv::bilateralFilter(result, result, 5, 50, 50);
//
//    return result;
//}

static cv::Mat combineImagesSmart(const std::vector<cv::Mat> &images,
                                  const std::vector<double> &distances,
                                  float weight_color = 0.7f,
                                  float weight_distance = 0.3f)
{
    if (images.empty()) return cv::Mat();

    size_t images_size = images.size();
    TL_ASSERT(images_size == distances.size(), "Number of images and distances must match");

    int rows = images[0].rows;
    int cols = images[0].cols;

    for (size_t i = 1; i < images_size; ++i) {
        TL_ASSERT(images[i].rows == rows && images[i].cols == cols, "All images must have same size");
    }

    int orig_channels = images[0].channels();
    int orig_depth = images[0].depth();

    std::vector<cv::Mat> valid_masks(images_size);
    for (size_t i = 0; i < images_size; ++i) {
        cv::inRange(images[i], cv::Scalar::all(0), cv::Scalar::all(0), valid_masks[i]);
        cv::bitwise_not(valid_masks[i], valid_masks[i]);
    }

    std::vector<cv::Mat> lab_images(images_size);
    for (size_t i = 0; i < images_size; ++i) {
        cv::Mat img = images[i];

        // Normalizar canales: 1->3, 4->3 (descartar alpha temporalmente)
        if (img.channels() == 1) {
            cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
        } else if (img.channels() != 3) {
            TL_THROW_EXCEPTION("Unsupported number of channels");
        }

        if (img.depth() == CV_8U) {
            img.convertTo(img, CV_32F, 1.0 / 255.0);
        } else if (img.depth() == CV_16U) {
            img.convertTo(img, CV_32F, 1.0 / 65535.0);
        } else if (img.depth() == CV_32F) {
            img.convertTo(img, CV_32F);
        } else {
            TL_THROW_EXCEPTION("Unsupported image depth");
        }

        cv::cvtColor(img, lab_images[i], cv::COLOR_BGR2Lab);
    }

    double min_distance = *std::min_element(distances.begin(), distances.end());
    double max_distance = *std::max_element(distances.begin(), distances.end());
    double distance_range = (max_distance > min_distance) ? (max_distance - min_distance) : 1.0;

    std::vector<float> normalized_distances(images_size);
    for (size_t i = 0; i < images_size; ++i)
        normalized_distances[i] = static_cast<float>((distances[i] - min_distance) / distance_range);

    cv::Mat result_lab(rows, cols, CV_32FC3, cv::Scalar(0, 0, 0));

    auto median_of = [](std::vector<float> &v) -> float {
        size_t n = v.size();
        size_t mid = n / 2;
        std::nth_element(v.begin(), v.begin() + mid, v.end());
        float med = v[mid];
        if (n % 2 == 0) {
            float left = *std::max_element(v.begin(), v.begin() + mid);
            med = 0.5f * (med + left);
        }
        return med;
        };

    tl::parallel_for(0, rows, [&](size_t y) {

        std::vector<float> L; 
        L.reserve(images_size);
        std::vector<float> A; 
        A.reserve(images_size);
        std::vector<float> B; 
        B.reserve(images_size);
        std::vector<float> color_distances; 
        color_distances.reserve(images_size);

        for (int x = 0; x < cols; ++x) {

            L.clear(); 
            A.clear();
            B.clear();

            // recolectar muestras válidas para este píxel
            std::vector<int> valid_idxs;
            valid_idxs.reserve(images_size);
            for (int i = 0; i < (int)images_size; ++i) {
                if (valid_masks[i].at<uchar>(static_cast<int>(y), x)) {
                    cv::Vec3f lab = lab_images[i].at<cv::Vec3f>(static_cast<int>(y), x);
                    L.push_back(lab[0]);
                    A.push_back(lab[1]);
                    B.push_back(lab[2]);
                    valid_idxs.push_back(i);
                }
            }

            if (valid_idxs.empty()) continue;

            // mediana inicial por canal
            std::vector<float> tmpL = L, tmpA = A, tmpB = B;
            float medL = median_of(tmpL);
            float medA = median_of(tmpA);
            float medB = median_of(tmpB);

            // distancias fotométricas a la mediana
            color_distances.assign(images_size, std::numeric_limits<float>::infinity());
            for (int idx : valid_idxs) {
                cv::Vec3f lab = lab_images[idx].at<cv::Vec3f>(static_cast<int>(y), x);
                float dL = lab[0] - medL;
                float dA = lab[1] - medA;
                float dB = lab[2] - medB;
                color_distances[idx] = std::sqrt(dL * dL + dA * dA + dB * dB);
            }

            // normalizar color distances solo entre válidos
            float minC = std::numeric_limits<float>::infinity();
            float maxC = std::numeric_limits<float>::lowest();
            for (int idx : valid_idxs) {
                float d = color_distances[idx];
                if (d < minC) minC = d;
                if (d > maxC) maxC = d;
            }
            float rangeC = (maxC > minC) ? (maxC - minC) : 1.0f;

            // elegir mejor imagen minimizando score = w_color*colorNorm + w_dist*distNorm
            float bestScore = std::numeric_limits<float>::infinity();
            int bestIdx = -1;
            for (int idx : valid_idxs) {
                float colorNorm = (color_distances[idx] - minC) / rangeC;
                float distNorm = normalized_distances[idx];
                float score = weight_color * colorNorm + weight_distance * distNorm;
                if (score < bestScore) { bestScore = score; bestIdx = idx; }
            }

            if (bestIdx < 0) continue;

            // Interpolación local bilateral (en la imagen bestIdx)
            int radius = 1;
            float sigma_s = 1.5f;
            // sigma_c: depende de la escala Lab (Lab L ~ 0..100 si input 0..1 -> L ~ 0..100),
            // como hemos convertido desde 0..1, OpenCV produce L~[0..100], a~[-128..127].
            // un valor razonable inicial:
            float sigma_c = 5.0f;

            cv::Vec3f centerLab = lab_images[bestIdx].at<cv::Vec3f>(static_cast<int>(y), x);

            cv::Vec3f sumLab(0.f, 0.f, 0.f);
            float sumW = 0.f;

            for (int dy = -radius; dy <= radius; ++dy) {

                int yy = static_cast<int>(y) + dy;
                if (yy < 0 || yy >= rows) continue;

                for (int dx = -radius; dx <= radius; ++dx) {

                    int xx = x + dx;
                    if (xx < 0 || xx >= cols) continue;
                    if (!valid_masks[bestIdx].at<uchar>(yy, xx)) continue;

                    cv::Vec3f labNeighbor = lab_images[bestIdx].at<cv::Vec3f>(yy, xx);

                    float dsq = static_cast<float>(dx * dx + dy * dy);
                    float dL = labNeighbor[0] - centerLab[0];
                    float dA = labNeighbor[1] - centerLab[1];
                    float dB = labNeighbor[2] - centerLab[2];
                    float dcolor = dL * dL + dA * dA + dB * dB;

                    float w_space = std::exp(-0.5f * dsq / (sigma_s * sigma_s));
                    float w_color = std::exp(-0.5f * dcolor / (sigma_c * sigma_c));
                    float w = w_space * w_color;

                    sumLab[0] += labNeighbor[0] * w;
                    sumLab[1] += labNeighbor[1] * w;
                    sumLab[2] += labNeighbor[2] * w;
                    sumW += w;
                }
            }

            cv::Vec3f labSmoothed = (sumW > 0.f) ? (sumLab / sumW) : centerLab;

            result_lab.at<cv::Vec3f>(static_cast<int>(y), x) = labSmoothed;
        }
        });

    // Convertir Lab float -> BGR float (CV_32F 3ch)
    cv::Mat result_bgr_f;
    cv::cvtColor(result_lab, result_bgr_f, cv::COLOR_Lab2BGR);

    // Postprocesado adaptativo preservando bordes
    // cv::Mat temp; 
    // cv::bilateralFilter(result_bgr_f, temp, 5, 0.1*255, 15); 
    // result_bgr_f = temp;

    // Reconversión a la profundidad y canales originales
    cv::Mat final_img;

    if (orig_channels == 1) {
        cv::Mat gray_f;
        cv::cvtColor(result_bgr_f, gray_f, cv::COLOR_BGR2GRAY);
        if (orig_depth == CV_8U) {
            gray_f.convertTo(final_img, CV_8U, 255.0);
        } else if (orig_depth == CV_16U) {
            gray_f.convertTo(final_img, CV_16U, 65535.0);
        } else { 
            final_img = gray_f.clone();
        }
    } else {
        cv::Mat bgr_out;
        if (orig_depth == CV_8U) {
            result_bgr_f.convertTo(bgr_out, CV_8U, 255.0);
        } else if (orig_depth == CV_16U) {
            result_bgr_f.convertTo(bgr_out, CV_16U, 65535.0);
        } else {
            bgr_out = result_bgr_f.clone();
        }

        if (orig_channels == 3) {
            final_img = bgr_out;
        } else {
            TL_THROW_EXCEPTION("Unsupported original channel count");
        }
    }

    return final_img;
}


OrthophotoTask::OrthophotoTask(double gsd,
                               const std::vector<Image> &images,
                               const std::map<int, Camera> &cameras,
                               const tl::Path &orthoPath,
                               const tl::Path &mdt,
                               const std::string &enuCrs,
                               const std::string &epsg,
                               const std::string &interpolation,
                               bool cuda)
  : tl::TaskBase(),
    mGSD(gsd),
    mPhotos(images),
    mCameras(cameras),
    mOrthoPath(orthoPath),
    mMdt(mdt),
    mEnuCrs(enuCrs),
    mEpsg(epsg),
    mInterpolation(interpolation),
    bCuda(cuda),
    mDataType(tl::DataType::TL_8U),
    mChannels(3)
{
}

OrthophotoTask::~OrthophotoTask()
{

}

auto OrthophotoTask::report() const -> OrthophotoReport
{
    return mOrthophotoReport;
}

void OrthophotoTask::setGSD(double gsd)
{
    mGSD = gsd;
}

void OrthophotoTask::setPhotos(const std::vector<Image> &images)
{
    mPhotos = images;
}

void OrthophotoTask::setOrthoPath(const tl::Path &orthoPath)
{
    mOrthoPath = orthoPath;
}

void OrthophotoTask::setMdt(const tl::Path &mdt)
{
    mMdt = mdt;
}

void OrthophotoTask::setCrs(const std::string &epsg)
{
    mEpsg = epsg;
}

void OrthophotoTask::setCuda(bool active)
{
    bCuda = active;
}

std::vector<std::vector<tl::WindowD>> OrthophotoTask::findGrid(int gridSize) const
{
    std::vector<std::vector<tl::WindowD>> grid;

    try {

        int grid_size = mGSD * gridSize;

        auto reader = tl::ImageReaderFactory::create(mMdt);
        reader->open();
        TL_ASSERT(reader->isOpen(), "Can not open the MDT");
        auto window = reader->window();
        int step_x = std::ceil(window.width() / grid_size);
        int step_y = std::ceil(window.height() / grid_size);

        auto center = window.center();

        double x_ini = center.x - ((step_x - 1) * grid_size) / 2.;
        double y_ini = center.y + ((step_y - 1) * grid_size) / 2.;

        for (size_t i = 0; i < step_x; i++) {

            std::vector<tl::WindowD> row_grid;

            double x = x_ini + grid_size * i;

            for (size_t j = 0; j < step_y; j++) {

                double y = y_ini - grid_size * j;
                row_grid.emplace_back(tl::Point<double>(x, y), grid_size);

            }

            grid.push_back(row_grid);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Compute grid error");
    }

    return grid;
}

void OrthophotoTask::execute(tl::Progress *progressBar)
{

    try {
        
        for (const auto &photo : mPhotos) {
            auto image_reader = tl::ImageReaderFactory::create(photo.path().toStdString());
            image_reader->open();
            if (!image_reader->isOpen()) continue;

            mDataType = image_reader->dataType();
            mChannels = image_reader->channels();
            break;
        }

        tl::Path footprint_file(mOrthoPath);
        footprint_file.append("footprint.shp");
        tl::Path graph_orthos = tl::Path(footprint_file).replaceBaseName("graph_orthos");

        tl::Path dsm_path = mMdt;
        dsm_path.replaceBaseName("dsm_enu");


        OrthoimageTask orthoimage_task(mPhotos,
                                       mCameras,
                                       dsm_path,
                                       mOrthoPath,
                                       graph_orthos,
                                       mEnuCrs,
                                       mEpsg,
                                       footprint_file,
                                       mGSD,
                                       mInterpolation,
                                       1.0,
                                       bCuda);

        this->subscribe([&](const tl::TaskStoppingEvent *event) {
            orthoimage_task.stop();
        });

        orthoimage_task.run(progressBar);

        if (status() == Status::stopping)  return;

        std::vector<std::vector<tl::WindowD>> grid = findGrid(500);

        //TODO: Para imagenes térmicas no se debe aplicar compensación de exposición
#ifndef FAST_ORTHO
        if (mDataType == tl::DataType::TL_8U) {
            orthoimageExposureCompensator(graph_orthos, progressBar);
        }
#endif
        auto orthos = searchTiles(graph_orthos, grid/*, 10*/);

        generateTiles(grid, orthos, progressBar);
        writeOrthomosaic(grid);

        // Limpiar archivos temporales
        //{
        //    for (size_t r = 0; r < grid.size(); r++) {

        //        tl::Path tile_folder(mOrthoPath);
        //        tile_folder.append(std::to_string(r));
        //        tl::Path::removeDirectory(tile_folder);
        //    }

        //    std::unique_ptr<tl::VectorReader> vectorReader;
        //    vectorReader = tl::VectorReaderFactory::create(graph_orthos);
        //    vectorReader->open();

        //    if (vectorReader->isOpen()) {

        //        if (vectorReader->layersCount() >= 1) {

        //            std::map<double, std::shared_ptr<tl::GPolygon>> entities;
        //            std::shared_ptr<tl::GLayer> layer = vectorReader->read(0);

        //            for (const auto &entity : *layer) {

        //                tl::GraphicEntity::Type type = entity->type();
        //                if (type == tl::GraphicEntity::Type::polygon_2d) {

        //                    /// se carga la primera imagen y se busca las que intersectan
        //                    std::shared_ptr<tl::GPolygon> polygon = std::dynamic_pointer_cast<tl::GPolygon>(entity);
        //                    std::shared_ptr<tl::TableRegister> data = polygon->data();
        //                    std::string ortho_to_compensate = data->value(0);

        //                    tl::Path::removeDirectory(ortho_to_compensate);
        //                }
        //            }
        //        }
        //    }
        //}

        if (progressBar) (*progressBar)();

        mOrthophotoReport.time = this->time();
        mOrthophotoReport.gsd = mGSD;
        mOrthophotoReport.epsg = QString::fromStdString(mEpsg);

        tl::Message::success("Orthophoto task finished in {:.2} minutes", mOrthophotoReport.time / 60.);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Orthophoto task error");
    }

}

auto OrthophotoTask::searchTiles(const tl::Path &graph_orthos,
                                 const std::vector<std::vector<tl::WindowD>> &grid,
                                 int maxImages) -> std::vector<std::vector<std::map<double, std::string>>>
{
    std::vector<std::vector<std::map<double, std::string>>> orthos(grid.size());

    std::unique_ptr<tl::VectorReader> vectorReader;
    vectorReader = tl::VectorReaderFactory::create(graph_orthos);
    vectorReader->open();

    TL_ASSERT(vectorReader->isOpen(), "");


    // Se buscan las imagenes correspondientes al grid y se ordenan

    if (vectorReader->layersCount() >= 1) {

        std::shared_ptr<tl::GLayer> layer = vectorReader->read(0);

        for (size_t r = 0; r < grid.size(); r++) {

            orthos[r].resize(grid[r].size());

            for (size_t c = 0; c < grid[r].size(); c++) {

                const auto &window = grid[r][c];
                mWindowAll = joinWindow(mWindowAll, window);
                auto window_center = window.center();
                //double min_distance = std::numeric_limits<double>::max();

                // Se busca la mejor ortoimagen
                for (const auto &entity : *layer) {

                    tl::GraphicEntity::Type type = entity->type();
                    if (type == tl::GraphicEntity::Type::polygon_2d) {
                        auto polygon = std::dynamic_pointer_cast<tl::GPolygon>(entity);
                        auto window_orto = polygon->window();
                        auto orto_center = window_orto.center();
                        auto dist = tl::distance(orto_center, window_center);
                        tl::Path orto_compensate(polygon->data()->value(0));
#ifndef FAST_ORTHO
                        if (mDataType == tl::DataType::TL_8U) {
                            std::string name = orto_compensate.baseName().toUtf8() + "_compensate.tif";
                            orto_compensate.replaceFileName(name);
                        }
#endif
                        orthos[r][c][dist] = orto_compensate.toUtf8();
                    }

                }

                if (maxImages > 0 && maxImages < orthos[r][c].size()) {
                    orthos[r][c].erase(std::next(orthos[r][c].begin(), maxImages), orthos[r][c].end());
                }

            }
        }
    }

    return orthos;
}

void OrthophotoTask::generateTiles(const std::vector<std::vector<tl::WindowD>> &grid, 
                                   std::vector<std::vector<std::map<double, std::string>>> &orthos, 
                                   tl::Progress *progressBar)
{
    double step = 0;
    if (progressBar) {
        auto max = progressBar->maximum() / 2 - 1;
        step = (progressBar->maximum() / 2 - 1) / static_cast<double>(grid.size() * grid[0].size());
    }
    double progress_value = 0;

    for (size_t r = 0; r < grid.size(); r++) {
        for (size_t c = 0; c < grid[r].size(); c++) {


            if (progressBar) {

                progress_value += step;
                int value = static_cast<int>(trunc(progress_value));
                if (value >= 1) {
                    (*progressBar)(value);
                    progress_value -= value;
                }
            }

            const auto &window = grid[r][c];

            // Tamaño de imagen
            // Solo expandir en el caso de la generación de mosaico con fusión. Si se hace con combineImagesSmart no haría falta
            auto window_aux = tl::expandWindow(window, ortho_overlap * mGSD);
            int image_size = tl::numberCast<int>(window_aux.width() / mGSD);

            // Todas las imagenes del elemento actual del grid
            std::vector<cv::Mat> images;
            std::vector<double> distances;

            for (auto &ortho : orthos[r][c]) {

                auto image_reader = tl::ImageReaderFactory::create(ortho.second);
                image_reader->open();
                if (!image_reader->isOpen()) {
                    tl::Message::error("Image open error :{}", ortho.second);
                    continue;
                }

                // Por ahora solo se utilizan las imagenes que contienen el grid. Se omiten las que intersectan
                if (!tl::intersectWindows(image_reader->window(), window)) continue;

                tl::Affine<int, 2> affine;
                auto image = image_reader->read(window_aux, 1., 1., &affine);
                auto data_type = image_reader->dataType();
                image_reader->close();

                if (image.rows != image_size || image.cols != image_size) {
                    cv::Mat aux = cv::Mat::zeros(image_size, image_size, image.type());

                    cv::Rect roi(affine.translation().x(), affine.translation().y(), image.cols, image.rows);
                    cv::Mat image_roi = aux(roi);
                    image.copyTo(image_roi);
                    image = aux;
                }

                images.push_back(image);
                distances.push_back(ortho.first);

            }

            if (images.empty()) continue;

            cv::Mat read_image;
            if (images.size() == 1) {
                read_image = images[0].clone();
            } else {
#ifdef FAST_ORTHO
                read_image = combineImagesSmart(images, distances);
#else
                read_image = combineImages(images);
#endif
            }

            if (read_image.empty()) continue;

            try {
                tl::Path tile(mOrthoPath);
                tile.append(std::to_string(r));
                tile.append(std::to_string(c));
                tile.createDirectories();
                tile.append("t.tif");

                auto image_writer = tl::ImageWriterFactory::create(tile);
                image_writer->open();

                image_writer->create(read_image.rows, read_image.cols, read_image.channels(), mDataType);
                tl::Crs crs(mEpsg);
                image_writer->setCRS(crs.toWktFormat());
                tl::Affine<double, 2> affine_ortho(mGSD, -mGSD, window_aux.pt1.x, window_aux.pt2.y, 0.0);
                image_writer->setGeoreference(affine_ortho);
                image_writer->write(read_image);
                image_writer->close();
            } catch (std::exception &e) {
                tl::printException(e);
            }
        }
    }
}

#ifdef FAST_ORTHO

void OrthophotoTask::writeOrthomosaic(const std::vector<std::vector<tl::WindowD>> &grid)
{

    try {

        tl::Message::info("Writing ortho image");

        tl::Path ortho_final(mOrthoPath);
        ortho_final.append("ortho.tif");
        std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(ortho_final);
        image_writer->open();
        int cols = static_cast<int>(std::round(mWindowAll.width() / mGSD));
        int rows = static_cast<int>(std::round(mWindowAll.height() / mGSD));

        auto options = std::make_shared<tl::TiffOptions>();
        options->enableTiled(true);
        options->setBigTiff(tl::TiffOptions::BigTiff::if_needed);
        options->setCompress(tl::TiffOptions::Compress::lzw);

        if (image_writer->isOpen()) {
            image_writer->create(rows, cols, mChannels, mDataType, options);

            auto metadata = std::make_shared<tl::ImageMetadata>();
            metadata->setMetadata("TIFFTAG_DOCUMENTNAME", "Orthomosaic");
            metadata->setMetadata("TIFFTAG_IMAGEDESCRIPTION", "Orthomosaic generated by GRAPHOS");
            metadata->setMetadata("TIFFTAG_SOFTWARE", "GRAPHOS");
            QString tiffDateTime = QDateTime::currentDateTime().toString("yyyy:MM:dd HH:mm:ss");
            metadata->setMetadata("TIFFTAG_DATETIME", tiffDateTime.toStdString());
            image_writer->setMetadata(metadata);
            tl::Crs crs(mEpsg);
            image_writer->setCRS(crs.toWktFormat());
            tl::Affine<double, 2> affine_ortho(mGSD, -mGSD, mWindowAll.pt1.x, mWindowAll.pt2.y, 0.0);
            image_writer->setGeoreference(affine_ortho);

            for (size_t r = 0; r < grid.size(); r++) {
                for (size_t c = 0; c < grid[r].size(); c++) {

                    try {
                        
                        tl::Path tile(mOrthoPath);
                        tile.append(std::to_string(r));
                        tile.append(std::to_string(c));
                        tile.append("t.tif");
                        if (!tile.exists()) continue;
                        auto image_reader = tl::ImageReaderFactory::create(tile);
                        image_reader->open();
                        if (!image_reader->isOpen()) {
                            continue;
                        }

                        auto tile_window = image_reader->window();

                        const auto &window = grid[r][c];
                        if (!intersectWindows(tile_window, window)) continue;

                        cv::Mat compensate_image = image_reader->read();
                        // Relleno de pixeles negros
                        cv::Mat blackPixelMask = createBlackPixelMask(compensate_image, 1024);
                        //cv::Mat black_pixel_mask = createBlackPixelMask(compensate_image, 512);
                        cv::inpaint(compensate_image, blackPixelMask, compensate_image, 3, cv::INPAINT_TELEA);
                        //cv::inpaint(compensate_image, black_pixel_mask, compensate_image, 5, cv::INPAINT_NS);

                        auto affine_ortho_inverse = affine_ortho.inverse();
                        tl::Point<double> p1_ortho = affine_ortho_inverse.transform(tile_window.pt1);
                        tl::Point<double> p2_ortho = affine_ortho_inverse.transform(tile_window.pt2);
                        tl::WindowI window_to_write(static_cast<tl::Point<int>>(p1_ortho), static_cast<tl::Point<int>>(p2_ortho));
                        window_to_write.normalized();
                        if (window_to_write.isValid())
                            image_writer->write(compensate_image, window_to_write);
                    } catch (std::exception &e) {
                        tl::printException(e);
                    }
                }
            }

        }

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

#else




static cv::Mat createWeightMapTile(int w = ortho_tile_w, int h = ortho_tile_h, int overlap = ortho_overlap)
{
    cv::Mat weight(h, w, CV_32F);

    for (int y = 0; y < h; ++y) {

        float dy = std::min<float>(y, h - 1 - y);
        float wy = std::min(1.0f, dy / float(overlap));

        for (int x = 0; x < w; ++x) {

            float dx = std::min<float>(x, w - 1 - x);
            float wx = std::min(1.0f, dx / float(overlap));
            weight.at<float>(y, x) = wx * wy;

        }
    }

    return weight;
}


static cv::Mat maskToFloat(const cv::Mat &mask8u)
{
    cv::Mat f;
    mask8u.convertTo(f, CV_32F, 1.0 / 255.0);
    return f;
}

void OrthophotoTask::writeOrthomosaic(const std::vector<std::vector<tl::WindowD>> &grid)
{
    try {

        tl::Message::info("Writing ortho image (blended)");

        tl::Path ortho_final(mOrthoPath);
        ortho_final.append("ortho.tif");
        std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(ortho_final);
        image_writer->open();

        int cols = static_cast<int>(std::round(mWindowAll.width() / mGSD));
        int rows = static_cast<int>(std::round(mWindowAll.height() / mGSD));

        auto options = std::make_shared<tl::TiffOptions>();
        options->enableTiled(true);
        options->setBigTiff(tl::TiffOptions::BigTiff::if_needed);
        options->setCompress(tl::TiffOptions::Compress::lzw);

        if (!image_writer->isOpen()) {
            tl::Message::error("Cannot open image writer for {}", ortho_final.toUtf8());
            return;
        }

        image_writer->create(rows, cols, mChannels, mDataType, options);

        auto metadata = std::make_shared<tl::ImageMetadata>();
        metadata->setMetadata("TIFFTAG_DOCUMENTNAME", "Orthomosaic");
        metadata->setMetadata("TIFFTAG_IMAGEDESCRIPTION", "Orthomosaic generated by GRAPHOS");
        metadata->setMetadata("TIFFTAG_SOFTWARE", "GRAPHOS");
        QString tiffDateTime = QDateTime::currentDateTime().toString("yyyy:MM:dd HH:mm:ss");
        metadata->setMetadata("TIFFTAG_DATETIME", tiffDateTime.toStdString());
        image_writer->setMetadata(metadata);
        tl::Crs crs(mEpsg);
        image_writer->setCRS(crs.toWktFormat());
        tl::Affine<double, 2> affine_ortho(mGSD, -mGSD, mWindowAll.pt1.x, mWindowAll.pt2.y, 0.0);
        image_writer->setGeoreference(affine_ortho);
        auto affine_ortho_inverse = affine_ortho.inverse();

        cv::Mat base_weight = createWeightMapTile(ortho_tile_w, ortho_tile_h, ortho_overlap);

        // Ajustar block_height según memoria
        const int block_height = 2048;
        for (int block_y = 0; block_y < rows; block_y += block_height) {

            int current_block_h = std::min(block_height, rows - block_y);

            cv::Mat accumulator(current_block_h, cols, CV_32FC(mChannels), cv::Scalar(0));
            cv::Mat weight_sum(current_block_h, cols, CV_32F, cv::Scalar(0));

            // Recorremos todas las tiles y acumulamos las que intersectan este bloque
            for (size_t r = 0; r < grid.size(); ++r) {
                for (size_t c = 0; c < grid[r].size(); ++c) {

                    try {

                        tl::Path tile(mOrthoPath);
                        tile.append(std::to_string(r));
                        tile.append(std::to_string(c));
                        tile.append("t.tif");
                        if (!tile.exists()) continue;

                        auto image_reader = tl::ImageReaderFactory::create(tile);
                        image_reader->open();
                        if (!image_reader->isOpen()) continue;

                        auto tile_window = image_reader->window();
                        const auto &window = grid[r][c];

                        if (!intersectWindows(tile_window, window)) continue;

                        cv::Mat tile_image = image_reader->read();

                        int tile_width = tile_image.cols;
                        int tile_height = tile_image.rows;
                        cv::Mat tile_weights;
                        if (tile_width == ortho_tile_w && tile_height == ortho_tile_h) {
                            tile_weights = base_weight;
                        } else {
                            tile_weights = createWeightMapTile(tile_width, tile_height, ortho_overlap);
                        }

                        // Máscara de píxeles negros (0 = negro, 255 = válido)
                        cv::Mat black_pixel_mask = createBlackPixelMask(tile_image, 1024);
                        cv::Mat valid_mask;
                        cv::bitwise_not(black_pixel_mask, valid_mask);
                        black_pixel_mask.release();

                        cv::Mat valid_mask_float = maskToFloat(valid_mask);
                        valid_mask.release();

                        cv::Mat weight_masked;
                        cv::multiply(tile_weights, valid_mask_float, weight_masked);
                        valid_mask_float.release();

                        auto p1_ortho = affine_ortho_inverse.transform(tile_window.pt1);
                        auto p2_ortho = affine_ortho_inverse.transform(tile_window.pt2);
                        tl::WindowI window_to_write(static_cast<tl::Point<int>>(p1_ortho), static_cast<tl::Point<int>>(p2_ortho));
                        window_to_write.normalized();
                        if (!window_to_write.isValid()) continue;

                        cv::Rect dst_rect(window_to_write.pt1.x, window_to_write.pt1.y, window_to_write.width(), window_to_write.height());

                        cv::Rect inter = dst_rect & cv::Rect(0, block_y, cols, current_block_h);
                        if (inter.width <= 0 || inter.height <= 0) continue;

                        int src_x = inter.x - dst_rect.x;
                        int src_y = inter.y - dst_rect.y;
                        cv::Rect src_rect(src_x, src_y, inter.width, inter.height);
                        cv::Rect accumulator_rect(inter.x, inter.y - block_y, inter.width, inter.height);

                        cv::Mat tile_float;
                        tile_image.convertTo(tile_float, CV_32F);

                        cv::Mat tile_patch = tile_float(src_rect); 
                        cv::Mat weight_patch = weight_masked(src_rect);

                        std::vector<cv::Mat> weight_channels(mChannels);
                        for (int ch = 0; ch < mChannels; ++ch) weight_channels[ch] = weight_patch;
                        cv::Mat weightPatch3;
                        cv::merge(weight_channels, weightPatch3);

                        cv::Mat accumulator_roi = accumulator(accumulator_rect);
                        cv::Mat weighted;
                        cv::multiply(tile_patch, weightPatch3, weighted);
                        accumulator_roi += weighted;

                        cv::Mat wsumROI = weight_sum(accumulator_rect);
                        wsumROI += weight_patch;

                    } catch (std::exception &e) {
                        tl::printException(e);
                    }
                }
            }

            std::vector<cv::Mat> wv(mChannels, cv::Mat());
            for (int ch = 0; ch < mChannels; ++ch) wv[ch] = weight_sum;
            cv::Mat denom;
            cv::merge(wv, denom);


            cv::Mat denomSafe = denom.clone();

            cv::Mat zeroMask;
            cv::compare(denomSafe, 0.0f, zeroMask, cv::CMP_EQ);
            denomSafe.setTo(1.0f, zeroMask);

            cv::Mat target = accumulator / denomSafe;

            cv::Mat target_out;
            switch (mDataType) {
                case tl::DataType::TL_8U:  
                    target.convertTo(target_out, CV_8U); 
                    break;
                case tl::DataType::TL_16U: 
                    target.convertTo(target_out, CV_16U); 
                    break;
                case tl::DataType::TL_32F: 
                    target_out = target.clone(); 
                    break;
                case tl::DataType::TL_64F:
                    target.convertTo(target_out, CV_64F);
                    break;
                default: 
                    target.convertTo(target_out, CV_32F);
                    break;
            }

            tl::WindowI write_window(tl::Point<int>(0, block_y), tl::Point<int>(cols, block_y + current_block_h));

            image_writer->write(target_out, write_window);
        }

    } catch (std::exception &e) {
        tl::printException(e);
    }
}
#endif


void OrthophotoTask::orthoimageExposureCompensator(const tl::Path &graph_orthos, tl::Progress *progressBar)
{

    try {

        tl::ChronoAuto chrono("Exposure compensator");

        tl::WindowD window_all;
        std::vector<cv::Point> corners;
        std::vector<cv::Mat> mat_orthos;
        std::vector<cv::UMat> umat_orthos;
        std::vector<cv::UMat> ortho_masks;

        //int type = cv::detail::ExposureCompensator::NO;
        int type = cv::detail::ExposureCompensator::GAIN;
        //int type = cv::detail::ExposureCompensator::GAIN_BLOCKS;
        //int type = cv::detail::ExposureCompensator::CHANNELS;
        //int type = cv::detail::ExposureCompensator::CHANNELS_BLOCKS;
        cv::Ptr<cv::detail::ExposureCompensator> compensator = cv::detail::ExposureCompensator::createDefault(type);

        std::unique_ptr<tl::VectorReader> vectorReader;
        vectorReader = tl::VectorReaderFactory::create(graph_orthos);
        vectorReader->open();

        if (vectorReader->isOpen()) {

            if (vectorReader->layersCount() >= 1) {

                std::map<double, std::shared_ptr<tl::GPolygon>> entities;
                std::shared_ptr<tl::GLayer> layer = vectorReader->read(0);

                for (const auto &entity : *layer) {

                    tl::GraphicEntity::Type type = entity->type();
                    if (type == tl::GraphicEntity::Type::polygon_2d) {

                        /// se carga la primera imagen y se busca las que intersectan
                        std::shared_ptr<tl::GPolygon> polygon = std::dynamic_pointer_cast<tl::GPolygon>(entity);
                        std::shared_ptr<tl::TableRegister> data = polygon->data();
                        std::string ortho_to_compensate = data->value(0);
                        tl::WindowD window = polygon->window();
                        tl::Point<double> center = window.center();

                        /// Busqueda de imagenes que intersectan

                        std::vector<std::string> orthos;
                        orthos.push_back(ortho_to_compensate);
                        std::vector<tl::WindowD> windows;
                        windows.push_back(window);

                        for (const auto &entity2 : *layer) {
                            std::shared_ptr<tl::GPolygon> polygon2 = std::dynamic_pointer_cast<tl::GPolygon>(entity2);
                            std::shared_ptr<tl::TableRegister> data = polygon2->data();
                            std::string orto = data->value(0);

                            if (orto != ortho_to_compensate) {

                                if (polygon2->isInner(center)) {
                                    orthos.push_back(orto);
                                    windows.push_back(polygon2->window());
                                    window_all = joinWindow(window_all, polygon2->window());
                                }

                            }

                        }

                        size_t n_orthos = orthos.size();
                        corners.resize(n_orthos);
                        ortho_masks.resize(n_orthos);
                        umat_orthos.resize(n_orthos);
                        mat_orthos.resize(n_orthos);

                        /// Aplicar un factor de escala para el calculo de la compensación de exposición
                        for (size_t i = 0; i < n_orthos; i++) {

                            try {
                                std::unique_ptr<tl::ImageReader> image_reader = tl::ImageReaderFactory::create(orthos[i]);
                                image_reader->open();
                                if (image_reader->isOpen()) {
                                    cv::Mat image = image_reader->read(exposure_compensator_factor, exposure_compensator_factor);
                                    mat_orthos[i] = image.clone();
                                    double scale = image_reader->georeference().scale().x();

                                    /// Esquinas
                                    corners[i].x = tl::roundToInteger((windows[i].pt1.x - window_all.pt1.x) * exposure_compensator_factor / scale);
                                    corners[i].y = tl::roundToInteger((window_all.pt2.y - windows[i].pt2.y) * exposure_compensator_factor / scale);

                                    /// La mascara debería leerse si se creó en la generación del MDS.
                                    ortho_masks[i].create(image.size(), CV_8U);
                                    cv::Mat gray;
                                    if (image.channels() != 1) {
                                        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
                                    } else {
                                        gray = image;
                                    }

                                    ortho_masks[i].setTo(cv::Scalar::all(0));
                                    ortho_masks[i].setTo(cv::Scalar::all(255), gray > 0);

                                }
                            } catch (std::exception &e) {
                                tl::printException(e);
                            }
                        }

                        cv::InputArrayOfArrays(mat_orthos).getUMatVector(umat_orthos);
                        compensator->feed(corners, umat_orthos, ortho_masks);

                        tl::Message::info("Seam finder");

                        cv::Ptr<cv::detail::SeamFinder> seam_finder;
                        //seam_finder = cv::makePtr<cv::detail::NoSeamFinder>();
                        seam_finder = cv::makePtr<cv::detail::VoronoiSeamFinder>();
                        //seam_finder = cv::makePtr<cv::detail::DpSeamFinder>(cv::detail::DpSeamFinder::COLOR);
                        //seam_finder = cv::makePtr<cv::detail::DpSeamFinder>(cv::detail::DpSeamFinder::COLOR_GRAD);
                        seam_finder->find(umat_orthos, corners, ortho_masks);
                        umat_orthos.clear();
                        mat_orthos.clear();

                        std::unique_ptr<tl::ImageReader> image_reader = tl::ImageReaderFactory::create(ortho_to_compensate);
                        image_reader->open();
                        if (image_reader->isOpen()) {
                            cv::Mat compensate_image = image_reader->read();

                            /// Se compensa la imagen
                            cv::Point corner = corners[0] / exposure_compensator_factor;
                            cv::Mat gray;
                            if (compensate_image.channels() == 1)
                                gray = compensate_image;
                            else
                                cv::cvtColor(compensate_image, gray, cv::COLOR_BGR2GRAY);
                            cv::Mat mask_full_size(compensate_image.size(), CV_8U);
                            mask_full_size.setTo(cv::Scalar::all(0));
                            mask_full_size.setTo(cv::Scalar::all(255), gray > 0);
                            //cv::Mat element = getStructuringElement(cv::MorphShapes::MORPH_RECT,
                            //                                        cv::Size(2 * 2 + 1, 2 * 2 + 1),
                            //                                        cv::Point(2, 2));
                            //Revisar
                            //cv::erode(mask_full_size, mask_full_size, element);
                            //cv::dilate(mask_full_size, mask_full_size, element);
                            compensator->apply(0, corner, compensate_image, mask_full_size);

                            tl::Path orto_compensate(ortho_to_compensate);
                            std::string name = orto_compensate.baseName().toUtf8() + "_compensate.tif";
                            orto_compensate.replaceFileName(name);
                            std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(orto_compensate);
                            image_writer->open();
                            if (image_writer->isOpen()) {
                                image_writer->create(image_reader->rows(), image_reader->cols(), image_reader->channels(), image_reader->dataType());
                                image_writer->setCRS(image_reader->crsWkt());
                                image_writer->setGeoreference(image_reader->georeference());
                                image_writer->write(compensate_image);
                                image_writer->close();
                                tl::Message::info("Compensated image: {}", orto_compensate.fileName().toUtf8());
                            }
                        }

                        if (progressBar) (*progressBar)();
                    }
                }
            }
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

}

} // namespace graphos
