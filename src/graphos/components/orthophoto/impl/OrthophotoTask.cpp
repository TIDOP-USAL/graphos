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
//#include "graphos/core/multispectral/Vignetting.h"
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
#include <tidop/img/img.h>
#include <tidop/geospatial/crstransf.h>

/* OpenCV */
#include <opencv2/stitching.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>

#include <QDateTime>

#include <map>

#define FAST_ORTHO 1

namespace graphos
{

constexpr double exposure_compensator_factor = 0.1;
constexpr int ortho_tile_w = 600;
constexpr int ortho_tile_h = 600;
#ifndef FAST_ORTHO
constexpr int ortho_overlap = 50;
#else
constexpr int ortho_overlap = 0;
#endif

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
            //float bestScore = std::numeric_limits<float>::infinity();
            //int bestIdx = -1;
            //for (int idx : valid_idxs) {
            //    float colorNorm = (color_distances[idx] - minC) / rangeC;
            //    float distNorm = normalized_distances[idx];
            //    float score = weight_color * colorNorm + weight_distance * distNorm;
            //    if (score < bestScore) { 
            //        bestScore = score; 
            //        bestIdx = idx; 
            //    }
            //}

            //if (bestIdx < 0) continue;

            //result_lab.at<cv::Vec3f>(static_cast<int>(y), x) = lab_images[bestIdx].at<cv::Vec3f>(static_cast<int>(y), x);



            // calcular score de cada imagen válida
            std::vector<std::pair<float, int>> scored;
            scored.reserve(valid_idxs.size());
            for (int idx : valid_idxs) {
                float colorNorm = (color_distances[idx] - minC) / rangeC;
                float distNorm = normalized_distances[idx];
                float score = weight_color * colorNorm + weight_distance * distNorm;
                scored.emplace_back(score, idx);
            }

            // ordenar por score ascendente (menor es mejor)
            std::sort(scored.begin(), scored.end(),
                [](const auto &a, const auto &b) { return a.first < b.first; });

            if (scored.empty()) continue;

            // número de mejores píxeles a usar
            const int TOP_N = 1;
            int n = std::min<int>(TOP_N, scored.size());

            // calcular media ponderada de los N mejores
            float total_w = 0.f;
            cv::Vec3f sum_lab(0.f, 0.f, 0.f);

            for (int i = 0; i < n; ++i) {
                int idx = scored[i].second;
                float w = 1.0f / (1e-3f + scored[i].first); // peso inverso al score
                cv::Vec3f lab = lab_images[idx].at<cv::Vec3f>(static_cast<int>(y), x);
                sum_lab += lab * w;
                total_w += w;
            }

            cv::Vec3f labWeighted = (total_w > 0.f) ? (sum_lab / total_w)
                : lab_images[scored[0].second].at<cv::Vec3f>(static_cast<int>(y), x);

            result_lab.at<cv::Vec3f>(static_cast<int>(y), x) = labWeighted;

            // Interpolación local bilateral (en la imagen bestIdx)
            // Se toman los pixeles vecinos para generar una imagen mas suavizada
            //int radius = 1;
            //float sigma_s = 1.5f;
            // sigma_c: depende de la escala Lab (Lab L ~ 0..100 si input 0..1 -> L ~ 0..100),
            // como hemos convertido desde 0..1, OpenCV produce L~[0..100], a~[-128..127].
            // un valor razonable inicial:
            //float sigma_c = 5.0f;

            //cv::Vec3f sumLab(0.f, 0.f, 0.f);
            //float sumW = 0.f;

            //for (int dy = -radius; dy <= radius; ++dy) {

            //    int yy = static_cast<int>(y) + dy;
            //    if (yy < 0 || yy >= rows) continue;

            //    for (int dx = -radius; dx <= radius; ++dx) {

            //        int xx = x + dx;
            //        if (xx < 0 || xx >= cols) continue;
            //        if (!valid_masks[bestIdx].at<uchar>(yy, xx)) continue;

