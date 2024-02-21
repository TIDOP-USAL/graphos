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

        // 1 - Read Mesh
        if (mDSM) {
//#ifndef  READ_POINTS

            //tl::Path mesh_path = mPointCloud.parentPath().parentPath();
            //mesh_path.append("mesh.pr.ply");

            //if (mesh_path.exists()) {

            //    //std::ifstream input(mesh_path.toString(), std::ios::binary);
            //    //Mesh mesh;
            //    //std::string comments;
            //    //bool error = CGAL::IO::read_PLY(input, mesh, comments, true);

            //    //if (progressBar) (*progressBar)(25);

            //    //if (status() == Task::Status::stopping) return;

            //    //// Aqui he quitado la creación del TIN
            //    //if (progressBar) (*progressBar)(25);

            //    //if (status() == Task::Status::stopping) return;

            //    //CGAL::Bbox_3 cgal_bbox = CGAL::bbox_3(mesh.points().begin(), mesh.points().end());

            //    //tl::BoundingBoxD bbox(tl::Point3d(cgal_bbox.xmin(), cgal_bbox.ymin(), cgal_bbox.zmin()),
            //    //                  tl::Point3d(cgal_bbox.xmax(), cgal_bbox.ymax(), cgal_bbox.zmax()));
            //    //
            //    //tl::Affine<double, 2> georeference(mGSD, -mGSD, cgal_bbox.xmin() + mOffset.x, cgal_bbox.ymax() + mOffset.y, 0.);

            //    //cv::Mat dsm_raster = extractDTMfromMesh(mesh, bbox, progressBar, mGSD, mOffset.z);

            //    //tl::Path mds_path = mDemPath;
            //    //mds_path.append("dsm.tif");
            //    //
            //    //writeDTM(mds_path, dsm_raster, georeference, mCrs.toStdString());
            //    //dsm_raster.release();
            //    //
            //    //tl::Message::info("DSM writed at: {}", mds_path.toString());

            //    std::ifstream input(mesh_path.toString(), std::ios::binary);
            //    SurfaceMesh mesh;
            //    std::string comments;
            //    bool error = CGAL::IO::read_PLY(input, mesh, comments, true);

            //    if (progressBar) (*progressBar)(25);

            //    if (status() == Task::Status::stopping) return;

            //    // Aqui he quitado la creación del TIN
            //    if (progressBar) (*progressBar)(25);

            //    if (status() == Task::Status::stopping) return;

            //    CGAL::Bbox_3 cgal_bbox = CGAL::bbox_3(mesh.points().begin(), mesh.points().end());

            //    tl::BoundingBoxD bbox(tl::Point3d(cgal_bbox.xmin(), cgal_bbox.ymin(), cgal_bbox.zmin()),
            //                      tl::Point3d(cgal_bbox.xmax(), cgal_bbox.ymax(), cgal_bbox.zmax()));
            //    
            //    tl::Affine<double, 2> georeference(mGSD, -mGSD, cgal_bbox.xmin() + mOffset.x, cgal_bbox.ymax() + mOffset.y, 0.);

            //    cv::Mat dsm_raster = extractDTMfromMesh(mesh, bbox, progressBar, mGSD, mOffset.z);

            //    tl::Path mds_path = mDemPath;
            //    mds_path.append("dsm.tif");
            //    
            //    writeDTM(mds_path, dsm_raster, georeference, mCrs.toStdString());
            //    dsm_raster.release();
            //    
            //    tl::Message::info("DSM writed at: {}", mds_path.toString());

            //} else {

                std::ifstream input_stream(filename.c_str(), std::ios::binary);
                CGAL::Point_set_3<Point_3> points;
                input_stream >> points;
                tl::Message::info("Read {} points", points.size());


                //// - Outlier removal
                //// Some acquisition techniques generate points which are far away from the surface. These points, commonly referred to as "outliers", have no relevance for reconstruction. Using the CGAL reconstruction algorithms on outlier-ridden point clouds produce overly distorted output, it is therefore strongly advised to filter these outliers before performing reconstruction.

                //typename Point_set::iterator rout_it = CGAL::remove_outliers<CGAL::Sequential_tag>
                //(points,
                //24, // Number of neighbors considered for evaluation
                //points.parameters().threshold_percent (5.0)); // Percentage of points to remove
                //points.remove(rout_it, points.end());

                //tl::Message::info("{} point(s) are outliers.", points.number_of_removed_points());

                //// Applying point set processing algorithm to a CGAL::Point_set_3
                //// object does not erase the points from memory but place them in
                //// the garbage of the object: memory can be freed by the user.
                //points.collect_garbage();

                //// - Simplification
                //// Some laser scanners generate points with widely variable sampling. Typically, lines of scan are very densely sampled but the gap between two lines of scan is much larger, leading to an overly massive point cloud with large variations of sampling density. This type of input point cloud might generate imperfect output using algorithms which, in general, only handle small variations of sampling density.

                //// CGAL provides several simplification algorithms. In addition to reducing the size of the input point cloud and therefore decreasing computation time, some of them can help making the input more uniform. This is the case of the function grid_simplify_point_set() which defines a grid of a user-specified size and keeps one point per occupied cell.

                //// Compute average spacing using neighborhood of 6 points
                //double spacing = CGAL::compute_average_spacing<CGAL::Sequential_tag> (points, 6);
                //// Simplify using a grid of size 2 * average spacing
                //typename Point_set::iterator gsim_it = CGAL::grid_simplify_point_set(points, 2. * spacing);
                //points.remove(gsim_it, points.end());

                //tl::Message::info("{} point(s) removed after simplification.", points.number_of_removed_points());
                //points.collect_garbage();

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

                TIN dtm_clean(points_dsm.points().begin(), points_dsm.points().end());
                dsm_raster = extractDTMfromTIN(dtm_clean, bbox, progressBar, mGSD, mOffset.z);

                tl::Path mds_path = mDemPath;
                mds_path.append("dsm.tif");
                
                writeDTM(mds_path, dsm_raster, georeference, mCrs.toStdString());

                dsm_raster.release();
                
                tl::Message::info("DSM writed at: {}", mds_path.toString());



            //}


//#else
//
//            // 1 - Read points
//
//            std::ifstream input_stream(filename.c_str(), std::ios::binary);
//            CGAL::Point_set_3<Point_3> points;
//            input_stream >> points;
//
//            tl::Message::info("Read {} points", points.size());
//
//#endif // ! READ_POINTS




//            // 2 - Create TIN
//
//#ifdef  READ_POINTS
//            TIN dsm(points.points().begin(), points.points().end());
//#else
//        //TIN dsm(mesh.points().begin(), mesh.points().end());
//#endif



            // 3 - Create DSM        
////#ifdef  READ_POINTS
////            CGAL::Bbox_3 cgal_bbox = CGAL::bbox_3(points.points().begin(), points.points().end());
////#else
//            CGAL::Bbox_3 cgal_bbox = CGAL::bbox_3(mesh.points().begin(), mesh.points().end());
////#endif
//            tl::BoundingBoxD bbox(tl::Point3d(cgal_bbox.xmin(), cgal_bbox.ymin(), cgal_bbox.zmin()),
//                                  tl::Point3d(cgal_bbox.xmax(), cgal_bbox.ymax(), cgal_bbox.zmax()));

            // Ajustar el progreso para tener en cuenta la extracción por filas en el DTM
            // Hay que considerar el avance realizado ¿10% , 20%?
            //dynamic_cast<ProgressHandler *>(progressBar)->setRange(0, (tl::roundToInteger(bbox.height()/ mGSD) + );

            


//#ifdef  READ_POINTS
//            //internal::TIN internal_tin;
//            //internal_tin.ref = &dsm;
//            cv::Mat dsm_raster = extractDTMfromTIN(dsm, bbox, progressBar, mGSD, mOffset.z);
//#else
            
//#endif
            // 4 - Write DSM
            

            //if (progressBar) (*progressBar)(10);

            //if (status() == Task::Status::stopping) return;
        }

        // DTM

