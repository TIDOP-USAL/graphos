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

#include "graphos/core/camera/Undistort.h"

#include <tidop/core/concurrency.h>
#include <tidop/core/progress.h>
#include <tidop/core/chrono.h>
#include <tidop/img/imgreader.h>
#include <tidop/img/imgwriter.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#ifdef HAVE_OPENCV_CUDAWARPING
#include <opencv2/cudawarping.hpp>
#endif

#include <unordered_map>
#include <memory>

using namespace tl;

namespace graphos
{

auto openCvCameraMatrix(const Calibration &calibration) -> cv::Mat
{
    float focal_x{};
    float focal_y{};

    if (calibration.existParameter(Calibration::Parameters::focal)) {
        focal_x = static_cast<float>(calibration.parameter(Calibration::Parameters::focal));
        focal_y = static_cast<float>(calibration.parameter(Calibration::Parameters::focal));
    } else {
        focal_x = static_cast<float>(calibration.parameter(Calibration::Parameters::focalx));
        focal_y = static_cast<float>(calibration.parameter(Calibration::Parameters::focaly));
    }

    float ppx = static_cast<float>(calibration.parameter(Calibration::Parameters::cx));
    float ppy = static_cast<float>(calibration.parameter(Calibration::Parameters::cy));

    std::array<std::array<float, 3>, 3> camera_matrix_data = {focal_x, 0.f, ppx,
                                                              0.f, focal_y, ppy,
                                                              0.f, 0.f, 1.f};

    cv::Mat camera_matrix(3, 3, CV_32F, camera_matrix_data.data());

    return camera_matrix.clone();
}

auto openCvDistortionCoefficients(const Calibration &calibration) -> cv::Mat
{
    Calibration::CameraModel camera_model = calibration.cameraModel();
    bool is_fisheye = calibration.checkCameraType(Calibration::CameraType::fisheye);

    int distortions_coefficients_size = 4;

    switch (camera_model) {
    case Calibration::CameraModel::radial1:
    case Calibration::CameraModel::radial2:
    case Calibration::CameraModel::simple_radial_fisheye:
    case Calibration::CameraModel::radial_fisheye:
    case Calibration::CameraModel::opencv:
    case Calibration::CameraModel::opencv_fisheye:
    case Calibration::CameraModel::simple_pinhole:
    case Calibration::CameraModel::pinhole:
        distortions_coefficients_size = 4;
        break;
    case Calibration::CameraModel::radial3:
        distortions_coefficients_size = 5;
        break;
    case Calibration::CameraModel::opencv_full:
        distortions_coefficients_size = 8;
        break;
    }

    cv::Mat distCoeffs = cv::Mat::zeros(1, distortions_coefficients_size, CV_32F);

    distCoeffs.at<float>(0) = calibration.existParameter(Calibration::Parameters::k1) ?
                              static_cast<float>(calibration.parameter(Calibration::Parameters::k1)) : 0.f;
    distCoeffs.at<float>(1) = calibration.existParameter(Calibration::Parameters::k2) ?
                              static_cast<float>(calibration.parameter(Calibration::Parameters::k2)) : 0.f;
    if (is_fisheye) {
        distCoeffs.at<float>(2) = calibration.existParameter(Calibration::Parameters::k3) ?
                                  static_cast<float>(calibration.parameter(Calibration::Parameters::k3)) : 0.f;
        distCoeffs.at<float>(3) = calibration.existParameter(Calibration::Parameters::k4) ?
                                  static_cast<float>(calibration.parameter(Calibration::Parameters::k4)) : 0.f;
    } else {

        distCoeffs.at<float>(2) = calibration.existParameter(Calibration::Parameters::p1) ?
                                  static_cast<float>(calibration.parameter(Calibration::Parameters::p1)) : 0.f;
        distCoeffs.at<float>(3) = calibration.existParameter(Calibration::Parameters::p2) ?
                                  static_cast<float>(calibration.parameter(Calibration::Parameters::p2)) : 0.f;

        if (camera_model == Calibration::CameraModel::opencv_full ||
            camera_model == Calibration::CameraModel::radial3) {
            distCoeffs.at<float>(4) = calibration.existParameter(Calibration::Parameters::k3) ?
                                      static_cast<float>(calibration.parameter(Calibration::Parameters::k3)) : 0.f;
        }

        if (camera_model == Calibration::CameraModel::opencv_full) {
            distCoeffs.at<float>(5) = calibration.existParameter(Calibration::Parameters::k4) ?
                                      static_cast<float>(calibration.parameter(Calibration::Parameters::k4)) : 0.f;
            distCoeffs.at<float>(6) = calibration.existParameter(Calibration::Parameters::k5) ?
                                      static_cast<float>(calibration.parameter(Calibration::Parameters::k5)) : 0.f;
            distCoeffs.at<float>(7) = calibration.existParameter(Calibration::Parameters::k6) ?
                                      static_cast<float>(calibration.parameter(Calibration::Parameters::k6)) : 0.f;
        }
    }

    return distCoeffs;
}


Undistort::Undistort() = default;

Undistort::Undistort(const Camera &camera)
  : mCamera(camera)
{
    init();
}

Undistort::Undistort(const Undistort &undistort)
    : mCamera(undistort.mCamera),
      mUndistortCamera(undistort.mUndistortCamera),
      mCameraMatrix(undistort.mCameraMatrix),
      mDistCoeffs(undistort.mDistCoeffs),
      mOptimalNewCameraMatrix(undistort.mOptimalNewCameraMatrix),
      mMap1(undistort.mMap1),
      mMap2(undistort.mMap2)
{
}

auto Undistort::operator=(const Undistort &undistort) -> Undistort&
{
    if (this != &undistort) {
        mCamera = undistort.mCamera;
        mUndistortCamera = undistort.mUndistortCamera;
        mCameraMatrix = undistort.mCameraMatrix;
        mDistCoeffs = undistort.mDistCoeffs;
        mOptimalNewCameraMatrix = undistort.mOptimalNewCameraMatrix;
        mMap1 = undistort.mMap1;
        mMap2 = undistort.mMap2;
    }

    return *this;
}

auto Undistort::camera() const -> Camera
{
    return mCamera;
}

void Undistort::setCamera(const Camera &camera)
{
    mCamera = camera;
    init();
}

auto Undistort::undistortCamera() const -> Camera
{
    return mUndistortCamera;
}

auto Undistort::undistortImage(const cv::Mat &image, bool cuda) const -> cv::Mat
{
    cv::Mat img_undistort;

    try {

#ifdef HAVE_OPENCV_CUDAWARPING
        if (cuda) {
            cv::cuda::GpuMat gMap1(mMap1);
            cv::cuda::GpuMat gMap2(mMap2);
            cv::cuda::GpuMat gImgOut(image);
            cv::cuda::GpuMat gImgUndistort;

            cv::cuda::remap(gImgOut, gImgUndistort, gMap1, gMap2, cv::INTER_LINEAR, 0, cv::Scalar());
            gImgUndistort.download(img_undistort);
        } else {
#endif

            cv::remap(image, img_undistort, mMap1, mMap2, cv::INTER_LINEAR);

#ifdef HAVE_OPENCV_CUDAWARPING
        }
#endif

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return img_undistort;
}

auto Undistort::undistortPoint(const tl::Point<float>& point) const -> tl::Point<float>
{
    std::vector<cv::Point2f> cv_point{cv::Point2f(point.x, point.y)};
    std::vector<cv::Point2f> cv_point_out;
    cv::undistortPoints(cv_point, cv_point_out, mCameraMatrix, mDistCoeffs, cv::Mat(), mOptimalNewCameraMatrix);
    return {cv_point_out[0].x, cv_point_out[0].y};
}

void Undistort::init()
{
    initCameraMatrix();
    initDistCoeffs();
    initOptimalNewCameraMatrix();
    initUndistortCamera();
    initUndistortMaps();
}

void Undistort::initCameraMatrix()
{
    mCameraMatrix = openCvCameraMatrix(*mCamera.calibration());
}

void Undistort::initDistCoeffs()
{
    mDistCoeffs = openCvDistortionCoefficients(*mCamera.calibration());
}

void Undistort::initOptimalNewCameraMatrix()
{
    cv::Size size(static_cast<int>(mCamera.width()),
                  static_cast<int>(mCamera.height()));

    bool b_fisheye = mCamera.calibration()->checkCameraType(Calibration::CameraType::fisheye);

    if (!b_fisheye) {
        mOptimalNewCameraMatrix = cv::getOptimalNewCameraMatrix(mCameraMatrix, mDistCoeffs, size, 1, size, nullptr);
    } else {
        cv::fisheye::estimateNewCameraMatrixForUndistortRectify(mCameraMatrix, mDistCoeffs, size, cv::Mat(), mOptimalNewCameraMatrix, 0.0, size);
    }
}

void Undistort::initUndistortCamera()
{
    mUndistortCamera = mCamera;

    double fx = static_cast<double>(mOptimalNewCameraMatrix.at<float>(0, 0));
    double fy = static_cast<double>(mOptimalNewCameraMatrix.at<float>(1, 1));
    double focal = (fx + fy) / 2.;
    mUndistortCamera.setFocal(focal);
    /// TODO: cámara pinhole?
    std::shared_ptr<Calibration> undistort_calibration = CalibrationFactory::create(mCamera.calibration()->cameraModel());
    undistort_calibration->setParameter(Calibration::Parameters::focal, focal);
    undistort_calibration->setParameter(Calibration::Parameters::focalx, mOptimalNewCameraMatrix.at<float>(0, 0));
    undistort_calibration->setParameter(Calibration::Parameters::focaly, mOptimalNewCameraMatrix.at<float>(1, 1));
    undistort_calibration->setParameter(Calibration::Parameters::cx, mOptimalNewCameraMatrix.at<float>(0, 2));
    undistort_calibration->setParameter(Calibration::Parameters::cy, mOptimalNewCameraMatrix.at<float>(1, 2));
    mUndistortCamera.setCalibration(undistort_calibration);
}

void Undistort::initUndistortMaps()
{
    cv::Size size(mCamera.width(), mCamera.height());

    bool b_fisheye = mCamera.calibration()->checkCameraType(Calibration::CameraType::fisheye);

    if (!b_fisheye) {
        cv::initUndistortRectifyMap(mCameraMatrix, mDistCoeffs, cv::Mat(), mOptimalNewCameraMatrix, size, CV_32FC1, mMap1, mMap2);
    } else {
        cv::fisheye::initUndistortRectifyMap(mCameraMatrix, mDistCoeffs, cv::Mat(), mOptimalNewCameraMatrix, size, CV_32FC1, mMap1, mMap2);
    }
}


namespace internal
{

std::mutex undistort_images_mutex;
std::atomic<bool> data_load_done(false);

class UndistortQueueData
{

public:

    UndistortQueueData() = default;

    UndistortQueueData(cv::Mat image,
                       const std::shared_ptr<Undistort> &undistort,
                       tl::Path undistortImage)
        : mImage(std::move(image)),
          mUndistort(undistort),
          mUndistortImage(std::move(undistortImage))
    {

    }

    void setImage(const cv::Mat &image)
    {
        mImage = image;
    }

    auto image() const -> cv::Mat
    {
        return mImage;
    }

    void setUndistort(const std::shared_ptr<Undistort> &undistort)
    {
        mUndistort = undistort;
    }

    auto undistort() const -> std::shared_ptr<Undistort>
    {
        return mUndistort;
    }

    void setUndistortImage(const tl::Path &undistortImage)
    {
        mUndistortImage = undistortImage;
    }

    auto undistortImage() const -> tl::Path
    {
        return mUndistortImage;
    }

private:

    cv::Mat mImage;
    std::shared_ptr<Undistort> mUndistort;
    tl::Path mUndistortImage;
};

class UndistortProducerImp
  : public tl::Producer<UndistortQueueData>
{

public:

    UndistortProducerImp(tl::QueueMPMC<UndistortQueueData> *queue,
                         const std::unordered_map<size_t, Image> *images,
                         const std::map<int, Camera> *cameras,
                         tl::Path undistortPath,
                         std::string extension,
                         tl::Task *parentTask = nullptr)
        : tl::Producer<UndistortQueueData>(queue),
          mImages(images),
          mCameras(cameras),
          mUndistortPath(std::move(undistortPath)),
          mExtension(std::move(extension)),
          mParentTask(parentTask)
    {
    }

    void operator() (size_t ini, size_t end) override
    {
        auto it_begin = mImages->begin();
        std::advance(it_begin, ini);
        auto it_end = mImages->begin();
        std::advance(it_end, end);

        while (it_begin != it_end) {
            if (mParentTask->status() == tl::Task::Status::stopping) {
                data_load_done = true;
                return;
            }

            producer(it_begin->second);

            ++it_begin;
        }
    }

    void operator() () override
    {
        for (const auto &image : *mImages) {

            if (mParentTask->status() == tl::Task::Status::stopping) {
                data_load_done = true;
                return;
            }

            producer(image.second);
        }
    }

private:

    void producer(const Image &image)
    {
        try {

            tl::Chrono chrono;
            chrono.run();

            std::string image_path = image.path().toStdString();
            int camera_id = image.cameraId();

            auto undistort = mUndistort.find(camera_id);
            if (undistort == mUndistort.end()) {

                const auto &camera = mCameras->find(camera_id);
                if (camera != mCameras->end()) {
                    mUndistort[camera->first] = std::make_shared<Undistort>(camera->second);
                } else {
                    return;
                }

            }


            cv::Mat mat = readImage(image);

            /* Write queue */

            tl::Path undistort_image_path(mUndistortPath);
            undistort_image_path.append(image.name().toStdString());
            undistort_image_path.replaceExtension(mExtension);

            UndistortQueueData data(mat, mUndistort.at(camera_id), undistort_image_path);

            queue()->push(data);

            double time = chrono.stop();
            Message::info("Read image {}: [Time: {} seconds]", image_path, time);

        } catch (std::exception &e) {
            printException(e);
            data_load_done = true;
        }
    }

    auto readImage(const Image& image) -> cv::Mat
    {
        cv::Mat mat;

        std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::create(image.path().toStdString());
        imageReader->open();
        if (imageReader->isOpen()) {

            mat = imageReader->read();

            imageReader->close();
        }

        normalizeImage(mat);

        return mat;
    }

    void normalizeImage(cv::Mat &mat)
    {
        if (mat.depth() != CV_8U) {
#ifdef HAVE_CUDA
            if (bUseGPU) {
                cv::cuda::GpuMat gImgIn(mat);
                cv::cuda::GpuMat gImgOut;
                cv::cuda::normalize(gImgIn, gImgOut, 0., 255., cv::NORM_MINMAX, CV_8U);
                gImgOut.download(mat);
            } else {
#endif
                cv::normalize(mat, mat, 0., 255., cv::NORM_MINMAX, CV_8U);
#ifdef HAVE_CUDA
            }
#endif
        }
    }

protected:

    const std::unordered_map<size_t, Image> *mImages;
    const std::map<int, Camera> *mCameras;
    std::map<int, std::shared_ptr<Undistort>> mUndistort;
    tl::Path mUndistortPath;
    std::string mExtension;
    tl::Task *mParentTask;
};



class UndistortConsumerImp
    : public tl::Consumer<UndistortQueueData>
{

public:

    UndistortConsumerImp(tl::QueueMPMC<UndistortQueueData> *buffer,
                         std::unordered_map<size_t, Image> *images,
                         bool useGPU,
                         tl::Progress *progressBar,
                         tl::Task *parentTask = nullptr)
        : tl::Consumer<UndistortQueueData>(buffer),
          mImages(images),
          bUseGPU(useGPU),
          mProgressBar(progressBar),
          mParentTask(parentTask)
    {
    }

    void operator() () override
    {
        while (!data_load_done) {

            while (!queue()->empty()) {

                if (mParentTask->status() == tl::Task::Status::stopping) {
                    data_load_done = true;
                    return;
                }

                consumer();
            }
        }

        while (!queue()->empty()) {

            if (mParentTask->status() == tl::Task::Status::stopping) {
                data_load_done = true;
                return;
            }

            consumer();
        }

    }

private:

    void consumer()
    {
        try {

            tl::Chrono chrono;
            chrono.run();

            UndistortQueueData data;
            if (!queue()->pop(data)) return;

            cv::Mat undistort_image = data.undistort()->undistortImage(data.image(), bUseGPU);

            std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::create(data.undistortImage());
            image_writer->open();
            if (image_writer->isOpen()) {
                image_writer->create(undistort_image.rows,
                                     undistort_image.cols,
                                     undistort_image.channels(),
                                     tl::openCVDataTypeToDataType(undistort_image.type()));
                image_writer->write(undistort_image);

                image_writer->close();
            }

            double time = chrono.stop();

            Message::info("Undistort image: {} [Time: {} seconds]", data.undistortImage().toString(), time);

            if (mProgressBar) (*mProgressBar)();

        } catch (std::exception &e) {
            printException(e);
            data_load_done = true;
        }
    }

private:

    std::unordered_map<size_t, Image> *mImages;
    std::string mDatabaseFile;
    bool bUseGPU;
    tl::Progress *mProgressBar;
    tl::Task *mParentTask;

};

}





UndistortImages::UndistortImages(const std::unordered_map<size_t, Image> &images,
                                 const std::map<int, Camera> &cameras,
                                 QString outputPath,
                                 Format outputFormat,
                                 bool cuda)
    : mImages(images),
      mCameras(cameras),
      mOutputPath(std::move(outputPath)),
      mOutputFormat(outputFormat),
      mUseCuda(cuda)
{
}

UndistortImages::~UndistortImages() = default;

void UndistortImages::execute(tl::Progress *progressBar)
{

    try {

        tl::Chrono chrono("Undistort Images finished");
        chrono.run();

        std::string extension;
        switch (mOutputFormat) {
        case Format::tiff:
            extension = ".tif";
            break;
        case Format::jpeg:
            extension = ".jpg";
            break;
        case Format::png:
            extension = ".png";
            break;
        }


        tl::Path undistort_path(mOutputPath.toStdWString());
        undistort_path.createDirectories();

        tl::QueueMPMC<internal::UndistortQueueData> queue(50);
        internal::UndistortProducerImp producer(&queue,
                                                &mImages,
                                                &mCameras,
                                                undistort_path,
                                                extension,
                                                this);
        internal::UndistortConsumerImp consumer(&queue,
                                                &mImages,
                                                mUseCuda,
                                                progressBar,
                                                this);

        tl::optimalNumberOfThreads();

        size_t num_threads = 1;
        std::vector<std::thread> producer_threads(num_threads);
        std::vector<std::thread> consumer_threads(num_threads);

        internal::data_load_done = false;

        size_t size = mImages.size() / num_threads;
        for (size_t i = 0; i < num_threads; ++i) {
            size_t _ini = i * size;
            size_t _end = _ini + size;
            if (i == num_threads - 1) _end = mImages.size();

            producer_threads[i] = std::thread(producer, _ini, _end);
        }

        for (auto &consumer_thread : consumer_threads) {
            consumer_thread = std::thread(consumer);
        }

        for (auto &producer_thread : producer_threads) {
            if (producer_thread.joinable()) {
                producer_thread.join();
            }
        }

        internal::data_load_done = true;
        queue.stop();

        for (auto &consumer_thread : consumer_threads) {
            if (consumer_thread.joinable()) {
                consumer_thread.join();
            }
        }

        if (status() == Status::stopping) {
            chrono.reset();
        } else {
            chrono.stop();
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Feature Extractor error");
    }

}


} // namespace graphos
