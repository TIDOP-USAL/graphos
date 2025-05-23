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

/* OpenCV */
#include <opencv2/stitching.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>
#include <tidop/geospatial/crstransf.h>


namespace graphos
{

constexpr double exposure_compensator_factor = 0.1;

//std::vector<tl::WindowD> findGrid(const tl::Path &footprint_file)
//{
//    std::vector<tl::WindowD> grid;
//    ///
//    std::vector<std::shared_ptr<tl::GPolygon>> grid_to_save;
//    ///
//    tl::WindowD window_all;
//
//    std::unique_ptr<tl::VectorReader> vectorReader = tl::VectorReaderFactory::create(footprint_file);
//    vectorReader->open();
//    if (vectorReader->isOpen()) {
//        if (vectorReader->layersCount() >= 1) {
//
//            std::map<double, std::shared_ptr<tl::GPolygon>> entities;
//            std::shared_ptr<tl::GLayer> layer = vectorReader->read(0);
//
//            /// Busqueda ventana total
//
//            double grid_step = -1;
//
//            for (const auto &entity : *layer) {
//                std::shared_ptr<tl::GPolygon> polygon = std::dynamic_pointer_cast<tl::GPolygon>(entity);
//                std::shared_ptr<tl::TableRegister> data = polygon->data();
//                tl::WindowD window = polygon->window();
//                // Esto lo estoy recalculando cada vez...
//                grid_step = std::min(window.width(), window.height());
//                window_all = tl::joinWindow(window_all, window);
//            }
//
//            grid_step /= 3.;
//
//            int grid_horizontal_size = static_cast<int>(window_all.width() / grid_step);
//            int grid_vertical_size = static_cast<int>(window_all.height() / grid_step);
//
//            double x_ini = window_all.pt1.x + grid_step / 2.;
//            double y_ini = window_all.pt2.y - grid_step / 2.;
//
//            grid.emplace_back(tl::Point<double>(x_ini, y_ini), grid_step /** 2*/);
//            ///
//            std::shared_ptr<tl::GPolygon> polygon = std::make_shared<tl::GPolygon>();
//            auto x_min = grid.back().pt1.x;
//            auto y_min = grid.back().pt1.y;
//            auto x_max = grid.back().pt2.x;
//            auto y_max = grid.back().pt2.y;
//
//            polygon->push_back(tl::Point<double>(x_min, y_min));
//            polygon->push_back(tl::Point<double>(x_max, y_min));
//            polygon->push_back(tl::Point<double>(x_max, y_max));
//            polygon->push_back(tl::Point<double>(x_min, y_max));
//            grid_to_save.push_back(polygon);
//            ///
//
//            tl::Point<double> point;
//            for (size_t i = 0; i < grid_horizontal_size; i++) {
//
//                point.x = x_ini + grid_step * i;
//
//                for (size_t j = 0; j < grid_vertical_size; j++) {
//
//                    point.y = y_ini - grid_step * j;
//
//                    grid.emplace_back(point, grid_step /** 2*/);
//                    ///
//                    std::shared_ptr<tl::GPolygon> polygon = std::make_shared<tl::GPolygon>();
//                    auto x_min = grid.back().pt1.x;
//                    auto y_min = grid.back().pt1.y;
//                    auto x_max = grid.back().pt2.x;
//                    auto y_max = grid.back().pt2.y;
//
//                    polygon->push_back(tl::Point<double>(x_min, y_min));
//                    polygon->push_back(tl::Point<double>(x_max, y_min));
//                    polygon->push_back(tl::Point<double>(x_max, y_max));
//                    polygon->push_back(tl::Point<double>(x_min, y_max));
//                    grid_to_save.push_back(polygon);
//                    ///
//                }
//            }
//
//        }
//
//    }
//
//
//    ///
//    auto grid_file = footprint_file;
//    grid_file.replaceBaseName("grid");
//    std::unique_ptr<tl::VectorWriter> vector_writer = tl::VectorWriterFactory::create(grid_file);
//    vector_writer->open();
//    if (!vector_writer->isOpen())throw std::runtime_error("Vector open error");
//    vector_writer->create();
//    vector_writer->setCRS(tl::Crs("EPSG:").toWktFormat());
//
//    std::shared_ptr<tl::TableField> field(new tl::TableField("image",
//                                          tl::TableField::Type::STRING,
//                                          254));
//    std::vector<std::shared_ptr<tl::TableField>> fields;
//    fields.push_back(field);
//
//    tl::GLayer layer;
//    layer.setName("footprint");
//    layer.addDataField(field);
//
//    for (const auto &footprint : grid_to_save) {
//        std::shared_ptr<tl::TableRegister> data(new tl::TableRegister(fields));
//        //data->setValue(0, footprint.first);
//        layer.push_back(footprint);
//    }
//
//        vector_writer->write(layer);
//
//        vector_writer->close();
//    ///
//
//    return grid;
//}

std::vector<tl::WindowD> findGrid(const tl::Path &footprint_file, double gsd)
{
    std::vector<tl::WindowD> grid;

    tl::WindowD window_all;

    std::unique_ptr<tl::VectorReader> vectorReader = tl::VectorReaderFactory::create(footprint_file);
    vectorReader->open();
    if (vectorReader->isOpen()) {
        if (vectorReader->layersCount() >= 1) {

            std::map<double, std::shared_ptr<tl::GPolygon>> entities;
            std::shared_ptr<tl::GLayer> layer = vectorReader->read(0);

            /// Busqueda ventana total
            for (const auto &entity : *layer) {
                std::shared_ptr<tl::GPolygon> polygon = std::dynamic_pointer_cast<tl::GPolygon>(entity);
                std::shared_ptr<tl::TableRegister> data = polygon->data();
                tl::WindowD window = polygon->window();
                // Esto lo estoy recalculando cada vez...
                //grid_step = std::min(window.width(), window.height());
                window_all = tl::joinWindow(window_all, window);
            }

            double grid_step = 256 * gsd;

            int grid_horizontal_size = static_cast<int>(window_all.width() / grid_step);
            int grid_vertical_size = static_cast<int>(window_all.height() / grid_step);

            double x_ini = window_all.pt1.x + grid_step / 2.;
            double y_ini = window_all.pt2.y - grid_step / 2.;

            grid.emplace_back(tl::Point<double>(x_ini, y_ini), grid_step * 2);

            tl::Point<double> point;
            for (size_t i = 0; i < grid_horizontal_size; i++) {

                point.x = x_ini + grid_step * i;

                for (size_t j = 0; j < grid_vertical_size; j++) {

                    point.y = y_ini - grid_step * j;

                    grid.emplace_back(point, grid_step * 2);
                }
            }
        }
    }

    return grid;
}


std::shared_ptr<tl::GPolygon> bestImage(const tl::Point<double> &pt, std::shared_ptr<tl::GLayer> layer)
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

void findOptimalFootprint(const tl::Path &footprint_file,
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

cv::Mat combineImages(const std::vector<cv::Mat> &images)
{
    cv::Mat result = images[0].clone(); // Clona la imagen base como resultado

    for (size_t i = 1; i < images.size(); ++i) {
        cv::Mat mask;
        // Crear máscara para los píxeles negros en la imagen resultante
        cv::inRange(result, cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), mask);
        if (cv::countNonZero(mask) == 0)
            break;

        // Dilatación de la máscara para cubrir bordes y suavizar uniones
        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
        cv::dilate(mask, mask, element);
        // Solo copiar los píxeles de la imagen actual donde result es negro
        images[i].copyTo(result, mask);

    }

    return result;
}

//cv::Mat combineImages(const std::vector<cv::Mat> &images)
//{
//    cv::Mat result = images[0].clone(); // Clona la imagen base como resultado
//
//    for (size_t i = 1; i < images.size(); ++i) {
//        // Crear máscara para la imagen actual, detectando sus áreas negras
//        cv::Mat mask;
//        cv::inRange(images[i], cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), mask);
//
//        if (cv::countNonZero(mask) == 0)
//            break;
//
//        // Dilatación de la máscara para cubrir bordes y suavizar uniones
//        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
//        cv::dilate(mask, mask, element);
//        cv::GaussianBlur(mask, mask, cv::Size(5, 5), 0);
//
//        // Convertir la máscara a tipo flotante y normalizar para el blending
//        cv::Mat floatMask;
//        mask.convertTo(floatMask, CV_32FC1, 1.0 / 255.0);
//
//        // Convertir la imagen actual y la acumulada (result) a flotantes para el blending
//        cv::Mat floatResult, floatImage;
//        result.convertTo(floatResult, CV_32FC3);
//        images[i].convertTo(floatImage, CV_32FC3);
//
//        // Aplicar la máscara individual a la imagen actual
//        for (int c = 0; c < 3; c++) { // Iterar sobre los canales
//            floatImage.forEach<cv::Vec3f>(
//                [&](cv::Vec3f &pixel, const int pos[]) -> void {
//                    pixel[c] *= (1.0f - floatMask.at<float>(pos[0], pos[1]));
//                }
//            );
//        }
//
//        // Realizar la mezcla de ambas imágenes usando la máscara
//        for (int c = 0; c < 3; c++) { // Iterar sobre los canales de color
//            floatResult.forEach<cv::Vec3f>(
//                [&](cv::Vec3f &pixel, const int pos[]) -> void {
//                    pixel[c] = pixel[c] * floatMask.at<float>(pos[0], pos[1]) +
//                        floatImage.at<cv::Vec3f>(pos[0], pos[1])[c];
//                }
//            );
//        }
//
//        // Convertir el resultado de vuelta a CV_8UC3 y actualizar el resultado acumulado
//        floatResult.convertTo(result, CV_8UC3);
//    }
//
//    return result;
//}

void orthoMosaic(const tl::Path &graph_orthos,
                 const tl::Path &ortho_path, 
                 double res_ortho, 
                 const tl::Crs &crs, 
                 const std::vector<std::vector<tl::WindowD>> &grid)
{
    try {

        tl::WindowD window_all;
        std::vector<std::vector<std::map<double,std::string>>> orthos(grid.size());

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
                    window_all = joinWindow(window_all, window);
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
                            //if (dist < min_distance) {
                            //    min_distance = dist;
                            //    orthos[r][c] = polygon->data()->value(0);
                            //}
                            orthos[r][c][dist] = polygon->data()->value(0);
                        }

                    }

                }
            }
        }
        

        /// Escritura de los tiles con solape


        for (size_t r = 0; r < grid.size(); r++) {
            for (size_t c = 0; c < grid[r].size(); c++) {

                const auto &window = grid[r][c];

                // Tamaño de imagen
                auto window_aux = tl::expandWindow(window, 50 * res_ortho);
                int image_size = tl::numberCast<int>(window_aux.width() / res_ortho);

                //tl::WindowD window_to_read;
                
                // Todas las imagenes del elemento actual del grid
                std::vector<cv::Mat> images;

                for (auto &ortho : orthos[r][c]) {

                    auto image_reader = tl::ImageReaderFactory::create(ortho.second/*orthos[r][c]*/);
                    image_reader->open();
                    if (!image_reader->isOpen()) {
                        tl::Message::error("Image open error :{}", ortho.second/*orthos[r][c]*/);
                        continue;
                    }

                    // Por ahora solo se utilizan las imagenes que contienen el grid. Se omiten las que intersectan
                    if (!image_reader->window().containsWindow(window)) continue;

                    
                    //auto _window_to_read = tl::windowIntersection(image_reader->window(), window_aux);

                    tl::Affine<int, 2> affine;
                    auto image = image_reader->read(window_aux, 1., 1., &affine);
                    image_reader->close();

                    if (image.rows != image_size || image.cols != image_size) {
                        cv::Mat aux = cv::Mat::zeros(image_size, image_size, image.type());
                        
                        cv::Rect roi(affine.translation().x(), affine.translation().y(), image.cols, image.rows);
                        cv::Mat image_roi = aux(roi);
                        image.copyTo(image_roi);
                        image = aux;
                    }

                    images.push_back(image);

                    /////
                    // Por ahora sigo escribiendo todo

                    try {
                        tl::Path tile(ortho_path);
                        tile.append(std::to_string(r));
                        tile.append(std::to_string(c));
                        tile.createDirectories();
                        tl::Path image_name(ortho.second);

                        tile.append(image_name.fileName().toString());
                        //if (!tile.exists()) continue;
                        auto image_writer = tl::ImageWriterFactory::create(tile);
                        image_writer->open();
                        int cols = static_cast<int>(std::round(window_aux.width() / res_ortho));
                        int rows = static_cast<int>(std::round(window_aux.height() / res_ortho));

                        image_writer->create(rows, cols, 3, tl::DataType::TL_8U);
                        image_writer->setCRS(crs.toWktFormat());
                        tl::Affine<double, 2> affine_ortho(res_ortho, -res_ortho, window_aux.pt1.x, window_aux.pt2.y, 0.0);
                        image_writer->setGeoreference(affine_ortho);
                        image_writer->write(image);
                        image_writer->close();

                    } catch (std::exception &e) {
                        tl::printException(e);
                    }
                    ////
                }

                if (images.empty()) continue;

                cv::Mat read_image;
                if (images.size() == 1) {
                    read_image = images[0].clone();
                } else {
                    read_image = combineImages(images);
                }
                

                // Best image
                //auto &ortho_it = orthos[r][c].begin();
                //{
                //    try {
                //        auto image_reader = tl::ImageReaderFactory::create(ortho_it->second/*orthos[r][c]*/);
                //        image_reader->open();
                //        if (!image_reader->isOpen()) {
                //            tl::Message::error("Image open error :{}", ortho_it->second/*orthos[r][c]*/);
                //            continue;
                //        }

                //        if (!intersectWindows(image_reader->window(), window)) continue;

                //        auto window_aux = tl::expandWindow(window, 50 * res_ortho);
                //        window_to_read = tl::windowIntersection(image_reader->window(), window_aux);

                //        read_image = image_reader->read(window_to_read);
                //        image_reader->close();
                //    } catch (std::exception &e) {
                //        tl::printException(e);
                //    }
                //}

                //Imagen con menos pixeles negros
                //int numBlackPixels = std::numeric_limits<int>::max();
                //for (auto &ortho : orthos[r][c]) {
                //    auto image_reader = tl::ImageReaderFactory::create(ortho.second/*orthos[r][c]*/);
                //    image_reader->open();
                //    if (!image_reader->isOpen()) {
                //        tl::Message::error("Image open error :{}", ortho.second/*orthos[r][c]*/);
                //        continue;
                //    }

                //    if (!image_reader->window().containsWindow(window)) continue;
                //    //if (!intersectWindows(image_reader->window(), window)) continue;

                //    auto window_aux = tl::expandWindow(window, 50 * res_ortho);
                //    auto _window_to_read = tl::windowIntersection(image_reader->window(), window_aux);

                //    auto image = image_reader->read(_window_to_read);
                //    image_reader->close();

                //    cv::Mat gray;
                //    if (image.channels() != 1) {
                //        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
                //    } else {
                //        gray = image.clone();
                //    }
                //    auto image_pixels = image.cols * image.rows;
                //    auto max_image_pixels = 255 * 255/*355 * 355*/;
                //    auto black_pixels = max_image_pixels - image_pixels;
                //    cv::Mat blackMask = (gray == 0);
                //    int _numBlackPixels = black_pixels + cv::countNonZero(blackMask);
                //    if (_numBlackPixels < numBlackPixels) {
                //        read_image = image.clone();
                //        numBlackPixels = _numBlackPixels;
                //        window_to_read = _window_to_read;
                //    }
                //}

                if (read_image.empty()) continue;

                /// Si la imagen está incompleta se rellena

                ////cv::Mat blackPixelMask = createBlackPixelMask(read_image, 1024, true);
                //cv::Mat gray;
                //if (read_image.channels() != 1) {
                //    cv::cvtColor(read_image, gray, cv::COLOR_BGR2GRAY);
                //} else {
                //    gray = read_image.clone();
                //}

                //// Create an initial mask for black pixels (value 0)
                //cv::Mat blackMask = (gray == 0);

                //// Find connected components to identify areas
                //cv::Mat labels, stats, centroids;
                //int numComponents = cv::connectedComponentsWithStats(blackMask, labels, stats, centroids);

                //// Create a new mask to store the valid black pixels
                ////cv::Mat finalMask = cv::Mat::zeros(gray.size(), CV_8U);

                //// Iterate through each component
                ////for (int i = 1; i < numComponents; i++) {  // Start from 1 to ignore the background
                //for (int i = 0; i < numComponents; i++) {
                //    int area = stats.at<int>(i+1, cv::CC_STAT_AREA);
                //    if (area > 1024) {
                //        cv::Mat componentMask = (labels == i+1);
                //        //finalMask |= componentMask;
                //        auto it = ortho_it;
                //        it++;
                //        for (; it != orthos[r][c].end(); it++) {

                //            tl::ImageReader::Ptr image_reader2 = tl::ImageReaderFactory::create(it->second);
                //            image_reader2->open();
                //            if (!image_reader2->isOpen()) {
                //                tl::Message::error("Image open error :{}", it->second);
                //                continue;
                //            }

                //            if (!intersectWindows(image_reader2->window(), window)) continue;

                //            auto window_aux = tl::expandWindow(window, 50 * res_ortho);
                //            auto window_to_read = tl::windowIntersection(image_reader2->window(), window_aux);

                //            cv::Mat second_image = image_reader2->read(window_to_read);
                //            image_reader2->close();

                //            if (second_image.size() != read_image.size()) continue;
                //            //cv::Mat blackPixelMask = createBlackPixelMask(read_image, 1024, true);
                //            cv::Mat finalMask = componentMask.clone();
                //            finalMask |= blackMask;

                //            // Se tiene que comprobar solo en el area de la componente
                //            // La mascara se establece sobre los pixeles negros asi que compruebo lo puntos que son mascara

                //            // Tengo que recalcular la mascara a la zona en concreto que se quiere clonar.
                //            // De esta forma se evita que se clonen trozos negros de imagen.
                //            int numBlackPixels = /*finalMask.size().area() -*/ cv::countNonZero(finalMask);
                //            if (finalMask.size().area() == numBlackPixels) continue;
                //            if (numBlackPixels < 512/*1024*/) {
                //                break;
                //            } else {
                //                try {
                //                    cv::Mat result;
                //                    cv::seamlessClone(second_image, read_image, componentMask, 
                //                                      cv::Point(read_image.cols / 2, read_image.rows / 2), result, cv::NORMAL_CLONE);
                //                    read_image = result.clone();
                //                    blackMask = finalMask.clone();
                //                } catch (std::exception &e) {
                //                    tl::printException(e);
                //                }
                //            }
                //        }
                //    }

                //    //for (; ortho_it != orthos[r][c].end(); ortho_it++) {
                //    //    auto image_reader = tl::ImageReaderFactory::create(ortho_it->second);

                //    //    if (!image_reader->isOpen()) {
                //    //        tl::Message::error("Image open error :{}", ortho_it->second);
                //    //        continue;
                //    //    }

                //    //    if (!intersectWindows(image_reader->window(), window)) continue;

                //    //    auto window_aux = tl::expandWindow(window, 50 * res_ortho);
                //    //    auto window_to_read = tl::windowIntersection(image_reader->window(), window_aux);

                //    //    cv::Mat second_image = image_reader->read(window_to_read);
                //    //    image_reader->close();
                //    //    cv::Mat blackPixelMask = createBlackPixelMask(read_image, 1024, true);

                //    //}

                //}

                try {
                    tl::Path tile(ortho_path);
                    tile.append(std::to_string(r));
                    tile.append(std::to_string(c));
                    tile.createDirectories();
                    tile.append("t.tif");
                    //if (!tile.exists()) continue;
                    auto image_writer = tl::ImageWriterFactory::create(tile);
                    image_writer->open();
                    //int cols = static_cast<int>(std::round(window.width() / res_ortho));
                    //int rows = static_cast<int>(std::round(window.height() / res_ortho));

                    image_writer->create(read_image.rows, read_image.cols, 3, tl::DataType::TL_8U);
                    image_writer->setCRS(crs.toWktFormat());
                    tl::Affine<double, 2> affine_ortho(res_ortho, -res_ortho, window_aux.pt1.x, window_aux.pt2.y, 0.0);
                    image_writer->setGeoreference(affine_ortho);
                    image_writer->write(read_image);
                    image_writer->close();
                } catch (std::exception &e) {
                    tl::printException(e);
                }
            }
        }

        /// Escritura de la orto
        tl::Path ortho_final(ortho_path);
        ortho_final.append("ortho.tif");
        std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(ortho_final);
        image_writer->open();
        int cols = static_cast<int>(std::round(window_all.width() / res_ortho));
        int rows = static_cast<int>(std::round(window_all.height() / res_ortho));

        if (image_writer->isOpen()) {
            image_writer->create(rows, cols, 3, tl::DataType::TL_8U);
            image_writer->setCRS(crs.toWktFormat());
            tl::Affine<double, 2> affine_ortho(res_ortho, -res_ortho, window_all.pt1.x, window_all.pt2.y, 0.0);
            image_writer->setGeoreference(affine_ortho);

            for (size_t r = 0; r < grid.size(); r++) {
                for (size_t c = 0; c < grid[r].size(); c++) {

                    try {
                        const auto &window = grid[r][c];
                        tl::Path tile(ortho_path);
                        tile.append(std::to_string(r));
                        tile.append(std::to_string(c));
                        tile.append("t.tif");
                        if (!tile.exists()) continue;
                        auto image_reader = tl::ImageReaderFactory::create(tile/*orthos[r][c].begin()->second*/);
                        image_reader->open();
                        if (!image_reader->isOpen()) {
                            tl::Message::error("Image open error :{}", orthos[r][c].begin()->second);
                            continue;
                        }

                        auto tile_window = image_reader->window();

                        if (!intersectWindows(tile_window, window) /*||
                            !intersectWindows(image_reader_seam->window(), window)*/) continue;

                        auto georef = image_reader->georeference();

                        //double scale_x = georef.scale().x();
                        //double scale_y = georef.scale().y();
                        //double read_scale_x = scale_x / res_ortho;
                        //double read_scale_y = scale_y / res_ortho;

                        //auto inverse_transform = georef.inverse();
                        //tl::Point<double> p1 = inverse_transform.transform(window.pt1);
                        //tl::Point<double> p2 = inverse_transform.transform(window.pt2);
                        //tl::WindowI window_to_read(static_cast<tl::Point<int>>(p1), static_cast<tl::Point<int>>(p2));
                        //window_to_read.normalized();

                        //tl::Affine<int, 2> affine;
                        //cv::Mat compensate_image = image_reader->read(window, read_scale_x, read_scale_y, &affine);
                        // Leo toda la imagen asi que lo anterior no tiene sentido
                        cv::Mat compensate_image = image_reader->read();
                        // Relleno de pixeles negros
                        cv::Mat blackPixelMask = createBlackPixelMask(compensate_image, 1024);
                        cv::inpaint(compensate_image, blackPixelMask, compensate_image, 3, cv::INPAINT_TELEA);

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

    } catch(std::exception &e){
        tl::printException(e);
    }
}

void orthoMosaicWithExposureCompensator(const tl::Path &graph_orthos,
                                        const tl::Path &ortho_path, 
                                        double res_ortho, 
                                        const tl::Crs &crs, 
                                        const std::vector<std::vector<tl::WindowD>> &grid)
{

    try {

        tl::Message::info("Exposure compensator");

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

                                //if (polygon2->isInner(polygon->at(0)) || 
                                //    polygon2->isInner(polygon->at(1)) || 
                                //    polygon2->isInner(polygon->at(2)) || 
                                //    polygon2->isInner(polygon->at(3))) {
                                /// No se si será suficiente o tengo que seleccionar todas las imagenes que intersecten...
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
                            std::string name = orto_compensate.baseName().toUtf8() + "_compensate.png";
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
                                //compensated_orthos.push_back(orto_compensate.toString());
                            }
                        }
                    }
                }
            }
        }
    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    try {

        tl::WindowD window_all;
        std::vector<std::vector<std::map<double,std::string>>> orthos(grid.size());

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
                    window_all = joinWindow(window_all, window);
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
                            std::string name = orto_compensate.baseName().toUtf8() + "_compensate.png";
                            orto_compensate.replaceFileName(name);
                            orthos[r][c][dist] = orto_compensate.toUtf8();
                        }

                    }

                }
            }
        }
        

        /// Escritura de los tiles con solape


        for (size_t r = 0; r < grid.size(); r++) {
            for (size_t c = 0; c < grid[r].size(); c++) {

                const auto &window = grid[r][c];

                // Tamaño de imagen
                auto window_aux = tl::expandWindow(window, 50 * res_ortho);
                int image_size = tl::numberCast<int>(window_aux.width() / res_ortho);

                //tl::WindowD window_to_read;
                
                // Todas las imagenes del elemento actual del grid
                std::vector<cv::Mat> images;

                //tl::Message::info("Tile: {}{}", r, c);

                for (auto &ortho : orthos[r][c]) {

                    tl::Message::info("Imagen: {}", ortho.second);
                    tl::Message::info("Distancia mejor imagen: {}", ortho.first);

                    auto image_reader = tl::ImageReaderFactory::create(ortho.second/*orthos[r][c]*/);
                    image_reader->open();
                    if (!image_reader->isOpen()) {
                        tl::Message::error("Image open error :{}", ortho.second/*orthos[r][c]*/);
                        continue;
                    }

                    // Por ahora solo se utilizan las imagenes que contienen el grid. Se omiten las que intersectan
                    if (!image_reader->window().containsWindow(window)) continue;

                    tl::Affine<int, 2> affine;
                    auto image = image_reader->read(window_aux, 1., 1., &affine);
                    image_reader->close();

                    if (image.rows != image_size || image.cols != image_size) {
                        cv::Mat aux = cv::Mat::zeros(image_size, image_size, image.type());
                        
                        cv::Rect roi(affine.translation().x(), affine.translation().y(), image.cols, image.rows);
                        cv::Mat image_roi = aux(roi);
                        image.copyTo(image_roi);
                        image = aux;
                    }

                    images.push_back(image);

                    /////
                    // Por ahora sigo escribiendo todo

                    try {
                        tl::Path tile(ortho_path);
                        tile.append(std::to_string(r));
                        tile.append(std::to_string(c));
                        tile.createDirectories();
                        tl::Path image_name(ortho.second);

                        tile.append(image_name.fileName().toUtf8());

                        auto image_writer = tl::ImageWriterFactory::create(tile);
                        image_writer->open();
                        int cols = static_cast<int>(std::round(window_aux.width() / res_ortho));
                        int rows = static_cast<int>(std::round(window_aux.height() / res_ortho));

                        image_writer->create(rows, cols, 3, tl::DataType::TL_8U);
                        image_writer->setCRS(crs.toWktFormat());
                        tl::Affine<double, 2> affine_ortho(res_ortho, -res_ortho, window_aux.pt1.x, window_aux.pt2.y, 0.0);
                        image_writer->setGeoreference(affine_ortho);
                        image_writer->write(image);
                        image_writer->close();

                    } catch (std::exception &e) {
                        tl::printException(e);
                    }
                    ////
                }

                if (images.empty()) continue;

                cv::Mat read_image;
                if (images.size() == 1) {
                    read_image = images[0].clone();
                } else {
                    read_image = combineImages(images);
                }


                if (read_image.empty()) continue;

                try {
                    tl::Path tile(ortho_path);
                    tile.append(std::to_string(r));
                    tile.append(std::to_string(c));
                    tile.createDirectories();
                    tile.append("t.tif");
                    //if (!tile.exists()) continue;
                    auto image_writer = tl::ImageWriterFactory::create(tile);
                    image_writer->open();
                    //int cols = static_cast<int>(std::round(window.width() / res_ortho));
                    //int rows = static_cast<int>(std::round(window.height() / res_ortho));

                    image_writer->create(read_image.rows, read_image.cols, 3, tl::DataType::TL_8U);
                    image_writer->setCRS(crs.toWktFormat());
                    tl::Affine<double, 2> affine_ortho(res_ortho, -res_ortho, window_aux.pt1.x, window_aux.pt2.y, 0.0);
                    image_writer->setGeoreference(affine_ortho);
                    image_writer->write(read_image);
                    image_writer->close();
                } catch (std::exception &e) {
                    tl::printException(e);
                }
            }
        }

        tl::Message::info("Writing ortho image");

        /// Escritura de la orto
        try {

            tl::Path ortho_final(ortho_path);
            ortho_final.append("ortho.tif");
            std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(ortho_final);
            image_writer->open();
            int cols = static_cast<int>(std::round(window_all.width() / res_ortho));
            int rows = static_cast<int>(std::round(window_all.height() / res_ortho));

            if (image_writer->isOpen()) {
                image_writer->create(rows, cols, 3, tl::DataType::TL_8U);
                image_writer->setCRS(crs.toWktFormat());
                tl::Affine<double, 2> affine_ortho(res_ortho, -res_ortho, window_all.pt1.x, window_all.pt2.y, 0.0);
                image_writer->setGeoreference(affine_ortho);

                for (size_t r = 0; r < grid.size(); r++) {
                    for (size_t c = 0; c < grid[r].size(); c++) {

                        try {
                            const auto &window = grid[r][c];
                            tl::Path tile(ortho_path);
                            tile.append(std::to_string(r));
                            tile.append(std::to_string(c));
                            tile.append("t.tif");
                            if (!tile.exists()) continue;
                            auto image_reader = tl::ImageReaderFactory::create(tile/*orthos[r][c].begin()->second*/);
                            image_reader->open();
                            if (!image_reader->isOpen()) {
                                tl::Message::error("Image open error :{}", orthos[r][c].begin()->second);
                                continue;
                            }

                            auto tile_window = image_reader->window();

                            if (!intersectWindows(tile_window, window) /*||
                                !intersectWindows(image_reader_seam->window(), window)*/) continue;

                            auto georef = image_reader->georeference();

                            //double scale_x = georef.scale().x();
                            //double scale_y = georef.scale().y();
                            //double read_scale_x = scale_x / res_ortho;
                            //double read_scale_y = scale_y / res_ortho;

                            //auto inverse_transform = georef.inverse();
                            //tl::Point<double> p1 = inverse_transform.transform(window.pt1);
                            //tl::Point<double> p2 = inverse_transform.transform(window.pt2);
                            //tl::WindowI window_to_read(static_cast<tl::Point<int>>(p1), static_cast<tl::Point<int>>(p2));
                            //window_to_read.normalized();

                            //tl::Affine<int, 2> affine;
                            //cv::Mat compensate_image = image_reader->read(window, read_scale_x, read_scale_y, &affine);
                            // Leo toda la imagen asi que lo anterior no tiene sentido
                            cv::Mat compensate_image = image_reader->read();
                            // Relleno de pixeles negros
                            cv::Mat blackPixelMask = createBlackPixelMask(compensate_image, 1024);
                            cv::inpaint(compensate_image, blackPixelMask, compensate_image, 3, cv::INPAINT_TELEA);

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

    } catch(std::exception &e){
        tl::printException(e);
    }
}

void orthoMosaic(tl::Path &optimal_footprint_path,
                 tl::Path &ortho_path,
                 double res_ortho,
                 tl::Crs &crs,
                 std::vector<tl::WindowD> &grid)
{
    std::vector<std::string> compensated_orthos;
    std::vector<std::string> ortho_seams;
    std::vector<cv::Point> corners;
    std::vector<cv::Mat> mat_orthos;
    std::vector<cv::UMat> umat_orthos;
    std::vector<cv::UMat> ortho_masks;
    tl::WindowD window_all;

    tl::Message::info("Exposure compensator");

    //int type = cv::detail::ExposureCompensator::NO;
    int type = cv::detail::ExposureCompensator::GAIN;
    //int type = cv::detail::ExposureCompensator::GAIN_BLOCKS;
    //int type = cv::detail::ExposureCompensator::CHANNELS;
    //int type = cv::detail::ExposureCompensator::CHANNELS_BLOCKS;
    cv::Ptr<cv::detail::ExposureCompensator> compensator = cv::detail::ExposureCompensator::createDefault(type);

    std::unique_ptr<tl::VectorReader> vectorReader;
    vectorReader = tl::VectorReaderFactory::create(optimal_footprint_path.toString());
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

                            //if (polygon2->isInner(polygon->at(0)) || 
                            //    polygon2->isInner(polygon->at(1)) || 
                            //    polygon2->isInner(polygon->at(2)) || 
                            //    polygon2->isInner(polygon->at(3))) {
                            /// No se si será suficiente o tengo que seleccionar todas las imagenes que intersecten...
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
                    }

                    cv::InputArrayOfArrays(mat_orthos).getUMatVector(umat_orthos);
                    compensator->feed(corners, umat_orthos, ortho_masks);

                    tl::Message::info("Seam finder");

                    cv::Ptr<cv::detail::SeamFinder> seam_finder;
                    seam_finder = cv::makePtr<cv::detail::NoSeamFinder>();
                    //seam_finder = cv::makePtr<cv::detail::VoronoiSeamFinder>();
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
                        cv::Mat element = getStructuringElement(cv::MorphShapes::MORPH_RECT,
                                                                cv::Size(2 * 2 + 1, 2 * 2 + 1),
                                                                cv::Point(2, 2));
                        //Revisar
                        //cv::erode(mask_full_size, mask_full_size, element);
                        //cv::dilate(mask_full_size, mask_full_size, element);
                        compensator->apply(0, corner, compensate_image, mask_full_size);

                        tl::Path orto_compensate(ortho_to_compensate);
                        std::string name = orto_compensate.baseName().toString() + "_compensate.png";
                        orto_compensate.replaceFileName(name);
                        std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(orto_compensate.toString());
                        image_writer->open();
                        if (image_writer->isOpen()) {
                            image_writer->create(image_reader->rows(), image_reader->cols(), image_reader->channels(), image_reader->dataType());
                            image_writer->setCRS(image_reader->crsWkt());
                            image_writer->setGeoreference(image_reader->georeference());
                            image_writer->write(compensate_image);
                            image_writer->close();
                            tl::Message::info("Image Compensate: {}", orto_compensate.fileName().toString());
                            compensated_orthos.push_back(orto_compensate.toString());
                        }

                        /// 2 - Busqueda de costuras (seam finder)

                        cv::Mat mask_finder = ortho_masks[0].getMat(cv::ACCESS_READ);
                        /// revisar
                        //cv::erode(mask_finder, mask_finder, element);
                        cv::resize(mask_finder, mask_finder, compensate_image.size());
                        mask_finder = mask_finder & mask_full_size;

                        tl::Path orto_seam(ortho_to_compensate);
                        name = orto_seam.baseName().toString() + "_seam.tif";
                        orto_seam.replaceFileName(name);
                        image_writer = tl::ImageWriterFactory::create(orto_seam.toString());
                        image_writer->open();
                        if (image_writer->isOpen()) {
                            image_writer->create(image_reader->rows(), image_reader->cols(), 1, image_reader->dataType());
                            image_writer->setCRS(image_reader->crsWkt());
                            image_writer->setGeoreference(image_reader->georeference());
                            image_writer->write(mask_finder);
                            image_writer->close();
                            tl::Message::info("Image seam: {}", orto_seam.fileName().toString());
                            ortho_seams.push_back(orto_seam.toString());
                        }

                        image_reader->close();
                    }

                } else {
                    tl::Message::error("It is not a footprint file");
                    return;
                }

            }

        }

        vectorReader->close();
    }

    /// 3 - mezcla (blender)

    bool try_cuda = false;
    //int blender_type = cv::detail::Blender::FEATHER;
    int blender_type = cv::detail::Blender::MULTI_BAND;
    cv::Ptr<cv::detail::Blender> blender;
    float blend_strength = 5;

    tl::Path ortho_final(ortho_path);
    ortho_final.append("ortho.tif");
    std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(ortho_final.toString());
    image_writer->open();
    int cols = static_cast<int>(std::round(window_all.width() / res_ortho));
    int rows = static_cast<int>(std::round(window_all.height() / res_ortho));

    if (image_writer->isOpen()) {
        image_writer->create(rows, cols, 3, tl::DataType::TL_8U);
        image_writer->setCRS(crs.toWktFormat());
        tl::Affine<double, 2> affine_ortho(res_ortho, -res_ortho, window_all.pt1.x, window_all.pt2.y, 0.0);
        image_writer->setGeoreference(affine_ortho);

        for (size_t i = 0; i < grid.size(); i++) {

            blender = cv::detail::Blender::createDefault(blender_type, try_cuda);

            int cols_grid = static_cast<int>(std::round(grid[i].width() / res_ortho));
            int rows_grid = static_cast<int>(std::round(grid[i].height() / res_ortho));
            cv::Rect rect(0, 0, cols_grid, rows_grid);

            //cv::Size dst_sz = rect.size();
            float blend_width = sqrt(static_cast<float>(rect.area())) * blend_strength / 100.f;

            if (blend_width < 1.f) {
                blender = cv::detail::Blender::createDefault(cv::detail::Blender::NO, try_cuda);
            } else if (blender_type == cv::detail::Blender::MULTI_BAND) {
                cv::detail::MultiBandBlender *multi_band_blender = dynamic_cast<cv::detail::MultiBandBlender *>(blender.get());
                multi_band_blender->setNumBands(4/*static_cast<int>(ceil(log(blend_width) / log(2.)) - 1.)*/);
                tl::Message::info("Multi-band blender, number of bands: {}", multi_band_blender->numBands());
            } else if (blender_type == cv::detail::Blender::FEATHER) {
                cv::detail::FeatherBlender *feather_blender = dynamic_cast<cv::detail::FeatherBlender *>(blender.get());
                feather_blender->setSharpness(0.02f/*1.f / blend_width*/);
                tl::Message::info("Feather blender, sharpness: {}", feather_blender->sharpness());
            }

            blender->prepare(rect);

            for (size_t j = 0; j < compensated_orthos.size(); j++) {
                try {
                    std::unique_ptr<tl::ImageReader> image_reader = tl::ImageReaderFactory::create(compensated_orthos[j]);
                    std::unique_ptr<tl::ImageReader> image_reader_seam = tl::ImageReaderFactory::create(ortho_seams[j]);
                    image_reader->open();
                    image_reader_seam->open();
                    if (!image_reader->isOpen()) {
                        tl::Message::error("Image open error :{}", compensated_orthos[j]);
                        continue;
                    }
                    if (!image_reader_seam->isOpen()) {
                        tl::Message::error("Image open error :{}", ortho_seams[j]);
                        continue;
                    }

                    if (!intersectWindows(image_reader->window(), grid[i]) ||
                        !intersectWindows(image_reader_seam->window(), grid[i])) continue;

                    auto georef = image_reader->georeference();

                    double scale_x = georef.scale().x();
                    double scale_y = georef.scale().y();
                    double read_scale_x = scale_x / res_ortho;
                    double read_scale_y = scale_y / res_ortho;

                    auto inverse_transform = georef.inverse();
                    tl::Point<double> p1 = inverse_transform.transform(grid[i].pt1);
                    tl::Point<double> p2 = inverse_transform.transform(grid[i].pt2);
                    tl::WindowI window_to_read(static_cast<tl::Point<int>>(p1), static_cast<tl::Point<int>>(p2));
                    window_to_read.normalized();

                    tl::Affine<int, 2> affine;
                    cv::Mat compensate_image;
                    cv::Mat seam_image;

                    compensate_image = image_reader->read(grid[i], read_scale_x, read_scale_y, &affine);
                    seam_image = image_reader_seam->read(grid[i], read_scale_x, read_scale_y);


                    if (!compensate_image.empty() && !seam_image.empty()) {

                        tl::Message::info("Ortho grid {}: {}", i, compensated_orthos[j]);

                        cv::Mat compensate_image_16s;
                        compensate_image.convertTo(compensate_image_16s, CV_16S);
                        compensate_image.release();

                        cv::Rect rect = cv::Rect(tl::roundToInteger(affine.translation().x()),
                                                 tl::roundToInteger(affine.translation().y()),
                                                 compensate_image_16s.cols,
                                                 compensate_image_16s.rows);
                        blender->feed(compensate_image_16s, seam_image, rect.tl());
                    }

                } catch (...) {
                    continue;
                }
            }
            cv::Mat ortho_blend;
            cv::Mat mask_blend;
            blender->blend(ortho_blend, mask_blend);
            ortho_blend.convertTo(ortho_blend, CV_8U);

            //affine_ortho.transform(grid[i].pt1);
            //affine_ortho.transform(grid[i].pt2);
            auto affine_ortho_inverse = affine_ortho.inverse();
            tl::Point<double> p1 = affine_ortho_inverse.transform(grid[i].pt1);
            tl::Point<double> p2 = affine_ortho_inverse.transform(grid[i].pt2);
            tl::WindowI window_to_write(static_cast<tl::Point<int>>(p1), static_cast<tl::Point<int>>(p2));
            window_to_write.normalized();
            if (window_to_write.isValid())
                image_writer->write(ortho_blend, window_to_write);

        }

        image_writer->close();
    }
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
    bCuda(cuda)
{
    //// Para hacer una prueba rapida...
    //         
    //auto epsg_geographic = std::make_shared<tl::Crs>("EPSG:4326");
    //auto epsg_geocentric = std::make_shared<tl::Crs>("EPSG:4978");
    //tl::CrsTransform crs_transfom_geocentric_to_geographic(epsg_geocentric, epsg_geographic);
    //auto lla = crs_transfom_geocentric_to_geographic.transform(offset);
    //auto rotation = tl::rotationEnuToEcef(lla.x, lla.y);
    //auto ecef_to_enu = std::make_shared<tl::EcefToEnu>(offset, rotation);
    //auto epsg_utm = std::make_shared<tl::Crs>(epsg);
    //tl::CrsTransform crs_transfom(epsg_geocentric, epsg_utm);

    //for (const auto &image : images) {

    //    Image photo(image);
    //    auto point_ecef = ecef_to_enu->inverse(image.cameraPose().position());
    //    auto point_utm = crs_transfom.transform(point_ecef);
    //    CameraPose camera_pose;
    //    camera_pose.setRotationMatrix(image.cameraPose().rotationMatrix());
    //    camera_pose.setPosition(point_utm);
    //    camera_pose.setCrs(QString::fromStdString(mEpsg));
    //    photo.setCameraPose(camera_pose);

    //    mPhotos.push_back(photo);
    //}
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

std::vector<tl::WindowD> OrthophotoTask::findGrid(const tl::Path &mdt, double gsd)
{
    std::vector<tl::WindowD> grid;
    /////
    //std::vector<std::shared_ptr<tl::GPolygon>> grid_to_save;
    /////

    auto reader = tl::ImageReaderFactory::create(mdt);
    reader->open();
    TL_ASSERT(reader->isOpen(), "Can not open the MDT");
    auto window = reader->window();
    int step_x = std::ceil(window.width() / (gsd * 255.));
    int step_y = std::ceil(window.height() / (gsd * 255.));

    auto center = window.center();

    //double x_ini = step_x % 2 ? center.x + (step_x-1) * (gsd * 255.) / 2. /* - (gsd * 255.) / 2.*/ : center.x + (step_x / 2) * (gsd * 255.);
    //double y_ini = step_y % 2 ? center.y + (step_y-1) * (gsd * 255.) / 2./* + (gsd * 255.) / 2.*/ : center.y + (step_y / 2) * (gsd * 255.);
    double x_ini = center.x - ((step_x-1) * gsd * 255.) / 2.;
    double y_ini = center.y + ((step_y-1) * gsd * 255.) / 2.;

    tl::Point<double> point;
    for (size_t i = 0; i < step_x; i++) {

        point.x = x_ini + (gsd * 255.) * i;

        for (size_t j = 0; j < step_y; j++) {

            point.y = y_ini - (gsd * 255.) * j;
            grid.emplace_back(point, (gsd * 255.));

            /////
            //std::shared_ptr<tl::GPolygon> polygon = std::make_shared<tl::GPolygon>();
            //auto x_min = grid.back().pt1.x;
            //auto y_min = grid.back().pt1.y;
            //auto x_max = grid.back().pt2.x;
            //auto y_max = grid.back().pt2.y;

            //polygon->push_back(tl::Point<double>(x_min, y_min));
            //polygon->push_back(tl::Point<double>(x_max, y_min));
            //polygon->push_back(tl::Point<double>(x_max, y_max));
            //polygon->push_back(tl::Point<double>(x_min, y_max));
            //grid_to_save.push_back(polygon);
            /////
        }

    }

    /////
    //auto grid_file = tl::Path("C:\\GRAPHOS\\urban2\\ortho\\grid.shp");
    //std::unique_ptr<tl::VectorWriter> vector_writer = tl::VectorWriterFactory::create(grid_file);
    //vector_writer->open();
    //if (!vector_writer->isOpen())throw std::runtime_error("Vector open error");
    //vector_writer->create();
    //vector_writer->setCRS(tl::Crs("EPSG:").toWktFormat());

    //std::shared_ptr<tl::TableField> field(new tl::TableField("image",
    //                                      tl::TableField::Type::STRING,
    //                                      254));
    //std::vector<std::shared_ptr<tl::TableField>> fields;
    //fields.push_back(field);

    //tl::GLayer layer;
    //layer.setName("footprint");
    //layer.addDataField(field);

    //for (const auto &footprint : grid_to_save) {
    //    std::shared_ptr<tl::TableRegister> data(new tl::TableRegister(fields));
    //    //data->setValue(0, footprint.first);
    //    layer.push_back(footprint);
    //}

    //    vector_writer->write(layer);

    //    vector_writer->close();
    /////

    return grid;
}

std::vector<std::vector<tl::WindowD>> OrthophotoTask::findGrid2(const tl::Path &mdt, double gsd, int gridSize)
{
    std::vector<std::vector<tl::WindowD>> grid;

    auto reader = tl::ImageReaderFactory::create(mdt);
    reader->open();
    TL_ASSERT(reader->isOpen(), "Can not open the MDT");
    auto window = reader->window();
    int step_x = std::ceil(window.width() / (gsd * gridSize));
    int step_y = std::ceil(window.height() / (gsd * gridSize));

    auto center = window.center();

    double x_ini = center.x - ((step_x-1) * gsd * gridSize) / 2.;
    double y_ini = center.y + ((step_y-1) * gsd * gridSize) / 2.;

    tl::Point<double> point;
    for (size_t i = 0; i < step_x; i++) {

        std::vector<tl::WindowD> row_grid;

        point.x = x_ini + (gsd * gridSize) * i;

        for (size_t j = 0; j < step_y; j++) {

            point.y = y_ini - (gsd * gridSize) * j;
            row_grid.emplace_back(point, (gsd * gridSize));

        }

        grid.push_back(row_grid);
    }

    return grid;
}

void OrthophotoTask::execute(tl::Progress *progressBar)
{

    try {

        tl::Path footprint_file(mOrthoPath);
        footprint_file.append("footprint.shp");
        tl::Path graph_orthos = tl::Path(footprint_file).replaceBaseName("graph_orthos");
        tl::Crs crs(mEpsg);

        /// Conversión del DTM a coordenadas ENU para poder trabajar con las orientaciones
        //tl::Point3<double> ecef_center = mOffset;

        //auto epsg_geographic = std::make_shared<tl::Crs>("EPSG:4326");
        //auto epsg_geocentric = std::make_shared<tl::Crs>("EPSG:4978");

        //tl::CrsTransform crs_transfom_geocentric_to_geographic(epsg_geocentric, epsg_geographic);
        //auto lla = crs_transfom_geocentric_to_geographic.transform(ecef_center);
        //auto rotation = tl::rotationEnuToEcef(lla.x, lla.y);
        //tl::EcefToEnu ecef_to_enu(ecef_center, rotation);

        //auto epsg_utm = std::make_shared<tl::Crs>(mEpsg);
        //TL_ASSERT(epsg_utm->isProjected(), "Only projected CRS's are allowed");

        //auto crs_transfom = std::make_shared<tl::CrsTransform>(epsg_geocentric, epsg_utm);

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

        orthoimage_task.run(progressBar);

        //std::vector<tl::WindowD> grid = findGrid(graph_orthos);
        //std::vector<tl::WindowD> grid = this->findGrid(mMdt, mGSD);
        std::vector<std::vector<tl::WindowD>> grid = this->findGrid2(mMdt, mGSD, 500);
        //std::vector<tl::WindowD> grid = this->findGrid(graph_orthos, mGSD);

        //tl::Path optimal_footprint_path(graph_orthos);
        //std::string name = optimal_footprint_path.baseName().toString() + "_optimal";
        //optimal_footprint_path.replaceBaseName(name);
        //findOptimalFootprint(graph_orthos, grid, optimal_footprint_path, crs);

        /// La ventana total de la orto tiene que ser la misma que la del MDT....
        //orthoMosaic(optimal_footprint_path, mOrthoPath, mGSD, crs, grid);
        //orthoMosaic(graph_orthos, mOrthoPath, mGSD, crs, grid);

        // Grid tiene que ser std::vector<std::vector<std::pair<std::string,tl::WindowD>>>> 
        // De está forma se puede recorrer como una matriz y ver las imagenes que están a los lados
        // Se expande la ventana correspondiente a un grid y las de sus vecinos y se procede a ajusta 
        // radiometricamente la imagen. 
        // Posteriormente se determinan las líneas de cosido y se fusionan las ortoimagenes en el mosaico.
        // Ortomosaico con mosaicos de 256x256 sin ninguna clase de ajuste.
        //orthoMosaic2(graph_orthos, mOrthoPath, mGSD, crs, grid);
        // Ortomosaico con mosaicos de 256x256 con relleno de zonas negras de area < 1024
        //orthoMosaic(graph_orthos, mOrthoPath, mGSD, crs, grid);
        orthoMosaicWithExposureCompensator(graph_orthos, mOrthoPath, mGSD, crs, grid);

        mOrthophotoReport.time = this->time();
        mOrthophotoReport.gsd = mGSD;

        tl::Message::success("Orthophoto task finished in {:.2} minutes", mOrthophotoReport.time / 60.);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Orthophoto task error");
    }

}

} // namespace graphos