            //        cv::Vec3f labNeighbor = lab_images[bestIdx].at<cv::Vec3f>(yy, xx);

            //        float dsq = static_cast<float>(dx * dx + dy * dy);
            //        float dL = labNeighbor[0] - centerLab[0];
            //        float dA = labNeighbor[1] - centerLab[1];
            //        float dB = labNeighbor[2] - centerLab[2];
            //        float dcolor = dL * dL + dA * dA + dB * dB;

            //        float w_space = std::exp(-0.5f * dsq / (sigma_s * sigma_s));
            //        float w_color = std::exp(-0.5f * dcolor / (sigma_c * sigma_c));
            //        float w = w_space * w_color;

            //        sumLab[0] += labNeighbor[0] * w;
            //        sumLab[1] += labNeighbor[1] * w;
            //        sumLab[2] += labNeighbor[2] * w;
            //        sumW += w;
            //    }
            //}
            
            //cv::Vec3f labSmoothed = (sumW > 0.f) ? (sumLab / sumW) : labWeighted;
            //result_lab.at<cv::Vec3f>(static_cast<int>(y), x) = labSmoothed;
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

static cv::Mat combineImagesSmartMono(const std::vector<cv::Mat> &images,
                                      const std::vector<double> &distances,
                                      float weight_intensity = 0.7f,
                                      float weight_distance = 0.3f)
{
    if (images.empty()) return cv::Mat();

    size_t images_size = images.size();
    TL_ASSERT(images_size == distances.size(), "Number of images and distances must match");

    int rows = images[0].rows;
    int cols = images[0].cols;
    int depth = images[0].depth();

    // Verificación de dimensiones y formato
    for (size_t i = 1; i < images_size; ++i) {
        TL_ASSERT(images[i].rows == rows && images[i].cols == cols, "All images must have same size");
        TL_ASSERT(images[i].channels() == 1, "combineImagesSmartMono only supports single-channel images");
        TL_ASSERT(images[i].depth() == depth, "All images must have the same bit depth");
    }

    // Máscaras de píxeles válidos (no negros)
    std::vector<cv::Mat> valid_masks(images_size);
    for (size_t i = 0; i < images_size; ++i) {
        cv::Mat mask_is_nodata;
        cv::compare(images[i], tl::NoData<float>, mask_is_nodata, cv::CMP_EQ);
        cv::bitwise_not(mask_is_nodata, valid_masks[i]);
    }

    // Normalizar distancias
    double min_distance = *std::min_element(distances.begin(), distances.end());
    double max_distance = *std::max_element(distances.begin(), distances.end());
    double distance_range = (max_distance > min_distance) ? (max_distance - min_distance) : 1.0;

    std::vector<float> normalized_distances(images_size);
    for (size_t i = 0; i < images_size; ++i)
        normalized_distances[i] = static_cast<float>((distances[i] - min_distance) / distance_range);

    cv::Mat result(rows, cols, images[0].type(), cv::Scalar(tl::NoData<float>));
    cv::Mat assigned(rows, cols, CV_8U, cv::Scalar(0));

    // Lambda para mediana robusta
    // Codigo repetido. Sacar a función o utilizar median de TidopLib
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
        std::vector<float> samples;
        std::vector<float> intensity_diffs;
        samples.reserve(images_size);
        intensity_diffs.reserve(images_size);

        for (int x = 0; x < cols; ++x) {
            samples.clear();

            // Recolectar intensidades válidas
            for (size_t i = 0; i < images_size; ++i) {
                if (valid_masks[i].at<uchar>(static_cast<int>(y), x)) {
                    float val = images[i].at<float>(static_cast<int>(y), x);
                    samples.push_back(val);
                }
            }

            if (samples.empty()) continue;

            // Calcular mediana
            std::vector<float> tmp = samples;
            float median = median_of(tmp);

            // Calcular distancias de intensidad
            intensity_diffs.assign(images_size, std::numeric_limits<float>::infinity());
            for (size_t i = 0; i < images_size; ++i) {
                if (!valid_masks[i].at<uchar>(static_cast<int>(y), x)) continue;
                float val = images[i].at<float>(static_cast<int>(y), x);
                intensity_diffs[i] = std::fabs(val - median);
            }

            float minC = std::numeric_limits<float>::infinity();
            float maxC = std::numeric_limits<float>::lowest();
            for (float d : intensity_diffs) {
                if (d < minC) minC = d;
                if (d > maxC) maxC = d;
            }
            float rangeC = (maxC > minC) ? (maxC - minC) : 1.0f;

            // Seleccionar mejor muestra
            //float bestScore = std::numeric_limits<float>::infinity();
            //int bestIdx = -1;
            //for (size_t i = 0; i < images_size; ++i) {
            //    if (intensity_diffs[i] == std::numeric_limits<float>::infinity()) continue;
            //    float colorNorm = (intensity_diffs[i] - minC) / rangeC;
            //    float score = weight_intensity * colorNorm + weight_distance * normalized_distances[i];
            //    if (score < bestScore) {
            //        bestScore = score;
            //        bestIdx = static_cast<int>(i);
            //    }
            //}

            //if (bestIdx >= 0) {
            //    result.at<float>(y, x) = images[bestIdx].at<float>(y, x);
            //    assigned.at<uchar>(y, x) = 255;
            //}

            // Calcular score de cada imagen válida
            std::vector<std::pair<float, int>> scored;
            scored.reserve(images_size);
            for (size_t i = 0; i < images_size; ++i) {
                if (intensity_diffs[i] == std::numeric_limits<float>::infinity()) continue;
                float colorNorm = (intensity_diffs[i] - minC) / rangeC;
                float score = weight_intensity * colorNorm + weight_distance * normalized_distances[i];
                scored.emplace_back(score, static_cast<int>(i));
            }

            if (scored.empty()) continue;

            // Ordenar por score ascendente (mejor primero)
            std::sort(scored.begin(), scored.end(),
                [](const auto &a, const auto &b) { return a.first < b.first; });

            // Número de mejores píxeles a usar
            const int TOP_N = 3;
            int n = std::min<int>(TOP_N, scored.size());

            // Calcular media ponderada de los N mejores
            float total_w = 0.f;
            float sum_val = 0.f;

            for (int i = 0; i < n; ++i) {
                size_t idx = static_cast<size_t>(scored[i].second);
                float score = scored[i].first;
                float w = 1.0f / (1e-3f + score); // peso inverso al score
                //float w = 1.0f / (1e-3f + score + 0.1f * normalized_distances[idx]); // No veo que mejore 
                // Se nota mucho mas el efecto
                //const float ALPHA = 15.0f; // Ajuste este valor. Cuanto más bajo, más suave la transición.
                //float w = std::exp(-ALPHA * score * score); // El peso es más grande si el score es bajo.
                float val = images[idx].at<float>(static_cast<int>(y), x);
                sum_val += val * w;
                total_w += w;
            }

            float weighted_val = (total_w > 0.f) ? (sum_val / total_w)
                : images[scored[0].second].at<float>(static_cast<int>(y), x);

            result.at<float>(static_cast<int>(y), x) = weighted_val;
            assigned.at<uchar>(static_cast<int>(y), x) = 255;

        }
    });

    // Rellenar píxeles sin asignar
    cv::Mat dil;
    cv::dilate(assigned, dil, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
    tl::parallel_for(0, rows, [&](size_t y) {
        for (int x = 0; x < cols; ++x) {
            if (assigned.at<uchar>(static_cast<int>(y), x) || !dil.at<uchar>(static_cast<int>(y), x)) continue;

            float bestScore = std::numeric_limits<float>::infinity();
            int bestIdx = -1;
            for (size_t i = 0; i < images_size; ++i) {
                if (!valid_masks[i].at<uchar>(static_cast<int>(y), x)) continue;
                float score = weight_distance * normalized_distances[i];
                if (score < bestScore) {
                    bestScore = score;
                    bestIdx = static_cast<int>(i);
                }
            }

            if (bestIdx >= 0) {
                result.at<float>(static_cast<int>(y), x) = images[bestIdx].at<float>(static_cast<int>(y), x);
            }
        }
    });

    return result;
}

