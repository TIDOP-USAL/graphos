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


#include "OrientationCommand.h"

#include "graphos/core/project.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/core/sfm/orientation.h"
#include "graphos/core/sfm/orientationcolmap.h"
#include "graphos/core/sfm/posesio.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>
#include <tidop/math/algebra/rotation_matrix.h>
#include <tidop/math/algebra/rotation_convert.h>

#include <colmap/base/reconstruction.h>
#include <colmap/util/option_manager.h>
#include <colmap/base/database_cache.h>
#include <colmap/controllers/incremental_mapper.h>

#include <QFileInfo>

#include <fstream>
#include <tidop/core/log.h>

#include "graphos/core/sfm/Reconstruction.h"

using namespace tl;

namespace graphos
{


auto cameraPositions(const ProjectImp& project) -> std::map<QString, std::array<double, 3>>
{
    std::map<QString, std::array<double, 3>> camera_positions;

    for (const auto &image : project.images()) {

        QString path = image.second.path();
        CameraPose camera_pose = image.second.cameraPose();

        if (!camera_pose.isEmpty()) {
            std::array<double, 3> positions = {
            camera_pose.position().x,
            camera_pose.position().y,
            camera_pose.position().z};
            camera_positions[path] = positions;
        }

    }

    return camera_positions;
}

OrientationCommand::OrientationCommand()
  : Command("ori", "3D Reconstruction")
{

    this->addArgument<tl::Path>("prj", 'p', "Project file");
    this->addArgument<bool>("fix_calibration", 'c', "Fix calibration", false);
    this->addArgument<bool>("use_rtk_accuracy", "Use RTK positioning accuracy", false);
    this->addArgument<bool>("use_gcp", "Use Ground Control Points for absolute orientation", true);
    this->addArgument<bool>("use_poses", "Use camera poses for absolute orientation", true);
    this->addArgument<bool>("use_rtk_accuracy", "Use RTK positioning accuracy", false);
    this->addArgument<bool>("absolute_orientation", 'a', "Absolute Orientation", false);

    this->addExample("ori -p 253/253.xml -a");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

bool OrientationCommand::run()
{
    bool r = false;

    tl::Log &log = tl::Log::instance();

    try {

        auto project_path = this->value<tl::Path>("prj");
        bool fix_calibration = this->value<bool>("fix_calibration");
        bool use_rtk_accuracy = this->value<bool>("use_rtk_accuracy");
        bool use_poses = this->value<bool>("use_poses");
        bool use_gcp = this->value<bool>("use_gcp");
        bool absolute_orientation = this->value<bool>("absolute_orientation");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        ProjectImp project;
        project.load(project_path);
        project.clearReconstruction();
        tl::Path database_path = project.database();
        tl::Path sfm_path = project.projectFolder();
        sfm_path.append("sfm");

        std::vector<Image> images;
        for (const auto &image : project.images()) {
            images.push_back(image.second);
        }

        tl::Path gcp_file = project.projectFolder();
        gcp_file.append("sfm").append("georef.xml");

        ReconstructionTask::Options options{};

        if (absolute_orientation) {

            options = options | ReconstructionTask::Options::absolute_orientation;

            if (use_gcp) {
                options = options | ReconstructionTask::Options::use_gcp;
            }

            if (use_poses) {
                options = options | ReconstructionTask::Options::use_poses;

                if (use_rtk_accuracy) {
                    options = options | ReconstructionTask::Options::use_rtk_positioning_accuracy;
                }
            }
        }

        if (fix_calibration) {
            options = options | ReconstructionTask::Options::fix_calibration;
        }

        ReconstructionTask reconstruction(database_path,
                                          sfm_path,
                                          images,
                                          project.cameras(),
                                          options,
                                          gcp_file);

        reconstruction.run();

        auto cameras = reconstruction.cameras();
        auto report = reconstruction.report();

        /// Se comprueba que se han generado todos los productos

        tl::Path sparse_model_path = sfm_path;
        sparse_model_path.append("sparse.ply");

        tl::Path ground_points_path = sfm_path;
        ground_points_path.append("ground_points.bin");

        tl::Path poses_path = sfm_path;
        poses_path.append("poses.bin");


        TL_ASSERT(sparse_model_path.exists(), "3D reconstruction fail");
        TL_ASSERT(ground_points_path.exists(), "3D reconstruction fail");
        TL_ASSERT(poses_path.exists(), "3D reconstruction fail");

        project.setSparseModel(sparse_model_path);
        project.setGroundPoints(ground_points_path);
        if (absolute_orientation) {
            project.setEnuCrs(QString::fromStdString(reconstruction.enuCrs()));
        }

        auto poses_reader = CameraPosesReaderFactory::create("GRAPHOS");
        poses_reader->read(poses_path);
        auto poses = poses_reader->cameraPoses();

        for (const auto &camera_pose : poses) {
            project.addPhotoOrientation(camera_pose.first, camera_pose.second);
        }

        tl::Message::info("Oriented {} images", poses.size());

        double oriented_percent = (static_cast<double>(poses.size()) / static_cast<double>(project.images().size())) * 100.;
        if (oriented_percent < 90.) {
            // Menos del 90% de imagenes orientadas
            tl::Message::warning("{} percent of images oriented. Increase image size and number of points in Feature detector.", tl::roundToInteger(oriented_percent));
        }

        for (const auto &camera : cameras) {
            project.updateCamera(camera.first, camera.second);
        }

        report.orientedImages = static_cast<int>(poses.size());
        report.type = absolute_orientation ? "Absolute" : "Relative";
        project.setOrientationReport(report);

        project.save(project_path);

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos
