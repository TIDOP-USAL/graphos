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

#include "graphos/core/project/Project.h"
#include "graphos/core/camera/CameraRepository.h"
#include "graphos/core/image/ImageRepository.h"
#include "graphos/core/orientation/CameraPosesRepository.h"

#include <QFileInfo>
#include <QSettings>
#include <QCoreApplication>

namespace graphos
{

OrientationModelImp::OrientationModelImp(Project *project,
                                         QObject *parent)
  : OrientationModel(parent),
    mProject(project),
    mSettings(new QSettings(QSettings::IniFormat,
                            QSettings::UserScope,
                            QCoreApplication::organizationName(),
                            QCoreApplication::applicationName()))
{
    OrientationModelImp::init();
}


void OrientationModelImp::init()
{
}

void OrientationModelImp::clear()
{

}

auto OrientationModelImp::orientationMethod() const -> std::string 
{
    if (auto config = mProject->orientationConfig()) {
        return config->name();
    }

    return "sequential";
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

void OrientationModelImp::setSparseModel(tl::Path sparseModel)
{
    mProject->setSparseModel(std::move(sparseModel));
}

auto OrientationModelImp::enuCrs() const -> std::string
{
    return mProject->info().enuCrs();
}

void OrientationModelImp::setEnuCrs(std::string enuCrs)
{
    mProject->info().setEnuCrs(std::move(enuCrs));
}

void OrientationModelImp::setGroundPoints(tl::Path groundPoints)
{
    mProject->setGroundPoints(std::move(groundPoints));
}

void OrientationModelImp::setCameraPosesFile(tl::Path poses)
{
    mProject->setCameraPosesFile(std::move(poses));
}

auto OrientationModelImp::isPhotoOriented(size_t imageId) const -> bool
{
    return mProject->cameraPoses().contains(imageId);
}

auto OrientationModelImp::photoOrientation(size_t imageId) const -> CameraPose
{
    if (const auto *camera_pose = mProject->cameraPoses().find(imageId)) {
        return *camera_pose;
    }

    return {};
}

void OrientationModelImp::addPhotoOrientation(size_t imageId,
                                              const CameraPose &orientation)
{
    mProject->cameraPoses().add(imageId, orientation);
}

auto OrientationModelImp::database() const -> tl::Path
{
    return mProject->info().database();
}


auto OrientationModelImp::projectFolder() const -> tl::Path
{
    return mProject->info().projectFolder();
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
    tl::Path gcp_file = mProject->info().projectFolder();
    gcp_file.append("sfm").append("georef.xml");
    return gcp_file.exists();
}

auto OrientationModelImp::groundControlPointsFile() const -> tl::Path
{
    tl::Path gcp_file = mProject->info().projectFolder();
    gcp_file.append("sfm").append("georef.xml");
    return gcp_file;
}

auto OrientationModelImp::existReconstruction() const -> bool
{
    return !mProject->sparseModel().empty() && !mProject->groundPoints().empty();
}

void OrientationModelImp::clearProject()
{
    mProject->clearOrientation();
}

auto OrientationModelImp::cameras() -> CameraRepository &
{
    return mProject->cameras();
}

auto OrientationModelImp::updateCamera(int id, const Camera& camera) -> bool
{
    return mProject->cameras().update(id, camera);
}

auto OrientationModelImp::images() const -> const ImageRepository &
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

void OrientationModelImp::setOrientationConfig(std::shared_ptr<OrientationProperties> config)
{
    mProject->setOrientationConfig(config);
}

auto OrientationModelImp::useCuda() const -> bool 
{
    return mSettings->value("UseCuda", true).toBool();
}

} // namespace graphos
