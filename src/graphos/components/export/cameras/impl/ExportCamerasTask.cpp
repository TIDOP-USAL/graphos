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

#include "ExportCamerasTask.h"

#include "graphos/core/sfm/orientationexport.h"

/* TidopLib */
#include <tidop/core/chrono.h>
#include <tidop/core/progress.h>
#include <tidop/math/algebra/rotation_convert.h>
#include <tidop/math/algebra/quaternion.h>


/* COLMAP */
#include <colmap/base/reconstruction.h>

#include <fstream>

using namespace tl;

namespace graphos
{

ExportCamerasTask::ExportCamerasTask(const tl::Path &file,
                                     const std::unordered_map<size_t, Image> &images,
                                     const std::unordered_map<size_t, CameraPose> &poses,
                                     const tl::Point3<double> &offset, 
                                     const QString &format)
    : tl::TaskBase(),
      mFile(file),
      mImages(images),
      mPoses(poses),
      mOffset(offset),
      mFormat(format),
      mQuaternions(false)
{
}

void ExportCamerasTask::setQuaternionRotation(bool quaternions)
{
    mQuaternions = quaternions;
}

void ExportCamerasTask::textExport()
{
    std::ofstream stream(mFile.toString(), std::ios::trunc);
    TL_ASSERT(stream.is_open(), "Can't open {}", mFile.toString());

    if (mQuaternions) {
        stream << "image X Y Z QW QX QY QZ" << std::endl;
    } else {
        stream << "image X Y Z Omega Phi Kappa" << std::endl;
    }

    for (auto &pose : mPoses) {
        auto image_id = pose.first;
        auto &camera_pose = pose.second;
        auto &image = mImages[image_id];

        auto position = camera_pose.position() + mOffset;
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

void ExportCamerasTask::execute(tl::Progress *progressBar)
{
    try {

        tl::Chrono chrono("Exported cameras");
        chrono.run();

        if (mFormat.compare("NVM") == 0) {
        } else if (mFormat.compare("Bundler") == 0) {
        } else if (mFormat.compare("MVE") == 0) {
        } else if (mFormat.compare("TXT") == 0) {
            textExport();
        }

        if (progressBar) (*progressBar)();

        if (status() == tl::Task::Status::stopping) {
            chrono.reset();
        } else {
            chrono.stop();
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Export cameras error");
    }

}

} // graphos
