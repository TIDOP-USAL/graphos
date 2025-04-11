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
#include "graphos/core/sfm/posesio.h"

#include <tidop/geospatial/crs.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geospatial/util.h>

#include <QSettings>
#include <QFileInfo>

/// TODO: mover
#include <colmap/base/reconstruction.h>


namespace graphos
{

OrthophotoModelImp::OrthophotoModelImp(Project *project, QObject *parent)
  : OrthophotoModel(parent),
    mProject(project),
    mSettings(new QSettings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName()))
{
    OrthophotoModelImp::init();
}

OrthophotoModelImp::~OrthophotoModelImp()
{
    if(mSettings) {
        delete mSettings;
        mSettings = nullptr;
    }
}

void OrthophotoModelImp::setGsd(double gsd)
{
    mProject->orthophoto().gsd = gsd;
}

void OrthophotoModelImp::setCrs(const QString &crs)
{
    mProject->orthophoto().epsgCode = crs;
}

void OrthophotoModelImp::setInterpolation(const QString &interpolation)
{
    mProject->orthophoto().interpolation = interpolation;
}

void OrthophotoModelImp::setReport(const OrthophotoReport &report)
{
    mProject->setOrthophotoReport(report);
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

auto OrthophotoModelImp::images() const -> Images
{
    Images images;

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

            photoOrientation.setPosition(photoOrientation.position());

            photo.setCameraPose(photoOrientation);

            images.push_back(photo);
        }

    }

    return images;
}

auto OrthophotoModelImp::cameras() const -> Cameras
{
    return mProject->cameras();
}

auto OrthophotoModelImp::projectFolder() const -> tl::Path
{
    return mProject->projectFolder();
}

auto OrthophotoModelImp::orthoPath() const -> tl::Path
{
    return mProject->orthophoto().path;
}

void OrthophotoModelImp::setOrthoPath(const tl::Path &orthoPath)
{
    mProject->orthophoto().path = orthoPath;
}

auto OrthophotoModelImp::dtmPath() const -> tl::Path
{
    return mProject->dem().dsmPath;
}

auto OrthophotoModelImp::enuCrs() const -> QString
{
    return mProject->enuCrs();
}

void OrthophotoModelImp::clearProject()
{
    /// TODO: 
    //mProject->clearOrtho();
}

auto OrthophotoModelImp::useCuda() const -> bool
{
    return mSettings->value("UseCuda", true).toBool();
}

auto OrthophotoModelImp::gsd() const -> double
{
    return mProject->orthophoto().gsd;
}

auto OrthophotoModelImp::interpolation() const -> QString
{
    return mProject->orthophoto().interpolation;
}

auto OrthophotoModelImp::crs() const -> QString
{
    QString epsg_code = mProject->orthophoto().epsgCode;

    try {

        epsg_code = mProject->orthophoto().epsgCode;

        if (epsg_code.isEmpty()) {
            auto enu_crs = enuCrs();
            auto v = tl::split<std::string>(enu_crs.toStdString(), ';');
            auto zone = tl::utmZoneFromLonLat(tl::stringToNumber<double>(v.at(1)), tl::stringToNumber<double>(v.at(2)));
            epsg_code = "EPSG:326";
            epsg_code.append(QString::number(zone.first));
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return epsg_code;
}

void OrthophotoModelImp::init()
{
    mReadSettings = mSettings->value("GENERAL/SAVE_PARAMETERS", false).toBool();
}

void OrthophotoModelImp::clear()
{

}

} // namespace graphos
