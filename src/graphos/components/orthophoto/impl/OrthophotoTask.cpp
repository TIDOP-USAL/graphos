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
#include "graphos/core/ortho/Orthoimage.h"
#include "graphos/core/image.h"

/* TidopLib */
#include <tidop/core/messages.h>
#include <tidop/core/exception.h>
#include <tidop/core/progress.h>
#include <tidop/geospatial/crs.h>
#include <tidop/vect/vectreader.h>
#include <tidop/graphic/layer.h>
#include <tidop/graphic/datamodel.h>
#include <tidop/graphic/entities/polygon.h>

/* OpenCV */
#include <opencv2/stitching.hpp>

namespace graphos
{

constexpr double exposure_compensator_factor = 0.5;


std::vector<tl::WindowD> findGrid(const tl::Path &footprint_file)
{
    std::vector<tl::WindowD> grid;

    tl::WindowD window_all;

    auto vector_reader = tl::VectorReaderFactory::create(footprint_file);
    vector_reader->open();
    if (vector_reader->isOpen()) {
        if (vector_reader->layersCount() >= 1) {

            std::map<double, std::shared_ptr<tl::GPolygon>> entities;
            std::shared_ptr<tl::GLayer> layer = vector_reader->read(0);

            /// Busqueda ventana total

            double grid_step = -1;

            for (const auto &entity : *layer) {
                std::shared_ptr<tl::GPolygon> polygon = std::dynamic_pointer_cast<tl::GPolygon>(entity);
                std::shared_ptr<tl::TableRegister> data = polygon->data();
                tl::WindowD window = polygon->window();
                grid_step = std::min(window.width(), window.height());
                window_all = tl::joinWindow(window_all, window);
            }

            grid_step /= 3.;

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

auto bestImage(const tl::Point<double>& pt, const std::shared_ptr<tl::GLayer>& layer) -> std::shared_ptr<tl::GPolygon>
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
                          const std::vector<tl::WindowD> &grid,
                          const tl::Path &optimal_footprint_path,
                          const tl::Crs &crs)
{
    std::map<std::string, std::shared_ptr<tl::GPolygon>> clean_footprint;

    auto vector_reader = tl::VectorReaderFactory::create(footprint_file);
    vector_reader->open();
    if (vector_reader->isOpen()) {

        if (vector_reader->layersCount() >= 1) {

            std::map<double, std::shared_ptr<tl::GPolygon>> entities;
            std::shared_ptr<tl::GLayer> layer = vector_reader->read(0);

            for (const auto& window : grid) {

                /// Busqueda de imagen mas centrada
                std::shared_ptr<tl::GPolygon> polygon = bestImage(window.center(), layer);
                if (polygon) {
                    std::shared_ptr<tl::TableRegister> data = polygon->data();
                    std::string ortho_to_compensate = data->value(0);
                    clean_footprint[ortho_to_compensate] = polygon;
                }

            }

            vector_reader->close();

        }

        tl::Message::info("Optimal footprint. {} retained images", clean_footprint.size());

        auto vector_writer = tl::VectorWriterFactory::create(optimal_footprint_path.toString());
        vector_writer->open();
        if (!vector_writer->isOpen())throw std::runtime_error("Vector open error");
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
            auto data = std::make_shared<tl::TableRegister>(fields);
            data->setValue(0, footprint.first);
            layer.push_back(footprint.second);
        }

        vector_writer->write(layer);

        vector_writer->close();
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

    auto vector_reader = tl::VectorReaderFactory::create(optimal_footprint_path.toString());
    vector_reader->open();

    if (vector_reader->isOpen()) {

        if (vector_reader->layersCount() >= 1) {

            std::map<double, std::shared_ptr<tl::GPolygon>> entities;
            std::shared_ptr<tl::GLayer> layer = vector_reader->read(0);

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

                        auto image_reader = tl::ImageReaderFactory::create(orthos[i]);
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

                    auto image_reader = tl::ImageReaderFactory::create(ortho_to_compensate);
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

        vector_reader->close();
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

            affine_ortho.transform(grid[i].pt1);
            affine_ortho.transform(grid[i].pt2);
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
                               const QString &epsg,
                               bool cuda)
  : tl::TaskBase(),
    mGSD(gsd),
    mPhotos(images),
    mCameras(cameras),
    mOrthoPath(orthoPath),
    mMdt(mdt),
    mEpsg(epsg),
    bCuda(cuda)
{

}

OrthophotoTask::~OrthophotoTask()
{

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

void OrthophotoTask::setCrs(const QString &epsg)
{
    mEpsg = epsg;
}

void OrthophotoTask::setCuda(bool active)
{
    bCuda = active;
}

void OrthophotoTask::execute(tl::Progress *progressBar)
{

    try {

        tl::Path footprint_file(mOrthoPath);
        footprint_file.append("footprint.shp");
        tl::Path graph_orthos = tl::Path(footprint_file).replaceBaseName("graph_orthos");
        tl::Crs crs(mEpsg.toStdString());

        OrthoimageProcess ortho_process(mPhotos,
                                        mCameras,
                                        mMdt,
                                        mOrthoPath,
                                        graph_orthos,
                                        crs,
                                        footprint_file,
                                        mGSD,
                                        0.8/*0.4*/,
                                        bCuda);
        ortho_process.run(progressBar);

        std::vector<tl::WindowD> grid = findGrid(graph_orthos);

        tl::Path optimal_footprint_path(graph_orthos);
        std::string name = optimal_footprint_path.baseName().toString() + "_optimal";
        optimal_footprint_path.replaceBaseName(name);
        findOptimalFootprint(graph_orthos, grid, optimal_footprint_path, crs);

        orthoMosaic(optimal_footprint_path, mOrthoPath, mGSD, crs, grid);

        tl::Message::success("Orthophoto task finished in {:.2} minutes", this->time() / 60.);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Orthophoto task error");
    }

}

} // namespace graphos
