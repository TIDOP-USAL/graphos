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


#include "GeoreferenceCommand.h"

#include "graphos/components/georeference/impl/GeoreferenceTask.h"
#include "graphos/core/sfm/groundpoint.h"
#include "graphos/core/sfm/posesio.h"
#include "graphos/core/project.h"

#include <tidop/core/msg/message.h>
#include <tidop/math/geometry/affine.h>

#include <QFileInfo>
#include <tidop/core/log.h>


namespace graphos
{


GeoreferenceCommand::GeoreferenceCommand()
  : Command("georef", "Georeference")
{
    this->addArgument<std::string>("prj", 'p', "Project file");
    this->addArgument<std::string>("cp", "Ground Control Points");
    this->addArgument<std::string>("crs", "CRS", "");

    this->addExample("georef --p 253/253.xml --cp 253/georef.xml");
}

GeoreferenceCommand::~GeoreferenceCommand() = default;

auto GeoreferenceCommand::readGroundPoints(const tl::Path &groundPointsPath) -> std::vector<GroundPoint>
{
    auto reader = GroundPointsReaderFactory::create("GRAPHOS");
    reader->read(groundPointsPath);

    return reader->points();
}

bool GeoreferenceCommand::run()
{
    bool r = false;

    tl::Log &log = tl::Log::instance();

    try {

        tl::Path project_path = this->value<std::string>("prj");
        tl::Path gcp = this->value<std::string>("cp");
        std::string crs = this->value<std::string>("crs");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path.toString());

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");
        TL_ASSERT(gcp.isFile(), "GCP file doesn't exist");

        ProjectImp project;
        project.load(project_path);

        tl::Path reconstruction_path = project.reconstructionPath();

        std::string format;
        if (tl::compareInsensitiveCase(gcp.extension().toString(), ".xml")) {
            format = "GRAPHOS";
        } else if (tl::compareInsensitiveCase(gcp.extension().toString(), ".txt")) {
            format = "ODM";
        } else {
            throw std::runtime_error("Ground control point format invalid");
        }

        auto reader = GCPsReaderFactory::create(format);
        reader->setImages(project.images());
        reader->read(gcp);
        std::vector<GroundControlPoint> ground_control_points = reader->gcps();


        auto ground_points = readGroundPoints(project.groundPoints());

        if (crs.empty()) {
            crs = reader->epsgCode();
        }

        GeoreferenceTask georeference_task(project.images(),
                                           project.cameras(),
                                           project.poses(),
                                           ground_points,
                                           ground_control_points,
                                           reconstruction_path,
                                           project.database());

        georeference_task.run();

        if (format == "ODM") {
            auto writer = GCPsWriterFactory::create("GRAPHOS");
            writer->setEPSGCode(crs);
            writer->setGCPs(reader->gcps());
            writer->setImages(project.images());
        }

        auto transform = georeference_task.transform();

        tl::Affine<double, 3> affine(transform.block(0, 2, 0, 3));
        auto rotation = affine.rotation();

        // Transforms the models

        auto sparse = project.sparseModel();
        if (sparse.exists()) transformModel(transform, sparse.toString());
        auto dense = project.denseModel();
        if (dense.exists()) transformModel(transform, dense.toString());
        auto mesh = project.meshPath();
        if (mesh.exists()) transformModel(transform, mesh.toString());


        // Transforms the cameras


        tl::Path poses_path = reconstruction_path;
        poses_path.append("poses.bin");

        auto poses_reader = CameraPosesReaderFactory::create("GRAPHOS");
        poses_reader->read(poses_path);
        auto poses = poses_reader->cameraPoses();

        tl::Point3<double> point;
        for (auto &pose : poses) {

            size_t image_id = pose.first;
            CameraPose camera_pose = pose.second;

            point = affine.transform(camera_pose.position());
            camera_pose.setPosition(point);

            auto rot = tl::Rotation<double, 3>(camera_pose.rotationMatrix()) * rotation.inverse();
            camera_pose.setRotationMatrix(tl::RotationMatrix<double>(rot.toMatrix()));

            project.addPhotoOrientation(image_id, camera_pose);
        }

        auto poses_writer = CameraPosesWriterFactory::create("GRAPHOS");
        poses_writer->setCameraPoses(poses);
        poses_writer->write(poses_path.replaceBaseName("poses"));


        // Transforms terrain points

        //tl::Path ground_points_path = reconstruction_path;
        //ground_points_path.append("ground_points.bin");

        //auto gp_reader = GroundPointsReaderFactory::create("GRAPHOS");
        //gp_reader->read(ground_points_path);
        //auto ground_points = gp_reader->points();
        
        for (auto &ground_point : ground_points) {
            ground_point.setPoint(affine.transform(ground_point));
        }

        auto gp_writer = GroundPointsWriterFactory::create("GRAPHOS");
        gp_writer->setGroundPoints(ground_points);
        gp_writer->write(project.groundPoints());




        //if (transform != tl::Matrix<double, 4, 4>::identity()) {
        //    project.setTransform(transform);
        //}

        //reconstruction_path.append("offset.txt");
        //project.setOffset(reconstruction_path);

        //reconstruction_path.replaceFileName("poses.bin");

        //auto poses_reader = CameraPosesReaderFactory::create("GRAPHOS");
        //poses_reader->read(reconstruction_path);
        //auto poses = poses_reader->cameraPoses();

        //for (const auto &camera_pose : poses) {
        //    project.addPhotoOrientation(camera_pose.first, camera_pose.second);
        //}

        //reconstruction_path.replaceFileName("sparse.ply");
        //project.setSparseModel(reconstruction_path);

        //reconstruction_path.replaceFileName("ground_points.bin");
        //project.setGroundPoints(reconstruction_path);

        project.setCrs(QString::fromStdString(crs));

        auto cameras = georeference_task.cameras();

        for (const auto &camera : cameras) {
            project.updateCamera(camera.first, camera.second);
        }

        project.save(project_path);

    } catch (const std::exception &e) {

        tl::printException(e);

        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos