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

#include <tidop/core/task/Progress.h>
#include <tidop/graphic/layer.h>
#include <tidop/geotools/GeoTools.h>
#include <tidop/geotools/CRSsTools.h>

#ifdef HAVE_OPENCV_CUDAWARPING
#include <opencv2/cudawarping.hpp>
#endif
#ifdef HAVE_OPENCV_CUDAARITHM
#include <opencv2/cudaarithm.hpp>
#endif
#include <opencv2/imgcodecs.hpp>

namespace graphos
{


cv::Mat visibilityMap(const Orthorectification &orthorectification,
                      const ZBuffer &zBuffer)
{
    tl::ChronoAuto chrono("Visibility map");

    cv::Mat z_buffer_x = zBuffer.mapX();
    cv::Mat z_buffer_y = zBuffer.mapY();

    tl::Rect<int> rect_dtm = orthorectification.rectDsm();
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

OrthoimageTask::OrthoimageTask(const std::unordered_map<size_t, Image> &images,
                                     const std::map<int, Camera> &cameras,
                                     const tl::Path &dtm,
                                     const tl::Path &orthoPath,
                                     const tl::Path &graphOrthos,
                                     const std::string &enuCrs,
                                     const std::string &crs,
                                     const tl::Affine<double, 2> &georeferenceOrthomosaic,
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
    mGeoreferenceOrthomosaic(georeferenceOrthomosaic),
    mGsd(gsd),
    mInterpolation(interpolation),
    mCrop(crop),
    bCuda(cuda)
{
    mOrthoPath.parentPath().createDirectories();
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

        tl::Chrono chrono;
        chrono.run();

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
            mask.setTo(cv::Scalar::all(255), dem > tl::NoData<float>);
            cv::Scalar zmean = cv::mean(dem, mask);
            z_ini = zmean(0);
        }

        // Ya no tiene sentido porque se utilizan las imagenes y cámaras corregidas
        //std::map<int, std::shared_ptr<Undistort>> undistort;

        for (const auto &image_pair : mImages) {

            const auto &image = image_pair.second;
            int camera_id = image.cameraId();

            try {

                if (status() == Status::stopping)  break;

                tl::Chrono chrono_orthoimage;
                chrono_orthoimage.run();

                tl::Path file(image.path().toStdWString());
                tl::Path file_name = file.fileName();

                //if (file_name.toString() != "DSC04741.JPG") continue; ///..........

                if (!file.exists()) {
                    tl::Message::warning("Image not found: {}", file_name.toString());
                    continue;
                }

                                    
                //auto _undistort = undistort.find(image.cameraId());
                //if (_undistort == undistort.end()) {

                //    const auto &camera = mCameras.find(image.cameraId());
                //    if (camera != mCameras.end()) {
                //        undistort[camera->first] = std::make_shared<Undistort>(camera->second);
                //    }

                //}

                ortho_file = mOrthoPath;
                ortho_file.append(file_name).replaceExtension(".tif");


                Orthorectification orthorectification(mDtm,
                                                      image.cameraPose(),
                                                      mCameras.at(camera_id),//undistort[image.cameraId()],
                                                      z_ini);
                orthorectification.setCuda(bCuda);
                if (!orthorectification.isValid()) continue;

                auto footprint_enu = orthorectification.footprint();

                // tendría que generar la ortoimagen con el GSD óptimo si es mayor que el de destino y despues escalar la imagen
                // Mostrar un Warning en esos casos

                double gsd = mGsd;
                //if (mGsd == -1) {
                //    /// Calculo de transformación afin entre coordenadas terreno e imagen para la orto para determinar un GSD optimo
                //    std::vector<tl::Point<double>> t_coor;
                //    t_coor.push_back(footprint_enu.at(0));
                //    t_coor.push_back(footprint_enu.at(1));
                //    t_coor.push_back(footprint_enu.at(2));
                //    t_coor.push_back(footprint_enu.at(3));

                //    tl::Rect<int> rect_image = orthorectification.rectImage();
                //    std::vector<tl::Point<double>> i_coor;
                //    i_coor.push_back(orthorectification.imageToPhotocoordinates(rect_image.topLeft()));
                //    i_coor.push_back(orthorectification.imageToPhotocoordinates(rect_image.topRight()));
                //    i_coor.push_back(orthorectification.imageToPhotocoordinates(rect_image.bottomRight()));
                //    i_coor.push_back(orthorectification.imageToPhotocoordinates(rect_image.bottomLeft()));

                //    tl::Affine<double, 2> affine_terrain_image = tl::Affine2DEstimator<double>::estimate(i_coor, t_coor);
                //    gsd = (affine_terrain_image.scale().x() + affine_terrain_image.scale().y()) / 2.;
                //}

                /******************************************************************************************************/
                /* 
                   Esto se esta haciendo en coordenadas ENU lo cual provoca que las ortos generadas, aunque están bien,
                   no están alineadas con la orto final. Para evitar interpolaciones y problemas en el método de fusión 
                   de teselas es mejor trabajar directamente en el sistema de la orto 
                */

                // Se reserva tamaño para la orto
                tl::Window<tl::Point<double>> window_ortho_enu = footprint_enu.window();
                //window_ortho_enu = tl::expandWindow(window_ortho_enu,
                //                                    window_ortho_enu.width() * (mCrop - 1.) / 2.,
                //                                    window_ortho_enu.height() * (mCrop - 1.) / 2.);
                auto window_dem_enu = orthorectification.windowDsm();
                window_ortho_enu = tl::windowIntersection(window_ortho_enu, window_dem_enu);
                tl::Size<int> size_ortho_enu(static_cast<int>(std::round(window_ortho_enu.width() / gsd)),
                                             static_cast<int>(std::round(window_ortho_enu.height() / gsd)));
                /******************************************************************************************************/

                // Huella de vuelo en el sistema de coordenadas de salida 
                auto footprint_projected = footprintProjected(footprint_enu);
                std::shared_ptr<tl::TableRegister> data(new tl::TableRegister(layer.tableFields()));
                data->setValue(0, ortho_file.toUtf8());
                footprint_projected->setData(data);
                layer.push_back(footprint_projected);

                /// Gráfico de ortofotos en el sistema de coordenadas de salida 

                tl::Rect<double> rect(window_ortho_enu.pt1, window_ortho_enu.pt2);
                rect.normalized();

                std::shared_ptr<tl::GPolygon> ortho_graph_projected = std::make_shared<tl::GPolygon>();

                tl::Point3d top_left_projected = static_cast<tl::Point3d>(rect.topLeft());
                geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, top_left_projected.x, top_left_projected.y, top_left_projected.z);
                ortho_graph_projected->push_back(static_cast<tl::Point2d>(top_left_projected));

                tl::Point3d top_right_projected = static_cast<tl::Point3d>(rect.topRight());
                geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, top_right_projected.x, top_right_projected.y, top_right_projected.z);
                ortho_graph_projected->push_back(static_cast<tl::Point2d>(top_right_projected));

                tl::Point3d bottom_right_projected = static_cast<tl::Point3d>(rect.bottomRight());
                geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, bottom_right_projected.x, bottom_right_projected.y, bottom_right_projected.z);
                ortho_graph_projected->push_back(static_cast<tl::Point2d>(bottom_right_projected));

                tl::Point3d bottom_left_projected = static_cast<tl::Point3d>(rect.bottomLeft());
                geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, bottom_left_projected.x, bottom_left_projected.y, bottom_left_projected.z);
                ortho_graph_projected->push_back(static_cast<tl::Point2d>(bottom_left_projected));