OrthophotoTask::OrthophotoTask(const std::unordered_map<size_t, Image> &images, 
                               const std::map<int, Camera> &cameras, 
                               const tl::Path &orthoPath, 
                               const tl::Path &mdt, 
                               const std::string &enuCrs, 
                               const std::string &epsg, 
                               const std::string &interpolation, 
                               double resolution, 
                               bool cuda)
  : tl::TaskBase(),
    mPhotos(images),
    mCameras(cameras),
    mOrthoPath(orthoPath),
    mMdt(mdt),
    mEnuCrs(enuCrs),
    mEpsg(epsg),
    mInterpolation(interpolation),
    mGSD(resolution),
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
        
        auto ortho_dir = mOrthoPath.parentPath();

        tl::Path temp_path(ortho_dir);
        temp_path.append("temp");
        temp_path.createDirectories();

        // Se tiene que comprobar que todas las imagenes tenga los mismos canales y profundidad de bits
        for (const auto &photo : mPhotos) {

            const auto &image = photo.second;
            int camera_id = image.cameraId();

            auto image_reader = tl::ImageReaderFactory::create(image.path().toStdString());
            image_reader->open();
            if (!image_reader->isOpen()) continue;

            mDataType = image_reader->dataType();
            mChannels = image_reader->channels();
            if (mChannels == 3) break;
        }

        tl::Path footprint_file(ortho_dir);
        footprint_file.append("footprint.shp");
        tl::Path graph_orthos = tl::Path(footprint_file).replaceBaseName("graph_orthos");

        tl::Path dsm_path = mMdt;
        dsm_path.replaceBaseName("dsm_enu");

        OrthoimageTask orthoimage_task(mPhotos,
                                       mCameras,
                                       dsm_path,
                                       temp_path,
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

        tl::Path::removeDirectory(temp_path);

        if (progressBar) (*progressBar)();

        mOrthophotoReport.time = this->time();
        mOrthophotoReport.gsd = mGSD;
        mOrthophotoReport.epsg = QString::fromStdString(mEpsg);
        mOrthophotoReport.cols = static_cast<int>(std::round(mWindowAll.width() / mGSD));
        mOrthophotoReport.rows = static_cast<int>(std::round(mWindowAll.height() / mGSD));
        mOrthophotoReport.channels = mChannels;

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
    try {

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
#ifndef FAST_ORTHO
                // Solo expandir en el caso de la generación de mosaico con fusión. Si se hace con combineImagesSmart no haría falta
                window = tl::expandWindow(window, ortho_overlap * mGSD);
#endif

                int image_size = tl::numberCast<int>(window.width() / mGSD);

                // Todas las imagenes del elemento actual del grid
                std::vector<cv::Mat> images;
                std::vector<double> distances;

                for (auto &ortho : orthos[r][c]) {

                    try {

                        auto image_reader = tl::ImageReaderFactory::create(ortho.second);
                        image_reader->open();
                        if (!image_reader->isOpen()) {
                            tl::Message::error("Image open error :{}", ortho.second);
                            continue;
                        }

                        auto georef = image_reader->georeference().inverse();
                        tl::Window<tl::Point<int>> window_image;
                        window_image.pt1 = georef.transform(window.pt1);
                        window_image.pt2 = georef.transform(window.pt2);
                        window_image.normalized();
                        tl::Rect<int> rect_image(window_image.pt1.x, window_image.pt1.y, window_image.width(), window_image.height());
                        tl::Rect<int> rect_full_image(0, 0, image_reader->cols(), image_reader->rows());
                        tl::Rect<int> rect_to_read = tl::intersect(rect_full_image, rect_image);
                        if (!rect_to_read.isValid()) continue;

                        auto image = image_reader->read(1., 1., rect_to_read);
                        auto data_type = image_reader->dataType();
                        image_reader->close();


                        if (image.rows != image_size || image.cols != image_size) {

                            cv::Scalar no_data;
                            if (image.type() == CV_32F || image.type() == CV_64F)
                                no_data = tl::NoData<float>;
                            else
                                no_data = 0;

                            cv::Mat aux = cv::Mat(image_size, image_size, image.type(), no_data);

                            auto offset = rect_to_read.topLeft() - rect_image.topLeft();
                            cv::Rect roi(offset.x, offset.y, image.cols, image.rows);
                            cv::Mat image_roi = aux(roi);
                            image.copyTo(image_roi);
                            image = aux;
                        }

                        images.push_back(image);
                        distances.push_back(ortho.first);
                    
                    } catch (std::exception &e) {
                        tl::Message::error("Window = [{}, {}, {}, {}]", window.pt1.x, window.pt1.y, window.pt2.x, window.pt2.y);

                        tl::printException(e);
                    }
                }

                if (images.empty()) continue;

                cv::Mat read_image;
                if (images.size() == 1) {
                    read_image = images[0].clone();
                } else {
#ifdef FAST_ORTHO

                    if (mChannels == 1) {
                        read_image = combineImagesSmartMono(images, distances);
                    } else if (mChannels == 3) {
                        read_image = combineImagesSmart(images, distances);
                    } else {
                        /// TODO: Contemplar otros casos
                    }

#else
                    read_image = combineImages(images);
#endif
                }

                if (read_image.empty()) continue;

                try {

                    tl::Path tile(mOrthoPath.parentPath());
                    tile.append("temp");
                    tile.append(std::to_string(r));
                    tile.append(std::to_string(c));
                    tile.createDirectories();
                    tile.append("t.tif");

                    auto image_writer = tl::ImageWriterFactory::create(tile);
                    image_writer->open();

                    image_writer->create(read_image.rows, read_image.cols, read_image.channels(), mDataType);
                    tl::Crs crs(mEpsg);
                    image_writer->setCRS(crs.toWktFormat());
                    tl::Affine<double, 2> affine_ortho(mGSD, -mGSD, window.pt1.x, window.pt2.y, 0.0);
                    image_writer->setGeoreference(affine_ortho);
                    if (mDataType == tl::DataType::TL_32F || mDataType == tl::DataType::TL_64F)
                        image_writer->setNoDataValue(tl::NoData<float>);
                    image_writer->write(read_image);
                    image_writer->close();

                } catch (std::exception &e) {
                    tl::printException(e);
                }
            }
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

#ifdef FAST_ORTHO

void OrthophotoTask::writeOrthomosaic(const std::vector<std::vector<tl::WindowD>> &grid)
{

    try {

        tl::Message::info("Writing ortho image");

        //tl::Path ortho_final(mOrthoPath);
        //ortho_final.append("ortho.tif");
        std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(mOrthoPath);
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

            if (mDataType == tl::DataType::TL_32F || mDataType == tl::DataType::TL_64F)
                image_writer->setNoDataValue(tl::NoData<float>);

            for (size_t r = 0; r < grid.size(); r++) {
                for (size_t c = 0; c < grid[r].size(); c++) {

                    try {
                        
                        tl::Path tile(mOrthoPath.parentPath());
                        tile.append("temp");
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

        //tl::Path ortho_final(mOrthoPath);
        //ortho_final.append("ortho.tif");
        std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(mOrthoPath);
        image_writer->open();

        int cols = static_cast<int>(std::round(mWindowAll.width() / mGSD));
        int rows = static_cast<int>(std::round(mWindowAll.height() / mGSD));

        auto options = std::make_shared<tl::TiffOptions>();
        options->enableTiled(true);
        options->setBigTiff(tl::TiffOptions::BigTiff::if_needed);
        options->setCompress(tl::TiffOptions::Compress::lzw);

        if (!image_writer->isOpen()) {
            tl::Message::error("Cannot open image writer for {}", mOrthoPath.toUtf8());
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

                        tl::Path tile(mOrthoPath.parent());
                        tile.append("temp");
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
