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

#include "ExportCameraPosesTask.h"

#include "graphos/core/sfm/orientationexport.h"
#include "graphos/core/utils.h"

/* TidopLib */
#include <tidop/core/chrono.h>
#include <tidop/core/progress.h>
#include <tidop/math/algebra/rotation_convert.h>
#include <tidop/math/algebra/quaternion.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geotools/CRSsTools.h>
#include <tidop/geotools/GeoTools.h>

/* Qt */
#include <QFile>
#include <QJsonArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <fstream>


using namespace tl;

namespace graphos
{

ExportCameraPosesTask::ExportCameraPosesTask(tl::Path file,
                                             const std::unordered_map<size_t, Image> &images,
                                             const std::unordered_map<size_t, CameraPose> &poses,
                                             const std::map<int, Camera> &cameras,
                                             QString enuCrs,
                                             QString format)
  : tl::TaskBase(),
    mFile(std::move(file)),
    mImages(images),
    mPoses(poses),
    mCameras(cameras),
    mEnuCrs(std::move(enuCrs)),
    mFormat(std::move(format)),
    mQuaternions(false)
{
}

void ExportCameraPosesTask::setQuaternionRotation(bool quaternions)
{
    mQuaternions = quaternions;
}

void ExportCameraPosesTask::textExport()
{
    std::ofstream stream(mFile.toString(), std::ios::trunc);
    TL_ASSERT(stream.is_open(), "Can't open {}", mFile.toUtf8());

    if (mQuaternions) {
        stream << "image X Y Z QW QX QY QZ" << std::endl;
    } else {
        stream << "image X Y Z Omega Phi Kappa" << std::endl;
    }

    for (auto &pose : mPoses) {
        auto image_id = pose.first;
        auto &camera_pose = pose.second;
        auto &image = mImages[image_id];

        TL_TODO("Permitir cambio de sistema de referencia")
        auto position = camera_pose.position() /*+ mOffset*/;
        auto quaternion = camera_pose.quaternion();

        stream << std::fixed << "\"" << image.path().toStdString() << "\" "
               << position.x << " " << position.y << " " << position.z << " ";

        if (mQuaternions) {
            
            stream << quaternion.w  << " " << quaternion.x << " " << quaternion.y << " " << quaternion.z << "\n";

        } else {

            EulerAngles<double> euler_angles;
            RotationConverter<double>::convert(quaternion, euler_angles);

            stream << euler_angles.x << " " << euler_angles.y << " " << euler_angles.z << "\n";
        }

    }

    stream << std::endl;
}

void ExportCameraPosesTask::odmExport()
{
    QJsonArray featuresArray;

    tl::GeoTools *ptrGeoTools = tl::GeoTools::getInstance();

    auto enu = mEnuCrs.toStdString();
    auto epsg = enuCrsToEpsg(mEnuCrs).toStdString();

    for (auto &pose : mPoses) {
        size_t image_id = pose.first;
        const auto &camera_pose = pose.second;
        const auto &image = mImages.at(image_id);

        auto position = camera_pose.position();
        auto quaternion = camera_pose.quaternion();

        EulerAngles<double> angles = quaternion;

        tl::Point3<double> pose_utm;
        if (!enu.empty() && !epsg.empty())
            pose_utm = ptrGeoTools->ptrCRSsTools()->crsOperation(enu, epsg, position);

        tl::Point3<double> pose_geo;
        if (!enu.empty() && !epsg.empty())
            pose_geo = ptrGeoTools->ptrCRSsTools()->crsOperation(enu, "EPSG:4326", position);
        

        Camera camera = mCameras.at(image.cameraId());
        auto calibration = camera.calibration();
        auto width = camera.width();
        auto height = camera.height();
        auto sensor_size = camera.sensorSize();
        auto focal = calibration->existParameter(Calibration::Parameters::focal) ?
                     calibration->parameter(Calibration::Parameters::focal) :
                     (calibration->parameter(Calibration::Parameters::focalx) + calibration->parameter(Calibration::Parameters::focaly)) / 2.;

        double focal_mm = focal * sensor_size / width;
        double focal_ratio = std::round((focal_mm / sensor_size) * 10000.0) / 10000.0;

        QJsonObject properties;
        properties["filename"] = image.name();
        properties["camera"] = QString::fromStdString(camera.make()).append(" ").
            append(QString::fromStdString(camera.model())).
            append(" ").append(QString::number(camera.width())).
            append(" ").append(QString::number(camera.height())).
            append(" ").append("brown ").append(QString::number(focal_ratio));

        properties["focal"] = focal_ratio;
        properties["width"] = width;
        properties["height"] = height;
        properties["capture_time"] = 0.0;

        QJsonArray translationArray;
        translationArray << pose_utm.x << pose_utm.y << pose_utm.z;
        properties["translation"] = translationArray;

        QJsonArray rotationArray;
        rotationArray << angles.x << angles.y << angles.z;
        properties["rotation"] = rotationArray;

        QJsonObject geometry;
        geometry["type"] = "Point";

        QJsonArray coordinatesArray;
        coordinatesArray << pose_geo.x << pose_geo.y << pose_geo.z;
        geometry["coordinates"] = coordinatesArray;

        QJsonObject feature;
        feature["type"] = "Feature";
        feature["properties"] = properties;
        feature["geometry"] = geometry;

        featuresArray.append(feature);
    }

    QJsonObject root;
    root["type"] = "FeatureCollection";
    root["features"] = featuresArray;

    QFile file(QString::fromStdString(mFile.toUtf8()));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        TL_THROW_EXCEPTION("No se pudo abrir el archivo para escribir shots.geojson");
    }

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}

void ExportCameraPosesTask::execute(tl::Progress *progressBar)
{
    try {

        tl::Chrono chrono("Exported camera poses");
        chrono.run();

        if (mFormat.compare("NVM") == 0) {
        } else if (mFormat.compare("Bundler") == 0) {
        } else if (mFormat.compare("MVE") == 0) {
        } else if (mFormat.compare("TXT") == 0) {
            textExport();
        } else if (mFormat.compare("ODM") == 0) {
            odmExport();
        }

        if (progressBar) (*progressBar)();

        if (status() == tl::Task::Status::stopping) {
            chrono.reset();
        } else {
            chrono.stop();
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Export camera poses error");
    }

}

} // graphos