//#ifdef DTM
//        if (mDTM) {
//            // 1 - Read points
//
//            std::ifstream input_stream(filename.c_str(), std::ios::binary);
//            CGAL::Point_set_3<Point_3> points/* = points_dsm*/;
//            input_stream >> points;
//
//            tl::Message::info("Read {} points", points.size());
//
//            CGAL::Bbox_3 cgal_bbox = CGAL::bbox_3(points.points().begin(), points.points().end());
//            tl::BoundingBoxD bbox(tl::Point3d(cgal_bbox.xmin(), cgal_bbox.ymin(), cgal_bbox.zmin()),
//                                  tl::Point3d(cgal_bbox.xmax(), cgal_bbox.ymax(), cgal_bbox.zmax()));
//            tl::Affine<double, 2> georeference(mGSD, -mGSD, cgal_bbox.xmin() + mOffset.x, cgal_bbox.ymax() + mOffset.y, 0.);
//
//            auto idx_to_point_with_info = [&](const Point_set::Index &idx) -> std::pair<Point_3, Point_set::Index> {
//                return std::make_pair(points.point(idx), idx);
//            };
//
//            TIN_with_info tin_with_info(boost::make_transform_iterator(points.begin(), idx_to_point_with_info),
//                                        boost::make_transform_iterator(points.end(), idx_to_point_with_info));
//            double spacing = CGAL::compute_average_spacing<Concurrency_tag>(points, 6);
//            spacing *= 2;
//            auto face_height = [&](const TIN_with_info::Face_handle fh) -> double {
//                double out = 0.;
//                for (int i = 0; i < 3; ++i)
//                    out = (std::max)(out, CGAL::abs(fh->vertex(i)->point().z() - fh->vertex((i + 1) % 3)->point().z()));
//                return out;
//            };
//
//            // Initialize faces info
//            for (TIN_with_info::Face_handle fh : tin_with_info.all_face_handles())
//                if (tin_with_info.is_infinite(fh) || face_height(fh) > spacing) // Filtered faces are given info() = -2
//                    fh->info() = -2;
//                else // Pending faces are given info() = -1;
//                    fh->info() = -1;
//
//            // Flooding algorithm
//            std::vector<int> component_size;
//            for (TIN_with_info::Face_handle fh : tin_with_info.finite_face_handles()) {
//                if (fh->info() != -1)
//                    continue;
//                std::queue<TIN_with_info::Face_handle> todo;
//                todo.push(fh);
//                int size = 0;
//                while (!todo.empty()) {
//                    TIN_with_info::Face_handle current = todo.front();
//                    todo.pop();
//                    if (current->info() != -1)
//                        continue;
//                    current->info() = int(component_size.size());
//                    ++size;
//                    for (int i = 0; i < 3; ++i)
//                        todo.push(current->neighbor(i));
//                }
//                component_size.push_back(size);
//            }
//
//            if (progressBar) (*progressBar)();
//
//            if (status() == Task::Status::stopping) return;
//
//            //std::cerr << component_size.size() << " connected component(s) found" << std::endl;
//
//            int min_size = int(points.size() / 2);
//            std::vector<TIN_with_info::Vertex_handle> to_remove;
//            for (TIN_with_info::Vertex_handle vh : tin_with_info.finite_vertex_handles()) {
//                TIN_with_info::Face_circulator circ = tin_with_info.incident_faces(vh),
//                    start = circ;
//                // Remove a vertex if it's only adjacent to components smaller than threshold
//                bool keep = false;
//                do {
//                    if (circ->info() >= 0 && component_size[std::size_t(circ->info())] > min_size) {
//                        keep = true;
//                        break;
//                    }
//                } while (++circ != start);
//                if (!keep)
//                    to_remove.push_back(vh);
//            }
//
//            tl::Message::info("{} vertices will be removed after filtering", to_remove.size());
//
//            for (TIN_with_info::Vertex_handle vh : to_remove)
//                tin_with_info.remove(vh);
//
//            if (progressBar) (*progressBar)();
//
//            if (status() == Task::Status::stopping) return;
//
//
//            SurfaceMesh dtm_mesh;
//            std::vector<SurfaceMesh::Face_index> face_selection;
//            SurfaceMesh::Property_map<SurfaceMesh::Face_index, bool> face_selection_map = dtm_mesh.add_property_map<SurfaceMesh::Face_index, bool>("is_selected", false).first;
//            double limit = CGAL::square(5 * spacing);
//            CGAL::copy_face_graph(tin_with_info, dtm_mesh,
//                                  CGAL::parameters::face_to_face_output_iterator
//                                  (boost::make_function_output_iterator
//                                  ([&](const std::pair<TIN_with_info::Face_handle, Mesh::Face_index> &ff) {
//                                      double longest_edge = 0.;
//                                      bool border = false;
//                                      for (int i = 0; i < 3; ++i) {
//                                          longest_edge = (std::max)(longest_edge, CGAL::squared_distance
//                                          (ff.first->vertex((i + 1) % 3)->point(),
//                                                                    ff.first->vertex((i + 2) % 3)->point()));
//                                          TIN_with_info::Face_circulator circ
//                                              = tin_with_info.incident_faces(ff.first->vertex(i)),
//                                              start = circ;
//                                          do {
//                                              if (tin_with_info.is_infinite(circ)) {
//                                                  border = true;
//                                                  break;
//                                              }
//                                          } while (++circ != start);
//                                          if (border)
//                                              break;
//                                      }
//                                      // Select if face is too big AND it's not
//                                      // on the border (to have closed holes)
//                                      if (!border && longest_edge > limit) {
//                                          face_selection_map[ff.second] = true;
//                                          face_selection.push_back(ff.second);
//                                      }
//                                  })));
//
//            //std::cerr << face_selection.size() << " face(s) are selected for removal" << std::endl;
//            if (progressBar) (*progressBar)();
//
//            if (status() == Task::Status::stopping) return;
//
//
//            CGAL::expand_face_selection_for_removal(face_selection, dtm_mesh, face_selection_map);
//            face_selection.clear();
//            for (SurfaceMesh::Face_index fi : faces(dtm_mesh))
//                if (face_selection_map[fi])
//                    face_selection.push_back(fi);
//
//            //std::cerr << face_selection.size() << " face(s) are selected for removal after expansion" << std::endl;
//
//            for (SurfaceMesh::Face_index fi : face_selection)
//                CGAL::Euler::remove_face(halfedge(fi, dtm_mesh), dtm_mesh);
//            dtm_mesh.collect_garbage();
//
//            TL_ASSERT(dtm_mesh.is_valid(), "Invalid mesh!");
//
//            if (progressBar) (*progressBar)();
//
//            if (status() == Task::Status::stopping) return;
//
//
//            // Get all holes
//            std::vector<SurfaceMesh::Halfedge_index> holes;
//            CGAL::Polygon_mesh_processing::extract_boundary_cycles(dtm_mesh, std::back_inserter(holes));
//            std::cerr << holes.size() << " hole(s) identified" << std::endl;
//
//            // Identify outer hull (hole with maximum size)
//            double max_size = 0.;
//            SurfaceMesh::Halfedge_index outer_hull;
//            for (SurfaceMesh::Halfedge_index hi : holes) {
//                CGAL::Bbox_3 hole_bbox;
//                for (SurfaceMesh::Halfedge_index haf : CGAL::halfedges_around_face(hi, dtm_mesh)) {
//                    auto &p = dtm_mesh.point(target(haf, dtm_mesh));
//                    hole_bbox += p.bbox();
//                }
//                double size = CGAL::squared_distance(Point_2(hole_bbox.xmin(), hole_bbox.ymin()),
//                                                     Point_2(hole_bbox.xmax(), hole_bbox.ymax()));
//                if (size > max_size) {
//                    max_size = size;
//                    outer_hull = hi;
//                }
//            }
//
//            // Fill all holes except the bigest (which is the outer hull of the mesh)
//            for (SurfaceMesh::Halfedge_index hi : holes)
//                if (hi != outer_hull)
//                    CGAL::Polygon_mesh_processing::triangulate_refine_and_fair_hole
//                    (dtm_mesh, hi, CGAL::Emptyset_iterator(), CGAL::Emptyset_iterator(),
//                     CGAL::parameters::fairing_continuity(0));
//
//            if (progressBar) (*progressBar)();
//
//            if (status() == Task::Status::stopping) return;
//
//
//            // En esta línea se producía una excepción en el proyecto de prueba...
//            //CGAL::Polygon_mesh_processing::isotropic_remeshing(faces(dtm_mesh), spacing, dtm_mesh);
//
//            //if (progressBar) (*progressBar)();
//
//            //if (status() == Task::Status::stopping) return;
//
//            TIN dtm_clean(dtm_mesh.points().begin(), dtm_mesh.points().end());
//            //internal::TIN internal_tin_dtm;
//            //internal_tin_dtm.ref = &dtm_clean;
//            //
//            cv::Mat dtm_raster = extractDTMfromTIN(dtm_clean, bbox, progressBar, mGSD, mOffset.z);
//            //cv::Mat dtm_raster = extractDTMfromMesh(dtm_mesh, bbox, progressBar, mGSD, mOffset.z);
//
//            tl::Path mdt_path = mDemPath;
//            mdt_path.append("dtm.tif");
//
//            writeDTM(mdt_path, dtm_raster, georeference, mCrs.toStdString());
//            dtm_raster.release();
//
//            tl::Message::info("DTM writed at: {}", mdt_path.toString());
//
//        }
//
//#endif //DTM



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
            //SurfaceMesh dtm_mesh;
            //CGAL::copy_face_graph(dtm, dtm_mesh);
            //std::ofstream dsm_ofile(mdt_path.toString(), std::ios_base::binary);
            //CGAL::IO::set_binary_mode(dsm_ofile);
            //CGAL::IO::write_PLY(dsm_ofile, dtm_mesh);
            //dsm_ofile.close();
            CGAL::Bbox_3 cgal_bbox = CGAL::bbox_3(points_ground.points().begin(), points_ground.points().end());
            tl::BoundingBoxD bbox(tl::Point3d(cgal_bbox.xmin(), cgal_bbox.ymin(), cgal_bbox.zmin()),
                                  tl::Point3d(cgal_bbox.xmax(), cgal_bbox.ymax(), cgal_bbox.zmax()));
            tl::Affine<double, 2> georeference(mGSD, -mGSD, cgal_bbox.xmin() + mOffset.x, cgal_bbox.ymax() + mOffset.y, 0.);

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
