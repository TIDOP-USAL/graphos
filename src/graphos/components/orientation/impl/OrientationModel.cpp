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

bool OrientationModelImp::calibratedCamera() const
{
    TL_TODO("Deberia contemplarse cámaras calibradas y sin calibrar")
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

void OrientationModelImp::setOffset(const tl::Path &offset)
{
    mProject->setOffset(offset);
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

auto OrientationModelImp::gpsPositions() const -> bool
{
    bool gps_orientation = false;

    auto it = mProject->images().begin();
    CameraPose camera_pose = it->second.cameraPose();
    if (!camera_pose.isEmpty())
        gps_orientation = true;

    return gps_orientation;
}

bool OrientationModelImp::rtkOrientations() const
{
    bool rtk_orientations = false;

    auto it = mProject->images().begin();
    CameraPose camera_pose = it->second.cameraPose();
    if (!camera_pose.isEmpty()) {
        tl::Quaternion<double> q = camera_pose.quaternion();
        if (q == tl::Quaternion<double>::zero())
            rtk_orientations = false;
        else
            rtk_orientations = true;
    }

    return rtk_orientations;
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
