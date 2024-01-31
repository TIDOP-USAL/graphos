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
    this->init();
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

bool OrientationModelImp::isPhotoOriented(size_t imageId) const
{
    return mProject->isPhotoOriented(imageId);
}

CameraPose OrientationModelImp::photoOrientation(size_t imageId) const
{
    return mProject->photoOrientation(imageId);
}

void OrientationModelImp::addPhotoOrientation(size_t imageId,
                                              const CameraPose &orientation)
{
    mProject->addPhotoOrientation(imageId, orientation);
}

tl::Path OrientationModelImp::database() const
{
    return mProject->database();
}


tl::Path OrientationModelImp::projectFolder() const
{
    return mProject->projectFolder();
}

bool OrientationModelImp::gpsPositions() const
{
    bool bGpsOrientation = false;

    auto it = mProject->images().begin();
    CameraPose camera_pose = it->second.cameraPose();
    if (!camera_pose.isEmpty())
        bGpsOrientation = true;

    return bGpsOrientation;
}

bool OrientationModelImp::rtkOrientations() const
{
    bool bRtkOrientations = false;

    auto it = mProject->images().begin();
    CameraPose camera_pose = it->second.cameraPose();
    if (!camera_pose.isEmpty()) {
        tl::Quaternion<double> q = camera_pose.quaternion();
        if (q == tl::Quaternion<double>::zero())
            bRtkOrientations = false;
        else
            bRtkOrientations = true;
    }

    return bRtkOrientations;
}

tl::Path OrientationModelImp::reconstructionPath() const
{
    return mProject->reconstructionPath();
}

//void OrientationModelImp::setReconstructionPath(const tl::Path &reconstructionPath)
//{
//    mProject->setReconstructionPath(reconstructionPath);
//}

void OrientationModelImp::clearProject()
{
    mProject->clearReconstruction();
}

const std::map<int, Camera> &OrientationModelImp::cameras() const
{
    return mProject->cameras();
}

bool OrientationModelImp::updateCamera(int id, const Camera &camera)
{
    return mProject->updateCamera(id, camera);
}

const std::unordered_map<size_t, Image> &OrientationModelImp::images() const
{
    return mProject->images();
}

OrientationReport OrientationModelImp::orientationReport() const
{
    return mProject->orientationReport();
}

void OrientationModelImp::setOrientationReport(const OrientationReport &orientationReport)
{
    mProject->setOrientationReport(orientationReport);
}

} // namespace graphos
