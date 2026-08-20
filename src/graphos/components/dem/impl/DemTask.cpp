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

#include "graphos/core/task/Progress.h"
#include "graphos/core/utils.h"

TL_DISABLE_WARNINGS
#include "csf/src/CSF.h"
TL_DEFAULT_WARNINGS

#include <tidop/core/app/Message.h>
//#include <tidop/core/base/Chrono.h>
#include <tidop/core/base/Path.h>
#include <tidop/core/base/Size.h>
//#include <tidop/geospatial/crstransf.h>
#include <tidop/geotools/CRSsTools.h>
#include <tidop/geotools/GeoTools.h>
#include <tidop/rastertools/io/Reader.h>
#include <tidop/geometry/spatial/BoundingBox.h>
#include <tidop/geometry/primitives/Point.h>
//#include <tidop/img/img.h>
#include <tidop/rastertools/io/Writer.h>
#include <tidop/geospatial/crs.h>
//#include <tidop/geometry/entities/bbox.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Projection_traits_xy_3.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
//#include <CGAL/boost/graph/graph_traits_Delaunay_triangulation_2.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Point_set_3/IO.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/locate.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/compute_average_spacing.h>
#include <CGAL/poisson_surface_reconstruction.h>
#include <CGAL/grid_simplify_point_set.h>
#include <CGAL/remove_outliers.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_tree.h>
//#include <CGAL/jet_smooth_point_set.h>
//#include <CGAL/jet_estimate_normals.h>
#include <CGAL/mst_orient_normals.h>

#include <opencv2/imgcodecs.hpp>

//#include <iostream>
#include <fstream>

//using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
//using Projection_traits = CGAL::Projection_traits_xy_3<Kernel>;
//using DelaunayTriangulation = CGAL::Delaunay_triangulation_2<Projection_traits>;
using Kernel = CGAL::Simple_cartesian<double>;
using Projection_traits = CGAL::Projection_traits_xy_3<Kernel>;
using DelaunayTriangulation = CGAL::Delaunay_triangulation_2<Projection_traits>;
using Point_2 = Kernel::Point_2;
using Point_3 = Kernel::Point_3;
using Vector_3 = Kernel::Vector_3;
using Ray3 = Kernel::Ray_3;
using SurfaceMesh = CGAL::Surface_mesh<Point_3>;
using Primitive2 = CGAL::AABB_face_graph_triangle_primitive<SurfaceMesh>;
using Traits2 = CGAL::AABB_traits<Kernel, Primitive2>;
using Tree2 = CGAL::AABB_tree<Traits2>;
using RayIntersection2 = boost::optional<Tree2::Intersection_and_primitive_id<Ray3>::Type>;
using Point_set = CGAL::Point_set_3<Point_3>;
using Vbi = CGAL::Triangulation_vertex_base_with_info_2<Point_set::Index, Projection_traits>;
using Fbi = CGAL::Triangulation_face_base_with_info_2<int, Projection_traits>;
using TDS = CGAL::Triangulation_data_structure_2<Vbi, Fbi>;
using TIN_with_info = CGAL::Delaunay_triangulation_2<Projection_traits, TDS>;
#ifdef CGAL_LINKED_WITH_TBB
using Concurrency_tag = CGAL::Parallel_tag;
#else
using Concurrency_tag = CGAL::Sequential_tag;
#endif

using SimpleCartesian = CGAL::Simple_cartesian<double>;
using PointCGAL = SimpleCartesian::Point_3;
using VectorCGAL = SimpleCartesian::Vector_3 ;
using Ray = SimpleCartesian::Ray_3;
using Mesh = CGAL::Surface_mesh<PointCGAL>;
using Primitive = CGAL::AABB_face_graph_triangle_primitive<Mesh>;
using Traits = CGAL::AABB_traits<SimpleCartesian, Primitive>;
using Tree = CGAL::AABB_tree<Traits>;
using RayIntersection = boost::optional<Tree::Intersection_and_primitive_id<Ray>::Type>;
using PrimitiveId = Tree::Primitive_id ;

using Point_set = CGAL::Point_set_3<Point_3, Vector_3>;

