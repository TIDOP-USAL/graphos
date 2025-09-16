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

namespace graphos
{

constexpr double exposure_compensator_factor = 0.1;


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
//    if (images.empty()) return cv::Mat();
//
//    // Nueva versión: Multi-band blending
//    cv::Ptr<cv::detail::Blender> blender = cv::makePtr<cv::detail::MultiBandBlender>(false, 5);
//    cv::Rect boundingBox;
//
//    // Preparar blender con el bounding box de todas las imágenes
//    for (const auto &img : images) {
//        boundingBox |= cv::Rect(0, 0, img.cols, img.rows);
//    }
//    blender->prepare(boundingBox);
//
//    for (size_t i = 0; i < images.size(); ++i) {
//        cv::Mat mask(images[i].size(), CV_8U, cv::Scalar::all(255));
//        blender->feed(images[i], mask, cv::Point(0, 0));
//    }
//
//    cv::Mat result, result_mask;
//    blender->blend(result, result_mask);
//
//    if (result.depth() != CV_8U) {
//        double minVal, maxVal;
//        cv::minMaxLoc(result, &minVal, &maxVal);
//        if (maxVal > 1.0) maxVal = 1.0; // Asegurar rango típico
//        if (maxVal > 0.0) {
//            result.convertTo(result, CV_8U, 255.0 / maxVal);
//        } else {
//            result.convertTo(result, CV_8U);
//        }
//    }
//    if (result.channels() != 3) {
//        cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
//    }
//
//    return result;
//}

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

    int grid_size = mGSD * gridSize;

    auto reader = tl::ImageReaderFactory::create(mMdt);
    reader->open();
    TL_ASSERT(reader->isOpen(), "Can not open the MDT");
    auto window = reader->window();
    int step_x = std::ceil(window.width() / grid_size);
    int step_y = std::ceil(window.height() / grid_size);

    auto center = window.center();

    double x_ini = center.x - ((step_x-1) * grid_size) / 2.;
    double y_ini = center.y + ((step_y-1) * grid_size) / 2.;

    for (size_t i = 0; i < step_x; i++) {

        std::vector<tl::WindowD> row_grid;

        double x = x_ini + grid_size * i;

        for (size_t j = 0; j < step_y; j++) {

            double y = y_ini - grid_size * j;
            row_grid.emplace_back(tl::Point<double>(x, y), grid_size);

        }

        grid.push_back(row_grid);
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

        orthoimageExposureCompensator(graph_orthos);
        auto orthos = searchTiles(graph_orthos, grid);
        generateTiles(grid, orthos);
        // Intento de compesar de exposición los tiles pero no funciona bien
        //tilesExposureCompensator(grid);
        //for (int r = 0; r < grid.size(); ++r) {
        //    for (int c = 0; c < grid[r].size(); ++c) {
        //        blendTileBlock(r, c, grid);
        //    }
        //}
        writeOrthomosaic(grid);

        mOrthophotoReport.time = this->time();
        mOrthophotoReport.gsd = mGSD;
        mOrthophotoReport.epsg = QString::fromStdString(mEpsg);

        tl::Message::success("Orthophoto task finished in {:.2} minutes", mOrthophotoReport.time / 60.);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Orthophoto task error");
    }

}


#define EXPOSURE_COMPENSATOR

auto OrthophotoTask::searchTiles(const tl::Path &graph_orthos,
                                 const std::vector<std::vector<tl::WindowD>> &grid) -> std::vector<std::vector<std::map<double, std::string>>>
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
#ifdef EXPOSURE_COMPENSATOR
                        std::string name = orto_compensate.baseName().toUtf8() + "_compensate.png";
                        orto_compensate.replaceFileName(name);
#endif
                        orthos[r][c][dist] = orto_compensate.toUtf8();
                    }

                }

            }
        }
    }

    return orthos;
}

