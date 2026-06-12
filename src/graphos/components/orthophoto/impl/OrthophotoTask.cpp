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
#include "graphos/core/utils.h"
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
#include <tidop/math/statistic/median.h>
#include <tidop/math/statistic/tukeyfences.h>

/* OpenCV */
#include <opencv2/stitching.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>

#include <QDateTime>

#include <map>

//#define FAST_ORTHO 1

namespace graphos
{

constexpr double exposure_compensator_factor = 0.1;
//constexpr int ortho_tile_w = 600;
//constexpr int ortho_tile_h = 600;
//#ifndef FAST_ORTHO
//constexpr int ortho_overlap = 50;
//#else
//constexpr int ortho_overlap = 0;
//#endif


static void orthoMosaic(tl::Path &optimal_footprint_path,
    tl::Path &ortho_path,
    double res_ortho,
    const std::string &epsg,
    std::vector<std::vector<tl::WindowD>> &grid)
{
    tl::WindowD window_all;
    std::vector<std::string> compensated_orthos;
    std::vector<std::string> ortho_seams;

    tl::Message::info("Exposure compensator");

    // Usar GAIN_BLOCKS que funciona mejor para mosaicos
    int type = cv::detail::ExposureCompensator::GAIN_BLOCKS;
    cv::Ptr<cv::detail::ExposureCompensator> compensator = cv::detail::ExposureCompensator::createDefault(type);

    auto vector_reader = tl::VectorReaderFactory::create(optimal_footprint_path.toString());
    vector_reader->open();

    if (!vector_reader->isOpen()) {
        tl::Message::error("Cannot open vector reader");
        return;
    }

    if (vector_reader->layersCount() < 1) {
        tl::Message::error("No layers found in footprint");
        vector_reader->close();
        return;
    }

    std::shared_ptr<tl::GLayer> layer = vector_reader->read(0);

    // Primera pasada: calcular ventana global completa
    for (const auto &entity : *layer) {
        if (entity->type() != tl::GraphicEntity::Type::polygon_2d) {
            continue;
        }
        std::shared_ptr<tl::GPolygon> polygon = std::dynamic_pointer_cast<tl::GPolygon>(entity);
        window_all = joinWindow(window_all, polygon->window());
    }

    tl::Message::info("Global window: [{}, {}] - [{}, {}]",
        window_all.pt1.x, window_all.pt1.y,
        window_all.pt2.x, window_all.pt2.y);

    // Segunda pasada: procesar todas las imágenes
    std::vector<std::string> all_orthos;
    std::vector<tl::WindowD> all_windows;
    std::vector<cv::Point> all_corners;
    std::vector<cv::UMat> all_images_comp;
    std::vector<cv::UMat> all_masks_comp;

    for (const auto &entity : *layer) {
        if (entity->type() != tl::GraphicEntity::Type::polygon_2d) continue;

        std::shared_ptr<tl::GPolygon> polygon = std::dynamic_pointer_cast<tl::GPolygon>(entity);
        std::shared_ptr<tl::TableRegister> data = polygon->data();
        std::string ortho_path_str = data->value(0);

        try {
            auto image_reader = tl::ImageReaderFactory::create(ortho_path_str);
            image_reader->open();
            if (!image_reader->isOpen()) {
                tl::Message::warning("Cannot open ortho image: {}", ortho_path_str);
                continue;
            }

            // Leer imagen a escala reducida para compensación
            cv::Mat image = image_reader->read(exposure_compensator_factor, exposure_compensator_factor);
            if (image.empty()) {
                tl::Message::warning("Empty image: {}", ortho_path_str);
                image_reader->close();
                continue;
            }

            // relleno de huecos (pocos pixeles)
            cv::Mat blackPixelMask = createBlackPixelMask(image, 32);
            cv::inpaint(image, blackPixelMask, image, 3, cv::INPAINT_TELEA);

            double scale = image_reader->georeference().scale().x();
            tl::WindowD window = polygon->window();

            // Calcular esquina en coordenadas de la ventana global (CORREGIDO)
            cv::Point corner;
            corner.x = tl::roundToInteger((window.pt1.x - window_all.pt1.x) * exposure_compensator_factor / scale);
            corner.y = tl::roundToInteger((window_all.pt2.y - window.pt2.y) * exposure_compensator_factor / scale);
            tl::Message::info("Image {} corner: ({}, {})", ortho_path_str, corner.x, corner.y);

            // Crear máscara
            cv::Mat mask;
            if (image.channels() == 1) {
                cv::threshold(image, mask, 1, 255, cv::THRESH_BINARY);
            } else {
                cv::Mat gray;
                cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
                cv::threshold(gray, mask, 1, 255, cv::THRESH_BINARY);
            }

            // Eliminar pequeños artefactos de la máscara
            //cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
            //cv::morphologyEx(mask, mask, cv::MORPH_ERODE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(6, 6)));

            cv::UMat umat_img, umat_mask;
            image.copyTo(umat_img);
            mask.copyTo(umat_mask);

            all_orthos.push_back(ortho_path_str);
            all_windows.push_back(window);
            all_corners.push_back(corner);
            all_images_comp.push_back(umat_img);
            all_masks_comp.push_back(umat_mask);

            image_reader->close();

        } catch (std::exception &e) {
            tl::Message::warning("Error processing {}: {}", ortho_path_str, e.what());
            continue;
        }
    }

    if (all_orthos.empty()) {
        tl::Message::error("No valid orthos found");
        vector_reader->close();
        return;
    }

    // Aplicar compensación radiométrica
    tl::Message::info("Applying exposure compensation to {} images", all_orthos.size());
    compensator->feed(all_corners, all_images_comp, all_masks_comp);

    // Preparar imágenes y mascaras para seam finding
    std::vector<cv::UMat> all_images_seam;
    for (size_t i = 0; i < all_images_comp.size(); ++i) {
        cv::UMat img32f;
        all_images_comp[i].convertTo(img32f, CV_32F, 1.0 / 255.0);
        all_images_seam.push_back(img32f);

        cv::morphologyEx(all_masks_comp[i], all_masks_comp[i], cv::MORPH_ERODE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
    }

    // Encontrar líneas de costura
    tl::Message::info("Seam finding");
    cv::Ptr<cv::detail::SeamFinder> seam_finder;

    try {
        seam_finder = cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinder::COST_COLOR_GRAD);
        seam_finder->find(all_images_seam, all_corners, all_masks_comp);
    } catch (const cv::Exception &e) {
        tl::Message::warning("GraphCut failed, using Voronoi: {}", e.what());
        seam_finder = cv::makePtr<cv::detail::VoronoiSeamFinder>();
        seam_finder->find(all_images_seam, all_corners, all_masks_comp);
    }

    // Aplicar compensación y guardar imágenes procesadas
    for (size_t i = 0; i < all_orthos.size(); ++i) {
        try {
            auto image_reader_full = tl::ImageReaderFactory::create(all_orthos[i]);
            image_reader_full->open();
            if (!image_reader_full->isOpen()) continue;

            cv::Mat full_img = image_reader_full->read();
            if (full_img.empty()) {
                image_reader_full->close();
                continue;
            }

            // Calcular corner para resolución completa (CORREGIDO)
            double scale_full = image_reader_full->georeference().scale().x();
            //cv::Point corner_full;
            //corner_full.x = tl::roundToInteger((all_windows[i].pt1.x - window_all.pt1.x) / scale_full);
            //corner_full.y = tl::roundToInteger((window_all.pt2.y - all_windows[i].pt2.y) / scale_full);
            cv::Point corner_full = all_corners[0] / exposure_compensator_factor;
            tl::Message::info("Full res corner for {}: ({}, {})", all_orthos[i], corner_full.x, corner_full.y);

            // Crear máscara para imagen completa
            cv::Mat mask_full;
            if (full_img.channels() == 1) {
                cv::threshold(full_img, mask_full, 1, 255, cv::THRESH_BINARY);
            } else {
                cv::Mat gray;
                cv::cvtColor(full_img, gray, cv::COLOR_BGR2GRAY);
                cv::threshold(gray, mask_full, 1, 255, cv::THRESH_BINARY);
            }

            // Limpiar la máscara
            //cv::morphologyEx(mask_full, mask_full, cv::MORPH_CLOSE,
            //                 cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));



            // Aplicar compensación
            compensator->apply(static_cast<int>(i), corner_full, full_img, mask_full);

            // Redimensionar máscara de seam a resolución completa
            cv::Mat seam_mask_full;
            if (!all_masks_comp[i].empty()) {
                //cv::resize(all_masks_comp[i], seam_mask_full, full_img.size(), 0, 0, cv::INTER_NEAREST);
                cv::resize(all_masks_comp[i], seam_mask_full, full_img.size(), 0, 0, cv::INTER_LINEAR);
                cv::threshold(seam_mask_full, seam_mask_full, 1, 255, cv::THRESH_BINARY);
                cv::morphologyEx(mask_full, mask_full, cv::MORPH_DILATE,
                                 cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(6, 6))); // De 6 a 9 no mejora mucho
            } else {
                seam_mask_full = mask_full.clone();
            }
            //cv::Mat seam_mask_full;
            //if (!all_masks_comp[i].empty()) {
            //    cv::resize(all_masks_comp[i], seam_mask_full, full_img.size(), 0, 0, cv::INTER_NEAREST);

            //    // Suavizar los bordes de la máscara de seam para evitar líneas duras
            //    cv::Mat seam_mask_float;
            //    seam_mask_full.convertTo(seam_mask_float, CV_32F, 1.0 / 255.0);

            //    // Aplicar un pequeño blur para suavizar transiciones
            //    cv::GaussianBlur(seam_mask_float, seam_mask_float, cv::Size(3, 3), 0.5);

            //    // Volver a binario pero con transiciones suaves
            //    seam_mask_float.convertTo(seam_mask_full, CV_8U, 255.0);
            //} else {
            //    seam_mask_full = mask_full.clone();
            //}

            // Combinar máscaras
            cv::Mat final_mask = seam_mask_full & mask_full;

            // Verificar que la imagen compensada no esté toda negra
            double min_val, max_val;
            cv::minMaxLoc(full_img, &min_val, &max_val);
            if (max_val < 1.0) {
                tl::Message::warning("Compensated image {} appears too dark (max value: {})", all_orthos[i], max_val);
            }

            // Guardar imagen compensada
            tl::Path ortho_comp(all_orthos[i]);
            std::string comp_name = ortho_comp.baseName().toString() + "_compensated.tif";
            ortho_comp.replaceFileName(comp_name);

            auto writer_comp = tl::ImageWriterFactory::create(ortho_comp.toString());
            writer_comp->open();
            if (writer_comp->isOpen()) {
                writer_comp->create(image_reader_full->rows(), image_reader_full->cols(),
                    image_reader_full->channels(), image_reader_full->dataType());
                writer_comp->setCRS(image_reader_full->crsWkt());
                writer_comp->setGeoreference(image_reader_full->georeference());
                writer_comp->write(full_img);
                writer_comp->close();
                compensated_orthos.push_back(ortho_comp.toString());
                tl::Message::info("Saved compensated: {}", ortho_comp.toString());
            }

            // Guardar máscara
            tl::Path seam_path(all_orthos[i]);
            std::string seam_name = seam_path.baseName().toString() + "_seam.tif";
            seam_path.replaceFileName(seam_name);

            auto writer_seam = tl::ImageWriterFactory::create(seam_path.toString());
            writer_seam->open();
            if (writer_seam->isOpen()) {
                writer_seam->create(image_reader_full->rows(), image_reader_full->cols(), 1, tl::DataType::TL_8U);
                writer_seam->setCRS(image_reader_full->crsWkt());
                writer_seam->setGeoreference(image_reader_full->georeference());
                writer_seam->write(final_mask);
                writer_seam->close();
                ortho_seams.push_back(seam_path.toString());
                tl::Message::info("Saved seam mask: {}", seam_path.toString());
            }

            image_reader_full->close();

        } catch (std::exception &e) {
            tl::Message::warning("Error processing full resolution {}: {}", all_orthos[i], e.what());
        }
    }

