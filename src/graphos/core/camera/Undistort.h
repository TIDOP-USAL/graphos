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
#include <stop_token>

#include <QString>

#include <opencv2/core/core.hpp>

#include <tidop/core/task/Task.h>

#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/Calibration.h"
#include "graphos/core/image.h"

namespace tl
{
class Progress;
}

namespace graphos
{

/// TODO: dejar sólo en el cpp para su uso por la clase Undistort
auto openCvCameraMatrix(const Calibration &calibration) -> cv::Mat;
auto openCvDistortionCoefficients(const Calibration &calibration) -> cv::Mat;


/*!
 * \brief Class for the correction of image distortion and image coordinate points.
 */
class Undistort
{

public:

    Undistort();
    Undistort(const Camera &camera);
    Undistort(const Undistort &undistort);
    ~Undistort() = default;

    auto operator=(const Undistort &undistort) -> Undistort&;

    auto camera() const -> Camera;
    void setCamera(const Camera &camera);
    auto undistortCamera() const -> Camera;
    auto undistortImage(const cv::Mat &image,
                        bool cuda = false) const -> cv::Mat;
    auto undistortPoint(const tl::Point2f &point) const -> tl::Point2f;

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


/*!
 * \brief Task for the correction of a set of images
 */
class UndistortImages
  : public tl::Task
{

public:

    enum class Format : uint8_t
    {
        tiff,
        jpeg,
        png
    };

public:

    UndistortImages(const std::unordered_map<size_t, Image> &images,
                    const std::map<int, Camera> &cameras,
                    tl::Path outputPath,
                    Format outputFormat,
                    bool cuda = false,
                    bool useIdAsName = false,
                    bool normalize = false);

    ~UndistortImages() override;

// Task

protected:

    void execute(tl::Progress *progressBar, std::stop_token stopToken) override;

private:

    std::unordered_map<size_t, Image> mImages;
    std::map<int, Camera> mCameras;
    tl::Path mOutputPath;
    Format mOutputFormat;
    bool mUseCuda;
    bool mUseIdAsName;
    bool mNormalize;
};


} // namespace graphos

#endif // GRAPHOS_CORE_CAMERA_UNDISTORT