                std::shared_ptr<tl::TableRegister> data_ortho(new tl::TableRegister(layer_ortho_graph.tableFields()));
                data_ortho->setValue(0, ortho_file.toUtf8());
                ortho_graph_projected->setData(data_ortho);
                layer_ortho_graph.push_back(ortho_graph_projected);

                // Ajustar a la georeferencia del mosaico
                tl::Window<tl::Point<double>> window_ortho_terrain = ortho_graph_projected->window();
                tl::Size<int> size_ortho_terrain(static_cast<int>(std::round(window_ortho_terrain.width() / gsd)),
                                                 static_cast<int>(std::round(window_ortho_terrain.height() / gsd)));
                {
                    auto affine_ortho_inverse = mGeoreferenceOrthomosaic.inverse();
                    tl::Point2i pt1 = affine_ortho_inverse.transform(window_ortho_terrain.pt1);
                    tl::Point2i pt2 = affine_ortho_inverse.transform(window_ortho_terrain.pt2);
                    window_ortho_terrain.pt1 = mGeoreferenceOrthomosaic.transform(static_cast<tl::Point<double>>(pt1));
                    window_ortho_terrain.pt2 = mGeoreferenceOrthomosaic.transform(static_cast<tl::Point<double>>(pt2));
                }
 
                // Z-Buffer en coordenadas ENU
                tl::Affine<double, 2> affine_ortho_enu(gsd, -gsd,
                                                       window_ortho_enu.pt1.x,
                                                       window_ortho_enu.pt2.y, 0.0);

