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

#include "DemTask.h"

#include "graphos/framework/task/ProgressHandler.h"
#include "graphos/core/utils.h"

TL_DISABLE_WARNINGS
#include "csf/src/CSF.h"
TL_DEFAULT_WARNINGS

#include <tidop/core/app/Message.h>
#include <tidop/core/base/Path.h>
#include <tidop/core/base/Size.h>
#include <tidop/geotools/CRSsTools.h>
#include <tidop/geotools/GeoTools.h>
#include <tidop/rastertools/io/Reader.h>
#include <tidop/rastertools/io/Writer.h>
#include <tidop/geometry/spatial/BoundingBox.h>
#include <tidop/geometry/primitives/Point.h>
#include <tidop/geospatial/crs.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Projection_traits_xy_3.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Polygon_mesh_processing/compute_normal.h>
#include <CGAL/Barycentric_coordinates_2/triangle_coordinates_2.h>

#include <opencv2/imgcodecs.hpp>

#include <fstream>
#include <cmath>
#include <vector>

using Kernel = CGAL::Simple_cartesian<double>;
using Projection_traits = CGAL::Projection_traits_xy_3<Kernel>;
using DelaunayTriangulation = CGAL::Delaunay_triangulation_2<Projection_traits>;
using Point_2 = Kernel::Point_2;
using Point_3 = Kernel::Point_3;