void OrthophotoTask::generateTiles(const std::vector<std::vector<tl::WindowD>> &grid, 
                                   std::vector<std::vector<std::map<double, std::string>>> &orthos)
{
    for (size_t r = 0; r < grid.size(); r++) {
        for (size_t c = 0; c < grid[r].size(); c++) {

            const auto &window = grid[r][c];

            // Tamaño de imagen
            auto window_aux = tl::expandWindow(window, 50 * mGSD);
            int image_size = tl::numberCast<int>(window_aux.width() / mGSD);

            // Todas las imagenes del elemento actual del grid
            std::vector<cv::Mat> images;

            for (auto &ortho : orthos[r][c]) {

                auto image_reader = tl::ImageReaderFactory::create(ortho.second);
                image_reader->open();
                if (!image_reader->isOpen()) {
                    tl::Message::error("Image open error :{}", ortho.second);
                    continue;
                }

                // Por ahora solo se utilizan las imagenes que contienen el grid. Se omiten las que intersectan
                if (!image_reader->window().containsWindow(window)) continue;

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

                // Relleno de pixeles negros
                cv::Mat blackPixelMask = createBlackPixelMask(image, 512);
                cv::inpaint(image, blackPixelMask, image, 3, cv::INPAINT_TELEA);

                images.push_back(image);

                /////
                // Por ahora sigo escribiendo todo

                try {
                    tl::Path tile(mOrthoPath);
                    tile.append(std::to_string(r));
                    tile.append(std::to_string(c));
                    tile.createDirectories();
                    tl::Path image_name(ortho.second);

                    tile.append(image_name.fileName().toUtf8());

                    auto image_writer = tl::ImageWriterFactory::create(tile);
                    image_writer->open();
                    int cols = static_cast<int>(std::round(window_aux.width() / mGSD));
                    int rows = static_cast<int>(std::round(window_aux.height() / mGSD));

                    image_writer->create(rows, cols, image.channels(), data_type);
                    tl::Crs crs(mEpsg);
                    image_writer->setCRS(crs.toWktFormat());
                    tl::Affine<double, 2> affine_ortho(mGSD, -mGSD, window_aux.pt1.x, window_aux.pt2.y, 0.0);
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
                tl::Path tile(mOrthoPath);
                tile.append(std::to_string(r));
                tile.append(std::to_string(c));
                tile.createDirectories();
                tile.append("t.tif");

                auto image_writer = tl::ImageWriterFactory::create(tile);
                image_writer->open();

                //tl::DataType data_type = tl::DataType::TL_8U;
                //auto opencv_depth = read_image.depth();
                //if (opencv_depth == CV_8U) {
                //    data_type = tl::DataType::TL_8U;
                //} else if (opencv_depth == CV_8S) {
                //    data_type = tl::DataType::TL_8S;
                //} else if (opencv_depth == CV_16U) {
                //    data_type = tl::DataType::TL_16U;
                //} else if (opencv_depth == CV_16S) {
                //    data_type = tl::DataType::TL_16S;
                //} else if (opencv_depth == CV_32S) {
                //    data_type = tl::DataType::TL_32S;
                //} else if (opencv_depth == CV_32F) {
                //    data_type = tl::DataType::TL_32F;
                //} else if (opencv_depth == CV_64F) {
                //    data_type = tl::DataType::TL_64F;
                //}

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

void OrthophotoTask::tilesExposureCompensator(const std::vector<std::vector<tl::WindowD>> &grid)
{
    try {

        tl::Message::info("Tiles exposure compensator");

        tl::WindowD window_all;
        std::vector<cv::Point> corners;
        std::vector<cv::Mat> mat_orthos;
        std::vector<cv::UMat> umat_orthos;
        std::vector<cv::UMat> ortho_masks;

        //int type = cv::detail::ExposureCompensator::NO;
        //int type = cv::detail::ExposureCompensator::GAIN;
        int type = cv::detail::ExposureCompensator::GAIN_BLOCKS;
        //int type = cv::detail::ExposureCompensator::CHANNELS;
        //int type = cv::detail::ExposureCompensator::CHANNELS_BLOCKS;
        cv::Ptr<cv::detail::ExposureCompensator> compensator = cv::detail::ExposureCompensator::createDefault(type);

        std::vector<tl::Path> orthos;
        std::vector<tl::WindowD> windows;

        // Busco todas las tiles
        for (size_t r = 0; r < grid.size(); r++) {
            for (size_t c = 0; c < grid[r].size(); c++) {

                tl::Path tile(mOrthoPath);
                tile.append(std::to_string(r));
                tile.append(std::to_string(c));
                tile.append("t.tif");

                if (!tile.exists()) continue;

                orthos.push_back(tile);
                windows.push_back(tl::expandWindow(grid[r][c], 50 * mGSD));
            }
        }

        size_t n_orthos = orthos.size();
        corners.resize(n_orthos);
        ortho_masks.resize(n_orthos);
        umat_orthos.resize(n_orthos);
        mat_orthos.resize(n_orthos);

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

        for (size_t i = 0; i < n_orthos; i++) {

            try {

                tl::Path tile = orthos[i];
                std::unique_ptr<tl::ImageReader> image_reader = tl::ImageReaderFactory::create(tile);
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

                    compensator->apply(i, corner, compensate_image, mask_full_size);

                    tile.replaceBaseName("compensate");
                    std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(tile);
                    image_writer->open();
                    if (image_writer->isOpen()) {
                        image_writer->create(image_reader->rows(), image_reader->cols(), image_reader->channels(), image_reader->dataType());
                        image_writer->setCRS(image_reader->crsWkt());
                        image_writer->setGeoreference(image_reader->georeference());
                        image_writer->write(compensate_image);
                        image_writer->close();
                        //tl::Message::info("Compensated image: {}", orto_compensate.fileName().toUtf8());
                    }
                }

            } catch (std::exception &e) {
                tl::printException(e);
            }
        }

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

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

            auto metadata = tl::ImageMetadataFactory::create("GTiff");
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
                        //tile.append("compensate.tif");
                        //tile.append("blend.tif");
                        if (!tile.exists()) continue;
                        auto image_reader = tl::ImageReaderFactory::create(tile);
                        image_reader->open();
                        if (!image_reader->isOpen()) {
                            //tl::Message::error("Image open error :{}", tile.toU);
                            continue;
                        }

                        auto tile_window = image_reader->window();

                        const auto &window = grid[r][c];
                        if (!intersectWindows(tile_window, window) /*||
                            !intersectWindows(image_reader_seam->window(), window)*/) continue;

                        auto georef = image_reader->georeference();

                        //double scale_x = georef.scale().x();
                        //double scale_y = georef.scale().y();
                        //double read_scale_x = scale_x / mGSD;
                        //double read_scale_y = scale_y / mGSD;

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
                        //cv::Mat blackPixelMask = createBlackPixelMask(compensate_image, 512);
                        cv::inpaint(compensate_image, blackPixelMask, compensate_image, 3, cv::INPAINT_TELEA);
                        //cv::inpaint(compensate_image, blackPixelMask, compensate_image, 5, cv::INPAINT_NS);

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

void OrthophotoTask::blendTileBlock(int r, int c, const std::vector<std::vector<tl::WindowD>> &grid)
{
    try {
        const int block_size = 3;
        const int borderSize = 50;  // puedes ajustar esto según el solape

        const int rows = grid.size();
        const int cols = grid[0].size();  // asumimos cuadrícula rectangular

        const int center_idx = 4; // posición central en un bloque 3x3 (fila 1, col 1)

        std::vector<cv::Mat> images(9, cv::Mat());
        std::vector<cv::Mat> masks(9, cv::Mat());
        std::vector<cv::Point> positions(9);
        bool has_tile = false;

        cv::Rect mosaic_roi;

        // Función auxiliar para crear máscara difusa
        auto createSoftMask = [](const cv::Size &size, int border) {
            cv::Mat base(size, CV_8U, cv::Scalar(0));
            cv::rectangle(base, cv::Rect(border, border, size.width - 2 * border, size.height - 2 * border),
                cv::Scalar(255), cv::FILLED);
            cv::Mat dist;
            cv::distanceTransform(255 - base, dist, cv::DIST_L2, 5);
            double maxVal;
            cv::minMaxLoc(dist, nullptr, &maxVal);
            dist.convertTo(dist, CV_32F, 1.0 / maxVal);
            return dist;
            };

        // Cargar 3x3 tiles alrededor del centro
        int idx = 0;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx, ++idx) {
                int rr = r + dy;
                int cc = c + dx;
                if (rr < 0 || rr >= rows || cc < 0 || cc >= cols) continue;

                tl::Path tile_path(mOrthoPath);
                tile_path.append(std::to_string(cc));
                tile_path.append(std::to_string(rr));
                tile_path.append("t.tif");

                if (!tile_path.exists()) continue;

                auto reader = tl::ImageReaderFactory::create(tile_path);
                reader->open();
                if (!reader->isOpen()) continue;

                cv::Mat image = reader->read();
                if (image.empty()) continue;

                // Inpaint si es necesario
                cv::Mat blackMask = createBlackPixelMask(image, 1024);
                cv::inpaint(image, blackMask, image, 3, cv::INPAINT_TELEA);

                cv::Mat image_f;
                image.convertTo(image_f, CV_32FC3);

                // Posición relativa en el bloque
                cv::Point offset((dx + 1) * image.cols, (dy + 1) * image.rows);
                positions[idx] = offset;

                images[idx] = image_f;
                masks[idx] = createSoftMask(image.size(), borderSize);
                has_tile = true;
            }
        }

        if (!has_tile || images[center_idx].empty())
            return; // nada que hacer

        const cv::Size tile_size = images[center_idx].size();
        const int blend_rows = tile_size.height * 3 - 100; // Por el solape de 50
        const int blend_cols = tile_size.width * 3 - 100;

        cv::Mat accum = cv::Mat::zeros(blend_rows, blend_cols, CV_32FC3);
        cv::Mat weight = cv::Mat::zeros(blend_rows, blend_cols, CV_32F);

        for (int i = 0; i < 9; ++i) {
            if (images[i].empty()) continue;

            // Aplicar máscara
            std::vector<cv::Mat> ch;
            cv::split(images[i], ch);
            for (auto &c : ch) c = c.mul(masks[i]);
            cv::Mat weighted;
            cv::merge(ch, weighted);

            cv::Rect roi(positions[i].x, positions[i].y, tile_size.width, tile_size.height);
            accum(roi) += weighted;
            weight(roi) += masks[i];
        }

        // Normalizar
        std::vector<cv::Mat> w_ch(3, weight);
        cv::Mat w_merge;
        cv::merge(w_ch, w_merge);
        cv::Mat result_f;
        cv::divide(accum, w_merge, result_f);
        cv::Mat result_u8;
        double minVal, maxVal;
        cv::minMaxLoc(result_f, &minVal, &maxVal);

        // Escalamos automáticamente al rango 0-255 si es 16-bit
        double scale = (maxVal > 255.0) ? 255.0 / maxVal : 1.0;
        result_f.convertTo(result_u8, CV_8U);

        // Recortar el centro
        cv::Rect center_roi(tile_size.width, tile_size.height, tile_size.width, tile_size.height);
        cv::Mat blended_center = result_u8(center_roi);

        // Guardar tile fusionada como blend.tif
        tl::Path out_path(mOrthoPath);
        out_path.append(std::to_string(r));
        out_path.append(std::to_string(c));
        out_path.append("blend.tif");

        // Usamos metadatos de la tile original
        tl::Path src_path(mOrthoPath);
        src_path.append(std::to_string(r));
        src_path.append(std::to_string(c));
        src_path.append("t.tif");

        auto src_reader = tl::ImageReaderFactory::create(src_path);
        src_reader->open();
        if (!src_reader->isOpen()) return;

        auto writer = tl::ImageWriterFactory::create(out_path);
        writer->open();
        writer->create(src_reader->rows(), src_reader->cols(), src_reader->channels(), src_reader->dataType());
        writer->setCRS(src_reader->crsWkt());
        writer->setGeoreference(src_reader->georeference());
        writer->write(blended_center);
        writer->close();

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void OrthophotoTask::orthoimageExposureCompensator(const tl::Path &graph_orthos)
{

#ifdef EXPOSURE_COMPENSATOR

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
#endif

}

} // namespace graphos
