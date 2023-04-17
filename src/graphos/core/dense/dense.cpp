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
#include "graphos/core/image.h"

#include <tidop/core/path.h>


namespace graphos
{

DensifierBase::DensifierBase(const std::unordered_map<size_t, Image> &images,
                             const std::map<int, Camera> &cameras,
                             const std::unordered_map<size_t, CameraPose> &poses,
                             const std::vector<GroundPoint> &groundPoints,
                             const tl::Path &outputPath)
  : mImages(images),
    mCameras(cameras),
    mPoses(poses),
    mGroundPoints(groundPoints),
    mOutputPath(outputPath),
    mCuda(false),
    mFormat(UndistortImages::Format::tiff)
{

}

DensifierBase::~DensifierBase()
{
}

void DensifierBase::enableCuda(bool enable)
{
  mCuda = enable;
}

tl::Path DensifierBase::denseModel() const
{
  return mDenseModel;
}

void DensifierBase::setUndistortImagesFormat(UndistortImages::Format format)
{
  mFormat = format;
}

void DensifierBase::undistort(const QString &dir)
{

  try {

    //TODO: Se están corrigiendo de distorsión todas las imágenes.
    //      Habría que ver si están orientadas
    UndistortImages undistort(mImages,
                              mCameras,
                              dir,
                              mFormat,
                              mCuda);
    undistort.run();

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

void DensifierBase::setDenseModel(const tl::Path &denseModel)
{
  mDenseModel = denseModel;
}


}
