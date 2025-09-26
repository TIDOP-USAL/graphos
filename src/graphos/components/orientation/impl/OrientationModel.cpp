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

#include "OrientationModel.h"

#include "graphos/core/project.h"

#include <QFileInfo>

namespace graphos
{

OrientationModelImp::OrientationModelImp(Project *project,
                                         QObject *parent)
  : OrientationModel(parent),
    mProject(project)
{
    OrientationModelImp::init();
}


void OrientationModelImp::init()
{
}

void OrientationModelImp::clear()
{

}

auto OrientationModelImp::hasPriorCalibration() const -> bool
{
    bool calibrated = false;
    for (const auto &camera : mProject->cameras()) {
        if (camera.second.priorCalibration()) calibrated = true;
    }
    return calibrated;
}

bool OrientationModelImp::hasAdjustedCalibration() const
{
    bool calibrated = false;
    for (const auto &camera : mProject->cameras()) {
        if (camera.second.calibration()) calibrated = true;
    }
    return calibrated;
}

void OrientationModelImp::setSparseModel(const tl::Path &sparseModel)
{
    mProject->setSparseModel(sparseModel);
}

void OrientationModelImp::setEnuCrs(const QString &enuCrs)
{
    mProject->setEnuCrs(enuCrs);
}

void OrientationModelImp::setGroundPoints(const tl::Path &groundPoints)
{
    mProject->setGroundPoints(groundPoints);
}

auto OrientationModelImp::isPhotoOriented(size_t imageId) const -> bool
{
    return mProject->isPhotoOriented(imageId);
}

auto OrientationModelImp::photoOrientation(size_t imageId) const -> CameraPose
{
    return mProject->photoOrientation(imageId);
}

void OrientationModelImp::addPhotoOrientation(size_t imageId,
                                              const CameraPose &orientation)
{
    mProject->addPhotoOrientation(imageId, orientation);
}

auto OrientationModelImp::database() const -> tl::Path
{
    return mProject->database();
}


auto OrientationModelImp::projectFolder() const -> tl::Path
{
    return mProject->projectFolder();
}

auto OrientationModelImp::hasCameraPoses() const -> bool
{
    bool has_camera_poses = false;

    for (const auto &images : mProject->images()) {

        CameraPose camera_pose = images.second.cameraPose();
        if (!camera_pose.isEmpty()) {
            has_camera_poses = true;
            break;
        }
    }

    return has_camera_poses;
}

auto OrientationModelImp::hasRtkPoses() const -> bool
{
    bool has_rtk_poses = false;

    for (const auto &images : mProject->images()) {

        CameraPose camera_pose = images.second.cameraPose();
        if (!camera_pose.isEmpty()) {
            if (camera_pose.rtkFlag() > 0) {
                has_rtk_poses = true;
                break;
            }
        }
    }

    return has_rtk_poses;
}

auto OrientationModelImp::hasGroundControlPoints() const -> bool
{
    tl::Path gcp_file = mProject->projectFolder();
    gcp_file.append("sfm").append("georef.xml");
    return gcp_file.exists();
}

auto OrientationModelImp::groundControlPointsFile() const -> tl::Path
{
    tl::Path gcp_file = mProject->projectFolder();
    gcp_file.append("sfm").append("georef.xml");
    return gcp_file;
}

auto OrientationModelImp::existReconstruction() const -> bool
{
    return !mProject->sparseModel().empty() && !mProject->groundPoints().empty();
}

void OrientationModelImp::clearProject()
{
    mProject->clearReconstruction();
}

auto OrientationModelImp::cameras() const -> const std::map<int, Camera>&
{
    return mProject->cameras();
}

auto OrientationModelImp::updateCamera(int id, const Camera& camera) -> bool
{
    return mProject->updateCamera(id, camera);
}

auto OrientationModelImp::images() const -> const std::unordered_map<size_t, Image>&
{
    return mProject->images();
}

auto OrientationModelImp::orientationReport() const -> OrientationReport
{
    return mProject->orientationReport();
}

void OrientationModelImp::setOrientationReport(const OrientationReport &orientationReport)
{
    mProject->setOrientationReport(orientationReport);
}

} // namespace graphos