    vector_reader->close();

    // --- BLENDING MEJORADO ---
    tl::Message::info("Starting blending with {} images", compensated_orthos.size());

    bool try_cuda = false;
    int blender_type = cv::detail::Blender::FEATHER;
    float blend_strength = 2.0f; // Aumentado para mejor transición

    std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(ortho_path);
    image_writer->open();

    int cols = static_cast<int>(std::round(window_all.width() / res_ortho));
    int rows = static_cast<int>(std::round(window_all.height() / res_ortho));

    if (image_writer->isOpen()) {
        image_writer->create(rows, cols, 3, tl::DataType::TL_8U);
        tl::Crs crs(epsg);
        image_writer->setCRS(crs.toWktFormat());
        tl::Affine<double, 2> affine_ortho(res_ortho, -res_ortho, window_all.pt1.x, window_all.pt2.y, 0.0);
        image_writer->setGeoreference(affine_ortho);

        for (size_t r = 0; r < grid.size(); ++r) {
            for (size_t c = 0; c < grid[r].size(); ++c) {
                tl::Message::info("Processing grid cell [{}, {}]", r, c);

                cv::Ptr<cv::detail::Blender> blender = cv::detail::Blender::createDefault(blender_type, try_cuda);

                int cols_grid = static_cast<int>(std::round(grid[r][c].width() / res_ortho));
                int rows_grid = static_cast<int>(std::round(grid[r][c].height() / res_ortho));
                cv::Rect rect(0, 0, cols_grid, rows_grid);

                float blend_width = sqrt(static_cast<float>(rect.area())) * blend_strength / 100.f;

                if (blend_width < 1.f) {
                    blender = cv::detail::Blender::createDefault(cv::detail::Blender::NO, try_cuda);
                } else if (blender_type == cv::detail::Blender::MULTI_BAND) {
                    cv::detail::MultiBandBlender *multi_band_blender = dynamic_cast<cv::detail::MultiBandBlender *>(blender.get());
                    //int num_bands = static_cast<int>(ceil(log(blend_width) / log(2.)) - 1.);
                    //num_bands = std::max(1, std::min(5, num_bands)); // Limitar entre 1-5 bandas
                    //multi_band_blender->setNumBands(num_bands);
                    int num_bands = static_cast<int>(ceil(log(blend_width) / log(2.)) - 1.);
                    tl::Message::info("Multi-band blender with {} bands", num_bands);
                    multi_band_blender->setNumBands(num_bands);
                } else if (blender_type == cv::detail::Blender::FEATHER) {
                    cv::detail::FeatherBlender *feather_blender = dynamic_cast<cv::detail::FeatherBlender *>(blender.get());
                    feather_blender->setSharpness(1.f / blend_width);
                    // Reducir sharpness para transiciones más suaves
                    //float sharpness = 1.f / (blend_width * 2.0f); // Más suave
                    //feather_blender->setSharpness(sharpness);
                }

                blender->prepare(rect);

                // Procesar todas las imágenes que intersectan con esta celda
                for (size_t j = 0; j < compensated_orthos.size(); j++) {
                    try {
                        auto image_reader = tl::ImageReaderFactory::create(compensated_orthos[j]);
                        auto image_reader_seam = tl::ImageReaderFactory::create(ortho_seams[j]);
                        image_reader->open();
                        image_reader_seam->open();

                        if (!image_reader->isOpen() || !image_reader_seam->isOpen()) {
                            image_reader->close();
                            image_reader_seam->close();
                            continue;
                        }

                        if (!intersectWindows(image_reader->window(), grid[r][c])) {
                            image_reader->close();
                            image_reader_seam->close();
                            continue;
                        }

                        auto georef = image_reader->georeference();
                        double read_scale_x = georef.scale().x() / res_ortho;
                        double read_scale_y = georef.scale().y() / res_ortho;

                        // Leer imagen compensada
                        tl::Affine<int, 2> affine;
                        cv::Mat compensate_image = image_reader->read(grid[r][c], read_scale_x, read_scale_y, &affine);
                        cv::Mat seam_image = image_reader_seam->read(grid[r][c], read_scale_x, read_scale_y);

                        if (compensate_image.empty() || seam_image.empty()) {
                            image_reader->close();
                            image_reader_seam->close();
                            continue;
                        }

                        // Asegurar que las máscaras sean binarias
                        //cv::Mat seam_mask_binary;
                        //cv::threshold(seam_image, seam_mask_binary, 128, 255, cv::THRESH_BINARY);

                        // Convertir a 16S para el blender
                        cv::Mat compensate_image_16s;
                        compensate_image.convertTo(compensate_image_16s, CV_16S);
                        compensate_image.release();

                        cv::Rect rct = cv::Rect(tl::roundToInteger(affine.translation().x()),
                            tl::roundToInteger(affine.translation().y()),
                            compensate_image_16s.cols,
                            compensate_image_16s.rows);

                        blender->feed(compensate_image_16s, seam_image, rct.tl());

                        // Calcular posición en el mosaico
                        //tl::Point<double> cell_origin = grid[r][c].pt1;
                        //tl::Point<double> image_origin = image_reader->window().pt1;

                        //int offset_x = static_cast<int>((image_origin.x - cell_origin.x) / res_ortho);
                        //int offset_y = static_cast<int>((cell_origin.y - image_origin.y) / res_ortho);

                        //cv::Rect rct(std::max(0, offset_x), std::max(0, offset_y),
                        //    compensate_image_16s.cols, compensate_image_16s.rows);

                        //if (rct.x < cols_grid && rct.y < rows_grid &&
                        //    rct.width > 0 && rct.height > 0) {
                        //    blender->feed(compensate_image_16s, seam_mask_binary, rct.tl());
                        //}

                        image_reader->close();
                        image_reader_seam->close();

                    } catch (std::exception &e) {
                        tl::Message::warning("Error in blending for image {}: {}", compensated_orthos[j], e.what());
                        continue;
                    }
                }

                // Realizar blending
                cv::Mat ortho_blend, mask_blend;
                blender->blend(ortho_blend, mask_blend);

                if (!ortho_blend.empty()) {
                    ortho_blend.convertTo(ortho_blend, CV_8U);

                    // Escribir en el mosaico final
                    auto affine_ortho_inverse = affine_ortho.inverse();
                    tl::Point<double> pp1 = affine_ortho_inverse.transform(grid[r][c].pt1);
                    tl::Point<double> pp2 = affine_ortho_inverse.transform(grid[r][c].pt2);
                    tl::WindowI window_to_write(static_cast<tl::Point<int>>(pp1), static_cast<tl::Point<int>>(pp2));
                    window_to_write.normalized();

                    if (window_to_write.isValid()) {

                        cv::Mat blackPixelMask = createBlackPixelMask(ortho_blend, 1024);
                        cv::inpaint(ortho_blend, blackPixelMask, ortho_blend, 3, cv::INPAINT_TELEA);

                        image_writer->write(ortho_blend, window_to_write);
                    }
                }

            }
        }

        image_writer->close();
        tl::Message::info("Mosaic completed: {}", ortho_path.toString());
    }
}

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