namespace graphos
{


std::array<double, 3> barycentricCoordinates(const Point_3 &pt1, const Point_3 &pt2, const Point_3 &pt3, const Point_3 &query)
{
    return CGAL::Polygon_mesh_processing::barycentric_coordinates(Point_2(pt1.x(), pt1.y()), 
                                                                  Point_2(pt2.x(), pt2.y()), 
                                                                  Point_2(pt3.x(), pt3.y()), 
                                                                  Point_2(query.x(), query.y()),
                                                                  Kernel());
}

cv::Mat extractDTMfromTIN(const DelaunayTriangulation &tin,
                          const tl::Size<int> &size,
                          const tl::Affine<double, 2> &georeference,
                          tl::Progress *progressBar,
                          const DemTask *demTask)
{
    cv::Mat mat(size.height(), size.width(), CV_32F, tl::NoData<float>);

    DelaunayTriangulation::Face_handle location;

    double increment = 40. / static_cast<double>(size.height());
    double progress = 0.;

    for (size_t r = 0; r < static_cast<size_t>(size.height()); ++r) {

        if (demTask->status() == DemTask::Status::stopping)  break;

        for (size_t c = 0; c < static_cast<size_t>(size.width()); ++c) {

            if (demTask->status() == DemTask::Status::stopping)  break;

            auto point = georeference.transform(tl::Point<double>(c, r));
            Point_3 query(point.x(), point.y(), 0.);
            location = tin.locate(query, location);

            if (!tin.is_infinite(location)) {
   
                const auto &v0 = location->vertex(0)->point();
                const auto &v1 = location->vertex(1)->point();
                const auto &v2 = location->vertex(2)->point();

                double twice_area = (v1.x() - v0.x()) * (v2.y() - v0.y()) - (v2.x() - v0.x()) * (v1.y() - v0.y());
                if (std::abs(twice_area) < 1e-5) {
                    continue;
                }

                auto barycentric_coordinates = barycentricCoordinates(v0,
                                                                      v1,
                                                                      v2,
                                                                      query);

                double height_at_query = (barycentric_coordinates[0] * v0.z()
                                        + barycentric_coordinates[1] * v1.z()
                                        + barycentric_coordinates[2] * v2.z());

                mat.at<float>(static_cast<int>(r), static_cast<int>(c)) = static_cast<float>(height_at_query); // Z offset
            }
        }

        if (progressBar) {
            progress += increment;
            int _progress = static_cast<int>(progress);
            if (_progress == 1) {
                (*progressBar)(_progress);
                progress -= _progress;
            }
        }
    }

    return mat;
}

cv::Mat extractDTMfromMesh(SurfaceMesh &mesh, const tl::BoundingBox2d &bbox, tl::Progress *progressBar, double gsd, double zOffset)
{

    tl::Size<int> size(tl::roundToInteger(bbox.width() / gsd), 
                       tl::roundToInteger(bbox.height()/ gsd));
    cv::Mat mat(size.height(), size.width(), CV_32F, tl::NoData<float>);

    DelaunayTriangulation::Face_handle location;

    double increment = 40. / static_cast<double>(size.height());
    double progress = 0.;

    Tree2 tree(faces(mesh).first, faces(mesh).second, mesh);

    for (size_t r = 0; r < static_cast<size_t>(size.height()); ++r) {

        for (size_t c = 0; c < static_cast<size_t>(size.width()); ++c) {


            Vector_3 orientation(0., 0., -1.);
            Point_3 camera_center = Point_3(bbox.min().x() + c * gsd, bbox.max().y() + (size.height() - r) * gsd, 100.);

            Ray3 ray(camera_center, orientation);
               
            if (RayIntersection2 intersection = tree.first_intersection(ray)) {

                if (Point_3 *p = boost::get<Point_3>(&(intersection->first))) {
                    mat.at<float>(static_cast<int>(r), static_cast<int>(c)) = static_cast<float>(p->z() + zOffset);
                }
            }

        }

        if (progressBar) {
            progress += increment;
            int _progress = static_cast<int>(progress);
            if (_progress == 1) {
                (*progressBar)(_progress);
                progress -= _progress;
            }

        }
    }

    return mat;
}

cv::Mat extractDSMfromPointCloud(const CGAL::Point_set_3<Point_3> &points, 
                                 const tl::BoundingBox2d &window,
                                 const tl::Affine<double, 2> &georeference,
                                 tl::CRSsTools *ptrCRSsTools,
                                 const std::string &crsIn,
                                 const std::string &crsOut,
                                 const DemTask *demTask)
{
    tl::Affine<double, 2> georeference_inverse = georeference.inverse();
    double gsd = georeference.scale().x();
    tl::Size<int> size(tl::roundToInteger(window.width() / gsd),
                       tl::roundToInteger(window.height() / gsd));
    cv::Mat mat(size.height(), size.width(), CV_32F, tl::NoData<float>);

    for (auto &point : points.points()) {

        if (demTask->status() == DemTask::Status::stopping)  break;

        tl::Point3d coordinates(point.x(), point.y(), point.z());
        ptrCRSsTools->crsOperation(crsIn, crsOut, coordinates.x(), coordinates.y(), coordinates.z());
        tl::Point2i point_image = static_cast<tl::Point2i>(georeference_inverse.transform(static_cast<tl::Point2d>(coordinates)));
        if (point_image.x() >= 0 && point_image.x() < size.width() && point_image.y() >= 0 && point_image.y() < size.height())
            mat.at<float>(point_image.y(), point_image.x()) = std::max(mat.at<float>(point_image.y(), point_image.x()), static_cast<float>(coordinates.z()));
    }

    return mat;
}

// Para el mds en coordenadas ENU
cv::Mat extractDSMfromPointCloud(const CGAL::Point_set_3<Point_3> &points, 
                                 const tl::BoundingBox3d &bbox, 
                                 double gsd, 
                                 const tl::Affine<double, 2> &georeference,
                                 const DemTask *demTask)
{
    tl::Size<int> size(tl::roundToInteger(bbox.width() / gsd),
                       tl::roundToInteger(bbox.height() / gsd));
    cv::Mat mat(size.height(), size.width(), CV_32F, tl::NoData<float>);

    for (auto &point : points.points()) {
        if (demTask->status() == DemTask::Status::stopping)  break;
        tl::Point2d _point(point.x(), point.y());
        tl::Point2i point_image = static_cast<tl::Point2i>(georeference.transform(_point));
        if (point_image.x() >= 0 && point_image.x() < size.width() && point_image.y() >= 0 && point_image.y() < size.height())
            mat.at<float>(point_image.y(), point_image.x()) = std::max(mat.at<float>(point_image.y(), point_image.x()), static_cast<float>(point.z()));
    }

    return mat;
}

//Probar esto
//cv::Mat extractDSMfromPointCloud(const CGAL::Point_set_3<Point_3> &points,
//    const tl::BoundingBoxD &bbox,
//    double gsd,
//    const tl::Affine<double, 2> &georeference,
//    double threshold)
//{
//    tl::Size<int> size(tl::roundToInteger(bbox.width() / gsd),
//        tl::roundToInteger(bbox.height() / gsd));
//    cv::Mat mat(size.height, size.width, CV_32F, tl::NoData<float>);  // Inicializa con valor de no datos
//
//    // Crea un contenedor para las celdas
//    std::vector<std::vector<Point_3>> grid_cells(size.height * size.width);
//
//    // Clasificar puntos en celdas
//    for (auto &point : points.points()) {
//        tl::Point2d _point(point.x(), point.y());
//        tl::Point2i point_image = georeference.transform(_point);
//        if (point_image.x >= 0 && point_image.x < size.width && point_image.y >= 0 && point_image.y < size.height) {
//            int cell_idx = point_image.y * size.width + point_image.x;
//            grid_cells[cell_idx].push_back(point);
//        }
//    }
//
//    // Procesar celdas
//    for (size_t i = 0; i < grid_cells.size(); ++i) {
//        std::vector<Point_3> &cell_points = grid_cells[i];
//
//        if (cell_points.empty()) continue;
//
//        // Encontrar el punto más alto en la celda
//        float max_z = -std::numeric_limits<float>::infinity();
//        for (auto &point : cell_points) {
//            max_z = std::max(max_z, static_cast<float>(point.z()));
//        }
//
//        // Filtrar puntos que están debajo del umbral respecto al punto más alto
//        std::vector<Point_3> filtered_points;
//        for (auto &point : cell_points) {
//            if (point.z() >= max_z - threshold) {  // Umbral de 1 metro
//                filtered_points.push_back(point);
//            }
//        }
//
//        // Si hay puntos válidos, usa el más alto para el DSM
//        if (!filtered_points.empty()) {
//            for (auto &point : filtered_points) {
//                tl::Point2d _point(point.x(), point.y());
//                tl::Point2i point_image = georeference.transform(_point);
//                if (point_image.x >= 0 && point_image.x < size.width && point_image.y >= 0 && point_image.y < size.height) {
//                    mat.at<float>(point_image.y, point_image.x) = std::max(mat.at<float>(point_image.y, point_image.x), static_cast<float>(point.z()));
//                }
//            }
//        }
//    }
//
//    return mat;
//}
void writeDTM(const tl::Path &file, const cv::Mat &mat,
              const tl::Affine<double, 2> &georeference,
              const std::string &epsg)
{
    tl::RasterWriter writer(file);
    TL_ASSERT(writer.isOpen(), "Can not create image: {}", file.toString());

    writer.create(mat.rows, mat.cols, 1, tl::DataType::TL_32F);
    writer.setGeoreference(georeference);
    if (!epsg.empty()) {
        tl::Crs crs(epsg);
        writer.setCRS(crs.toWktFormat());
    }
    writer.setNoDataValue(tl::NoData<float>);
    writer.write(mat);
}

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

}

