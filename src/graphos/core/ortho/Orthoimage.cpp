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

#include "graphos/core/ortho/Orthoimage.h"

#include "graphos/core/ortho/ZBuffer.h"
#include "graphos/core/ortho/Orthorectification.h"

#include <tidop/core/progress.h>
#include <tidop/graphic/layer.h>

#ifdef HAVE_OPENCV_CUDAARITHM
#include <opencv2/cudaarithm.hpp>
#endif
#include <opencv2/imgproc.hpp>

namespace graphos
{

Orthoimage::Orthoimage(const tl::Path &image,
                       Orthorectification *orthorectification,
                       const tl::Crs &crs,
                       const tl::Rect<int> &rectOrtho,
                       const tl::Affine<double, 2> &georeference,
                       bool cuda)
  : mImageReader(tl::ImageReaderFactory::create(image)),
    mOrthorectification(orthorectification),
    mCrs(crs),
    mRectOrtho(rectOrtho),
    mGeoreference(georeference),
    bCuda(cuda)
{
}

Orthoimage::~Orthoimage()
{
}

void Orthoimage::run(const tl::Path &ortho, const cv::Mat &visibilityMap)
{
    TL_TODO("Comprobar que visibilityMap tenga el tamaño adecuado")

    try {

        tl::Rect<int> rect_image = mOrthorectification->rectImage();
        tl::Rect<int> rect_dtm = mOrthorectification->rectDtm();

        //// Carga de la imagen

        mImageReader->open();
        if (!mImageReader->isOpen()) throw std::runtime_error("Image open error");
        cv::Mat image = mImageReader->read();
        int depth = mImageReader->depth();

        if (depth != 8) {
#ifdef HAVE_OPENCV_CUDAARITHM
            if (bCuda) {
                cv::cuda::GpuMat gImgIn(image);
                cv::cuda::GpuMat gImgOut;
                cv::cuda::normalize(gImgIn, gImgOut, 0., 255., cv::NORM_MINMAX, CV_8U);
                gImgOut.download(image);
            } else {
#endif

                cv::normalize(image, image, 0., 255., cv::NORM_MINMAX, CV_8U);

#ifdef HAVE_OPENCV_CUDAARITHM
            }
#endif

            depth = 8;
        }

        /// Undistort

        cv::Mat undistort_image = mOrthorectification->undistort(image);
        image.release();
        /// georeferencia orto

        mOrthophotoWriter = tl::ImageWriterFactory::create(ortho);
        mOrthophotoWriter->open();
        if (!mOrthophotoWriter->isOpen()) throw std::runtime_error("Image open error");
        int channels_ortho = mImageReader->channels();
        tl::DataType data_type_ortho = tl::DataType::TL_8U;// mImageReader->dataType();
        mOrthophotoWriter->create(mRectOrtho.height, mRectOrtho.width, channels_ortho, data_type_ortho);
        cv::Mat mat_ortho(mRectOrtho.height, mRectOrtho.width, CV_MAKETYPE(dataTypeToOpenCVDataType(data_type_ortho), channels_ortho));
        mat_ortho = cv::Scalar(0, 0, 0);

        std::vector<tl::Point3<double>> dtm_grid_terrain_points(4);
        std::vector<tl::Point<double>> ortho_image_coordinates(4);
        std::vector<tl::Point<double>> photo_photocoordinates(4);
        std::vector<tl::Point<double>> photo_image_coordinates(4);

        for (int r = rect_dtm.y; r < rect_dtm.y + rect_dtm.height - 1; r++) {
            for (int c = rect_dtm.x; c < rect_dtm.x + rect_dtm.width - 1; c++) {

                if (!visibilityMap.empty() && visibilityMap.at<uchar>(r - rect_dtm.y, c - rect_dtm.x) == 0) continue;

                dtm_grid_terrain_points[0] = mOrthorectification->dtmToTerrain(tl::Point<int>(c, r));
                dtm_grid_terrain_points[1] = mOrthorectification->dtmToTerrain(tl::Point<int>(c + 1, r));
                dtm_grid_terrain_points[2] = mOrthorectification->dtmToTerrain(tl::Point<int>(c + 1, r + 1));
                dtm_grid_terrain_points[3] = mOrthorectification->dtmToTerrain(tl::Point<int>(c, r + 1));

                if (mOrthorectification->hasNodataValue()) {
                    double nodata_value = mOrthorectification->nodataValue();
                    if (dtm_grid_terrain_points[0].z == nodata_value ||
                        dtm_grid_terrain_points[1].z == nodata_value ||
                        dtm_grid_terrain_points[2].z == nodata_value ||
                        dtm_grid_terrain_points[3].z == nodata_value) {
                        continue;
                    }
                }

                auto inverse_transform = mGeoreference.inverse();

                //ortho_image_coordinates[0] = mGeoreference.transform(dtm_grid_terrain_points[0], tl::geom::Transform::Order::inverse);
                //ortho_image_coordinates[1] = mGeoreference.transform(dtm_grid_terrain_points[1], tl::geom::Transform::Order::inverse);
                //ortho_image_coordinates[2] = mGeoreference.transform(dtm_grid_terrain_points[2], tl::geom::Transform::Order::inverse);
                //ortho_image_coordinates[3] = mGeoreference.transform(dtm_grid_terrain_points[3], tl::geom::Transform::Order::inverse);
                ortho_image_coordinates[0] = inverse_transform.transform(static_cast<tl::Point<double>>(dtm_grid_terrain_points[0]));
                ortho_image_coordinates[1] = inverse_transform.transform(static_cast<tl::Point<double>>(dtm_grid_terrain_points[1]));
                ortho_image_coordinates[2] = inverse_transform.transform(static_cast<tl::Point<double>>(dtm_grid_terrain_points[2]));
                ortho_image_coordinates[3] = inverse_transform.transform(static_cast<tl::Point<double>>(dtm_grid_terrain_points[3]));

                photo_photocoordinates[0] = mOrthorectification->terrainToPhotocoordinates(dtm_grid_terrain_points[0]);
                photo_photocoordinates[1] = mOrthorectification->terrainToPhotocoordinates(dtm_grid_terrain_points[1]);
                photo_photocoordinates[2] = mOrthorectification->terrainToPhotocoordinates(dtm_grid_terrain_points[2]);
                photo_photocoordinates[3] = mOrthorectification->terrainToPhotocoordinates(dtm_grid_terrain_points[3]);

                photo_image_coordinates[0] = mOrthorectification->photocoordinatesToImage(photo_photocoordinates[0]);
                photo_image_coordinates[1] = mOrthorectification->photocoordinatesToImage(photo_photocoordinates[1]);
                photo_image_coordinates[2] = mOrthorectification->photocoordinatesToImage(photo_photocoordinates[2]);
                photo_image_coordinates[3] = mOrthorectification->photocoordinatesToImage(photo_photocoordinates[3]);

                if (rect_image.contains(photo_image_coordinates[0]) &&
                    rect_image.contains(photo_image_coordinates[1]) &&
                    rect_image.contains(photo_image_coordinates[2]) &&
                    rect_image.contains(photo_image_coordinates[3]) &&
                    mRectOrtho.contains(ortho_image_coordinates[0]) &&
                    mRectOrtho.contains(ortho_image_coordinates[1]) &&
                    mRectOrtho.contains(ortho_image_coordinates[2]) &&
                    mRectOrtho.contains(ortho_image_coordinates[3])) {

                    tl::WindowI window_ortho_in = boundingWindow(ortho_image_coordinates.begin(), ortho_image_coordinates.end());
                    if (!window_ortho_in.isValid()) continue;
                    cv::Mat out = cv::Mat::zeros(cv::Size(window_ortho_in.width(), window_ortho_in.height()), undistort_image.type());

                    tl::WindowI window_image_in = boundingWindow(photo_image_coordinates.begin(), photo_image_coordinates.end());
                    if (!window_image_in.isValid()) continue;


                    /// Estoy expandiendo despues de comprobar si se sale de los limites de la imagen... Solucionar cuando refactorice
                    /// Habría que calcular la intersección de window_aux con la ventana imagen total.
                    tl::WindowI window_aux = tl::expandWindow(window_image_in, 2);
                    if (rect_image.contains(window_aux.pt1) &&
                        rect_image.contains(window_aux.pt2)) {
                        window_image_in = window_aux;
                    }
                    cv::Mat in(window_image_in.height(), window_image_in.width(), undistort_image.type());

                    cv::Point2f cv_photo_image_coordinates[4];
                    cv::Point2f cv_ortho_image_coordinates[4];
                    for (int i = 0; i < 4; i++) {
                        cv_photo_image_coordinates[i] = cv::Point2f(static_cast<float>(photo_image_coordinates[i].x - window_image_in.pt1.x),
                                                                    static_cast<float>(photo_image_coordinates[i].y - window_image_in.pt1.y));
                        cv_ortho_image_coordinates[i] = cv::Point2f(static_cast<float>(ortho_image_coordinates[i].x - window_ortho_in.pt1.x),
                                                                    static_cast<float>(ortho_image_coordinates[i].y - window_ortho_in.pt1.y));
                    }
                    cv::Mat h = cv::getPerspectiveTransform(cv_ortho_image_coordinates, cv_photo_image_coordinates);

                    undistort_image.colRange(window_image_in.pt1.x, window_image_in.pt2.x)
                        .rowRange(window_image_in.pt1.y, window_image_in.pt2.y).copyTo(in);

                    cv::warpPerspective(in, out, h, cv::Size(window_ortho_in.width(), window_ortho_in.height()), cv::INTER_NEAREST | cv::WARP_INVERSE_MAP, cv::BORDER_TRANSPARENT);

                    out.copyTo(mat_ortho.colRange(window_ortho_in.pt1.x, window_ortho_in.pt2.x)
                               .rowRange(window_ortho_in.pt1.y, window_ortho_in.pt2.y));
                }
            }
        }

        mOrthophotoWriter->setCRS(mCrs.toWktFormat());
        mOrthophotoWriter->setGeoreference(mGeoreference);
        mOrthophotoWriter->write(mat_ortho);
        mOrthophotoWriter->close();

    } catch (std::exception &e) {
        if (mOrthophotoWriter) mOrthophotoWriter->close();
        tl::Message::error("Orthorectified image fail: {}", ortho.fileName().toString());
        tl::printException(e);
    } catch (...) {
        if (mOrthophotoWriter) mOrthophotoWriter->close();
        tl::Message::error("Orthorectified image fail: {}", ortho.fileName().toString());
        tl::Message::error("Unhandled exception");
    }
}







/* OrthoimageProcess */

OrthoimageProcess::OrthoimageProcess(const std::vector<Image> &images,
                                     const std::map<int, Camera> &cameras,
                                     const tl::Path &dtm,
                                     const tl::Path &orthoPath,
                                     const tl::Path &graphOrthos,
                                     const tl::Crs &crs,
                                     const tl::Path &footprint,
                                     double scale,
                                     double crop,
                                     bool cuda)
  : mImages(images),
    mCameras(cameras),
    mDtm(dtm),
    mOrthoPath(orthoPath),
    mCrs(crs),
    mScale(scale),
    mCrop(crop),
    bCuda(cuda)
{
    mOrthoPath.createDirectories();
    graphOrthos.parentPath().createDirectories();
    footprint.parentPath().createDirectories();

    mFootprintWriter = tl::VectorWriterFactory::create(footprint);
    mFootprintWriter->open();

    mGraphOrthosWriter = tl::VectorWriterFactory::create(graphOrthos);
    mGraphOrthosWriter->open();
}

OrthoimageProcess::~OrthoimageProcess()
{
}

void OrthoimageProcess::execute(tl::Progress *progressBar)
{

    if (!mFootprintWriter->isOpen()) throw std::runtime_error("Footprint open error");
    if (!mGraphOrthosWriter) throw std::runtime_error("Graph Orthos open error");

    std::shared_ptr<tl::TableField> field(new tl::TableField("image",
                                          tl::TableField::Type::STRING,
                                          254));

    mFootprintWriter->create();
    mFootprintWriter->setCRS(mCrs.toWktFormat());

    tl::GLayer layer;
    layer.setName("footprint");
    layer.addDataField(field);

    std::shared_ptr<tl::TableField> field_ortho(new tl::TableField("orthoimage",
                                                tl::TableField::Type::STRING,
                                                254));

    mGraphOrthosWriter->create();
    mGraphOrthosWriter->setCRS(mCrs.toWktFormat());

    tl::GLayer layer_ortho_graph;
    layer_ortho_graph.setName("ortho_graph");
    layer_ortho_graph.addDataField(field_ortho);

    tl::Path ortho_file;

    TL_TODO("Producer-consumer...")

    //if (progressBar) progressBar->setMaximum(mImages.size());

    for (const auto &image : mImages) {

        try {

            tl::Path file(image.path().toStdWString());
            tl::Path file_name = file.fileName();

            //if (file_name.toString() != "DSC04741.JPG") continue; ///..........

            if (!file.exists()) {
                 tl::Message::warning("Image not found: {}", file_name.toString());
                continue;
            }

            ortho_file = mOrthoPath;
            ortho_file.append(file_name).replaceExtension(".png");

            Orthorectification orthorectification(mDtm, mCameras[image.cameraId()], image.cameraPose());
            orthorectification.setCuda(bCuda);
            if (!orthorectification.isValid()) continue;

            std::shared_ptr<tl::GPolygon> entity = std::make_shared<tl::GPolygon>(orthorectification.footprint());

            double scale = mScale;
            if (mScale == -1) {
                /// Calculo de transformación afin entre coordenadas terreno e imagen para la orto para determinar una escala optima
                std::vector<tl::Point<double>> t_coor;
                t_coor.push_back(entity->at(0));
                t_coor.push_back(entity->at(1));
                t_coor.push_back(entity->at(2));
                t_coor.push_back(entity->at(3));

                tl::Rect<int> rect_image = orthorectification.rectImage();
                std::vector<tl::Point<double>> i_coor;
                i_coor.push_back(orthorectification.imageToPhotocoordinates(rect_image.topLeft()));
                i_coor.push_back(orthorectification.imageToPhotocoordinates(rect_image.topRight()));
                i_coor.push_back(orthorectification.imageToPhotocoordinates(rect_image.bottomRight()));
                i_coor.push_back(orthorectification.imageToPhotocoordinates(rect_image.bottomLeft()));

                tl::geom::Affine<tl::Point<double>> affine_terrain_image;
                affine_terrain_image.compute(i_coor, t_coor);
                scale = (affine_terrain_image.scaleY() + affine_terrain_image.scaleX()) / 2.;
            }

            // Se reserva tamaño para la orto
            tl::Window<tl::Point<double>> window_ortho_terrain = orthorectification.footprint().window();
            window_ortho_terrain = expandWindow(window_ortho_terrain,
                                                window_ortho_terrain.width() * (mCrop - 1.) / 2.,
                                                window_ortho_terrain.height() * (mCrop - 1.) / 2.);
            int rows_ortho = static_cast<int>(std::round(window_ortho_terrain.height() / scale));
            int cols_ortho = static_cast<int>(std::round(window_ortho_terrain.width() / scale));
            tl::Rect<int> rect_ortho = tl::Rect<int>(0, 0, cols_ortho, rows_ortho);

            tl::Affine<double, 2> affine_ortho(scale, -scale, 
                                               window_ortho_terrain.pt1.x,
                                               window_ortho_terrain.pt2.y, 0.0);
            /// Grafico ortofotos
            {
                tl::Rect<double> rect(window_ortho_terrain.pt1, window_ortho_terrain.pt2);
                rect.normalized();
                std::shared_ptr<tl::GPolygon> entity_ortho = std::make_shared<tl::GPolygon>();
                entity_ortho->push_back(rect.topLeft());
                entity_ortho->push_back(rect.topRight());
                entity_ortho->push_back(rect.bottomRight());
                entity_ortho->push_back(rect.bottomLeft());
                std::shared_ptr<tl::TableRegister> data_ortho(new tl::TableRegister(layer_ortho_graph.tableFields()));
                data_ortho->setValue(0, ortho_file.toString());
                entity_ortho->setData(data_ortho);
                layer_ortho_graph.push_back(entity_ortho);
            }


            ZBuffer zBuffer(&orthorectification, rect_ortho, affine_ortho);
            zBuffer.run();

            cv::Mat visibility_map = visibilityMap(orthorectification, zBuffer);

            Orthoimage orthoimage(file, &orthorectification, mCrs, rect_ortho, affine_ortho, bCuda);
            orthoimage.run(ortho_file, visibility_map);

            std::shared_ptr<tl::TableRegister> data(new tl::TableRegister(layer.tableFields()/*fields*/));
            data->setValue(0, ortho_file.toString());
            entity->setData(data);
            layer.push_back(entity);

            tl::Message::info("Write orthoimage: {}", ortho_file.fileName().toString());

        } catch (const std::exception &e) {
            tl::printException(e);
            tl::Message::error("Write orthoimage error: {}", ortho_file.fileName().toString());
        }

        if (progressBar) (*progressBar)();
    }

    mFootprintWriter->write(layer);
    mFootprintWriter->close();

    mGraphOrthosWriter->write(layer_ortho_graph);
    mGraphOrthosWriter->close();
}

cv::Mat OrthoimageProcess::visibilityMap(const Orthorectification &orthorectification,
                                         const ZBuffer &zBuffer) const
{
    cv::Mat z_buffer_x = zBuffer.mapX();
    cv::Mat z_buffer_y = zBuffer.mapY();

    tl::Rect<int> rect_dtm = orthorectification.rectDtm();
    cv::Mat visibility_map = cv::Mat::zeros(rect_dtm.height, rect_dtm.width, CV_8U);

    for (int r = 0; r < z_buffer_y.rows; r++) {
        for (int c = 0; c < z_buffer_y.cols; c++) {
            int row = z_buffer_y.at<int>(r, c);
            int col = z_buffer_x.at<int>(r, c);
            if (row != -1 && col != -1) {
                visibility_map.at<uchar>(row, col) = 255;
            }
        }
    }

    return visibility_map;
}


} // namespace graphos
