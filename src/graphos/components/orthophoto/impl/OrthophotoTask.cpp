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
#include "graphos/core/image/Image.h"
#include "graphos/core/utils.h"
#include "graphos/core/ortho/SbbProperties.h"
#include "graphos/components/orthophoto/impl/OrthoimageTask.h"

/* TidopLib */
#include <tidop/core/base/Exception.h>
#include <tidop/core/task/Progress.h>
#include <tidop/core/base/Chrono.h>
#include <tidop/core/app/Message.h>
#include <tidop/geospatial/crs.h>
#include <tidop/vectortools/io/Reader.h>
#include <tidop/vectortools/io/Writer.h>
#include <tidop/graphic/model/GLayer.h>
#include <tidop/geometry/algorithms/analysis/Intersects.h>
#include <tidop/geometry/algorithms/analysis/Contains.h>
#include <tidop/geometry/algorithms/measurement/Distance.h>
#include <tidop/geometry/algorithms/spatial/Intersection.h>
#include <tidop/rastertools/io/Reader.h>
#include <tidop/rastertools/io/Writer.h>
#include <tidop/rastertools/io/Formats.h>
#include <tidop/math/statistic/algorithms/descriptive/Median.h>
#include <tidop/geotools/CRSsTools.h>
#include <tidop/geotools/GeoTools.h>

/* OpenCV */
#include <opencv2/stitching.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>

#include <QDateTime>

#include <map>
#include <stdexcept>
#include <utility>

namespace graphos
{

constexpr double exposure_compensator_factor = 0.25;
//constexpr int ortho_tile_w = 600;
//constexpr int ortho_tile_h = 600;
//#ifndef FAST_ORTHO
constexpr auto ortho_overlap = 50.;
//#else
//constexpr int ortho_overlap = 0;
//#endif


static void orthoMosaic(tl::Path &optimal_footprint_path,
                        tl::Path &ortho_path,
                        double res_ortho,
                        const std::string &epsg,
                        std::vector<std::vector<tl::BoundingBox2d>> &grid,
                        tl::DataType dataType,
                        int channels,
                        const SbbProperties *sbbConfig = nullptr,
                        bool try_cuda = false)
{
    try {

        tl::Message::info("Starting orthoMosaic function");
        tl::Message::info("Input parameters - dataType: {}, channels: {}, res_ortho: {}", static_cast<int>(dataType), channels, res_ortho);
        tl::Message::info("Optimal footprint path: {}", optimal_footprint_path.toString());
        tl::Message::info("Grid size: {}x{}", grid.size(), grid.empty() ? 0 : grid[0].size());

        tl::BoundingBox2d window_all;
        std::vector<std::pair<std::string, std::string>> compensated_orthos;

        tl::Message::info("Step 1: Exposure compensator initialization");

        // Usar GAIN_BLOCKS que funciona mejor para mosaicos
        //int type = cv::detail::ExposureCompensator::GAIN_BLOCKS;
        int type = cv::detail::ExposureCompensator::GAIN_BLOCKS;
        if (sbbConfig) {
            std::string exposure_compensator = sbbConfig->exposureCompensator();
            if (exposure_compensator == "GAIN") 
                type = cv::detail::ExposureCompensator::GAIN;
            else if (exposure_compensator == "GAIN_BLOCKS") 
                type = cv::detail::ExposureCompensator::GAIN_BLOCKS;
            else if (exposure_compensator == "NO") 
                type = cv::detail::ExposureCompensator::NO;
            else if (exposure_compensator == "CHANNELS") 
                type = cv::detail::ExposureCompensator::CHANNELS;
            else if (exposure_compensator == "CHANNELS_BLOCKS") 
                type = cv::detail::ExposureCompensator::CHANNELS_BLOCKS;
        }

        cv::Ptr<cv::detail::ExposureCompensator> compensator = cv::detail::ExposureCompensator::createDefault(type);

        tl::Message::info("Step 2: Opening vector reader");
        tl::VectorReader vector_reader(optimal_footprint_path);

        TL_ASSERT(vector_reader.isOpen(), "Cannot open vector reader: {}", optimal_footprint_path.toString());
        TL_ASSERT(vector_reader.layersCount() >= 1, "No layers found in footprint");

        tl::Message::info("Step 3: Reading layer");
        std::shared_ptr<tl::GLayer> layer = vector_reader.read(0);
        tl::Message::info("Layer read successfully, entities: {}", layer->size());

        // Primera pasada: calcular ventana global completa
        for (const auto &entity : *layer) {
            if (entity->type() != tl::GraphicEntity::Type::polygon_2d) {
                continue;
            }
            auto polygon = dynamic_cast<tl::GPolygon *>(entity.get());
            const std::string ortho_path = polygon->attributes().value(0);
            tl::RasterReader image_reader(ortho_path);
            if (!image_reader.isOpen()) {
                tl::Message::warning("Cannot open ortho image while calculating mosaic bounds: {}", ortho_path);
                continue;
            }
            window_all = tl::merge(window_all, image_reader.window());
            image_reader.close();
        }
        TL_ASSERT(window_all.isValid(), "No valid ortho image bounds found");

        tl::Message::info("Global window: [{}, {}] - [{}, {}]",
            window_all.min().x(), window_all.min().y(),
            window_all.max().x(), window_all.max().y());

        // Segunda pasada: procesar todas las imágenes
        std::vector<std::string> all_orthos;
        std::vector<tl::BoundingBox2d> all_windows;
        std::vector<cv::Point> all_corners;
        std::vector<cv::UMat> all_images_comp;
        std::vector<cv::UMat> all_masks_comp;

        for (const auto &entity : *layer) {
            if (entity->type() != tl::GraphicEntity::Type::polygon_2d) continue;

            auto polygon = dynamic_cast<tl::GPolygon *>(entity.get());
            tl::TableRegister data = polygon->attributes();
            std::string ortho_path_str = data.value(0);

            try {
                tl::RasterReader image_reader(ortho_path_str);
                if (!image_reader.isOpen()) {
                    tl::Message::warning("Cannot open ortho image: {}", ortho_path_str);
                    continue;
                }

                // Leer imagen a escala reducida para compensación
                cv::Mat image = image_reader.read(exposure_compensator_factor, exposure_compensator_factor);
                if (image.empty()) {
                    tl::Message::warning("Empty image: {}", ortho_path_str);
                    image_reader.close();
                    continue;
                }

                // relleno de huecos
                cv::Mat blackPixelMask = createBlackPixelMask(image, 32);
                cv::inpaint(image, blackPixelMask, image, 3, cv::INPAINT_TELEA);

                double scale = image_reader.georeference().scale().x();
                tl::BoundingBox2d window = image_reader.window();

                // Calcular esquina en coordenadas de la ventana global
                cv::Point corner;
                corner.x = tl::roundToInteger((window.min().x() - window_all.min().x()) * exposure_compensator_factor / scale);
                corner.y = tl::roundToInteger((window_all.max().y() - window.max().y()) * exposure_compensator_factor / scale);
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

                image_reader.close();

            } catch (std::exception &e) {
                tl::Message::warning("Error processing {}: {}", ortho_path_str, e.what());
                continue;
            }
        }

        TL_ASSERT(!all_orthos.empty(), "No valid orthos found");

        // Aplicar compensación radiométrica
        tl::Message::info("Applying exposure compensation to {} images", all_orthos.size());
        compensator->feed(all_corners, all_images_comp, all_masks_comp);

        // Preparar imágenes y mascaras para seam finding
        std::vector<cv::UMat> all_images_seam;
        for (size_t i = 0; i < all_images_comp.size(); ++i) {
            cv::UMat compensated_image;
            all_images_comp[i].copyTo(compensated_image);
            compensator->apply(
                static_cast<int>(i), all_corners[i], compensated_image, all_masks_comp[i]);

            cv::UMat color_image;
            if (compensated_image.channels() == 1) {
                cv::cvtColor(compensated_image, color_image, cv::COLOR_GRAY2BGR);
            } else if (compensated_image.channels() == 4) {
                cv::cvtColor(compensated_image, color_image, cv::COLOR_BGRA2BGR);
            } else {
                TL_ASSERT(compensated_image.channels() == 3,
                    "Unsupported ortho channel count for seam finding: {}",
                    compensated_image.channels());
                compensated_image.copyTo(color_image);
            }

            cv::UMat img32f;
            color_image.convertTo(img32f, CV_32F);
            all_images_seam.push_back(img32f);
        }

        // Encontrar líneas de costura
        tl::Message::info("Seam finding");
        cv::Ptr<cv::detail::SeamFinder> seam_finder;

        try {
            
            

            if (sbbConfig) {

                auto seam_finder_type = sbbConfig->seamFinder();
                if (seam_finder_type == "GRAPH_CUT") {
                    seam_finder = cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinder::COST_COLOR_GRAD);
                } else {
                    seam_finder = cv::makePtr<cv::detail::VoronoiSeamFinder>();
                }

            } else {
                seam_finder = cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinder::COST_COLOR_GRAD);
            }

            seam_finder->find(all_images_seam, all_corners, all_masks_comp);

        } catch (const cv::Exception &e) {
            tl::Message::warning("GraphCut failed, using Voronoi: {}", e.what());
            seam_finder = cv::makePtr<cv::detail::VoronoiSeamFinder>();
            seam_finder->find(all_images_seam, all_corners, all_masks_comp);
        }