auto DemTask::report() const -> DemReport
{
    return mDemReport;
}

void DemTask::execute(tl::Progress *progressBar, std::stop_token stopToken)
{

    try {

        TL_ASSERT(mPointCloud.exists(), "Point cloud file not exist: '{}'", mPointCloud.toString().c_str());
        TL_ASSERT(mPointCloud.isFile(), "The path is not valid: '{}'", mPointCloud.toString().c_str());
        
        mDemPath.createDirectories();       

        std::ifstream input_stream(mPointCloud.toString(), std::ios::binary);
        CGAL::Point_set_3<Point_3> points;
        input_stream >> points;
        tl::Message::info("Read {} points", points.size());

        if (status() == Status::stopping) return;

        CGAL::Bbox_3 cgal_bbox = CGAL::bbox_3(points.points().begin(), points.points().end());

        tl::BoundingBox2d window;

        auto crs = mProperties->crs();

        tl::GeoTools *geo_tools = tl::GeoTools::getInstance();
        {
            std::vector<tl::Point<double>> points;
            tl::Point3d pt1(cgal_bbox.xmin(), cgal_bbox.ymin(), cgal_bbox.zmin());
            geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, crs, pt1.x(), pt1.y(), pt1.z());
            points.emplace_back(pt1.x(), pt1.y());
            tl::Point3d pt2(cgal_bbox.xmin(), cgal_bbox.ymax(), cgal_bbox.zmin());
            geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, crs, pt2.x(), pt2.y(), pt2.z());
            points.emplace_back(pt2.x(), pt2.y());
            tl::Point3d pt3(cgal_bbox.xmax(), cgal_bbox.ymax(), cgal_bbox.zmin());
            geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, crs, pt3.x(), pt3.y(), pt3.z());
            points.emplace_back(pt3.x(), pt3.y());
            tl::Point3d pt4(cgal_bbox.xmax(), cgal_bbox.ymin(), cgal_bbox.zmin());
            geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, crs, pt4.x(), pt4.y(), pt4.z());
            points.emplace_back(pt4.x(), pt4.y());
            window = tl::BoundingBox2d(points);
        }
        //tl::Message::info("Window [{}, {}, {}, {}]", window.min().x(), window.min().y(), window.max().x(), window.max().y());

        auto gsd = mProperties->gsd();

        /// DSM ENU
        {
            tl::Affine<double, 2> georeference_enu(gsd, -gsd, cgal_bbox.xmin(), cgal_bbox.ymax(), 0.);
            tl::BoundingBox3d bbox(tl::Point3d(cgal_bbox.xmin(), cgal_bbox.ymin(), cgal_bbox.zmin()),
                                   tl::Point3d(cgal_bbox.xmax(), cgal_bbox.ymax(), cgal_bbox.zmax()));
            cv::Mat dsm_raster_enu = extractDSMfromPointCloud(points, bbox, gsd, georeference_enu.inverse(), this);
            std::vector<Point_3> points_dsm;
            points_dsm.reserve(dsm_raster_enu.rows * dsm_raster_enu.cols);

            for (int r = 0; r < dsm_raster_enu.rows; r++) {
                for (int c = 0; c < dsm_raster_enu.cols; c++) {

                    auto z = dsm_raster_enu.at<float>(r, c);
                    if (z <= -9000.0f || !std::isfinite(z)) continue;
                    auto point = georeference_enu.transform(tl::Point<double>(c, r));
                    points_dsm.emplace_back(point.x(), point.y(), z);
                }
            }

            if (points_dsm.size() >= 3) {
                DelaunayTriangulation dtm_clean(points_dsm.begin(), points_dsm.end());
                tl::Size<int> size(tl::roundToInteger(bbox.width() / gsd),
                                   tl::roundToInteger(bbox.height() / gsd));
                dsm_raster_enu = extractDTMfromTIN(dtm_clean, size, georeference_enu, nullptr, this);

                tl::Path mds_path = mDemPath;
                mds_path.append("dsm_enu.tif");

                writeDTM(mds_path, dsm_raster_enu, georeference_enu, "");

                dsm_raster_enu.release();
            } else {
                tl::Message::warning("Insufficient valid points to generate DSM ENU Delaunay triangulation.");
            }

        }

        tl::Affine<double, 2> georeference(gsd, -gsd, window.min().x(), window.max().y(), 0.);
        cv::Mat dsm_raster = extractDSMfromPointCloud(points, window, georeference, geo_tools->ptrCRSsTools(), mEnuCrs, crs, this);

        std::vector<Point_3> points_dsm;
        points_dsm.reserve(dsm_raster.rows * dsm_raster.cols);

        for (int r = 0; r < dsm_raster.rows; r++) {
            for (int c = 0; c < dsm_raster.cols; c++) {

                auto z = dsm_raster.at<float>(r, c);
                if (z <= -9000.0f || !std::isfinite(z)) continue;

                auto point = georeference.transform(tl::Point<double>(c, r));
                points_dsm.emplace_back(point.x(), point.y(), z);

            }
        }

        if (status() == Status::stopping) return;

        tl::Size<int> size(tl::roundToInteger(window.width() / gsd),
                           tl::roundToInteger(window.height() / gsd));

        if (mProperties->dsm()) {

            DelaunayTriangulation dtm_clean(points_dsm.begin(), points_dsm.end());
            dsm_raster = extractDTMfromTIN(dtm_clean, size, georeference, progressBar, this);

            tl::Path mds_path = mDemPath;
            mds_path.append("dsm.tif");

            writeDTM(mds_path, dsm_raster, georeference, crs);

            dsm_raster.release();

            tl::Message::info("DSM writed at: {}", mds_path.toUtf8());


            // TODO: En el caso que se utilice un datum vertical
            // gdalwarp -overwrite -s_srs EPSG:25830 -t_srs EPSG:25830+5782 -of GTiff \
            // -tr 0.20 0.20 -tap -cutline roi.shp -cl roi -crop_to_cutline \
            // -co "COMPRESS=LZW" dtm_20mm.tif 25830_dtm_clipped.tif


            // Write overviews
            addOverviewsToImage(mds_path.toString());

        }

        if (status() == Status::stopping) return;

        if (mProperties->dtm()) {

            csf::PointCloud csf_points;
            csf_points.reserve(points_dsm.size());

            for (auto &cgal_point : points_dsm) {
                csf::Point csf_point;
                csf_point.x = cgal_point.x();
                csf_point.y = -cgal_point.y();
                csf_point.z = cgal_point.z();
                csf_points.push_back(csf_point);
            }

            CSF csf;
            csf.setPointCloud(csf_points);
            std::vector<int> ground_idx;
            std::vector<int> off_ground_idx;
            csf.do_filtering(ground_idx, off_ground_idx);

            CGAL::Point_set_3<Point_3> points_ground;
            for (const auto &idx : ground_idx) {
                points_ground.insert(Point_3(csf_points[idx].x, -csf_points[idx].y, csf_points[idx].z));
            }

            tl::Path mdt_path = mDemPath;
            mdt_path.append("dtm.tif");

            DelaunayTriangulation dtm(points_ground.points().begin(), points_ground.points().end());

            cv::Mat dtm_raster = extractDTMfromTIN(dtm, size, georeference, progressBar, this);
            writeDTM(mdt_path, dtm_raster, georeference, crs);
            dtm_raster.release();

            // Write overviews
            addOverviewsToImage(mdt_path.toString());

            tl::Message::info("DTM writed at: {}", mdt_path.toString());
        }

        mDemReport.time = this->time();
        mDemReport.gsd = gsd;
        mDemReport.epsg = crs;
        mDemReport.cols = size.width();
        mDemReport.rows = size.height();

        tl::Message::success("DEM task finished in {:.2} minutes", mDemReport.time / 60.);

        if (progressBar) (*progressBar)(10);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("DEM task error");
    }

}



} // graphos