                ZBuffer zBuffer(&orthorectification, size_ortho_enu, affine_ortho_enu);
                zBuffer.run();

                cv::Mat visibility_map = visibilityMap(orthorectification, zBuffer);


                tl::Affine<double, 2> affine_ortho_terrain(gsd, -gsd,
                                                           window_ortho_terrain.pt1.x,
                                                           window_ortho_terrain.pt2.y, 0.0);

                Orthoimage orthoimage(file,
                                      &orthorectification,
                                      mEnuCrs,
                                      mCrs,
                                      size_ortho_terrain,
                                      affine_ortho_terrain,     
                                      mInterpolation,
                                      bCuda);

                orthoimage.run(ortho_file, visibility_map);

                tl::Message::info("Orthoimage {} generated in {:.2} minutes", ortho_file.fileName().toUtf8(), chrono_orthoimage.stop() / 60.);

            } catch (const std::exception &e) {
                tl::printException(e);
                tl::Message::error("Orthoimage error: {}", ortho_file.fileName().toUtf8());
            }

            if (progressBar) (*progressBar)();
        }

        mFootprintWriter->write(layer);
        mFootprintWriter->close();

        mGraphOrthosWriter->write(layer_ortho_graph);
        mGraphOrthosWriter->close();

        tl::Message::info("Orthoimages generated in {:.2} minutes", chrono.stop() / 60.);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

//auto OrthoimageTask::orthoimageGraphProjected(const tl::Window<tl::Point<double>> &windowOrthoTerrain) const -> std::shared_ptr<tl::GPolygon>
//{
//    tl::GeoTools *geo_tools = tl::GeoTools::getInstance();
//
//    tl::Rect<double> rect(windowOrthoTerrain.pt1, windowOrthoTerrain.pt2);
//    rect.normalized();
//
//    std::shared_ptr<tl::GPolygon> entity_ortho = std::make_shared<tl::GPolygon>();
//
//    tl::Point3d top_left_utm = static_cast<tl::Point3d>(rect.topLeft());
//    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, top_left_utm.x, top_left_utm.y, top_left_utm.z);
//    entity_ortho->push_back(static_cast<tl::Point2d>(top_left_utm));
//
//    tl::Point3d top_right_utm = static_cast<tl::Point3d>(rect.topRight());
//    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, top_right_utm.x, top_right_utm.y, top_right_utm.z);
//    entity_ortho->push_back(static_cast<tl::Point2d>(top_right_utm));
//
//    tl::Point3d bottom_right_utm = static_cast<tl::Point3d>(rect.bottomRight());
//    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, bottom_right_utm.x, bottom_right_utm.y, bottom_right_utm.z);
//    entity_ortho->push_back(static_cast<tl::Point2d>(bottom_right_utm));
//
//    tl::Point3d bottom_left_utm = static_cast<tl::Point3d>(rect.bottomLeft());
//    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, bottom_left_utm.x, bottom_left_utm.y, bottom_left_utm.z);
//    entity_ortho->push_back(static_cast<tl::Point2d>(bottom_left_utm));
//    
//    return entity_ortho;
//}

auto OrthoimageTask::footprintProjected(const tl::GPolygon &footprint) const -> std::shared_ptr<tl::GPolygon>
{
    tl::GeoTools *geo_tools = tl::GeoTools::getInstance();

    std::shared_ptr<tl::GPolygon> entity = std::make_shared<tl::GPolygon>();

    tl::Point3d top_left_utm = static_cast<tl::Point3d>(static_cast<tl::Point3d>(footprint.at(0)));
    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, top_left_utm.x, top_left_utm.y, top_left_utm.z);
    entity->push_back(static_cast<tl::Point2d>(top_left_utm));

    tl::Point3d top_right_utm = static_cast<tl::Point3d>(static_cast<tl::Point3d>(footprint.at(1)));
    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, top_right_utm.x, top_right_utm.y, top_right_utm.z);
    entity->push_back(static_cast<tl::Point2d>(top_right_utm));

    tl::Point3d bottom_right_utm = static_cast<tl::Point3d>(static_cast<tl::Point3d>(footprint.at(2)));
    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, bottom_right_utm.x, bottom_right_utm.y, bottom_right_utm.z);
    entity->push_back(static_cast<tl::Point2d>(bottom_right_utm));

    tl::Point3d bottom_left_utm = static_cast<tl::Point3d>(static_cast<tl::Point3d>(footprint.at(3)));
    geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, bottom_left_utm.x, bottom_left_utm.y, bottom_left_utm.z);
    entity->push_back(static_cast<tl::Point2d>(bottom_left_utm));

    return entity;
}



} // namespace graphos