        // Aplicar compensación y guardar imágenes procesadas
        for (size_t i = 0; i < all_orthos.size(); ++i) {
            try {
                tl::RasterReader image_reader_full(all_orthos[i]);
                if (!image_reader_full.isOpen()) continue;

                cv::Mat full_img = image_reader_full.read();
                if (full_img.empty()) {
                    image_reader_full.close();
                    continue;
                }

                // Recalculate each image's position at full resolution.
                double scale_full = image_reader_full.georeference().scale().x();
                cv::Point corner_full;
                corner_full.x = tl::roundToInteger((all_windows[i].min().x() - window_all.min().x()) / scale_full);
                corner_full.y = tl::roundToInteger((window_all.max().y() - all_windows[i].max().y()) / scale_full);

                // Rellenar huecos negros en la imagen completa antes de la compensación
                cv::Mat blackPixelMask = createBlackPixelMask(full_img, 32);
                if (cv::countNonZero(blackPixelMask) > 0) {
                    cv::inpaint(full_img, blackPixelMask, full_img, 3, cv::INPAINT_TELEA);
                }
                
                // Crear máscara para imagen completa
                cv::Mat mask_full;
                if (full_img.channels() == 1) {
                    cv::threshold(full_img, mask_full, 1, 255, cv::THRESH_BINARY);
                } else {
                    cv::Mat gray;
                    cv::cvtColor(full_img, gray, cv::COLOR_BGR2GRAY);
                    cv::threshold(gray, mask_full, 1, 255, cv::THRESH_BINARY);
                }

                // Aplicar compensación
                compensator->apply(static_cast<int>(i), corner_full, full_img, mask_full);

                // Redimensionar máscara de seam a resolución completa
                cv::Mat seam_mask_full;
                if (!all_masks_comp[i].empty()) {
                    cv::resize(all_masks_comp[i], seam_mask_full, full_img.size(), 0, 0, cv::INTER_NEAREST);

                    // DILATACIÓN: Evita líneas negras en las costuras disjuntas
                    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9)); // Probar con un tamaño mayor si es necesario
                    cv::dilate(seam_mask_full, seam_mask_full, element);
                } else {
                    seam_mask_full = mask_full.clone();
                }

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

