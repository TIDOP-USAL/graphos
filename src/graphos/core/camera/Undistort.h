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

#ifndef GRAPHOS_CORE_CAMERA_UNDISTORT
#define GRAPHOS_CORE_CAMERA_UNDISTORT

#include <unordered_map>

#include <QString>

#include <opencv2/core/core.hpp>

#include <tidop/core/task.h>

#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/Calibration.h"

namespace tl
{
class Progress;
}

namespace graphos
{

class Image;

TL_DEPRECATED("Undistort","2.0")
cv::Mat openCVCameraMatrix(const Calibration &calibration);
TL_DEPRECATED("Undistort", "2.0")
cv::Mat openCVDistortionCoefficients(const Calibration &calibration);
//Camera undistortCamera(const Camera &camera);

class Undistort
{

public:

  Undistort();
  Undistort(const Camera &camera);
  Undistort(const Undistort &undistort);
  ~Undistort() = default;

  Camera camera() const;
  void setCamera(const Camera &camera);
  Camera undistortCamera() const;
  cv::Mat undistortImage(const cv::Mat &image,
                         bool cuda = false);

private:

  void init();
  void initCameraMatrix();
  void initDistCoeffs();
  void initOptimalNewCameraMatrix();
  void initUndistortCamera();
  void initUndistortMaps();

private:

  Camera mCamera;
  Camera mUndistortCamera;
  cv::Mat mCameraMatrix;
  cv::Mat mDistCoeffs;
  cv::Mat mOptimalNewCameraMatrix;
  cv::Mat mMap1;
  cv::Mat mMap2;
};

class UndistortImages
  : public tl::TaskBase
{

public:

  UndistortImages(const std::unordered_map<size_t, Image> &images,
                  const std::map<int, Camera> &cameras,
                  const QString &outputPath,
                  bool cuda);
  ~UndistortImages();

// TaskBase
  
protected:
  
  void execute(tl::Progress *progressBar = nullptr) override;

private:

  const std::unordered_map<size_t, Image> &mImages;
  const std::map<int, Camera> &mCameras;
  QString mOutputPath;
  bool bUseCuda;

};


} // namespace graphos

#endif // GRAPHOS_CORE_CAMERA_UNDISTORT