static float ciede2000(const cv::Vec3f &lab1, const cv::Vec3f &lab2)
{
    float L1 = lab1[0]; 
    float a1 = lab1[1];
    float b1 = lab1[2];
    float L2 = lab2[0]; 
    float a2 = lab2[1]; 
    float b2 = lab2[2];

    // 1. Calcular C' y h'
    float C1 = std::sqrt(a1 * a1 + b1 * b1);
    float C2 = std::sqrt(a2 * a2 + b2 * b2);
    float C_bar = (C1 + C2) * 0.5f;

    float G = 0.5f * (1.0f - std::sqrt(std::pow(C_bar, 7.0f) / (std::pow(C_bar, 7.0f) + std::pow(25.0f, 7.0f))));

    float a1_prime = (1.0f + G) * a1;
    float a2_prime = (1.0f + G) * a2;

    float C1_prime = std::sqrt(a1_prime * a1_prime + b1 * b1);
    float C2_prime = std::sqrt(a2_prime * a2_prime + b2 * b2);

    // Calcular h' (hue angles)
    auto hPrime = [](float b, float a_prime) -> float {
        if (b == 0 && a_prime == 0) return 0.0f;
        float angle = std::atan2(b, a_prime) * tl::consts::rad_to_deg<float>;
        if (angle < 0) angle += 360.0f;
        return angle;
        };

    float h1_prime = hPrime(b1, a1_prime);
    float h2_prime = hPrime(b2, a2_prime);

    // 2. Calcular Deltas
    float delta_L_prime = L2 - L1;
    float delta_C_prime = C2_prime - C1_prime;

    float delta_h_prime = 0.0f;
    if (C1_prime * C2_prime != 0) {
        if (std::abs(h2_prime - h1_prime) <= 180.0f) {
            delta_h_prime = h2_prime - h1_prime;
        } else {
            if (h2_prime <= h1_prime) delta_h_prime = h2_prime - h1_prime + 360.0f;
            else delta_h_prime = h2_prime - h1_prime - 360.0f;
        }
    }

    float delta_H_prime = 2.0f * std::sqrt(C1_prime * C2_prime) * std::sin((delta_h_prime * 0.5f) * tl::consts::deg_to_rad<float>);

    // 3. Calcular medias para los factores de peso
    float L_bar_prime = (L1 + L2) * 0.5f;
    float C_bar_prime = (C1_prime + C2_prime) * 0.5f;

    float h_bar_prime = 0.0f;
    if (C1_prime * C2_prime != 0) {
        if (std::abs(h1_prime - h2_prime) <= 180.0f) {
            h_bar_prime = (h1_prime + h2_prime) * 0.5f;
        } else {
            if ((h1_prime + h2_prime) < 360.0f) h_bar_prime = (h1_prime + h2_prime + 360.0f) * 0.5f;
            else h_bar_prime = (h1_prime + h2_prime - 360.0f) * 0.5f;
        }
    } else {
        h_bar_prime = h1_prime + h2_prime;
    }

    // 4. Calcular funciones de peso S_L, S_C, S_H
    float T = 1.0f - 0.17f * std::cos((h_bar_prime - 30.0f) * tl::consts::deg_to_rad<float>)
        + 0.24f * std::cos((2.0f * h_bar_prime) * tl::consts::deg_to_rad<float>)
        + 0.32f * std::cos((3.0f * h_bar_prime + 6.0f) * tl::consts::deg_to_rad<float>)
        - 0.20f * std::cos((4.0f * h_bar_prime - 63.0f) * tl::consts::deg_to_rad<float>);

    float delta_theta = 30.0f * std::exp(-std::pow((h_bar_prime - 275.0f) / 25.0f, 2.0f));
    float R_C = 2.0f * std::sqrt(std::pow(C_bar_prime, 7.0f) / (std::pow(C_bar_prime, 7.0f) + std::pow(25.0f, 7.0f)));
    float S_L = 1.0f + (0.015f * std::pow(L_bar_prime - 50.0f, 2.0f)) / std::sqrt(20.0f + std::pow(L_bar_prime - 50.0f, 2.0f));
    float S_C = 1.0f + 0.045f * C_bar_prime;
    float S_H = 1.0f + 0.015f * C_bar_prime * T;
    float R_T = -std::sin(2.0f * delta_theta * tl::consts::deg_to_rad<float>) * R_C;

    // Parametros kL, kC, kH suelen ser 1 para aplicaciones gráficas genéricas
    float kL = 1.0f;
    float kC = 1.0f;
    float kH = 1.0f;

    // 5. Ecuación final CIEDE2000
    float val = std::pow(delta_L_prime / (kL * S_L), 2.0f) +
        std::pow(delta_C_prime / (kC * S_C), 2.0f) +
        std::pow(delta_H_prime / (kH * S_H), 2.0f) +
        R_T * (delta_C_prime / (kC * S_C)) * (delta_H_prime / (kH * S_H));

    return std::sqrt(val);
}