                tl::RasterWriter writer_comp(ortho_comp);
                tl::Path seam_path(all_orthos[i]);
                std::string seam_name = seam_path.baseName().toString() + "_seam.tif";
                seam_path.replaceFileName(seam_name);

                tl::RasterWriter writer_seam(seam_path.toString());
                TL_ASSERT(writer_comp.isOpen() && writer_seam.isOpen(), "Cannot create compensated image or seam mask");

                writer_comp.create(image_reader_full.rows(), image_reader_full.cols(), image_reader_full.channels(), image_reader_full.dataType());
                writer_comp.setCRS(image_reader_full.crsWkt());
                writer_comp.setGeoreference(image_reader_full.georeference());
                writer_comp.write(full_img);
                writer_comp.close();

                writer_seam.create(image_reader_full.rows(), image_reader_full.cols(), 1, tl::DataType::TL_8U);
                writer_seam.setCRS(image_reader_full.crsWkt());
                writer_seam.setGeoreference(image_reader_full.georeference());
                writer_seam.write(final_mask);
                writer_seam.close();

                compensated_orthos.emplace_back(ortho_comp.toString(), seam_path.toString());
                tl::Message::info("Saved compensated image and seam mask: {}", ortho_comp.toString());

                image_reader_full.close();

            } catch (std::exception &e) {
                tl::Message::warning("Error processing full resolution {}: {}", all_orthos[i], e.what());
            }
        }

        vector_reader.close();

        // BLENDING

        TL_ASSERT(!compensated_orthos.empty(), "No compensated orthos were written");

        tl::Message::info("Starting blending with {} images", compensated_orthos.size());

        int blender_type = cv::detail::Blender::FEATHER;
        float blend_strength = 2.f;
        if (sbbConfig) {
            auto blender = sbbConfig->blenderType();
            if (blender == "NO") {
                blender_type = cv::detail::Blender::NO;
            } else if (blender == "FEATHER") {
                blender_type = cv::detail::Blender::FEATHER;
            } else if (blender == "MULTI_BAND") {
                //blend_strength = 5.f;
                blender_type = cv::detail::Blender::MULTI_BAND;
            }
        }

        tl::RasterWriter image_writer(ortho_path);

        int cols = static_cast<int>(std::round(window_all.width() / res_ortho));
        int rows = static_cast<int>(std::round(window_all.height() / res_ortho));

        TL_ASSERT(image_writer.isOpen(), "Cannot open orthomosaic writer {}", ortho_path.toString());
        
        image_writer.create(rows, cols, channels, dataType);
        tl::Crs crs(epsg);
        image_writer.setCRS(crs.toWktFormat());
        tl::Affine<double, 2> affine_ortho(res_ortho, -res_ortho, window_all.min().x(), window_all.max().y(), 0.0);
        image_writer.setGeoreference(affine_ortho);

        for (size_t r = 0; r < grid.size(); ++r) {
            for (size_t c = 0; c < grid[r].size(); ++c) {
                tl::Message::info("Processing grid cell [{}, {}]", r, c);

                cv::Ptr<cv::detail::Blender> blender = cv::detail::Blender::createDefault(blender_type, try_cuda);

                int cols_grid = static_cast<int>(std::round(grid[r][c].width() / res_ortho));
                int rows_grid = static_cast<int>(std::round(grid[r][c].height() / res_ortho));
                cv::Rect rect(0, 0, cols_grid, rows_grid);

                float blend_width = sqrt(static_cast<float>(rect.area())) * blend_strength / 100.f;

                if (blend_width < 1.f || blender_type == cv::detail::Blender::NO) {
                    blender = cv::detail::Blender::createDefault(cv::detail::Blender::NO, try_cuda);
                } else if (blender_type == cv::detail::Blender::MULTI_BAND) {
                    cv::detail::MultiBandBlender *multi_band_blender = dynamic_cast<cv::detail::MultiBandBlender *>(blender.get());
                    int num_bands = static_cast<int>(ceil(log(blend_width) / log(2.)) - 1.);
                    num_bands = std::min(5, num_bands);
                    tl::Message::info("Multi-band blender with {} bands", num_bands);
                    multi_band_blender->setNumBands(num_bands);
                    //multi_band_blender->setNumBands(4);
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
                        tl::RasterReader image_reader(compensated_orthos[j].first);
                        tl::RasterReader image_reader_seam(compensated_orthos[j].second);

                        if (!image_reader.isOpen() || !image_reader_seam.isOpen()) {
                            continue;
                        }

                        //auto window = grid[r][c];
                        //window = tl::expand(window, ortho_overlap * res_ortho);

                        if (!tl::intersects(image_reader.window(), grid[r][c])) {
                            continue;
                        }

                        auto georef = image_reader.georeference().inverse();

                        tl::BoundingBox2i window_image;
                        const auto &window = grid[r][c];
                        window_image.min() = static_cast<tl::Point2i>(georef.transform(window.min()));
                        window_image.max() = static_cast<tl::Point2i>(georef.transform(window.max()));
                        window_image.normalized();
                        tl::Rect<int> rect_image(window_image.min().x(), window_image.min().y(), window_image.width(), window_image.height());
                        tl::Rect<int> rect_full_image(0, 0, image_reader.cols(), image_reader.rows());
                        tl::Rect<int> rect_to_read = tl::intersect(rect_full_image, rect_image);

                        if (!rect_to_read.isValid()) continue;

                        // Leer imagen compensada
                        cv::Mat compensate_image = image_reader.read(1., 1., rect_to_read);
                        cv::Mat seam_image = image_reader_seam.read(1., 1., rect_to_read);

                        if (compensate_image.empty() || seam_image.empty()) {
                            image_reader.close();
                            image_reader_seam.close();
                            continue;
                        }

                        // Convertir a 16S para el blender (OpenCV blender requiere CV_16S)
                        cv::Mat compensate_image_16s;
                        if (compensate_image.depth() == CV_8U) {
                            compensate_image.convertTo(compensate_image_16s, CV_16S);
                        } else if (compensate_image.depth() == CV_16U) {
                            compensate_image.convertTo(compensate_image_16s, CV_16S);
                        } else if (compensate_image.depth() == CV_32F) {
                            compensate_image.convertTo(compensate_image_16s, CV_16S, 255.0);
                        } else {
                            compensate_image.convertTo(compensate_image_16s, CV_16S);
                        }

                        if (seam_image.size() != compensate_image_16s.size()) {
                            cv::resize(seam_image, seam_image, compensate_image_16s.size(),
                                0.0, 0.0, cv::INTER_NEAREST);
                        }
                        if (seam_image.channels() == 3) {
                            cv::cvtColor(seam_image, seam_image, cv::COLOR_BGR2GRAY);
                        } else if (seam_image.channels() == 4) {
                            cv::cvtColor(seam_image, seam_image, cv::COLOR_BGRA2GRAY);
                        }
                        TL_ASSERT(seam_image.channels() == 1,
                            "Expected a single-channel seam mask, got {} channels from {}",
                            seam_image.channels(), compensated_orthos[j].second);
                        seam_image.convertTo(seam_image, CV_8U);
                        cv::threshold(seam_image, seam_image, 0, 255, cv::THRESH_BINARY);

                        auto offset = rect_to_read.topLeft() - rect_image.topLeft();
                        cv::Rect rct = cv::Rect(offset.x(),
                                                offset.y(),
                                                compensate_image_16s.cols,
                                                compensate_image_16s.rows);
                        blender->feed(compensate_image_16s, seam_image, rct.tl());
                        compensate_image.release();

                        image_reader.close();
                        image_reader_seam.close();

                    } catch (std::exception &e) {
                        tl::Message::warning("Error in blending for image {}: {}", compensated_orthos[j].first, e.what());
                        continue;
                    }
                }

                // Realizar blending
                cv::Mat ortho_blend, mask_blend;
                blender->blend(ortho_blend, mask_blend);

                if (!ortho_blend.empty()) {
                    // Convertir al tipo de datos correcto
                    cv::Mat ortho_blend_output;
                    if (dataType == tl::DataType::TL_8U) {
                        ortho_blend.convertTo(ortho_blend_output, CV_8U);
                    } else if (dataType == tl::DataType::TL_16U) {
                        ortho_blend.convertTo(ortho_blend_output, CV_16U, 255.0);
                    } else if (dataType == tl::DataType::TL_32F) {
                        ortho_blend.convertTo(ortho_blend_output, CV_32F, 1.0 / 255.0);
                    } else {
                        ortho_blend.convertTo(ortho_blend_output, CV_8U);
                    }

                    // Escribir en el mosaico final
                    auto affine_ortho_inverse = affine_ortho.inverse();
                    tl::Point2d pp1 = affine_ortho_inverse.transform(grid[r][c].min());
                    tl::Point2d pp2 = affine_ortho_inverse.transform(grid[r][c].max());
                    tl::BoundingBox2i window_to_write(static_cast<tl::Point2i>(pp1), static_cast<tl::Point2i>(pp2));
                    window_to_write.normalized();

                    if (window_to_write.isValid()) {

                        cv::Mat blackPixelMask = createBlackPixelMask(ortho_blend_output, 1024);
                        cv::inpaint(ortho_blend_output, blackPixelMask, ortho_blend_output, 3, cv::INPAINT_TELEA);

                        image_writer.write(ortho_blend_output, window_to_write);
                    }
                }

            }
        }

        image_writer.close();

        tl::Message::info("Mosaic completed: {}", ortho_path.toString());

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Orthophoto task error");
    }
}

