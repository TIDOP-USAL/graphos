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

#include "graphos/core/dense/dense.h"

#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/Undistort.h"
#include "graphos/core/image.h"

#include <tidop/core/path.h>


namespace graphos
{

DensifierBase::DensifierBase(const std::unordered_map<size_t, Image> &images,
                             const std::map<int, Camera> &cameras,
                             const std::unordered_map<size_t, CameraPose> &poses,
                             const std::vector<GroundPoint> &groundPoints,
                             const QString &outputPath/*,
                             const QString &undistortPath*/)
  : mImages(images),
    mCameras(cameras),
    mPoses(poses),
    mGroundPoints(groundPoints),
    mOutputPath(outputPath.toStdWString())/*,
    mUndistortPath(undistortPath.toStdWString())*/,
    mCuda(false)
{

}

DensifierBase::~DensifierBase()
{
}

//void DensifierBase::createDirectory(const QString &dir)
//{
//  tl::Path _path(mOutputPath);
//  _path.append(dir.toStdWString());
//  if (!_path.exists() && !_path.createDirectories()) {
//    std::string err = "The output directory cannot be created: ";
//    err.append(_path.toString());
//    throw std::runtime_error(err);
//  }
//}

void DensifierBase::enableCuda(bool enable)
{
  mCuda = enable;
}

QString DensifierBase::denseModel() const
{
  return mDenseModel;
}

//void DensifierBase::setOutputPath(const QString &outputPath)
//{
//  mOutputPath = outputPath.toStdWString();
//}
//
//void DensifierBase::setUndistortPath(const QString &undistortPath)
//{
//  mUndistortPath = undistortPath.toStdWString();
//}

void DensifierBase::undistort(const QString &dir)
{

  /// TODO: Ver si ya están generadas las imágenes corregidas

  try {

    UndistortImages undistort(mImages,
                              mCameras,
                              dir,
                              mCuda);
    undistort.run();

    //if (undistort.status() != tl::Task::Status::error) 

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }
}

tl::Path DensifierBase::outputPath() const
{
  return mOutputPath;
}

const std::unordered_map<size_t, Image> &DensifierBase::images() const
{
  return mImages;
}

const std::map<int, Camera> &DensifierBase::cameras() const
{
  return mCameras;
}

const std::unordered_map<size_t, CameraPose> &DensifierBase::poses() const
{
  return mPoses;
}

const std::vector<GroundPoint> &DensifierBase::groundPoints() const
{
  return mGroundPoints;
}

void DensifierBase::setDenseModel(const QString &denseModel)
{
  mDenseModel = denseModel;
}

//tl::Path DensifierBase::undistortPath() const
//{
//  return tl::Path();
//}


}
