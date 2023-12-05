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

#include "SaveProjectModel.h"

#include <tidop/math/algebra/rotation_convert.h>
#include <tidop/math/algebra/affine.h>

#include "graphos/core/project.h"
#include "graphos/core/sfm/posesio.h"
#include "graphos/core/sfm/groundpoint.h"

//#include <colmap/base/similarity_transform.h>

#include <QStandardPaths>

namespace graphos
{

SaveProjectModelImp::SaveProjectModelImp(Project *project,
                                         QObject *parent)
  : SaveProjectModel(parent),
    mProject(project)
{
    SaveProjectModelImp::init();
}

SaveProjectModelImp::~SaveProjectModelImp()
{
}

void SaveProjectModelImp::save()
{

    try {

        auto &transform = mProject->transform();
        if (transform != tl::Matrix<double, 4, 4>::identity()) {

            // Transforma los modelos

            auto sparse = mProject->sparseModel();
            if (sparse.exists()) transformModel(transform, sparse.toString());
            auto dense = mProject->denseModel();
            if (dense.exists()) transformModel(transform, dense.toString());
            auto mesh = mProject->meshPath();
            if (mesh.exists()) transformModel(transform, mesh.toString());

            tl::Path sfm_path = mProject->reconstructionPath();

            tl::Affine<double, 3> affine(transform.block(0, 2, 0, 3));
            auto rotation = affine.rotation();

            // Transforma las cámaras
            {

                tl::Path poses_path = sfm_path;
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

                    auto rot = camera_pose.rotationMatrix() * rotation.inverse();
                    camera_pose.setRotationMatrix(tl::RotationMatrix<double>(rot));

                    mProject->addPhotoOrientation(image_id, camera_pose);
                }

                auto poses_writer = CameraPosesWriterFactory::create("GRAPHOS");
                poses_writer->setCameraPoses(poses);
                poses_writer->write(poses_path.replaceBaseName("poses2"));
            }

            /// Transforma los puntos terreno
            {
                tl::Path ground_points_path = sfm_path;
                ground_points_path.append("ground_points.bin");
                auto gp_reader = GroundPointsReaderFactory::create("GRAPHOS");
                gp_reader->read(ground_points_path);
                auto ground_points = gp_reader->points();

                for (auto &ground_point : ground_points) {
                    auto point = affine.transform(ground_point);
                    ground_point.setPoint(point);
                }

                auto gp_writer = GroundPointsWriterFactory::create("GRAPHOS");
                gp_writer->setGroundPoints(ground_points);
                gp_writer->write(ground_points_path.replaceBaseName("ground_points2"));
            }

            transform = tl::Matrix<double, 4, 4>::identity();
            //mProject->setTransform(tl::Matrix<double, 4, 4>::identity());
        }

    } catch (std::exception &e) {
        tl::printException(e);
    }

    mProject->save(mProject->projectPath());
}

void SaveProjectModelImp::init()
{
}

void SaveProjectModelImp::clear()
{
    mProject->clear();
}

} // namespace graphos