namespace graphos
{

namespace
{

auto barycentricCoordinates(const Point_3 &pt1,
                            const Point_3 &pt2, 
                            const Point_3 &pt3, 
                            const Point_3 &query) -> std::array<double, 3>
{
    Point_2 p1(pt1.x(), pt1.y());
    Point_2 p2(pt2.x(), pt2.y());
    Point_2 p3(pt3.x(), pt3.y());
    Point_2 q(query.x(), query.y());

    std::array<double, 3> output;
    CGAL::Barycentric_coordinates::triangle_coordinates_2(p1, p2, p3, q, output.begin());
    return output;
}

//auto getValidFaces(const DelaunayTriangulation &tin, 
//                   double maxBoundaryEdgeLength) -> std::unordered_set<DelaunayTriangulation::Face_handle>
//{
//    std::unordered_set<DelaunayTriangulation::Face_handle> valid_faces;
//    std::unordered_set<DelaunayTriangulation::Face_handle> visited;
//    std::queue<DelaunayTriangulation::Face_handle> queue;
//
//    double max_sq_dist = maxBoundaryEdgeLength * maxBoundaryEdgeLength;
//
//    // Insertar en la cola todas las caras finitas que colindan con el exterior (infinite_vertex)
//    for (auto fit = tin.all_faces_begin(); fit != tin.all_faces_end(); ++fit) {
//        if (tin.is_infinite(fit)) continue;
//
//        valid_faces.insert(fit); // Inicialmente asumimos que todas son válidas
//
//        // Si la cara tiene un vértice infinito, su vecina es una cara de borde
//        for (int i = 0; i < 3; ++i) {
//            if (tin.is_infinite(fit->vertex(i))) {
//                // Esta cara está en la frontera exterior
//                queue.push(fit);
//                break;
//            }
//        }
//    }
//
//    // Erosionar desde el borde exterior hacia el interior
//    while (!queue.empty()) {
//        auto face = queue.front();
//        queue.pop();
//
//        if (visited.count(face)) continue;
//        visited.insert(face);
//
//        // Comprobar si alguna de sus aristas finitas supera el umbral del borde
//        bool should_remove = false;
//        for (int i = 0; i < 3; ++i) {
//            auto v1 = face->vertex((i + 1) % 3);
//            auto v2 = face->vertex((i + 2) % 3);
//
//            if (tin.is_infinite(v1) || tin.is_infinite(v2)) continue;
//
//            if (CGAL::squared_distance(v1->point(), v2->point()) > max_sq_dist) {
//                should_remove = true;
//                break;
//            }
//        }
//
//        // Si la cara de contorno supera el límite, se elimina y se propagan sus vecinas
//        if (should_remove) {
//            valid_faces.erase(face);
//
//            for (int i = 0; i < 3; ++i) {
//                auto neighbor = face->neighbor(i);
//                if (!tin.is_infinite(neighbor) && !visited.count(neighbor)) {
//                    queue.push(neighbor);
//                }
//            }
//        }
//    }
//
//    return valid_faces;
//}

auto extractDTMfromTIN(const DelaunayTriangulation &tin,
                       const tl::Size<int> &size,
                       const tl::Affine<double, 2> &georeference,
                       tl::Progress *progressBar,
                       const DemTask *demTask) -> cv::Mat
{
    cv::Mat mat(size.height(), size.width(), CV_32F, tl::NoData<float>);
    DelaunayTriangulation::Face_handle location;

    const double increment = 40.0 / static_cast<double>(size.height());
    double progress = 0.0;

    for (int r = 0; r < size.height(); ++r) {
        
        if (demTask->status() == DemTask::Status::stopping)  break;

        for (int c = 0; c < size.width(); ++c) {
            auto point = georeference.transform(tl::Point2d(c, r));
            Point_3 query(point.x(), point.y(), 0.0);
            location = tin.locate(query, location);

            if (!tin.is_infinite(location)) {
                const auto &v0 = location->vertex(0)->point();
                const auto &v1 = location->vertex(1)->point();
                const auto &v2 = location->vertex(2)->point();

                double twice_area = (v1.x() - v0.x()) * (v2.y() - v0.y()) - (v2.x() - v0.x()) * (v1.y() - v0.y());
                if (std::abs(twice_area) < 1e-5) {
                    continue;
                }

                auto barycentric = barycentricCoordinates(v0, v1, v2, query);
                double height_at_query = (barycentric[0] * v0.z() + barycentric[1] * v1.z() + barycentric[2] * v2.z());

                mat.at<float>(r, c) = static_cast<float>(height_at_query);
            }
        }

        if (progressBar) {
            progress += increment;
            int step = static_cast<int>(progress);
            if (step >= 1) {
                (*progressBar)(step);
                progress -= step;
            }
        }
    }

    return mat;
}

auto extractDSMfromPointCloud(const CGAL::Point_set_3<Point_3> &points,
                              const tl::BoundingBox2d &window,
                              const tl::Affine<double, 2> &georeference,
                              tl::CRSsTools *ptrCRSsTools,
                              const std::string &crsIn,
                              const std::string &crsOut,
                              const DemTask *demTask) -> cv::Mat
{
    tl::Affine<double, 2> georeference_inverse = georeference.inverse();
    double gsd = georeference.scale().x();
    tl::Size<int> size(tl::roundToInteger(window.width() / gsd),
                       tl::roundToInteger(window.height() / gsd));
    cv::Mat mat(size.height(), size.width(), CV_32F, tl::NoData<float>);

    for (const auto &point : points.points()) {

        if (demTask->status() == DemTask::Status::stopping)  break;

        tl::Point3d coords(point.x(), point.y(), point.z());
        if (ptrCRSsTools && !crsIn.empty() && !crsOut.empty()) {
            ptrCRSsTools->crsOperation(crsIn, crsOut, coords.x(), coords.y(), coords.z());
        }

        tl::Point2i point_image = static_cast<tl::Point2i>(georeference_inverse.transform(static_cast<tl::Point2d>(coords)));
        if (point_image.x() >= 0 && point_image.x() < size.width() &&
            point_image.y() >= 0 && point_image.y() < size.height() &&
            std::isfinite(coords.z())) {
            float &cell = mat.at<float>(point_image.y(), point_image.x());
            const float elevation = static_cast<float>(coords.z());
            cell = (cell == tl::NoData<float>) ? elevation : std::max(cell, elevation);
        }
    }

    return mat;
}

auto rasterToPointList(const cv::Mat &mat, 
                       const tl::Affine<double, 2> &georeference) -> std::vector<Point_3>
{
    std::vector<Point_3> points;
    points.reserve(static_cast<size_t>(mat.rows * mat.cols));

    for (int r = 0; r < mat.rows; ++r) {
        for (int c = 0; c < mat.cols; ++c) {
            float z = mat.at<float>(r, c);
            if (z == tl::NoData<float> || !std::isfinite(z)) continue;

            auto pt = georeference.transform(tl::Point2d(c, r));
            points.emplace_back(pt.x(), pt.y(), z);
        }
    }

    return points;
}

void writeDTM(const tl::Path &file, 
              const cv::Mat &mat,
              const tl::Affine<double, 2> &georeference,
              const std::string &epsg)
{
    tl::RasterWriter writer(file);
    TL_ASSERT(writer.isOpen(), "Cannot create image: {}", file.toString());

    writer.create(mat.rows, mat.cols, 1, tl::DataType::TL_32F);
    writer.setGeoreference(georeference);
    if (!epsg.empty()) {
        tl::Crs crs(epsg);
        writer.setCRS(crs.toWktFormat());
    }
    writer.setNoDataValue(tl::NoData<float>);
    writer.write(mat);
}

auto computeBoundingBox2dInCrs(const CGAL::Bbox_3 &cgalBbox,
                               const std::string &srcCrs,
                               const std::string &dstCrs,
                               tl::CRSsTools *crsTools) -> tl::BoundingBox2d
{
    std::vector<tl::Point2d> bbox_points;
    std::vector<tl::Point3d> pts = {
        tl::Point3d{cgalBbox.xmin(), cgalBbox.ymin(), cgalBbox.zmin()},
        tl::Point3d{cgalBbox.xmin(), cgalBbox.ymax(), cgalBbox.zmin()},
        tl::Point3d{cgalBbox.xmax(), cgalBbox.ymax(), cgalBbox.zmin()},
        tl::Point3d{cgalBbox.xmax(), cgalBbox.ymin(), cgalBbox.zmin()}
    };

    for (auto &pt : pts) {
        if (crsTools) {
            crsTools->crsOperation(srcCrs, dstCrs, pt.x(), pt.y(), pt.z());
        }
        bbox_points.emplace_back(pt.x(), pt.y());
    }

    return tl::BoundingBox2d(bbox_points);
}

void processDsmEnu(const CGAL::Point_set_3<Point_3> &points,
                   const CGAL::Bbox_3 &cgalBbox,
                   double gsd,
                   const tl::Path &outputPath,
                   const DemTask *demTask)
{
    tl::Affine<double, 2> georef_enu(gsd, -gsd, cgalBbox.xmin(), cgalBbox.ymax(), 0.0);
    tl::BoundingBox2d bbox_enu_2d(tl::Point2d(cgalBbox.xmin(), cgalBbox.ymin()),
                                  tl::Point2d(cgalBbox.xmax(), cgalBbox.ymax()));

    cv::Mat dsm_raster_enu = extractDSMfromPointCloud(points, bbox_enu_2d, georef_enu, nullptr, "", "", demTask);
    std::vector<Point_3> points_dsm_enu = rasterToPointList(dsm_raster_enu, georef_enu);

    if (points_dsm_enu.size() >= 3) {
        DelaunayTriangulation dtm_clean(points_dsm_enu.begin(), points_dsm_enu.end());
        tl::Size<int> size_enu(tl::roundToInteger(bbox_enu_2d.width() / gsd),
            tl::roundToInteger(bbox_enu_2d.height() / gsd));

        dsm_raster_enu = extractDTMfromTIN(dtm_clean, size_enu, georef_enu, nullptr, demTask);
        writeDTM(outputPath, dsm_raster_enu, georef_enu, "");
    } else {
        tl::Message::warning("Insufficient valid points to generate DSM ENU Delaunay triangulation.");
    }
}

} // namespace


DemTask::DemTask(tl::Path pointCloud,
                 tl::Path demPath,
                 std::string enuCrs,
                 const std::shared_ptr<DemProperties> &properties)
  : tl::Task(),
    mPointCloud(std::move(pointCloud)),
    mDemPath(std::move(demPath)),
    mEnuCrs(std::move(enuCrs)),
    mProperties(properties)
{
    if (mProperties->dsm()) {
        mDsmPath = mDemPath;
        mDsmPath.append("dsm.tif");
    }

    if (mProperties->dtm()) {
        mDtmPath = mDemPath;
        mDtmPath.append("dtm.tif");
    }

    mDsmEnuPath = mDemPath;
    mDsmEnuPath.append("dsm_enu.tif");
}

auto DemTask::report() const -> DemReport 
{ 
    return mDemReport; 
}

auto DemTask::dsmPath() const -> tl::Path 
{ 
    return mDsmPath; 
}

auto DemTask::dtmPath() const -> tl::Path
{
    return mDtmPath;
}

auto DemTask::dsmEnuPath() const -> tl::Path
{
    return mDsmEnuPath;
}

void DemTask::execute(tl::Progress *progressBar, std::stop_token stopToken)
{
    try {

        TL_ASSERT(mPointCloud.exists(), "Point cloud file does not exist: '{}'", mPointCloud.toString());
        TL_ASSERT(mPointCloud.isFile(), "The path is not valid: '{}'", mPointCloud.toString());

        mDemPath.createDirectories();

        std::ifstream input_stream(mPointCloud.toString(), std::ios::binary);
        CGAL::Point_set_3<Point_3> points;
        input_stream >> points;

        TL_ASSERT(points.size() > 0, "No points found in the point cloud file: '{}'", mPointCloud.toString());

        tl::Message::info("Read {} points", points.size());

        if (status() == Status::stopping) return;

        CGAL::Bbox_3 cgal_bbox = CGAL::bbox_3(points.points().begin(), points.points().end());

        auto crs = mProperties->crs();
        double gsd = mProperties->gsd();

        tl::GeoTools *geo_tools = tl::GeoTools::getInstance();

        /// 1. Generación DSM ENU
        processDsmEnu(points, cgal_bbox, gsd, mDsmEnuPath, this);

        if (status() == Status::stopping) return;

        /// 2. Proyección y extracción DSM principal
        tl::BoundingBox2d window = computeBoundingBox2dInCrs(cgal_bbox, mEnuCrs, crs, geo_tools->ptrCRSsTools());
        tl::Affine<double, 2> georeference(gsd, -gsd, window.min().x(), window.max().y(), 0.0);
        cv::Mat dsm_raster = extractDSMfromPointCloud(points, window, georeference, geo_tools->ptrCRSsTools(), mEnuCrs, crs, this);
        std::vector<Point_3> points_dsm = rasterToPointList(dsm_raster, georeference);

        if (status() == Status::stopping) return;

        tl::Size<int> size(tl::roundToInteger(window.width() / gsd),
                           tl::roundToInteger(window.height() / gsd));

        /// 3. Procesamiento y guardado DSM
        if (mProperties->dsm()) {
            DelaunayTriangulation dtm_clean(points_dsm.begin(), points_dsm.end());
            dsm_raster = extractDTMfromTIN(dtm_clean, size, georeference, progressBar, this);

            writeDTM(mDsmPath, dsm_raster, georeference, crs);
            addOverviewsToImage(mDsmPath.toString());
            tl::Message::info("DSM written at: {}", mDsmPath.toUtf8());
        }

        if (status() == Status::stopping) return;

        /// 4. Procesamiento y guardado DTM (Filtro CSF)
        if (mProperties->dtm()) {
            csf::PointCloud csf_points;
            csf_points.reserve(points_dsm.size());

            for (const auto &cgal_point : points_dsm) {
                csf::Point csf_pt;
                csf_pt.x = cgal_point.x();
                csf_pt.y = -cgal_point.y();
                csf_pt.z = cgal_point.z();
                csf_points.push_back(csf_pt);
            }

            CSF csf;
            csf.setPointCloud(csf_points);
            std::vector<int> ground_idx, off_ground_idx;
            csf.do_filtering(ground_idx, off_ground_idx);

            CGAL::Point_set_3<Point_3> points_ground;
            for (const auto &idx : ground_idx) {
                points_ground.insert(Point_3(csf_points[idx].x, -csf_points[idx].y, csf_points[idx].z));
            }

            DelaunayTriangulation dtm(points_ground.points().begin(), points_ground.points().end());

            cv::Mat dtm_raster = extractDTMfromTIN(dtm, size, georeference, progressBar, this);
            writeDTM(mDtmPath, dtm_raster, georeference, crs);
            addOverviewsToImage(mDtmPath.toString());
            tl::Message::info("DTM written at: {}", mDtmPath.toString());
        }

        mDemReport.time = this->time();
        mDemReport.gsd = gsd;
        mDemReport.epsg = crs;
        mDemReport.cols = size.width();
        mDemReport.rows = size.height();

        tl::Message::success("DEM task finished in {:.2f} minutes", mDemReport.time / 60.0);

        if (progressBar) (*progressBar)(10);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("DEM task error");
    }
}

} // namespace graphos