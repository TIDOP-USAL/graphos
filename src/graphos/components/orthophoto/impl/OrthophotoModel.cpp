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

#include "OrthophotoModel.h"

#include "graphos/core/camera/Camera.h"
#include "graphos/core/sfm/poses.h"

#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QFileInfo>

/// TODO: mover
#include <colmap/base/reconstruction.h>
#include <opencv2/calib3d.hpp>

namespace graphos
{

OrthophotoModelImp::OrthophotoModelImp(Project *project, QObject *parent)
  : OrthophotoModel(parent),
    mSettings(new QSettings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName())),
    mProject(project)
{
    this->init();
}

OrthophotoModelImp::~OrthophotoModelImp()
{
    if(mSettings) {
        delete mSettings;
        mSettings = nullptr;
    }
}

void OrthophotoModelImp::setGSD(bool gsd)
{
    mProject->orthophoto().gsd = gsd;
}

void OrthophotoModelImp::loadSettings()
{
    if(mReadSettings) {

        /* Read Settings here

        Example (replace PropertyName):

        mParameters->setPropertyName(mSettings->value("ORTHOPHOTO/PropertyName", mParameters->propertyName()).toInt());

        */

    }
}

void OrthophotoModelImp::saveSettings()
{
    if(mReadSettings) {

        /* Write Settings here

        Example:

        mSettings->setValue("ORTHOPHOTO/PropertyName", mParameters->propertyName());

        */

    }
}

std::vector<Image> OrthophotoModelImp::images() const
{
    std::vector<Image> images;

    tl::Point3<double> offset;

    std::ifstream ifs;
    ifs.open(mProject->offset().toString(), std::ifstream::in);
    if(ifs.is_open()) {

        ifs >> offset.x >> offset.y >> offset.z;

        ifs.close();
    }

    for(const auto &image : mProject->images()) {

        Image photo(image.second);
        size_t image_id = image.first;

        if(mProject->isPhotoOriented(image_id)) {
            CameraPose photoOrientation = mProject->photoOrientation(image_id);
            auto rotation_matrix = photoOrientation.rotationMatrix();
            rotation_matrix.at(1, 0) = -photoOrientation.rotationMatrix().at(1, 0);
            rotation_matrix.at(1, 1) = -photoOrientation.rotationMatrix().at(1, 1);
            rotation_matrix.at(1, 2) = -photoOrientation.rotationMatrix().at(1, 2);
            rotation_matrix.at(2, 0) = -photoOrientation.rotationMatrix().at(2, 0);
            rotation_matrix.at(2, 1) = -photoOrientation.rotationMatrix().at(2, 1);
            rotation_matrix.at(2, 2) = -photoOrientation.rotationMatrix().at(2, 2);
            photoOrientation.setRotationMatrix(rotation_matrix);

            photoOrientation.setPosition(photoOrientation.position() + offset);

            photo.setCameraPose(photoOrientation);

            images.push_back(photo);
        }

    }

    return images;
}

std::map<int, Camera> OrthophotoModelImp::cameras() const
{
    return mProject->cameras();
}

tl::Path OrthophotoModelImp::projectFolder() const
{
    return mProject->projectFolder();
}

tl::Path OrthophotoModelImp::orthoPath() const
{
    return mProject->orthophoto().path;
}

void OrthophotoModelImp::setOrthoPath(const tl::Path &orthoPath)
{
    mProject->orthophoto().path = orthoPath;
}

tl::Path OrthophotoModelImp::dtmPath() const
{
    return mProject->dtm().dsmPath;
}

QString OrthophotoModelImp::epsCode() const
{
    return mProject->crs();
}

void OrthophotoModelImp::clearProject()
{
    /// TODO: 
    //mProject->clearOrtho();
}

bool OrthophotoModelImp::useCuda() const
{
    return mSettings->value("UseCuda", true).toBool();
}

bool OrthophotoModelImp::gsd() const
{
    return mProject->orthophoto().gsd;
}

void OrthophotoModelImp::init()
{
    mReadSettings = mSettings->value("GENERAL/SAVE_PARAMETERS", false).toBool();
}

void OrthophotoModelImp::clear()
{

}

} // namespace graphos
