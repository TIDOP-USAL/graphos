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

#include "OrthoimageTask.h"

#include "graphos/components/cameras/impl/CamerasModel.h"
#include "graphos/components/orthophoto/impl/ZBuffer.h"
#include "graphos/components/orthophoto/impl/Orthorectification.h"
#include "graphos/components/orthophoto/impl/Orthoimage.h"
#include "graphos/components/orthophoto/impl/Orthorectification.h"

#include <tidop/core/progress.h>
#include <tidop/graphic/layer.h>
#include <tidop/geotools/GeoTools.h>
#include <tidop/geotools/CRSsTools.h>

namespace graphos
{


cv::Mat visibilityMap(const Orthorectification &orthorectification,
                      const ZBuffer &zBuffer)
{
    tl::ChronoAuto chrono("Visibility map");

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


/* OrthoimageTask */

OrthoimageTask::OrthoimageTask(const std::vector<Image> &images,
                                     const std::map<int, Camera> &cameras,
                                     const tl::Path &dtm,
                                     const tl::Path &orthoPath,
                                     const tl::Path &graphOrthos,
                                     const std::string &enuCrs,
                                     const std::string &crs,
                                     const tl::Path &footprint,
                                     double gsd,
                                     const std::string &interpolation,
                                     double crop,
                                     bool cuda)
  : mImages(images),
    mCameras(cameras),
    mDtm(dtm),
    mOrthoPath(orthoPath),
    mEnuCrs(enuCrs),
    mCrs(crs),
    mGsd(gsd),
    mInterpolation(interpolation),
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

OrthoimageTask::~OrthoimageTask()
{
}

void OrthoimageTask::execute(tl::Progress *progressBar)
{
    try {

        tl::GeoTools *geo_tools = tl::GeoTools::getInstance();

        TL_ASSERT(mFootprintWriter->isOpen(), "Footprint open error");
        TL_ASSERT(mGraphOrthosWriter->isOpen(), "Graph Orthos open error");

        std::shared_ptr<tl::TableField> field(new tl::TableField("image",
                                              tl::TableField::Type::STRING,
                                              254));

        mFootprintWriter->create();
        tl::Crs crs(mCrs);
        mFootprintWriter->setCRS(crs.toWktFormat());

        tl::GLayer layer;
        layer.setName("footprint");
        layer.addDataField(field);

        std::shared_ptr<tl::TableField> field_ortho(new tl::TableField("orthoimage",
                                                    tl::TableField::Type::STRING,
                                                    254));

        mGraphOrthosWriter->create();
        mGraphOrthosWriter->setCRS(crs.toWktFormat());

        tl::GLayer layer_ortho_graph;
        layer_ortho_graph.setName("ortho_graph");
        layer_ortho_graph.addDataField(field_ortho);

        tl::Path ortho_file;

        TL_TODO("Producer-consumer...")

        //if (progressBar) progressBar->setMaximum(mImages.size());
        
        // Z inicial
        double z_ini = 0.;
        {
            std::unique_ptr<tl::ImageReader> dem_reader = tl::ImageReaderFactory::create(mDtm);
            dem_reader->open();
            TL_ASSERT(dem_reader->isOpen(), "Error al cargar el DEM");
            cv::Mat dem = dem_reader->read(0.1, 0.1); 
            dem_reader->close();
            cv::Mat mask = cv::Mat::zeros(dem.rows, dem.cols, CV_8U);
            mask.setTo(cv::Scalar::all(255), dem > -9999.);
            cv::Scalar zmean = cv::mean(dem, mask);
            z_ini = zmean(0);
        }


        for (const auto &image : mImages) {

            try {

                tl::Chrono chrono;
                chrono.run();

                tl::Path file(image.path().toStdWString());
                tl::Path file_name = file.fileName();

                //if (file_name.toString() != "DSC04741.JPG") continue; ///..........

                if (!file.exists()) {
                    tl::Message::warning("Image not found: {}", file_name.toString());
                    continue;
                }

                ortho_file = mOrthoPath;
                ortho_file.append(file_name).replaceExtension(".png");

                Orthorectification orthorectification(mDtm,
                                                      mCameras[image.cameraId()],
                                                      image.cameraPose(),
                                                      z_ini);
                orthorectification.setCuda(bCuda);
                if (!orthorectification.isValid()) continue;

                std::shared_ptr<tl::GPolygon> entity = std::make_shared<tl::GPolygon>(orthorectification.footprint());

                double gsd = mGsd;
                if (mGsd == -1) {
                    /// Calculo de transformación afin entre coordenadas terreno e imagen para la orto para determinar un GSD optimo
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

                    tl::Affine<double, 2> affine_terrain_image = tl::Affine2DEstimator<double>::estimate(i_coor, t_coor);
                    gsd = (affine_terrain_image.scale().x() + affine_terrain_image.scale().y()) / 2.;
                }

                // Se reserva tamaño para la orto
                tl::Window<tl::Point<double>> window_ortho_terrain = entity->window();
                window_ortho_terrain = expandWindow(window_ortho_terrain,
                    window_ortho_terrain.width() * (mCrop - 1.) / 2.,
                    window_ortho_terrain.height() * (mCrop - 1.) / 2.);
                int rows_ortho = static_cast<int>(std::round(window_ortho_terrain.height() / gsd));
                int cols_ortho = static_cast<int>(std::round(window_ortho_terrain.width() / gsd));
                tl::Rect<int> rect_ortho = tl::Rect<int>(0, 0, cols_ortho, rows_ortho);

                tl::Affine<double, 2> affine_ortho(gsd, -gsd,
                    window_ortho_terrain.pt1.x,
                    window_ortho_terrain.pt2.y, 0.0);
                /// Grafico ortofotos
                {
                    tl::Rect<double> rect(window_ortho_terrain.pt1, window_ortho_terrain.pt2);
                    rect.normalized();
                    std::shared_ptr<tl::GPolygon> entity_ortho = std::make_shared<tl::GPolygon>();

                    tl::Point3d top_left_utm = static_cast<tl::Point3d>(rect.topLeft());
                    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, top_left_utm.x, top_left_utm.y, top_left_utm.z);
                    entity_ortho->push_back(static_cast<tl::Point2d>(top_left_utm));

                    tl::Point3d top_right_utm = static_cast<tl::Point3d>(rect.topRight());
                    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, top_right_utm.x, top_right_utm.y, top_right_utm.z);
                    entity_ortho->push_back(static_cast<tl::Point2d>(top_right_utm));

                    tl::Point3d bottom_right_utm = static_cast<tl::Point3d>(rect.bottomRight());
                    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, bottom_right_utm.x, bottom_right_utm.y, bottom_right_utm.z);
                    entity_ortho->push_back(static_cast<tl::Point2d>(bottom_right_utm));

                    tl::Point3d bottom_left_utm = static_cast<tl::Point3d>(rect.bottomLeft());
                    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, bottom_left_utm.x, bottom_left_utm.y, bottom_left_utm.z);
                    entity_ortho->push_back(static_cast<tl::Point2d>(bottom_left_utm));

                    std::shared_ptr<tl::TableRegister> data_ortho(new tl::TableRegister(layer_ortho_graph.tableFields()));
                    data_ortho->setValue(0, ortho_file.toString());
                    entity_ortho->setData(data_ortho);
                    layer_ortho_graph.push_back(entity_ortho);
                }


                ZBuffer zBuffer(&orthorectification, rect_ortho, affine_ortho);
                zBuffer.run();

                cv::Mat visibility_map = visibilityMap(orthorectification, zBuffer);

                Orthoimage orthoimage(file,
                                      &orthorectification,
                                      mEnuCrs,
                                      mCrs,
                                      rect_ortho,
                                      affine_ortho,
                                      mInterpolation,
                                      bCuda);

                orthoimage.run(ortho_file, visibility_map);

                std::shared_ptr<tl::TableRegister> data(new tl::TableRegister(layer.tableFields()));
                data->setValue(0, ortho_file.toString());
                entity->setData(data);

                {
                    tl::Point3d top_left_utm = static_cast<tl::Point3d>(static_cast<tl::Point3d>(entity->at(0)));
                    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, top_left_utm.x, top_left_utm.y, top_left_utm.z);
                    tl::Point3d top_right_utm = static_cast<tl::Point3d>(static_cast<tl::Point3d>(entity->at(1)));
                    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, top_right_utm.x, top_right_utm.y, top_right_utm.z);
                    tl::Point3d bottom_right_utm = static_cast<tl::Point3d>(static_cast<tl::Point3d>(entity->at(2)));
                    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, bottom_right_utm.x, bottom_right_utm.y, bottom_right_utm.z);
                    tl::Point3d bottom_left_utm = static_cast<tl::Point3d>(static_cast<tl::Point3d>(entity->at(2)));
                    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, bottom_left_utm.x, bottom_left_utm.y, bottom_left_utm.z);

                    entity->at(0) = static_cast<tl::Point2d>(top_left_utm);
                    entity->at(1) = static_cast<tl::Point2d>(top_right_utm);
                    entity->at(2) = static_cast<tl::Point2d>(bottom_right_utm);
                    entity->at(3) = static_cast<tl::Point2d>(bottom_left_utm);
                }

                layer.push_back(entity);

                tl::Message::info("Write orthoimage {} in {:.2} minutes", ortho_file.fileName().toString(), chrono.stop() / 60.);

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

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}



} // namespace graphos
