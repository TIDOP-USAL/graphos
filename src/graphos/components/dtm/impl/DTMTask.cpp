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

#include "DTMTask.h"

#include "graphos/core/task/Progress.h"

TL_DISABLE_WARNINGS
#include "csf/src/CSF.h"
TL_DEFAULT_WARNINGS

#include <tidop/core/msg/message.h>
#include <tidop/core/chrono.h>
#include <tidop/core/path.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/geometry/size.h>
#include <tidop/img/img.h>
#include <tidop/img/imgwriter.h>
#include <tidop/geospatial/crs.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Projection_traits_xy_3.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/boost/graph/graph_traits_Delaunay_triangulation_2.h>
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
#include <CGAL/jet_smooth_point_set.h>
#include <CGAL/jet_estimate_normals.h>
#include <CGAL/mst_orient_normals.h>

#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <fstream>

using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Projection_traits = CGAL::Projection_traits_xy_3<Kernel>;
using TIN = CGAL::Delaunay_triangulation_2<Projection_traits>;
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

//#define READ_POINTS 1
//#define DTM 1


//namespace internal
//{
//struct TIN
//{
//    ::TIN *ref;
//};
//
//}

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

cv::Mat extractDTMfromTIN(const TIN &tin, const tl::BoundingBoxD &bbox, tl::Progress *progressBar, double gsd, double zOffset)
{
    tl::Size<int> size(tl::roundToInteger(bbox.width() / gsd), 
                       tl::roundToInteger(bbox.height()/ gsd));
    cv::Mat mat(size.height, size.width, CV_32F, -9999.);

    TIN::Face_handle location;

    double increment = 40. / static_cast<double>(size.height);
    double progress = 0.;

    for (std::size_t r = 0; r < size.height; ++r) {

        //if (status() == Task::Status::stopping) return mat;

        for (std::size_t c = 0; c < size.width; ++c) {

            Point_3 query(bbox.pt1.x + c * gsd, bbox.pt1.y + (size.height - r) * gsd, 0.);
            location = tin.locate(query, location);

            if (!tin.is_infinite(location)) {
   
                auto barycentric_coordinates = barycentricCoordinates(location->vertex(0)->point(),
                                                                      location->vertex(1)->point(),
                                                                      location->vertex(2)->point(),
                                                                      query);

                double height_at_query = (barycentric_coordinates[0] * location->vertex(0)->point().z()
                                        + barycentric_coordinates[1] * location->vertex(1)->point().z()
                                        + barycentric_coordinates[2] * location->vertex(2)->point().z());

                mat.at<float>(r, c) = static_cast<float>(height_at_query + zOffset); // Z offset
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

cv::Mat extractDTMfromMesh(SurfaceMesh &mesh, const tl::BoundingBoxD &bbox, tl::Progress *progressBar, double gsd, double zOffset)
{


    tl::Size<int> size(tl::roundToInteger(bbox.width() / gsd), 
                       tl::roundToInteger(bbox.height()/ gsd));
    cv::Mat mat(size.height, size.width, CV_32F, -9999.);

    TIN::Face_handle location;

    double increment = 40. / static_cast<double>(size.height);
    double progress = 0.;

    /*Tree*/Tree2 tree(faces(mesh).first, faces(mesh).second, mesh);

    for (std::size_t r = 0; r < size.height; ++r) {

        //if (status() == Task::Status::stopping) return mat;

        for (std::size_t c = 0; c < size.width; ++c) {


            Vector_3 orientation(0., 0., -1.);
            Point_3 camera_center = Point_3(bbox.pt1.x + c * gsd, bbox.pt1.y + (size.height - r) * gsd, 100.);

            Ray3 ray(camera_center, orientation);
               
            if (RayIntersection2 intersection = tree.first_intersection(ray)) {

                if (Point_3 *p = boost::get<Point_3>(&(intersection->first))) {
                    mat.at<float>(r, c) = static_cast<float>(p->z() + zOffset);
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
                                 const tl::BoundingBoxD &bbox, 
                                 tl::Progress *progressBar, 
                                 double gsd, 
                                 const tl::Affine<double, 2> &georeference, 
                                 tl::Point3<double> offset)
{
    tl::Size<int> size(tl::roundToInteger(bbox.width() / gsd),
                       tl::roundToInteger(bbox.height() / gsd));
    cv::Mat mat(size.height, size.width, CV_32F, -9999.);

    for (auto &point : points.points()) {
        tl::Point2d _point(point.x()+ offset.x, point.y() + offset.y);
        tl::Point2i point_image = georeference.transform(_point);
        if (point_image.x >= 0 && point_image.x < size.width && point_image.y >= 0 && point_image.y < size.height)
            mat.at<float>(point_image.y, point_image.x) = std::max(mat.at<float>(point_image.y, point_image.x), static_cast<float>(point.z() + offset.z));
    }

    return mat;
}

void writeDTM(const tl::Path &file, const cv::Mat &mat,
              const tl::Affine<double, 2> &georeference,
              const std::string &epsg)
{
    auto image_writer_mds = tl::ImageWriterFactory::create(file);
    image_writer_mds->open();
    
    TL_ASSERT(image_writer_mds->isOpen(), "Can not create image: {}", file.toString());

    image_writer_mds->create(mat.rows, mat.cols, 1, tl::DataType::TL_32F);
    image_writer_mds->setGeoreference(georeference);
    tl::Crs crs(epsg);
    image_writer_mds->setCRS(crs.toWktFormat());
    image_writer_mds->setNoDataValue(-9999.);
    image_writer_mds->write(mat);

    image_writer_mds->close();

}

DtmTask::DtmTask(const tl::Path &pointCloud, 
                 const tl::Point3<double> &offset, 
                 const tl::Path &demPath,
                 double gsd, 
                 const QString &crs,
                 bool dsm,
                 bool dtm)
  : tl::TaskBase(),
    mPointCloud(pointCloud),
    mOffset(offset),
    mDemPath(demPath),
    mGSD(gsd),
    mCrs(crs),
    mDSM(dsm),
    mDTM(dtm)
{

}

void DtmTask::execute(tl::Progress *progressBar)
{

    try {

        TL_ASSERT(mPointCloud.exists(), "Point cloud file not exist: '{}'", mPointCloud.toString().c_str());
        TL_ASSERT(mPointCloud.isFile(), "The path is not valid: '{}'", mPointCloud.toString().c_str());

        mDemPath.createDirectories();

        std::string filename = mPointCloud.toString();

        CGAL::Point_set_3<Point_3> points_dsm;



        std::ifstream input_stream(filename.c_str(), std::ios::binary);
        CGAL::Point_set_3<Point_3> points;
        input_stream >> points;
        tl::Message::info("Read {} points", points.size());


        CGAL::Bbox_3 cgal_bbox = CGAL::bbox_3(points.points().begin(), points.points().end());
        tl::BoundingBoxD bbox(tl::Point3d(cgal_bbox.xmin(), cgal_bbox.ymin(), cgal_bbox.zmin()),
                              tl::Point3d(cgal_bbox.xmax(), cgal_bbox.ymax(), cgal_bbox.zmax()));

        tl::Affine<double, 2> georeference(mGSD, -mGSD, cgal_bbox.xmin() + mOffset.x, cgal_bbox.ymax() + mOffset.y, 0.);

        cv::Mat dsm_raster = extractDSMfromPointCloud(points, bbox, progressBar, mGSD, georeference.inverse(), mOffset);

        for (int r = 0; r < dsm_raster.rows; r++) {
            for (int c = 0; c < dsm_raster.cols; c++) {

                tl::Point3<double> point = georeference.transform(tl::Point<double>(c, r));
                point.z = dsm_raster.at<float>(r, c);
                if (point.z == -9999) continue;
                auto _point = point - mOffset;
                points_dsm.insert(Point_3(_point.x, _point.y, _point.z));
            }
        }

        if (mDSM) {

            TIN dtm_clean(points_dsm.points().begin(), points_dsm.points().end());
            dsm_raster = extractDTMfromTIN(dtm_clean, bbox, progressBar, mGSD, mOffset.z);

            tl::Path mds_path = mDemPath;
            mds_path.append("dsm.tif");

            writeDTM(mds_path, dsm_raster, georeference, mCrs.toStdString());

            dsm_raster.release();

            tl::Message::info("DSM writed at: {}", mds_path.toString());

        }

        if (mDTM) {

            size_t size = points_dsm.points().size();

            csf::PointCloud csf_points;
            csf_points.reserve(size);

            for (auto &cgal_point : points_dsm.points()) {
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

            TIN dtm(points_ground.points().begin(), points_ground.points().end());

            //CGAL::Bbox_3 cgal_bbox = CGAL::bbox_3(points_ground.points().begin(), points_ground.points().end());
            //tl::BoundingBoxD bbox(tl::Point3d(cgal_bbox.xmin(), cgal_bbox.ymin(), cgal_bbox.zmin()),
            //                      tl::Point3d(cgal_bbox.xmax(), cgal_bbox.ymax(), cgal_bbox.zmax()));
            //tl::Affine<double, 2> georeference(mGSD, -mGSD, cgal_bbox.xmin() + mOffset.x, cgal_bbox.ymax() + mOffset.y, 0.);

            cv::Mat dtm_raster = extractDTMfromTIN(dtm, bbox, progressBar, mGSD, mOffset.z);
            writeDTM(mdt_path, dtm_raster, georeference, mCrs.toStdString());
            dtm_raster.release();

            tl::Message::info("DTM writed at: {}", mdt_path.toString());
        }


        tl::Message::success("DTM task finished in {:.2} minutes", this->time() / 60.);

        if (progressBar) (*progressBar)(10);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("DTM tast error");
    }

}



} // graphos