static const tl::GPolygon* bestImage(const tl::Point2d &pt, std::shared_ptr<tl::GLayer> layer)
{
    const tl::GPolygon* footprint_image;

    std::map<double, const tl::GPolygon *> entities;

    for (const auto &entity : *layer) {
        tl::GraphicEntity::Type type = entity->type();
        if (type == tl::GraphicEntity::Type::polygon_2d) {

            auto polygon = dynamic_cast<tl::GPolygon *>(entity.get());
            //if (polygon->isInner(pt)) {
            if (tl::contains(polygon->geometry(), pt)) {
                tl::Point2d center = polygon->window().center();
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
                                 std::vector<tl::BoundingBox2d> &grid,
                                 const tl::Path &optimal_footprint_path,
                                 const tl::Crs &crs)
{
    std::map<std::string, const tl::GPolygon *> clean_footprint;
    ///....
    std::vector<std::pair<std::string, std::shared_ptr<tl::GPolygon>>> grid_images;
    ///....

    tl::VectorReader vectorReader(footprint_file);
    if (vectorReader.isOpen()) {

        if (vectorReader.layersCount() >= 1) {

            std::map<double, std::shared_ptr<tl::GPolygon>> entities;
            std::shared_ptr<tl::GLayer> layer = vectorReader.read(0);

            for (size_t i = 0; i < grid.size(); i++) {

                /// Busqueda de imagen mas centrada
                const tl::GPolygon *polygon = bestImage(grid[i].center(), layer);
                if (polygon) {
                    auto data = polygon->attributes();
                    std::string ortho_to_compensate = data.value(0);
                    clean_footprint[ortho_to_compensate] = polygon;

                    ///....
                    std::shared_ptr<tl::GPolygon> polygon = std::make_shared<tl::GPolygon>();
                    auto x_min = grid[i].min().x();
                    auto y_min = grid[i].min().y();
                    auto x_max = grid[i].max().x();
                    auto y_max = grid[i].max().y();

                    polygon->geometry().outer().push_back(tl::Point2d(x_min, y_min));
                    polygon->geometry().outer().push_back(tl::Point2d(x_max, y_min));
                    polygon->geometry().outer().push_back(tl::Point2d(x_max, y_max));
                    polygon->geometry().outer().push_back(tl::Point2d(x_min, y_max));

                    grid_images.push_back({ortho_to_compensate, polygon});
                    ///....
                }

            }

            vectorReader.close();

        }

        tl::Message::info("Optimal footprint. {} retained images", clean_footprint.size());
        {
            tl::VectorWriter vector_writer(optimal_footprint_path);
            TL_ASSERT(vector_writer.isOpen(), "Vector open error: {}", optimal_footprint_path.toString());
            vector_writer.setCRS(crs.toWktFormat());

            tl::TableField field("image", tl::TableField::Type::STRING, 254);
            std::vector<tl::TableField> fields;
            fields.push_back(field);

            tl::GLayer layer;
            layer.setName("footprint");
            layer.addDataField(field);

            for (const auto &[value, polygon] : clean_footprint) {
                std::shared_ptr<tl::TableRegister> data(new tl::TableRegister(fields));
                data->setValue(0, value);
                auto polygon_ptr = std::make_unique<tl::GPolygon>(*polygon);
                polygon_ptr->setAttributes(*data);
                layer.push_back(std::move(polygon_ptr));
            }

            vector_writer.write(layer);

            vector_writer.close();
        }

        /// Write grid
        {

            auto grid_path = optimal_footprint_path;
            grid_path.replaceBaseName("grid");
            tl::VectorWriter grid_writer(grid_path);
            TL_ASSERT(grid_writer.isOpen(), "Vector open error: {}", grid_path.toString());
            grid_writer.setCRS(crs.toWktFormat());

            tl::TableField field_image("image", tl::TableField::Type::STRING, 254);
            std::vector<tl::TableField> fields;
            fields.push_back(field_image);

            tl::GLayer layer;
            layer.setName("grid");
            layer.addDataField(field_image);
            int i = 0;
            for (const auto &[image_name, polygon] : grid_images) {
                tl::TableRegister data(fields);
                data.setValue(0, image_name);
                auto polygon_ptr = std::make_unique<tl::GPolygon>(*polygon);
                polygon_ptr->setAttributes(data);
                layer.push_back(std::move(polygon_ptr));
            }

            grid_writer.write(layer);

            grid_writer.close();
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

//TODO: Permitir trabajar con distintos espacios de color para comparar los resultados y ver cuál es más eficiente y preciso para la combinación de imágenes.
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
            float medL = tl::median(L);
            float medA = tl::median(A);
            float medB = tl::median(B);
             
            // distancias de color a la mediana (CIE 1976 - ΔE*)
            color_distances.assign(images_size, std::numeric_limits<float>::infinity());
            for (size_t idx : valid_idxs) {
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
            for (size_t idx : valid_idxs) {
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
            for (size_t idx : valid_idxs) {
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

            for (size_t idx : valid_idxs) {

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
                scored.emplace_back(score, static_cast<int>(idx));
            }

            if (scored.empty()) continue;

            // ordenar por score ascendente (menor es mejor)
            std::sort(scored.begin(), scored.end(),
                [](const auto &a, const auto &b) { return a.first < b.first; });

            // número de mejores píxeles a usar
            const int TOP_N = 5;
            int n = std::min<int>(TOP_N, static_cast<int>(scored.size()));

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

auto robustLocalBlendigRGB(const std::vector<cv::Mat> &images,
                           const std::vector<cv::Point2f> &centers,
                           float weight_color = 0.3f,
                           float weight_distance = 0.7f) -> cv::Mat
{
    return cv::Mat();
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
            for (size_t idx : valid_idxs) {
                float val = images[idx].at<float>(static_cast<int>(r), c);
                intensity_diffs[idx] = std::fabs(val - median);
            }

            float min = std::numeric_limits<float>::infinity();
            float max = std::numeric_limits<float>::lowest();
            for (size_t idx : valid_idxs) {
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
            for (size_t idx : valid_idxs) {
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

            for (size_t idx : valid_idxs) {

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
                scored.emplace_back(score, static_cast<int>(idx));

            }

            if (scored.empty()) continue;

            // Ordenar por score ascendente (menor es mejor)
            std::sort(scored.begin(), scored.end(),
                [](const auto &a, const auto &b) { return a.first < b.first; });

            // Número de mejores píxeles a usar
            const int TOP_N = 5;
            int n = std::min<int>(TOP_N, static_cast<int>(scored.size()));

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

OrthophotoTask::OrthophotoTask(const std::shared_ptr<OrthophotoProperties> &properties, 
                               std::unordered_map<size_t, Image> images,
                               std::map<int, Camera> cameras,
                               tl::Path orthoPath, 
                               tl::Path mdt, 
                               std::string enuCrs, 
/*                               std::string epsg, 
                               std::string interpolation, 
                               double resolution,*/ 
                               bool cuda/*,
                               std::string method*/)
  : tl::Task(),
    mProperties(properties),
    mImages(std::move(images)),
    mCameras(std::move(cameras)),
    mOrthoPath(std::move(orthoPath)),
    mMdt(std::move(mdt)),
    mEnuCrs(std::move(enuCrs)),
    //mEpsg(std::move(epsg)),
    //mInterpolation(std::move(interpolation)),
    //mGSD(resolution),
    bCuda(cuda),
    mDataType(tl::DataType::TL_8U),
    mChannels(3)/*,
    mMethod(std::move(method))*/
{
}

OrthophotoTask::~OrthophotoTask()
{

}

auto OrthophotoTask::report() const -> OrthophotoReport
{
    return mOrthophotoReport;
}

auto OrthophotoTask::orthoPath() const -> tl::Path
{
    return mOrthoPath;
}

std::vector<std::vector<tl::BoundingBox2d>> OrthophotoTask::findGrid(int gridSize, const tl::BoundingBox2d &bbox) const
{
    std::vector<std::vector<tl::BoundingBox2d>> grid;

    try {

        int grid_size = mProperties->gsd() * gridSize;

        //tl::RasterReader reader(mMdt);
        //TL_ASSERT(reader.isOpen(), "Can not open the MDT");
        //auto window = reader.window();
        int step_x = std::ceil(bbox.width() / grid_size);
        int step_y = std::ceil(bbox.height() / grid_size);

        auto center = bbox.center();

        double x_ini = center.x() - ((step_x - 1) * grid_size) / 2.;
        double y_ini = center.y() + ((step_y - 1) * grid_size) / 2.;

        for (size_t i = 0; i < step_x; i++) {

            std::vector<tl::BoundingBox2d> row_grid;

            double x = x_ini + grid_size * i;

            for (size_t j = 0; j < step_y; j++) {

                double y = y_ini - grid_size * j;
                row_grid.emplace_back(tl::Point2d(x, y), grid_size);

            }

            grid.push_back(row_grid);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Compute grid error");
    }

    return grid;
}

void OrthophotoTask::execute(tl::Progress *progressBar, std::stop_token stopToken)
{

    try {
        
        auto ortho_dir = mOrthoPath.parentPath();

        tl::Path temp_path(ortho_dir);
        temp_path.append("temp");
        temp_path.createDirectories();

        // Se tiene que comprobar que todas las imagenes tenga los mismos canales y profundidad de bits
        for (const auto &[_, image] : mImages) {

            //const auto &image = photo.second;
            int camera_id = image.cameraId();

            tl::RasterReader image_reader(image.path());
            if (!image_reader.isOpen()) continue;

            mDataType = image_reader.dataType();
            mChannels = image_reader.channels();
            if (mChannels == 3) break;
        }

        tl::Path footprint_file(ortho_dir);
        footprint_file.append("footprint.shp");
        tl::Path graph_orthos = tl::Path(footprint_file).replaceBaseName("graph_orthos");

        tl::Path dsm_path = mMdt;
        dsm_path.replaceBaseName("dsm_enu");

        //// Crear la malla de teselas
        //std::vector<std::vector<tl::BoundingBox2d>> grid = findGrid(500);

        //// Ventana ortomosaico
        //mWindowAll.min().x() = grid[0][0].min().x();
        //mWindowAll.min().y() = grid.back().back().min().y();
        //mWindowAll.max().x() = grid.back().back().max().x();
        //mWindowAll.max().y() = grid[0][0].max().y();

        auto gsd = mProperties->gsd();
        auto crs = mProperties->crs();
        auto interpolation = mProperties->interpolation();

        //tl::Affine<double, 2> affine_ortho(gsd, -gsd, mWindowAll.min().x(), mWindowAll.max().y(), 0.0);
                /// Calculo de la ventana del DEM en coordenadas de salida
        tl::BoundingBox2d dem_bbox;
        {
            tl::RasterReader reader(dsm_path);
            TL_ASSERT(reader.isOpen(), "Can not open the MDT");
            dem_bbox = reader.window();
            
            std::vector<tl::Point2d> pts = {
                dem_bbox.min(),
                tl::Point2d{dem_bbox.min().x(), dem_bbox.max().y()},
                dem_bbox.max(),
                tl::Point2d{dem_bbox.max().x(), dem_bbox.min().y()}
            };
            tl::GeoTools *geo_tools = tl::GeoTools::getInstance();
            std::vector<tl::Point2d> bbox_points;
            if (geo_tools) {
                tl::CRSsTools *crsTools = geo_tools->ptrCRSsTools();
                for (auto &pt : pts) {
                    if (crsTools) {
                        double z = 0;
                        crsTools->crsOperation(mEnuCrs, crs, pt.x(), pt.y(), z);
                    }
                    bbox_points.emplace_back(pt.x(), pt.y());
                }
                reader.close();
                dem_bbox = tl::BoundingBox2d(bbox_points);
            }
        }

        tl::Affine<double, 2> affine_ortho(gsd, -gsd, dem_bbox.min().x(), dem_bbox.max().y(), 0.0);

        OrthoimageTask orthoimage_task(mImages,
                                       mCameras,
                                       dsm_path,
                                       temp_path,
                                       graph_orthos,
                                       mEnuCrs,
                                       crs,
                                       affine_ortho,
                                       footprint_file,
                                       gsd,
                                       interpolation,
                                       1.0,
                                       bCuda);

        this->subscribe([&](const tl::TaskStoppingEvent &event) {
            orthoimage_task.stop();
        });

        orthoimage_task.run(progressBar);

        if (status() == Status::stopping)  return;
      
        std::string mosaic_method = "RLB";
        auto orthomosaic_properties = mProperties->orthomosaicProperties();
        if (orthomosaic_properties) {
            mosaic_method = orthomosaic_properties->name();
        }

        // Se calcula la ventana del mosaico como intersección del DSM/DTM y la huella de vuelo
        tl::VectorReader vectorReader(graph_orthos);
        TL_ASSERT(vectorReader.isOpen(), "Graph orthos not found: {}", graph_orthos.toString());
        auto orthos_bbox = vectorReader.boundingBox();
        mWindowAll = tl::intersection(dem_bbox, orthos_bbox);
        auto grid = findGrid(500, mWindowAll);

        if (mosaic_method == "RLB") {
            tl::Message::info("Generating orthophoto tiles with Robust Local Blending method");
            auto orthos = searchTiles(graph_orthos, grid/*, 10*/);

            generateTiles(grid, orthos, progressBar);
            writeOrthomosaic(grid);
        } else {
            tl::Message::info("Generating orthophoto tiles with Seam-based Blending method");

            auto sbb_config = dynamic_cast<SbbProperties *>(orthomosaic_properties.get());
            orthoMosaic(graph_orthos, mOrthoPath, gsd, crs, grid, mDataType, mChannels, sbb_config, bCuda);
        }

        tl::Path::removeDirectory(temp_path);

        if (progressBar) (*progressBar)();

        mOrthophotoReport.time = this->time();
        mOrthophotoReport.gsd = gsd;
        mOrthophotoReport.epsg = crs;
        mOrthophotoReport.cols = static_cast<int>(std::round(mWindowAll.width() / gsd));
        mOrthophotoReport.rows = static_cast<int>(std::round(mWindowAll.height() / gsd));
        mOrthophotoReport.channels = mChannels;

        tl::Message::success("Orthophoto task finished in {:.2} minutes", mOrthophotoReport.time / 60.);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Orthophoto task error");
    }

}

auto OrthophotoTask::searchTiles(const tl::Path &graph_orthos,
                                 const std::vector<std::vector<tl::BoundingBox2d>> &grid,
                                 int maxImages) -> std::vector<std::vector<std::map<double, std::string>>>
{
    std::vector<std::vector<std::map<double, std::string>>> orthos(grid.size());

    tl::VectorReader vectorReader(graph_orthos);

    TL_ASSERT(vectorReader.isOpen(), "Graph orthos not found: {}", graph_orthos.toString());

    // Se buscan las imagenes correspondientes al grid y se ordenan

    if (vectorReader.layersCount() >= 1) {

        std::shared_ptr<tl::GLayer> layer = vectorReader.read(0);

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
                        auto polygon = dynamic_cast<tl::GPolygon *>(entity.get());
                        auto window_orto = polygon->window();
                        auto orto_center = window_orto.center();
                        auto dist = tl::distance(orto_center, window_center);
                        tl::Path orto_compensate(polygon->attributes().value(0));
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

void OrthophotoTask::generateTiles(const std::vector<std::vector<tl::BoundingBox2d>> &grid, 
                                   std::vector<std::vector<std::map<double, std::string>>> &orthos, 
                                   tl::Progress *progressBar)
{
    try {

        auto gsd = mProperties->gsd();
        auto epsg_code = mProperties->crs();

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
                int image_size = tl::numberCast<int>(window.width() / gsd);

                // Todas las imagenes del elemento actual del grid
                std::vector<cv::Mat> images;
                // Cambio distances por centros proyectados para ser mas precisos en la selección del pixel optimo
                std::vector<cv::Point2f> centers_in_tile_coords;
                //std::vector<double> distances;

                for (auto &ortho : orthos[r][c]) {

                    try {

                        tl::RasterReader image_reader(ortho.second);
                        if (!image_reader.isOpen()) {
                            tl::Message::error("Image open error :{}", ortho.second);
                            continue;
                        }

                        double src_w = image_reader.cols();
                        double src_h = image_reader.rows();
                        auto georef = image_reader.georeference().inverse();

                        auto ortoimage_center = image_reader.window().center();
                        auto tile_center = window.center();

                        double center_x_tile = (ortoimage_center.x() - tile_center.x()) / gsd;
                        double center_y_tile = (tile_center.y() - ortoimage_center.y()) / gsd;


                        tl::BoundingBox2i window_image;
                        window_image.min() = static_cast<tl::Point2i>(georef.transform(window.min()));
                        window_image.max() = static_cast<tl::Point2i>(georef.transform(window.max()));
                        window_image.normalized();
                        tl::Rect<int> rect_image(window_image.min().x(), window_image.min().y(), window_image.width(), window_image.height());
                        tl::Rect<int> rect_full_image(0, 0, image_reader.cols(), image_reader.rows());
                        tl::Rect<int> rect_to_read = tl::intersect(rect_full_image, rect_image);
                        if (!rect_to_read.isValid()) continue;

                        auto image = image_reader.read(1., 1., rect_to_read);
                        auto data_type = image_reader.dataType();
                        image_reader.close();


                        if (image.rows != image_size || image.cols != image_size) {

                            cv::Scalar no_data;
                            if (image.type() == CV_32F || image.type() == CV_64F)
                                no_data = tl::NoData<float>;
                            else
                                no_data = 0;

                            cv::Mat aux = cv::Mat(image_size, image_size, image.type(), no_data);

                            auto offset = rect_to_read.topLeft() - rect_image.topLeft();
                            cv::Rect roi(offset.x(), offset.y(), image.cols, image.rows);
                            cv::Mat image_roi = aux(roi);
                            image.copyTo(image_roi);
                            image = aux;
                        }

                        images.push_back(image);
                        // Guardamos el punto central relativo al (0,0) de la tesela actual
                        centers_in_tile_coords.push_back(cv::Point2f(static_cast<float>(center_x_tile), static_cast<float>(center_y_tile)));
                        //distances.push_back(ortho.first);
                    
                    } catch (std::exception &e) {
                        tl::Message::error("Window = [{}, {}, {}, {}]", window.min().x(), window.min().y(), window.max().x(), window.max().y());

                        tl::printException(e);
                    }
                }

                if (images.empty()) continue;

                cv::Mat read_image;
                if (images.size() == 1) {
                    read_image = images[0].clone();
                } else {

                    if (mChannels == 1) {
                        read_image = combineImagesSmartMono(images, centers_in_tile_coords);
                    } else if (mChannels == 3) {
                        read_image = combineImagesSmart(images, centers_in_tile_coords);
                    } else {
                        /// TODO: Contemplar otros casos
                    }

                }

                if (read_image.empty()) continue;

                try {

                    tl::Path tile(mOrthoPath.parentPath());
                    tile.append("temp");
                    tile.append(std::to_string(r));
                    tile.append(std::to_string(c));
                    tile.createDirectories();
                    tile.append("t.tif");

                    tl::RasterWriter image_writer(tile);

                    image_writer.create(read_image.rows, read_image.cols, read_image.channels(), mDataType);
                    tl::Crs crs(epsg_code);
                    image_writer.setCRS(crs.toWktFormat());
                    tl::Affine<double, 2> affine_ortho(gsd, -gsd, window.min().x(), window.max().y(), 0.0);
                    image_writer.setGeoreference(affine_ortho);
                    if (mDataType == tl::DataType::TL_32F || mDataType == tl::DataType::TL_64F)
                        image_writer.setNoDataValue(tl::NoData<float>);
                    image_writer.write(read_image);
                    image_writer.close();
                } catch (std::exception &e) {
                    tl::printException(e);
                }
            }
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

void OrthophotoTask::writeOrthomosaic(const std::vector<std::vector<tl::BoundingBox2d>> &grid)
{

    try {

        tl::Message::info("Writing ortho image");

        auto gsd = mProperties->gsd();
        auto epsg_code = mProperties->crs();

        //tl::Path ortho_final(mOrthoPath);
        //ortho_final.append("ortho.tif");
        tl::RasterWriter image_writer(mOrthoPath);
        int cols = static_cast<int>(std::round(mWindowAll.width() / gsd));
        int rows = static_cast<int>(std::round(mWindowAll.height() / gsd));

        tl::ImageOptions options;
        options.set(tl::tiff::Tiled, "YES");
        options.set(tl::tiff::BigTiff, "IF_NEEDED");
        options.set(tl::tiff::Compress, "LZW");

        if (image_writer.isOpen()) {
            image_writer.create(rows, cols, mChannels, mDataType, options);

            tl::ImageMetadata metadata;
            metadata.set("TIFFTAG_DOCUMENTNAME", "Orthomosaic");
            metadata.set("TIFFTAG_IMAGEDESCRIPTION", "Orthomosaic generated by GRAPHOS");
            metadata.set("TIFFTAG_SOFTWARE", "GRAPHOS");
            QString tiffDateTime = QDateTime::currentDateTime().toString("yyyy:MM:dd HH:mm:ss");
            metadata.set("TIFFTAG_DATETIME", tiffDateTime.toStdString());
            image_writer.setMetadata(metadata);
            tl::Crs crs(epsg_code);
            image_writer.setCRS(crs.toWktFormat());
            tl::Affine<double, 2> affine_ortho(gsd, -gsd, mWindowAll.min().x(), mWindowAll.max().y(), 0.0);
            image_writer.setGeoreference(affine_ortho);

            if (mDataType == tl::DataType::TL_32F || mDataType == tl::DataType::TL_64F)
                image_writer.setNoDataValue(tl::NoData<float>);
            for (size_t r = 0; r < grid.size(); r++) {
                for (size_t c = 0; c < grid[r].size(); c++) {

                    try {
                        
                        tl::Path tile(mOrthoPath.parentPath());
                        tile.append("temp");
                        tile.append(std::to_string(r));
                        tile.append(std::to_string(c));
                        tile.append("t.tif");
                        if (!tile.exists()) continue;
                        tl::RasterReader image_reader(tile);
                        if (!image_reader.isOpen()) {
                            continue;
                        }

                        auto tile_window = image_reader.window();

                        const auto &window = grid[r][c];
                        if (!tl::intersects(tile_window, window)) continue;

                        cv::Mat compensate_image = image_reader.read();
                        // Relleno de pixeles negros
                        cv::Mat blackPixelMask = createBlackPixelMask(compensate_image, 1024);
                        //cv::Mat black_pixel_mask = createBlackPixelMask(compensate_image, 512);
                        cv::inpaint(compensate_image, blackPixelMask, compensate_image, 3, cv::INPAINT_TELEA);
                        //cv::inpaint(compensate_image, black_pixel_mask, compensate_image, 5, cv::INPAINT_NS);

                        auto affine_ortho_inverse = affine_ortho.inverse();
                        tl::Point2d p1_ortho = affine_ortho_inverse.transform(tile_window.min());
                        tl::Point2d p2_ortho = affine_ortho_inverse.transform(tile_window.max());
                        tl::BoundingBox2i window_to_write(static_cast<tl::Point2i>(p1_ortho), static_cast<tl::Point2i>(p2_ortho));
                        window_to_write.normalized();
                        if (window_to_write.isValid())
                            image_writer.write(compensate_image, window_to_write);
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

} // namespace graphos
