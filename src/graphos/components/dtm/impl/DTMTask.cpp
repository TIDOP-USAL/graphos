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


#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_tree.h>

#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <fstream>

using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Projection_traits = CGAL::Projection_traits_xy_3<Kernel>;
using TIN = CGAL::Delaunay_triangulation_2<Projection_traits>;
using Point_2 = Kernel::Point_2;
using Point_3 = Kernel::Point_3;
//using Mesh = CGAL::Surface_mesh<Point_3>;
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

namespace internal
{
struct TIN
{
    ::TIN *ref;
};

}

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

cv::Mat DtmTask::extractDTMfromTIN(const internal::TIN &dsm, const tl::BoundingBoxD &bbox, tl::Progress *progressBar/*, double gsd, double zOffset*/)
{
    tl::Size<int> size(tl::roundToInteger(bbox.width() / mGSD), 
                       tl::roundToInteger(bbox.height()/ mGSD));
    cv::Mat mat(size.height, size.width, CV_32F, -9999.);

    TIN::Face_handle location;

    double increment = 40. / static_cast<double>(size.height);
    double progress = 0.;

    for (std::size_t r = 0; r < size.height; ++r) {

        if (status() == Task::Status::stopping) return mat;

        for (std::size_t c = 0; c < size.width; ++c) {

            Point_3 query(bbox.pt1.x + c * mGSD, bbox.pt1.y + (size.height - r) * mGSD, 0.);
            location = dsm.ref->locate(query, location);

            if (!dsm.ref->is_infinite(location)) {
   
                auto barycentric_coordinates = barycentricCoordinates(location->vertex(0)->point(),
                                                                      location->vertex(1)->point(),
                                                                      location->vertex(2)->point(),
                                                                      query);

                double height_at_query = (barycentric_coordinates[0] * location->vertex(0)->point().z()
                                        + barycentric_coordinates[1] * location->vertex(1)->point().z()
                                        + barycentric_coordinates[2] * location->vertex(2)->point().z());

                mat.at<float>(r, c) = static_cast<float>(height_at_query + mOffset.z); // Z offset
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

cv::Mat extractDTMfromMesh(Mesh &mesh, const tl::BoundingBoxD &bbox, tl::Progress *progressBar, double gsd, double zOffset)
{


    tl::Size<int> size(tl::roundToInteger(bbox.width() / gsd), 
                       tl::roundToInteger(bbox.height()/ gsd));
    cv::Mat mat(size.height, size.width, CV_32F, -9999.);

    TIN::Face_handle location;

    double increment = 40. / static_cast<double>(size.height);
    double progress = 0.;

    Tree tree(faces(mesh).first, faces(mesh).second, mesh);

    for (std::size_t r = 0; r < size.height; ++r) {

        //if (status() == Task::Status::stopping) return mat;

        for (std::size_t c = 0; c < size.width; ++c) {


            VectorCGAL orientation(0., 0., -1.);
            PointCGAL camera_center = PointCGAL(bbox.pt1.x + c * gsd, bbox.pt1.y + (size.height - r) * gsd, 100.);

            Ray ray(camera_center, orientation);
               
            if (RayIntersection intersection = tree.first_intersection(ray)) {

                if (PointCGAL *p = boost::get<PointCGAL>(&(intersection->first))) {
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
                 const tl::Path &dtmFile,
                 double gsd, 
                 //bool dsm, 
                 const QString &crs)
  : tl::TaskBase(),
    mPointCloud(pointCloud),
    mOffset(offset),
    mDtmFile(dtmFile),
    mGSD(gsd),
    //mDSM(dsm),
    mCrs(crs)
{

}

void DtmTask::execute(tl::Progress *progressBar)
{

    try {

        tl::Chrono chrono("DTM finished");
        chrono.run();

        TL_ASSERT(mPointCloud.exists(), "Point cloud file not exist: '{}'", mPointCloud.toString().c_str());
        TL_ASSERT(mPointCloud.isFile(), "The path is not valid: '{}'", mPointCloud.toString().c_str());

        tl::Path dtm_folder = mDtmFile.parentPath();
        dtm_folder.createDirectories();

        std::string filename = mPointCloud.toString();

        // 1 - Read Mesh

#ifndef  READ_POINTS

        tl::Path mesh_path = mPointCloud.parentPath().parentPath();
        mesh_path.append("mesh.pr.ply");
        std::ifstream input(mesh_path.toString(), std::ios::binary);
        Mesh mesh;
        std::string comments;
        bool error = CGAL::IO::read_PLY(input, mesh, comments, true);

#else

        // 1 - Read points

        std::ifstream input_stream(filename.c_str(), std::ios::binary);
        CGAL::Point_set_3<Point_3> points;
        input_stream >> points;

        tl::Message::info("Read {} points", points.size());

#endif // ! READ_POINTS

        if (progressBar) (*progressBar)(25);

        if (status() == Task::Status::stopping) return;


        // 2 - Create TIN

#ifdef  READ_POINTS
        TIN dsm(points.points().begin(), points.points().end());
#else
        //TIN dsm(mesh.points().begin(), mesh.points().end());
#endif

        if (progressBar) (*progressBar)(25);

        if (status() == Task::Status::stopping) return;

        // 3 - Create DSM        
#ifdef  READ_POINTS
        CGAL::Bbox_3 cgal_bbox = CGAL::bbox_3(points.points().begin(), points.points().end());   
#else
        CGAL::Bbox_3 cgal_bbox = CGAL::bbox_3(mesh.points().begin(), mesh.points().end());
#endif
        tl::BoundingBoxD bbox(tl::Point3d(cgal_bbox.xmin(), cgal_bbox.ymin(), cgal_bbox.zmin()), 
                              tl::Point3d(cgal_bbox.xmax(), cgal_bbox.ymax(), cgal_bbox.zmax()));

        // Ajustar el progreso para tener en cuenta la extracción por filas en el DTM
        // Hay que considerar el avance realizado ¿10% , 20%?
        //dynamic_cast<ProgressHandler *>(progressBar)->setRange(0, (tl::roundToInteger(bbox.height()/ mGSD) + );

        tl::Affine<double, 2> georeference(mGSD, -mGSD, cgal_bbox.xmin() + mOffset.x, cgal_bbox.ymax() + mOffset.y, 0.);


#ifdef  READ_POINTS
        internal::TIN internal_tin;
        internal_tin.ref = &dsm;
        cv::Mat dsm_raster = extractDTMfromTIN(internal_tin, bbox, progressBar/*, mGSD, mOffset.z*/);
#else
        cv::Mat dsm_raster = extractDTMfromMesh(mesh, bbox, progressBar, mGSD, mOffset.z);
#endif
        // 4 - Write DSM
        tl::Path mds_path = mDtmFile;
        mds_path.replaceBaseName("dsm");

        writeDTM(mds_path, dsm_raster, georeference, mCrs.toStdString());
        dsm_raster.release();

        tl::Message::info("DSM writed at: {}", mds_path.toString());

        //if (progressBar) (*progressBar)(10);

        //if (status() == Task::Status::stopping) return;


        // DTM

#ifdef DTM
        auto idx_to_point_with_info = [&](const Point_set::Index &idx) -> std::pair<Point_3, Point_set::Index> {
            return std::make_pair(points.point(idx), idx);
        };

        TIN_with_info tin_with_info(boost::make_transform_iterator(points.begin(), idx_to_point_with_info),
                                    boost::make_transform_iterator(points.end(), idx_to_point_with_info));
        double spacing = CGAL::compute_average_spacing<Concurrency_tag>(points, 6);
        spacing *= 2;
        auto face_height = [&](const TIN_with_info::Face_handle fh) -> double {
            double out = 0.;
            for (int i = 0; i < 3; ++i)
                out = (std::max)(out, CGAL::abs(fh->vertex(i)->point().z() - fh->vertex((i + 1) % 3)->point().z()));
            return out;
        };

        // Initialize faces info
        for (TIN_with_info::Face_handle fh : tin_with_info.all_face_handles())
            if (tin_with_info.is_infinite(fh) || face_height(fh) > spacing) // Filtered faces are given info() = -2
                fh->info() = -2;
            else // Pending faces are given info() = -1;
                fh->info() = -1;

        // Flooding algorithm
        std::vector<int> component_size;
        for (TIN_with_info::Face_handle fh : tin_with_info.finite_face_handles()) {
            if (fh->info() != -1)
                continue;
            std::queue<TIN_with_info::Face_handle> todo;
            todo.push(fh);
            int size = 0;
            while (!todo.empty()) {
                TIN_with_info::Face_handle current = todo.front();
                todo.pop();
                if (current->info() != -1)
                    continue;
                current->info() = int(component_size.size());
                ++size;
                for (int i = 0; i < 3; ++i)
                    todo.push(current->neighbor(i));
            }
            component_size.push_back(size);
        }

        if (progressBar) (*progressBar)();

        if (status() == Task::Status::stopping) return;

        //std::cerr << component_size.size() << " connected component(s) found" << std::endl;

        int min_size = int(points.size() / 2);
        std::vector<TIN_with_info::Vertex_handle> to_remove;
        for (TIN_with_info::Vertex_handle vh : tin_with_info.finite_vertex_handles()) {
            TIN_with_info::Face_circulator circ = tin_with_info.incident_faces(vh),
                start = circ;
            // Remove a vertex if it's only adjacent to components smaller than threshold
            bool keep = false;
            do {
                if (circ->info() >= 0 && component_size[std::size_t(circ->info())] > min_size) {
                    keep = true;
                    break;
                }
            } while (++circ != start);
            if (!keep)
                to_remove.push_back(vh);
        }
    
        tl::Message::info("{} vertices will be removed after filtering", to_remove.size());

        for (TIN_with_info::Vertex_handle vh : to_remove)
            tin_with_info.remove(vh);

        if (progressBar) (*progressBar)();

        if (status() == Task::Status::stopping) return;


        Mesh dtm_mesh;
        std::vector<Mesh::Face_index> face_selection;
        Mesh::Property_map<Mesh::Face_index, bool> face_selection_map = dtm_mesh.add_property_map<Mesh::Face_index, bool>("is_selected", false).first;
        double limit = CGAL::square(5 * spacing);
        CGAL::copy_face_graph(tin_with_info, dtm_mesh,
                              CGAL::parameters::face_to_face_output_iterator
                              (boost::make_function_output_iterator
                              ([&](const std::pair<TIN_with_info::Face_handle, Mesh::Face_index> &ff) {
                                  double longest_edge = 0.;
                                  bool border = false;
                                  for (int i = 0; i < 3; ++i) {
                                      longest_edge = (std::max)(longest_edge, CGAL::squared_distance
                                      (ff.first->vertex((i + 1) % 3)->point(),
                                                                ff.first->vertex((i + 2) % 3)->point()));
                                      TIN_with_info::Face_circulator circ
                                          = tin_with_info.incident_faces(ff.first->vertex(i)),
                                          start = circ;
                                      do {
                                          if (tin_with_info.is_infinite(circ)) {
                                              border = true;
                                              break;
                                          }
                                      } while (++circ != start);
                                      if (border)
                                          break;
                                  }
                                  // Select if face is too big AND it's not
                                  // on the border (to have closed holes)
                                  if (!border && longest_edge > limit) {
                                      face_selection_map[ff.second] = true;
                                      face_selection.push_back(ff.second);
                                  }
                              })));

        //std::cerr << face_selection.size() << " face(s) are selected for removal" << std::endl;
        if (progressBar) (*progressBar)();

        if (status() == Task::Status::stopping) return;


        CGAL::expand_face_selection_for_removal(face_selection, dtm_mesh, face_selection_map);
        face_selection.clear();
        for (Mesh::Face_index fi : faces(dtm_mesh))
            if (face_selection_map[fi])
                face_selection.push_back(fi);

        //std::cerr << face_selection.size() << " face(s) are selected for removal after expansion" << std::endl;

        for (Mesh::Face_index fi : face_selection)
            CGAL::Euler::remove_face(halfedge(fi, dtm_mesh), dtm_mesh);
        dtm_mesh.collect_garbage();

        TL_ASSERT(dtm_mesh.is_valid(), "Invalid mesh!");

        if (progressBar) (*progressBar)();

        if (status() == Task::Status::stopping) return;


        // Get all holes
        std::vector<Mesh::Halfedge_index> holes;
        CGAL::Polygon_mesh_processing::extract_boundary_cycles(dtm_mesh, std::back_inserter(holes));
        std::cerr << holes.size() << " hole(s) identified" << std::endl;

        // Identify outer hull (hole with maximum size)
        double max_size = 0.;
        Mesh::Halfedge_index outer_hull;
        for (Mesh::Halfedge_index hi : holes) {
            CGAL::Bbox_3 hole_bbox;
            for (Mesh::Halfedge_index haf : CGAL::halfedges_around_face(hi, dtm_mesh)) {
                const Point_3 &p = dtm_mesh.point(target(haf, dtm_mesh));
                hole_bbox += p.bbox();
            }
            double size = CGAL::squared_distance(Point_2(hole_bbox.xmin(), hole_bbox.ymin()),
                                                 Point_2(hole_bbox.xmax(), hole_bbox.ymax()));
            if (size > max_size) {
                max_size = size;
                outer_hull = hi;
            }
        }

        // Fill all holes except the bigest (which is the outer hull of the mesh)
        for (Mesh::Halfedge_index hi : holes)
            if (hi != outer_hull)
                CGAL::Polygon_mesh_processing::triangulate_refine_and_fair_hole
                (dtm_mesh, hi, CGAL::Emptyset_iterator(), CGAL::Emptyset_iterator(),
                 CGAL::parameters::fairing_continuity(0));

        if (progressBar) (*progressBar)();

        if (status() == Task::Status::stopping) return;


        // En esta línea se producía una excepción en el proyecto de prueba...
        //CGAL::Polygon_mesh_processing::isotropic_remeshing(faces(dtm_mesh), spacing, dtm_mesh);

        //if (progressBar) (*progressBar)();

        //if (status() == Task::Status::stopping) return;


        TIN dtm_clean(dtm_mesh.points().begin(), dtm_mesh.points().end());

        cv::Mat dtm_raster = extractDTMfromTIN(dtm_clean, bbox, mGSD, mOffset.z);

        writeDTM(mDtmFile, dtm_raster, georeference, mCrs.toStdString());
        dtm_raster.release();

        tl::Message::info("DTM writed at: {}", mDtmFile.toString());

#endif //DTM

        chrono.stop();

        if (progressBar) (*progressBar)(10);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("DTM tast error");
    }

}



} // graphos