static cv::Mat combineImagesSmart(const std::vector<cv::Mat> &images,
                                  const std::vector<cv::Point2f> &centers,
                                  float weight_color = 0.3f,
                                  float weight_distance = 0.7f)
{
    if (images.empty()) return cv::Mat();

    size_t images_size = images.size();
    TL_ASSERT(images_size == centers.size(), "Number of images and distances must match");

    // Verificación de formato
    int rows = images[0].rows;
    int cols = images[0].cols;
    int depth = images[0].depth();
    int channels = images[0].channels();
    int type = images[0].type();

    TL_ASSERT(channels == 3, "combineImagesSmart only supports 3-channel images");

    for (size_t i = 1; i < images_size; ++i) {
        TL_ASSERT(images[i].rows == rows && images[i].cols == cols, "All images must be same size");
        TL_ASSERT(images[i].channels() == channels, "All images must have same number of channels");
        TL_ASSERT(images[i].depth() == depth, "All images must have same bit depth");
    }

    // máscaras de píxeles válidos (no negros)
    std::vector<cv::Mat> valid_masks(images_size);
    for (size_t i = 0; i < images_size; ++i) {
        cv::inRange(images[i], cv::Scalar::all(0), cv::Scalar::all(0), valid_masks[i]);
        cv::bitwise_not(valid_masks[i], valid_masks[i]);
    }

    // Conversión a CIELab
    std::vector<cv::Mat> lab_images(images_size);
    for (size_t i = 0; i < images_size; ++i) {
        cv::Mat img = images[i];

        if (depth == CV_8U) {
            img.convertTo(img, CV_32F, 1.0 / 255.0);
        } else if (depth == CV_16U) {
            img.convertTo(img, CV_32F, 1.0 / 65535.0);
        } else if (depth == CV_32F) {
            img.convertTo(img, CV_32F);
        } else {
            TL_THROW_EXCEPTION("Unsupported image depth");
        }

        cv::cvtColor(img, lab_images[i], cv::COLOR_BGR2Lab);
    }

    // Pre-calcular rangos de distancia espacial para normalizar
    // Como ahora es pixel a pixel, esto es una estimación, pero podemos
    // calcular la distancia máxima posible teórica o dinámica.
    // Para simplificar y evitar calcular min/max por pixel (muy lento), 
    // normalizaremos usando la diagonal de la tesela como referencia de escala.
    float max_dist_ref = std::sqrt((float)(rows * rows + cols * cols)) * 1.5f;

    cv::Mat result_lab(rows, cols, CV_32FC3, cv::Scalar(0, 0, 0));
    cv::Mat result_hf(rows, cols, CV_32FC3, cv::Scalar(0, 0, 0));

    tl::parallel_for(0, rows, [&](size_t r) {

        std::vector<float> L; 
        L.reserve(images_size);
        std::vector<float> A; 
        A.reserve(images_size);
        std::vector<float> B; 
        B.reserve(images_size);
        std::vector<float> color_distances; 
        color_distances.reserve(images_size);

        for (int c = 0; c < cols; ++c) {

            L.clear(); 
            A.clear();
            B.clear();

            // recolectar muestras válidas para este píxel
            std::vector<size_t> valid_idxs;
            valid_idxs.reserve(images_size);
            for (size_t i = 0; i < images_size; ++i) {
                if (valid_masks[i].at<uchar>(static_cast<int>(r), c)) {
                    cv::Vec3f lab = lab_images[i].at<cv::Vec3f>(static_cast<int>(r), c);
                    L.push_back(lab[0]);
                    A.push_back(lab[1]);
                    B.push_back(lab[2]);
                    valid_idxs.push_back(i);
                }
            }

            if (valid_idxs.empty()) continue;

            // mediana inicial por canal
            float medL = tl::median(L.begin(), L.end());
            float medA = tl::median(A.begin(), A.end());
            float medB = tl::median(B.begin(), B.end());
             
            // distancias de color a la mediana (CIE 1976 - ΔE*)
            color_distances.assign(images_size, std::numeric_limits<float>::infinity());
            for (int idx : valid_idxs) {
                cv::Vec3f lab = lab_images[idx].at<cv::Vec3f>(static_cast<int>(r), c);

                // CIE 1976 - ΔE*
                float dL = lab[0] - medL;
                float dA = lab[1] - medA;
                float dB = lab[2] - medB;
                color_distances[idx] = std::sqrt(dL * dL + dA * dA + dB * dB);

                // CIE 2000 - ciede2000
                //color_distances[idx] = ciede2000(lab, cv::Vec3f(medL, medA, medB));
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

            //tl::Series<float> data;
            // ---------------------------------------------------------
            // PASO EXTRA: CALCULAR UMBRAL DE OUTLIERS (Robust Rejection)
            // ---------------------------------------------------------
            // Copiamos las distancias de color para buscar su mediana (MAD)
            std::vector<float> dists_copy;
            dists_copy.reserve(valid_idxs.size());
            for (int idx : valid_idxs) {
                dists_copy.push_back(color_distances[idx]);
                //data.push_back(color_distances[idx]);
            }
            // Calculamos la mediana de las desviaciones (MAD)
            //float mad_val = median_of(dists_copy);
            float mad_val = tl::median(dists_copy.begin(), dists_copy.end());

            // Definimos un umbral. 
            // 3.0 * MAD es estándar estadístico para outliers.
            // Ponemos un suelo (p.ej. 10.0 o 15.0 en espacio Lab) para no 
            // eliminar ruido de textura natural si todas las imágenes son muy parecidas.
            float noise_floor = 10.0f;
            float outlier_threshold = std::max(noise_floor, 3.0f * mad_val);
            // ---------------------------------------------------------
            
            // Tukey's fences en lugar de MAD
            //tl::TukeyFences<float> tukey;
            //auto inliers = tukey.eval(data, tl::TukeyFences<float>::K::outlier);

            // calcular score de cada imagen válida
            std::vector<std::pair<float, int>> scored;
            scored.reserve(valid_idxs.size());

            for (int idx : valid_idxs) {

                if (color_distances[idx] > outlier_threshold) continue;
                //if (!inliers[idx]) continue;

                // 1. Distancia de color normalizada (0..1)
                float colorNorm = (color_distances[idx] - minC) / rangeC;

                // 2. Distancia Espacial Pixel a Centro
                // Vector desde el pixel actual (x,y) al centro de la imagen fuente (centers[idx])
                float dx = c - centers[idx].x;
                float dy = r - centers[idx].y;
                float dist_pixel = std::sqrt(dx * dx + dy * dy);

                // Normalizamos la distancia espacial (0..1 aprox)
                // Usamos max_dist_ref para que no dependa de los vecinos locales, sino de la escala global de la tesela
                float distNorm = dist_pixel / max_dist_ref;

                // Score final
                float score = weight_color * colorNorm + weight_distance * distNorm;
                scored.emplace_back(score, idx);
            }

            if (scored.empty()) continue;

            // ordenar por score ascendente (menor es mejor)
            std::sort(scored.begin(), scored.end(),
                [](const auto &a, const auto &b) { return a.first < b.first; });

            // número de mejores píxeles a usar
            const int TOP_N = 5;
            int n = std::min<int>(TOP_N, scored.size());

            if (n == 1) {
                result_lab.at<cv::Vec3f>(static_cast<int>(r), c) = lab_images[scored[0].second].at<cv::Vec3f>(static_cast<int>(r), c);
            } else {
                // calcular media ponderada de los N mejores
                float total_w = 0.f;
                cv::Vec3f sum_lab(0.f, 0.f, 0.f);

                for (int i = 0; i < n; ++i) {
                    int idx = scored[i].second;
                    float w = 1.0f / (1e-3f + scored[i].first); // peso inverso al score
                    cv::Vec3f lab = lab_images[idx].at<cv::Vec3f>(static_cast<int>(r), c);
                    sum_lab += lab * w;
                    total_w += w;
                }

                cv::Vec3f labWeighted = (total_w > 0.f) ? (sum_lab / total_w)
                    : lab_images[scored[0].second].at<cv::Vec3f>(static_cast<int>(r), c);

                result_lab.at<cv::Vec3f>(static_cast<int>(r), c) = labWeighted;
            }

            //result_hf.at<cv::Vec3f>(static_cast<int>(r), c) = lab_images[scored[0].second].at<cv::Vec3f>(static_cast<int>(r), c);

        }
    });

    //cv::Mat final_lab;
    //cv::Mat result_hf_blurred;
    //cv::GaussianBlur(result_hf, result_hf_blurred, cv::Size(5, 5), 0);

    // Alta Frecuencia 
    // Contiene valores cercanos a 0, positivos en bordes claros y negativos en oscuros.
    //cv::Mat high_frequency_map;
    //cv::subtract(result_hf, result_hf_blurred, high_frequency_map);

    //Opciones para resaltar la imagen:
    // 1 - Añadir imagen de altas frecuencias 
    //cv::add(result_lab, high_frequency_map, final_lab);

    // 2 - Control de Ganancia (alpha)
    // No sumar el 100% de la HF para evitar realzar demasiado el ruido o artefactos.
    //float alpha = 0.7f; // 70% de detalle
    //cv::addWeighted(result_lab, 1.0, high_frequency_map, alpha, 0, final_lab);

    // 3 - Control de Ganancia con umbral (solo realzar detalles si la HF supera cierto umbral)
    // 3.1
    //float hf_threshold = 0.05f; // umbral de detalle (ajustable)
    //cv::Mat hf_mask;
    //cv::inRange(high_frequency_map, cv::Scalar(-hf_threshold, -hf_threshold, -hf_threshold),
    //            cv::Scalar(hf_threshold, hf_threshold, hf_threshold), hf_mask);
    //cv::Mat hf_mask_float;
    //hf_mask.convertTo(hf_mask_float, CV_32F, 1.0 / 255.0);
    //cv::Mat hf_weighted;
    //cv::multiply(high_frequency_map, hf_mask_float, hf_weighted);
     
    // 3.2
    // Si el detalle es menor a 'epsilon', lo hacemos cero
    //float epsilon = 0.05f; // Ajusta según el ruido (rango 0-1)
    //cv::Mat mask;
    //cv::absdiff(high_frequency_map, cv::Scalar::all(0), mask);
    //cv::threshold(mask, mask, epsilon, 1.0, cv::THRESH_TOZERO);

    // Ahora 'high_freq' solo tiene valores donde había bordes reales
    //cv::Mat clean_hf;
    //high_frequency_map.copyTo(clean_hf, mask > 0);
    //cv::add(result_lab, clean_hf, final_lab);

    //cv::Mat result_bgr_f;
    //cv::cvtColor(final_lab, result_bgr_f, cv::COLOR_Lab2BGR);

    // Convertir Lab float -> BGR float (CV_32F 3ch)
    cv::Mat result_bgr_f;
    cv::cvtColor(result_lab, result_bgr_f, cv::COLOR_Lab2BGR);

    // Reconversión a la profundidad y canales originales
    cv::Mat final_img;

    cv::Mat bgr_out;
    if (depth == CV_8U) {
        result_bgr_f.convertTo(bgr_out, CV_8U, 255.0);
    } else if (depth == CV_16U) {
        result_bgr_f.convertTo(bgr_out, CV_16U, 65535.0);
    } else {
        bgr_out = result_bgr_f.clone();
    }

    final_img = bgr_out;

    // Postprocesado adaptativo preservando bordes
    //cv::bilateralFilter(bgr_out, final_img, 5, 0.1*255, 15);
    //Filtro de realce (unsharp mask)
    //cv::Mat sharp, blurred;
    //cv::GaussianBlur(bgr_out, blurred, cv::Size(0, 0), 1.0);
    //cv::addWeighted(bgr_out, 1.5, blurred, -0.5, 0, sharp);
    //final_img = sharp;

    return final_img;
}

static cv::Mat combineImagesSmartMono(const std::vector<cv::Mat> &images_,
                                      //const std::vector<double> &distances,
                                      const std::vector<cv::Point2f> &centers,
                                      float weight_intensity = 0.7f,
                                      float weight_distance = 0.3f)
{
    if (images_.empty()) return cv::Mat();

    float nodata_value;
    if (images_[0].type() == CV_32F)
        nodata_value = tl::NoData<float>;
    //else if (images_[0].type() == CV_64F)
    //    nodata_value = tl::NoData<double>;
    else
        nodata_value = 0.f;

    std::vector<cv::Mat> images(images_.size());

    for (size_t i = 0; i < images_.size(); ++i) {
        if (images_[i].depth() == CV_32F) {
            images[i] = images_[i];
        } else if (images_[i].depth() == CV_16U) {
            // Para imágenes de 16 bits: normalizar a [0,1]
            images_[i].convertTo(images[i], CV_32F, 1.0 / 65535.0);
        } else {
            TL_THROW_EXCEPTION("Unsupported image depth");
        }
    }

    size_t images_size = images.size();
    TL_ASSERT(images_size == centers.size(), "Number of images and centers must match");

    // Verificación de dimensiones y formato
    int rows = images[0].rows;
    int cols = images[0].cols;
    int depth = images[0].depth();
    int channels = images[0].channels();
    int type = images[0].type();

    TL_ASSERT(channels == 1, "combineImagesSmartMono only supports single-channel images");

    for (size_t i = 1; i < images_size; ++i) {
        TL_ASSERT(images[i].rows == rows && images[i].cols == cols, "All images must have same size");
        TL_ASSERT(images[i].channels() == channels, "All images must have same number of channels");
        TL_ASSERT(images[i].depth() == depth, "All images must have the same bit depth");
    }

    // Máscaras de píxeles válidos (no negros)
    std::vector<cv::Mat> valid_masks(images_size);
    for (size_t i = 0; i < images_size; ++i) {
        cv::Mat mask_is_nodata;
        cv::compare(images[i], nodata_value, mask_is_nodata, cv::CMP_EQ);
        cv::bitwise_not(mask_is_nodata, valid_masks[i]);
    }

    // Normalizar distancias
    //double min_distance = *std::min_element(distances.begin(), distances.end());
    //double max_distance = *std::max_element(distances.begin(), distances.end());
    //double distance_range = (max_distance > min_distance) ? (max_distance - min_distance) : 1.0;

    //std::vector<float> normalized_distances(images_size);
    //for (size_t i = 0; i < images_size; ++i)
    //    normalized_distances[i] = static_cast<float>((distances[i] - min_distance) / distance_range);

    float max_dist_ref = std::sqrt((float)(rows * rows + cols * cols)) * 1.5f;

    cv::Mat result(rows, cols, type, cv::Scalar(nodata_value));
    //cv::Mat assigned(rows, cols, CV_8U, cv::Scalar(0));

    // Lambda para mediana robusta
    // Codigo repetido. Sacar a función o utilizar median de TidopLib
    auto median_of = [](std::vector<float> &v) -> float {
        size_t n = v.size(); 

        if (n == 0) return 0.0f;

        size_t mid = n / 2;
        std::nth_element(v.begin(), v.begin() + mid, v.end());
        float med = v[mid];
        if (n % 2 == 0) {
            float left = *std::max_element(v.begin(), v.begin() + mid);
            med = 0.5f * (med + left);
        }
        return med;
    };

    tl::parallel_for(0, rows, [&](size_t r) {

        std::vector<float> samples;
        std::vector<float> intensity_diffs;
        samples.reserve(images_size);
        intensity_diffs.reserve(images_size);

        for (int c = 0; c < cols; ++c) {

            samples.clear();

            // Recolectar intensidades válidas
            std::vector<size_t> valid_idxs;
            valid_idxs.reserve(images_size);
            for (size_t i = 0; i < images_size; ++i) {
                if (valid_masks[i].at<uchar>(static_cast<int>(r), c)) {
                    float val = images[i].at<float>(static_cast<int>(r), c);
                    samples.push_back(val);
                    valid_idxs.push_back(i);
                }
            }

            if (valid_idxs.empty()) continue;

            // Calcular mediana
            std::vector<float> tmp = samples;
            float median = median_of(tmp);

            // Calcular distancias de intensidad a la mediana
            intensity_diffs.assign(images_size, std::numeric_limits<float>::infinity());
            for (int idx : valid_idxs) {
                float val = images[idx].at<float>(static_cast<int>(r), c);
                intensity_diffs[idx] = std::fabs(val - median);
            }

            float min = std::numeric_limits<float>::infinity();
            float max = std::numeric_limits<float>::lowest();
            for (int idx : valid_idxs) {
                float d = intensity_diffs[idx];
                if (d < min) min = d;
                if (d > max) max = d;
            }
            float range = (max > min) ? (max - min) : 1.0f;

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

            std::vector<float> intensity_diffs_copy;
            intensity_diffs_copy.reserve(valid_idxs.size());
            for (int idx : valid_idxs) {
                intensity_diffs_copy.push_back(intensity_diffs[idx]);
            }
            // Calculamos la mediana de las desviaciones (MAD)
            float mad_val = median_of(intensity_diffs_copy);

            //float noise_floor = 15.0f;
            float noise_floor = 0.1f; // Ajustado para valores normalizados [0,1]
            float outlier_threshold = std::max(noise_floor, 3.0f * mad_val);

            // Calcular score de cada imagen válida
            std::vector<std::pair<float, int>> scored;
            scored.reserve(valid_idxs.size());

            // con distancia
            //for (size_t i = 0; i < images_size; ++i) {
            //    if (intensity_diffs[i] == std::numeric_limits<float>::infinity()) continue;
            //    float colorNorm = (intensity_diffs[i] - minC) / rangeC;
            //    float score = weight_intensity * colorNorm + weight_distance * normalized_distances[i];
            //    scored.emplace_back(score, static_cast<int>(i));
            //}

            for (int idx : valid_idxs) {

                if (intensity_diffs[idx] > outlier_threshold) continue;

                // 1. Distancia de intensidad normalizada (0..1)
                float intensityNorm = (intensity_diffs[idx] - min) / range;
                // 2. Distancia espacial pixel a centro
                float dx = c - centers[idx].x;
                float dy = r - centers[idx].y;
                float dist_pixel = std::sqrt(dx * dx + dy * dy);
                float distNorm = dist_pixel / max_dist_ref;

                // Score final
                float score = weight_intensity * intensityNorm + weight_distance * distNorm;
                scored.emplace_back(score, idx);

            }

            if (scored.empty()) continue;

            // Ordenar por score ascendente (menor es mejor)
            std::sort(scored.begin(), scored.end(),
                [](const auto &a, const auto &b) { return a.first < b.first; });

            // Número de mejores píxeles a usar
            const int TOP_N = 5;
            int n = std::min<int>(TOP_N, scored.size());

            // Calcular media ponderada de los N mejores
            //float total_w = 0.f;
            //float sum_val = 0.f;

            //float weighted_val;

            if (n == 1) {
                //weighted_val = images[scored[0].second].at<float>(static_cast<int>(r), c);
                result.at<float>(static_cast<int>(r), c) = images[scored[0].second].at<float>(static_cast<int>(r), c);
            } else {

                float total_w = 0.f;
                float sum_val = 0.f;

                for (int i = 0; i < n; ++i) {
                    size_t idx = static_cast<size_t>(scored[i].second);
                    float score = scored[i].first;
                    //float w = 1.0f;
                    float w = 1.0f / (1e-3f + score); // peso inverso al score
                    //float w = 1.0f / (1e-3f + score + 0.1f * normalized_distances[idx]); // No veo que mejore 
                    // Se nota mucho mas el efecto
                    //const float ALPHA = 15.0f; // Cuanto más bajo, más suave la transición.
                    //float w = std::exp(-ALPHA * score * score); // El peso es más grande si el score es bajo.
                    float val = images[idx].at<float>(static_cast<int>(r), c);
                    sum_val += val * w;
                    total_w += w;
                }

                float weighted_val = (total_w > 0.f) ? (sum_val / total_w)
                   : images[scored[0].second].at<float>(static_cast<int>(r), c);

                result.at<float>(static_cast<int>(r), c) = weighted_val;
            }

            //result.at<float>(static_cast<int>(r), c) = weighted_val;
            //assigned.at<uchar>(static_cast<int>(r), c) = 255;

        }
    });

    //// Rellenar píxeles sin asignar
    //cv::Mat dil;
    //cv::dilate(assigned, dil, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
    //tl::parallel_for(0, rows, [&](size_t r) {

    //    for (int c = 0; c < cols; ++c) {

    //        if (assigned.at<uchar>(static_cast<int>(r), c) || !dil.at<uchar>(static_cast<int>(r), c)) continue;

    //        float bestScore = std::numeric_limits<float>::infinity();
    //        int bestIdx = -1;
    //        for (size_t i = 0; i < images_size; ++i) {
    //            if (!valid_masks[i].at<uchar>(static_cast<int>(r), c)) continue;
    //            float score = weight_distance * normalized_distances[i];
    //            if (score < bestScore) {
    //                bestScore = score;
    //                bestIdx = static_cast<int>(i);
    //            }
    //        }

    //        if (bestIdx >= 0) {
    //            result.at<float>(static_cast<int>(y), x) = images[bestIdx].at<float>(static_cast<int>(y), x);
    //        }
    //    }
    //});

    if (images_[0].depth() == CV_16U) {
        cv::Mat result_16u;
        result.convertTo(result_16u, CV_16U, 65535.0);
        return result_16u;
    }

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
    mChannels(3),
    mMethod("Robust Local Blending")
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

        // Crear la malla de teselas
        std::vector<std::vector<tl::WindowD>> grid = findGrid(500);

        // Ventana ortomosaico
        mWindowAll.pt1.x = grid[0][0].pt1.x;
        mWindowAll.pt1.y = grid.back().back().pt1.y;
        mWindowAll.pt2.x = grid.back().back().pt2.x;
        mWindowAll.pt2.y = grid[0][0].pt2.y;

        // Transformación afín ortomosaico, 
        // Usar cuando se escriba la orto
        tl::Affine<double, 2> affine_ortho(mGSD, -mGSD, mWindowAll.pt1.x, mWindowAll.pt2.y, 0.0);

        OrthoimageTask orthoimage_task(mPhotos,
                                       mCameras,
                                       dsm_path,
                                       temp_path,
                                       graph_orthos,
                                       mEnuCrs,
                                       mEpsg,
                                       affine_ortho,
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

        

//        //TODO: Para imagenes térmicas no se debe aplicar compensación de exposición
//#ifndef FAST_ORTHO
//        if (mDataType == tl::DataType::TL_8U) {
//            orthoimageExposureCompensator(graph_orthos, progressBar);
//        }
//#endif
        
        if (mMethod == "Robust Local Blending") {
            auto orthos = searchTiles(graph_orthos, grid/*, 10*/);

            generateTiles(grid, orthos, progressBar);
            writeOrthomosaic(grid);
        } else {
            orthoMosaic(graph_orthos, mOrthoPath, mGSD, mEpsg, grid);
        }

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
//#ifndef FAST_ORTHO
//                        if (mDataType == tl::DataType::TL_8U) {
//                            std::string name = orto_compensate.baseName().toUtf8() + "_compensate.tif";
//                            orto_compensate.replaceFileName(name);
//                        }
//#endif
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
//#ifdef FAST_ORTHO
                const auto &window = grid[r][c];
//#else
//                // Solo expandir en el caso de la generación de mosaico con fusión. Si se hace con combineImagesSmart no haría falta
//                auto window = grid[r][c];
//                window = tl::expandWindow(window, ortho_overlap * mGSD);
//#endif

                int image_size = tl::numberCast<int>(window.width() / mGSD);

                // Todas las imagenes del elemento actual del grid
                std::vector<cv::Mat> images;
                // Cambio distances por centros proyectados para ser mas precisos en la selección del pixel optimo
                std::vector<cv::Point2f> centers_in_tile_coords;
                //std::vector<double> distances;

                for (auto &ortho : orthos[r][c]) {

                    try {

                        auto image_reader = tl::ImageReaderFactory::create(ortho.second);
                        image_reader->open();
                        if (!image_reader->isOpen()) {
                            tl::Message::error("Image open error :{}", ortho.second);
                            continue;
                        }

                        double src_w = image_reader->cols();
                        double src_h = image_reader->rows();
                        auto georef = image_reader->georeference().inverse();

                        auto ortoimage_center = image_reader->window().center();
                        auto tile_center = window.center();

                        double center_x_tile = (ortoimage_center.x - tile_center.x) / mGSD;
                        double center_y_tile = (tile_center.y - ortoimage_center.y) / mGSD;


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
                        // Guardamos el punto central relativo al (0,0) de la tesela actual
                        centers_in_tile_coords.push_back(cv::Point2f(static_cast<float>(center_x_tile), static_cast<float>(center_y_tile)));
                        //distances.push_back(ortho.first);
                    
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
//#ifdef FAST_ORTHO

                    if (mChannels == 1) {
                        read_image = combineImagesSmartMono(images, centers_in_tile_coords);
                    } else if (mChannels == 3) {
                        read_image = combineImagesSmart(images, centers_in_tile_coords);
                    } else {
                        /// TODO: Contemplar otros casos
                    }

//#else
//                    read_image = combineImages(images);
//#endif
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


//#ifdef FAST_ORTHO

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

        // Write overviews

        addOverviewsToImage(mOrthoPath.toString());

    } catch (std::exception &e) {
        tl::printException(e);
    }
}
//
//#else
//
//
//
//
//static cv::Mat createWeightMapTile(int w = ortho_tile_w, int h = ortho_tile_h, int overlap = ortho_overlap)
//{
//    cv::Mat weight(h, w, CV_32F);
//
//    for (int y = 0; y < h; ++y) {
//
//        float dy = std::min<float>(y, h - 1 - y);
//        float wy = std::min(1.0f, dy / float(overlap));
//
//        for (int x = 0; x < w; ++x) {
//
//            float dx = std::min<float>(x, w - 1 - x);
//            float wx = std::min(1.0f, dx / float(overlap));
//            weight.at<float>(y, x) = wx * wy;
//
//        }
//    }
//
//    return weight;
//}
//
//
//static cv::Mat maskToFloat(const cv::Mat &mask8u)
//{
//    cv::Mat f;
//    mask8u.convertTo(f, CV_32F, 1.0 / 255.0);
//    return f;
//}
//
//void OrthophotoTask::writeOrthomosaic(const std::vector<std::vector<tl::WindowD>> &grid)
//{
//    try {
//
//        tl::Message::info("Writing ortho image (blended)");
//
//        //tl::Path ortho_final(mOrthoPath);
//        //ortho_final.append("ortho.tif");
//        std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(mOrthoPath);
//        image_writer->open();
//
//        int cols = static_cast<int>(std::round(mWindowAll.width() / mGSD));
//        int rows = static_cast<int>(std::round(mWindowAll.height() / mGSD));
//
//        auto options = std::make_shared<tl::TiffOptions>();
//        options->enableTiled(true);
//        options->setBigTiff(tl::TiffOptions::BigTiff::if_needed);
//        options->setCompress(tl::TiffOptions::Compress::lzw);
//
//        if (!image_writer->isOpen()) {
//            tl::Message::error("Cannot open image writer for {}", mOrthoPath.toUtf8());
//            return;
//        }
//
//        image_writer->create(rows, cols, mChannels, mDataType, options);
//
//        auto metadata = std::make_shared<tl::ImageMetadata>();
//        metadata->setMetadata("TIFFTAG_DOCUMENTNAME", "Orthomosaic");
//        metadata->setMetadata("TIFFTAG_IMAGEDESCRIPTION", "Orthomosaic generated by GRAPHOS");
//        metadata->setMetadata("TIFFTAG_SOFTWARE", "GRAPHOS");
//        QString tiffDateTime = QDateTime::currentDateTime().toString("yyyy:MM:dd HH:mm:ss");
//        metadata->setMetadata("TIFFTAG_DATETIME", tiffDateTime.toStdString());
//        image_writer->setMetadata(metadata);
//        tl::Crs crs(mEpsg);
//        image_writer->setCRS(crs.toWktFormat());
//        tl::Affine<double, 2> affine_ortho(mGSD, -mGSD, mWindowAll.pt1.x, mWindowAll.pt2.y, 0.0);
//        image_writer->setGeoreference(affine_ortho);
//        auto affine_ortho_inverse = affine_ortho.inverse();
//
//        cv::Mat base_weight = createWeightMapTile(ortho_tile_w, ortho_tile_h, ortho_overlap);
//
//        // Ajustar block_height según memoria
//        const int block_height = 2048;
//        for (int block_y = 0; block_y < rows; block_y += block_height) {
//
//            int current_block_h = std::min(block_height, rows - block_y);
//
//            cv::Mat accumulator(current_block_h, cols, CV_32FC(mChannels), cv::Scalar(0));
//            cv::Mat weight_sum(current_block_h, cols, CV_32F, cv::Scalar(0));
//
//            // Recorremos todas las tiles y acumulamos las que intersectan este bloque
//            for (size_t r = 0; r < grid.size(); ++r) {
//                for (size_t c = 0; c < grid[r].size(); ++c) {
//
//                    try {
//
//                        tl::Path tile(mOrthoPath.parentPath());
//                        tile.append("temp");
//                        tile.append(std::to_string(r));
//                        tile.append(std::to_string(c));
//                        tile.append("t.tif");
//                        if (!tile.exists()) continue;
//
//                        auto image_reader = tl::ImageReaderFactory::create(tile);
//                        image_reader->open();
//                        if (!image_reader->isOpen()) continue;
//
//                        auto tile_window = image_reader->window();
//                        const auto &window = grid[r][c];
//
//                        if (!intersectWindows(tile_window, window)) continue;
//
//                        cv::Mat tile_image = image_reader->read();
//
//                        int tile_width = tile_image.cols;
//                        int tile_height = tile_image.rows;
//                        cv::Mat tile_weights;
//                        if (tile_width == ortho_tile_w && tile_height == ortho_tile_h) {
//                            tile_weights = base_weight;
//                        } else {
//                            tile_weights = createWeightMapTile(tile_width, tile_height, ortho_overlap);
//                        }
//
//                        // Máscara de píxeles negros (0 = negro, 255 = válido)
//                        cv::Mat black_pixel_mask = createBlackPixelMask(tile_image, 1024);
//                        cv::Mat valid_mask;
//                        cv::bitwise_not(black_pixel_mask, valid_mask);
//                        black_pixel_mask.release();
//
//                        cv::Mat valid_mask_float = maskToFloat(valid_mask);
//                        valid_mask.release();
//
//                        cv::Mat weight_masked;
//                        cv::multiply(tile_weights, valid_mask_float, weight_masked);
//                        valid_mask_float.release();
//
//                        auto p1_ortho = affine_ortho_inverse.transform(tile_window.pt1);
//                        auto p2_ortho = affine_ortho_inverse.transform(tile_window.pt2);
//                        tl::WindowI window_to_write(static_cast<tl::Point<int>>(p1_ortho), static_cast<tl::Point<int>>(p2_ortho));
//                        window_to_write.normalized();
//                        if (!window_to_write.isValid()) continue;
//
//                        cv::Rect dst_rect(window_to_write.pt1.x, window_to_write.pt1.y, window_to_write.width(), window_to_write.height());
//
//                        cv::Rect inter = dst_rect & cv::Rect(0, block_y, cols, current_block_h);
//                        if (inter.width <= 0 || inter.height <= 0) continue;
//
//                        int src_x = inter.x - dst_rect.x;
//                        int src_y = inter.y - dst_rect.y;
//                        cv::Rect src_rect(src_x, src_y, inter.width, inter.height);
//                        cv::Rect accumulator_rect(inter.x, inter.y - block_y, inter.width, inter.height);
//
//                        cv::Mat tile_float;
//                        tile_image.convertTo(tile_float, CV_32F);
//
//                        cv::Mat tile_patch = tile_float(src_rect); 
//                        cv::Mat weight_patch = weight_masked(src_rect);
//
//                        std::vector<cv::Mat> weight_channels(mChannels);
//                        for (int ch = 0; ch < mChannels; ++ch) weight_channels[ch] = weight_patch;
//                        cv::Mat weightPatch3;
//                        cv::merge(weight_channels, weightPatch3);
//
//                        cv::Mat accumulator_roi = accumulator(accumulator_rect);
//                        cv::Mat weighted;
//                        cv::multiply(tile_patch, weightPatch3, weighted);
//                        accumulator_roi += weighted;
//
//                        cv::Mat wsumROI = weight_sum(accumulator_rect);
//                        wsumROI += weight_patch;
//
//                    } catch (std::exception &e) {
//                        tl::printException(e);
//                    }
//                }
//            }
//
//            std::vector<cv::Mat> wv(mChannels, cv::Mat());
//            for (int ch = 0; ch < mChannels; ++ch) wv[ch] = weight_sum;
//            cv::Mat denom;
//            cv::merge(wv, denom);
//
//
//            cv::Mat denomSafe = denom.clone();
//
//            cv::Mat zeroMask;
//            cv::compare(denomSafe, 0.0f, zeroMask, cv::CMP_EQ);
//            denomSafe.setTo(1.0f, zeroMask);
//
//            cv::Mat target = accumulator / denomSafe;
//
//            cv::Mat target_out;
//            switch (mDataType) {
//                case tl::DataType::TL_8U:  
//                    target.convertTo(target_out, CV_8U); 
//                    break;
//                case tl::DataType::TL_16U: 
//                    target.convertTo(target_out, CV_16U); 
//                    break;
//                case tl::DataType::TL_32F: 
//                    target_out = target.clone(); 
//                    break;
//                case tl::DataType::TL_64F:
//                    target.convertTo(target_out, CV_64F);
//                    break;
//                default: 
//                    target.convertTo(target_out, CV_32F);
//                    break;
//            }
//
//            tl::WindowI write_window(tl::Point<int>(0, block_y), tl::Point<int>(cols, block_y + current_block_h));
//
//            image_writer->write(target_out, write_window);
//        }
//
//    } catch (std::exception &e) {
//        tl::printException(e);
//    }
//}
//#endif

//
//void OrthophotoTask::orthoimageExposureCompensator(const tl::Path &graph_orthos, tl::Progress *progressBar)
//{
//
//    try {
//
//        tl::ChronoAuto chrono("Exposure compensator");
//
//        tl::WindowD window_all;
//        std::vector<cv::Point> corners;
//        std::vector<cv::Mat> mat_orthos;
//        std::vector<cv::UMat> umat_orthos;
//        std::vector<cv::UMat> ortho_masks;
//
//        //int type = cv::detail::ExposureCompensator::NO;
//        int type = cv::detail::ExposureCompensator::GAIN;
//        //int type = cv::detail::ExposureCompensator::GAIN_BLOCKS;
//        //int type = cv::detail::ExposureCompensator::CHANNELS;
//        //int type = cv::detail::ExposureCompensator::CHANNELS_BLOCKS;
//        cv::Ptr<cv::detail::ExposureCompensator> compensator = cv::detail::ExposureCompensator::createDefault(type);
//
//        std::unique_ptr<tl::VectorReader> vectorReader;
//        vectorReader = tl::VectorReaderFactory::create(graph_orthos);
//        vectorReader->open();
//
//        if (vectorReader->isOpen()) {
//
//            if (vectorReader->layersCount() >= 1) {
//
//                std::map<double, std::shared_ptr<tl::GPolygon>> entities;
//                std::shared_ptr<tl::GLayer> layer = vectorReader->read(0);
//
//                for (const auto &entity : *layer) {
//
//                    tl::GraphicEntity::Type type = entity->type();
//                    if (type == tl::GraphicEntity::Type::polygon_2d) {
//
//                        /// se carga la primera imagen y se busca las que intersectan
//                        std::shared_ptr<tl::GPolygon> polygon = std::dynamic_pointer_cast<tl::GPolygon>(entity);
//                        std::shared_ptr<tl::TableRegister> data = polygon->data();
//                        std::string ortho_to_compensate = data->value(0);
//                        tl::WindowD window = polygon->window();
//                        tl::Point<double> center = window.center();
//
//                        /// Busqueda de imagenes que intersectan
//
//                        std::vector<std::string> orthos;
//                        orthos.push_back(ortho_to_compensate);
//                        std::vector<tl::WindowD> windows;
//                        windows.push_back(window);
//
//                        for (const auto &entity2 : *layer) {
//                            std::shared_ptr<tl::GPolygon> polygon2 = std::dynamic_pointer_cast<tl::GPolygon>(entity2);
//                            std::shared_ptr<tl::TableRegister> data = polygon2->data();
//                            std::string orto = data->value(0);
//
//                            if (orto != ortho_to_compensate) {
//
//                                if (polygon2->isInner(center)) {
//                                    orthos.push_back(orto);
//                                    windows.push_back(polygon2->window());
//                                    window_all = joinWindow(window_all, polygon2->window());
//                                }
//
//                            }
//
//                        }
//
//                        size_t n_orthos = orthos.size();
//                        corners.resize(n_orthos);
//                        ortho_masks.resize(n_orthos);
//                        umat_orthos.resize(n_orthos);
//                        mat_orthos.resize(n_orthos);
//
//                        /// Aplicar un factor de escala para el calculo de la compensación de exposición
//                        for (size_t i = 0; i < n_orthos; i++) {
//
//                            try {
//                                std::unique_ptr<tl::ImageReader> image_reader = tl::ImageReaderFactory::create(orthos[i]);
//                                image_reader->open();
//                                if (image_reader->isOpen()) {
//                                    cv::Mat image = image_reader->read(exposure_compensator_factor, exposure_compensator_factor);
//                                    mat_orthos[i] = image.clone();
//                                    //mat_orthos[i].convertTo(mat_orthos[i], CV_32F, 1.0 / 255.0);
//                                    double scale = image_reader->georeference().scale().x();
//
//                                    /// Esquinas
//                                    corners[i].x = tl::roundToInteger((windows[i].pt1.x - window_all.pt1.x) * exposure_compensator_factor / scale);
//                                    corners[i].y = tl::roundToInteger((window_all.pt2.y - windows[i].pt2.y) * exposure_compensator_factor / scale);
//
//                                    /// La mascara debería leerse si se creó en la generación del MDS.
//                                    //ortho_masks[i].create(image.size(), CV_8U);
//                                    //cv::Mat gray;
//                                    //if (image.channels() != 1) {
//                                    //    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
//                                    //} else {
//                                    //    gray = image;
//                                    //}
//                                    ortho_masks[i] = cv::UMat(image.size(), CV_8U);
//                                    cv::inRange(image, cv::Scalar::all(1), cv::Scalar::all(255), ortho_masks[i]); // Más robusto
//
//                                    //ortho_masks[i].setTo(cv::Scalar::all(0));
//                                    //ortho_masks[i].setTo(cv::Scalar::all(255), gray > 0);
//
//                                }
//                            } catch (std::exception &e) {
//                                tl::printException(e);
//                            }
//                        }
//
//                        cv::InputArrayOfArrays(mat_orthos).getUMatVector(umat_orthos);
//                        compensator->feed(corners, umat_orthos, ortho_masks);
//
//                        tl::Message::info("Seam finder");
//
//                        cv::Ptr<cv::detail::SeamFinder> seam_finder;
//                        //seam_finder = cv::makePtr<cv::detail::NoSeamFinder>();
//                        //seam_finder = cv::makePtr<cv::detail::VoronoiSeamFinder>();
//                        //seam_finder = cv::makePtr<cv::detail::DpSeamFinder>(cv::detail::DpSeamFinder::COLOR);
//                        seam_finder = cv::makePtr<cv::detail::DpSeamFinder>(cv::detail::DpSeamFinder::COLOR_GRAD);
//                        seam_finder->find(umat_orthos, corners, ortho_masks);
//                        umat_orthos.clear();
//                        mat_orthos.clear();
//
//                        // Guardar las máscaras para debug
//                        
//                        for (size_t i = 0; i < n_orthos; ++i) {
//                            tl::Path mask_path(ortho_to_compensate);
//                            std::string name = mask_path.baseName().toUtf8() + "_mask.tif";
//                            mask_path.replaceBaseName(name);
//                            cv::imwrite(mask_path.toString(), ortho_masks[i]);
//                        }
//
//                        std::unique_ptr<tl::ImageReader> image_reader = tl::ImageReaderFactory::create(ortho_to_compensate);
//                        image_reader->open();
//                        if (image_reader->isOpen()) {
//
//                            cv::Mat compensate_image = image_reader->read();
//
//                            /// Se compensa la imagen
//                            cv::Point corner = corners[0] / exposure_compensator_factor;
//
//                            //cv::Mat gray;
//                            //if (compensate_image.channels() == 1)
//                            //    gray = compensate_image;
//                            //else
//                            //    cv::cvtColor(compensate_image, gray, cv::COLOR_BGR2GRAY);
//                            //cv::Mat mask_full_size(compensate_image.size(), CV_8U);
//                            //mask_full_size.setTo(cv::Scalar::all(0));
//                            //mask_full_size.setTo(cv::Scalar::all(255), gray > 0);
//
//                            // Máscara completa (rellena donde hay píxeles válidos)
//                            cv::Mat mask_full(compensate_image.size(), CV_8U);
//                            cv::inRange(compensate_image, cv::Scalar::all(1), cv::Scalar::all(255), mask_full);
//
//                            //cv::Mat element = getStructuringElement(cv::MorphShapes::MORPH_RECT,
//                            //                                        cv::Size(2 * 2 + 1, 2 * 2 + 1),
//                            //                                        cv::Point(2, 2));
//                            //Revisar
//                            //cv::erode(mask_full_size, mask_full_size, element);
//                            //cv::dilate(mask_full_size, mask_full_size, element);
//
//                            // Aplicar compensación
//                            compensator->apply(0, corner, compensate_image, mask_full);
//
//                            // Recortar usando la máscara de seam (muy importante)
//                            cv::Mat seam_mask_resized;
//                            cv::resize(ortho_masks[0], seam_mask_resized, compensate_image.size(), 0, 0, cv::INTER_NEAREST);
//
//                            // Aplicar seam final a la imagen compensada
//                            cv::Mat final_compensated;
//                            compensate_image.copyTo(final_compensated, seam_mask_resized);
//
//
//                            tl::Path orto_compensate(ortho_to_compensate);
//                            std::string name = orto_compensate.baseName().toUtf8() + "_compensate.tif";
//                            orto_compensate.replaceFileName(name);
//                            std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(orto_compensate);
//                            image_writer->open();
//                            if (image_writer->isOpen()) {
//                                image_writer->create(image_reader->rows(), image_reader->cols(), image_reader->channels(), image_reader->dataType());
//                                image_writer->setCRS(image_reader->crsWkt());
//                                image_writer->setGeoreference(image_reader->georeference());
//                                image_writer->write(final_compensated);
//                                image_writer->close();
//                                tl::Message::info("Compensated image: {}", orto_compensate.fileName().toUtf8());
//                            }
//                        }
//
//                        if (progressBar) (*progressBar)();
//                    }
//                }
//            }
//        }
//
//    } catch (...) {
//        TL_THROW_EXCEPTION_WITH_NESTED("");
//    }
//
//}


} // namespace graphos
