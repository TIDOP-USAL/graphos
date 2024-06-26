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

#include "featextract.h"

#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/Colmap.h"
//#include "graphos/core/preprocess/clahe.h"
//#include "preprocess/dhe.h"
//#include "preprocess/pohe.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/exception.h>
#include <tidop/core/concurrency.h>
#include <tidop/img/imgreader.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#ifdef HAVE_CUDA
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaarithm.hpp>
#endif // HAVE_CUDA

#include <colmap/base/database.h>
#include <colmap/base/camera_models.h>
#include <colmap/feature/sift.h>
#include <colmap/feature/utils.h>


using namespace tl;

namespace graphos
{

namespace internal
{

std::atomic<bool> featextract_opencv_read(true);
std::mutex featextract_mutex;
std::atomic<bool> featextract_done(false);

struct queue_data
{
    cv::Mat mat;
    colmap::image_t colmap_image_id;
    double scale;
    size_t image_id;
};

class ProducerImp
{

public:

    ProducerImp(const std::unordered_map<size_t, Image> *images,
                const std::map<int, Camera> *cameras,
                const colmap::Database *database,
                int maxImageSize,
                bool useGPU,
                QueueMPMC<queue_data> *buffer,
                FeatureExtractorTask *featureExtractorTask)
      : mImages(images),
        mCameras(cameras),
        mDatabase(database),
        mMaxImageSize(maxImageSize),
        bUseGPU(useGPU),
        mBuffer(buffer),
        mFeatureExtractorTask(featureExtractorTask)
    {
    }

    void operator() (size_t, size_t)
    {
        for (const auto &image : *mImages) {

            if (mFeatureExtractorTask->status() == tl::Task::Status::stopping) {
                featextract_done = true;
                return;
            }

            producer(image.second);
        }
    }

private:

    void producer(const Image &image)
    {
        try {

            std::string image_path = image.path().toStdString();

            colmap::image_t colmap_image_id;

            featextract_mutex.lock();
            bool exist_image = mDatabase->ExistsImageWithName(image_path);
            featextract_mutex.unlock();
            if (!exist_image) {
                colmap::camera_t camera_id = static_cast<colmap::camera_t>(image.cameraId());

                featextract_mutex.lock();
                bool exists_camera = mDatabase->ExistsCamera(camera_id);
                featextract_mutex.unlock();

                if (!exists_camera) {

                    colmap::Camera camera_colmap;

                    Camera camera;
                    auto it = mCameras->find(image.cameraId());
                    if (it != mCameras->end()) {
                        camera = mCameras->at(image.cameraId());
                    } else {
                        throw std::runtime_error(std::string("Camera not found for image: ").append(image_path));
                    }

                    QString colmap_camera_type = cameraToColmapType(camera);
                    int camera_model_id = colmap::CameraModelNameToId(colmap_camera_type.toStdString());
                    if (camera_model_id == -1) throw std::runtime_error("Camera model unknow");

                    size_t width = static_cast<size_t>(camera.width());
                    size_t height = static_cast<size_t>(camera.height());

                    double focal_lenght = camera.focal();

                    if (focal_lenght > 0.) {
                        camera_colmap.SetPriorFocalLength(true);
                    } else {
                        focal_lenght = 1.2 * std::max(width, height);
                        camera_colmap.SetPriorFocalLength(false);
                    }

                    camera_colmap.InitializeWithId(camera_model_id, focal_lenght, width, height);
                    camera_id = mDatabase->WriteCamera(camera_colmap);
                    camera_colmap.SetCameraId(camera_id);
                }

                colmap::Image image_colmap;
                image_colmap.SetName(image_path);

                tl::Point3<double> position = image.cameraPose().position();
                if (position != tl::Point3<double>()) {
                    image_colmap.TvecPrior(0) = image.cameraPose().position().x;
                    image_colmap.TvecPrior(1) = image.cameraPose().position().y;
                    image_colmap.TvecPrior(2) = image.cameraPose().position().z;
                }

                tl::Quaternion<double> q = image.cameraPose().quaternion();
                if (q != tl::Quaternion<double>::zero()) {
                    image_colmap.QvecPrior(0) = q.w;
                    image_colmap.QvecPrior(1) = q.x;
                    image_colmap.QvecPrior(2) = q.y;
                    image_colmap.QvecPrior(3) = q.z;
                } else {
                    image_colmap.QvecPrior().setConstant(std::numeric_limits<double>::quiet_NaN());
                }

                image_colmap.SetCameraId(camera_id);

                featextract_mutex.lock();
                colmap_image_id = mDatabase->WriteImage(image_colmap, false);
                featextract_mutex.unlock();

            } else {

                featextract_mutex.lock();
                colmap::Image image_colmap = mDatabase->ReadImageWithName(image_path);
                featextract_mutex.unlock();
                colmap_image_id = image_colmap.ImageId();
            }

            /* Lectura de imagen */

            double scale = 1.;
            cv::Mat mat = readImage(image, scale);

            /* Write queue */

            queue_data data;
            data.mat = mat;
            data.colmap_image_id = colmap_image_id;
            data.image_id = image.id();
            data.scale = scale;

            mBuffer->push(data);

        } catch (std::exception &e) {
            tl::printException(e);
            featextract_done = true;
        }
    }

    cv::Mat readImage(const Image &image, double &scale)
    {
        cv::Mat mat;

        if (featextract_opencv_read) {

            mat = cv::imread(image.path().toStdString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_GRAYSCALE | cv::IMREAD_ANYDEPTH);

            //mat = cv::imread(image.path().toStdString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_COLOR | cv::IMREAD_ANYDEPTH);
            //if (mat.channels() >= 3)
            //    convertRgbToGray(mat);

            if (mat.empty()) {
                featextract_opencv_read = false;
            } else {

                cv::Size size(mat.cols, mat.rows);
                double max_dimension = std::max(size.width, size.height);

                if (mMaxImageSize > 0 && mMaxImageSize < max_dimension) {

                    scale = max_dimension / mMaxImageSize;
                    size.width = tl::roundToInteger(size.width / scale);
                    size.height = tl::roundToInteger(size.height / scale);

                    resizeImage(mat, size);
                }
            }
        }

        if (!featextract_opencv_read) {

            auto image_reader = tl::ImageReaderFactory::create(image.path().toStdString());
            image_reader->open();
            if (image_reader->isOpen()) {

                double max_dimension = std::max(image_reader->cols(), image_reader->rows());

                if (mMaxImageSize > 0 && mMaxImageSize < max_dimension) {
                    scale = mMaxImageSize / max_dimension;
                    mat = image_reader->read(scale, scale);
                    scale = 1. / scale;
                } else {
                    mat = image_reader->read();
                }

                if (mat.channels() >= 3) {
                    convertRgbToGray(mat);
                }

                image_reader->close();
            }
        }

        normalizeImage(mat);

        return mat;
    }

    /*!
     * \brief Convert an RGB image to gray
     * \param [in|out] mat Image to convert
     */
    void convertRgbToGray(cv::Mat &mat) const
    {
#ifdef HAVE_CUDA
        if (bUseGPU) {
            cv::cuda::GpuMat gImgIn(mat);
            cv::cuda::GpuMat gImgGray;
            cv::cuda::cvtColor(gImgIn, gImgGray, cv::COLOR_BGR2GRAY);
            gImgGray.download(mat);
        } else {
#endif

            cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);

#ifdef HAVE_CUDA
        }
#endif
        //cv::Mat color_boost;
        //cv::decolor(mat, mat, color_boost);
        //color_boost.release();

        ///// CLAHE Preprocess
        //ClahePreprocessCuda clahe;
        //mat = clahe.process(mat);
        /// CLAHE Preprocess
        //DhePreprocess dhe;
        //mat = dhe.process(mat);
        //PohePreprocess pohe;
        //mat = pohe.process(mat);

    }

    void normalizeImage(cv::Mat &mat) const
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

    void resizeImage(cv::Mat &mat, const cv::Size &size) const
    {
#ifdef HAVE_CUDA
        if (bUseGPU) {
            cv::cuda::GpuMat gImgIn(mat);
            cv::cuda::GpuMat gImgResize;
            cv::cuda::resize(gImgIn, gImgResize, size);
            gImgResize.download(mat);
        } else {
#endif
            cv::resize(mat, mat, size);
#ifdef HAVE_CUDA
        }
#endif
    }

protected:

    const std::unordered_map<size_t, Image> *mImages;
    const std::map<int, Camera> *mCameras;
    const colmap::Database *mDatabase;
    int mMaxImageSize;
    bool bUseGPU;
    QueueMPMC<queue_data> *mBuffer;
    FeatureExtractorTask *mFeatureExtractorTask;
};



class ConsumerImp
{

public:

    ConsumerImp(const std::unordered_map<size_t, Image> *images,
                FeatureExtractor *featExtractor,
                std::string databaseFile,
                colmap::Database *database,
                QueueMPMC<queue_data> *buffer,
                FeatureExtractorTask *featureExtractorTask,
                bool useGPU,
                tl::Progress *progressBar)
      : mImages(images),
        mFeatExtractor(featExtractor),
        mDatabaseFile(std::move(databaseFile)),
        mDatabase(database),
        mBuffer(buffer),
        mFeatureExtractorTask(featureExtractorTask),
        bUseGPU(useGPU),
        mProgressBar(progressBar)
    {
    }

    void operator() ()
    {
        while (!featextract_done || !mBuffer->empty()) {
            if (mFeatureExtractorTask->status() == tl::Task::Status::stopping) {
                featextract_done = true;
                return;
            }

            consumer();
        }

    }

private:

    void consumer()
    {
        try {

            queue_data data;
            mBuffer->pop(data);

            /* Feature extraction */

            std::vector<cv::KeyPoint> featureKeypoints;
            cv::Mat featureDescriptors;

            featureExtraction(data.mat, featureKeypoints, featureDescriptors);
            resizeFeatures(featureKeypoints, data.scale);
            writeFeatures(data.colmap_image_id, featureKeypoints, featureDescriptors);

            QString image_name = mImages->at(data.image_id).name();
            mFeatureExtractorTask->features_extracted(data.image_id, image_name + "@" + mDatabaseFile.c_str());

        } catch (std::exception &e) {
            tl::printException(e);
            featextract_done = true;
        }

        if (mProgressBar) (*mProgressBar)();
    }

    void featureExtraction(const cv::Mat &mat,
                           std::vector<cv::KeyPoint> &keyPoints,
                           cv::Mat &descriptors) const
    {
        mFeatExtractor->run(mat, keyPoints, descriptors);
    }

    static void resizeFeatures(std::vector<cv::KeyPoint> &keyPoints,
                               double scale)
    {
        if (scale > 1) {
            for (auto &keypoint : keyPoints) {
                keypoint.pt *= scale;
                keypoint.size *= static_cast<float>(scale);
            }
        }
    }

    void writeFeatures(const colmap::image_t &image_id,
                       const std::vector<cv::KeyPoint> &keyPoints,
                       cv::Mat &descriptors) const
    {
        std::lock_guard<std::mutex> lck(featextract_mutex);

        size_t features_size = keyPoints.size();

        colmap::FeatureKeypoints keypoints_colmap(features_size);
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> descriptors_float(features_size, descriptors.cols);

        for (size_t i = 0; i < features_size; i++) {

            keypoints_colmap[i] = colmap::FeatureKeypoint(keyPoints[i].pt.x,
                                                          keyPoints[i].pt.y,
                                                          keyPoints[i].size,
                                                          keyPoints[i].angle);

            for (size_t j = 0; j < static_cast<size_t>(descriptors.cols); j++) {
                descriptors_float(i, j) = descriptors.at<float>(static_cast<int>(i), static_cast<int>(j));
            }

        }

        colmap::SiftExtractionOptions options;
        if (options.normalization == colmap::SiftExtractionOptions::Normalization::L2) {
            descriptors_float = colmap::L2NormalizeFeatureDescriptors(descriptors_float);
        } else if (options.normalization == colmap::SiftExtractionOptions::Normalization::L1_ROOT) {
            descriptors_float = colmap::L1RootNormalizeFeatureDescriptors(descriptors_float);
        } else {
            throw std::runtime_error("Description normalization type not supported");
        }

        colmap::FeatureDescriptors descriptors_colmap = colmap::FeatureDescriptorsToUnsignedByte(descriptors_float);

        mDatabase->WriteKeypoints(image_id, keypoints_colmap);
        mDatabase->WriteDescriptors(image_id, descriptors_colmap);

    }

private:

    const std::unordered_map<size_t, Image> *mImages;
    FeatureExtractor *mFeatExtractor;
    std::string mDatabaseFile;
    colmap::Database *mDatabase;
    QueueMPMC<queue_data> *mBuffer;
    FeatureExtractorTask *mFeatureExtractorTask;
    bool bUseGPU;
    tl::Progress *mProgressBar;
};

}



/******************************************************************/



FeatureExtractorTask::FeatureExtractorTask(const std::unordered_map<size_t, Image> &images,
                                           const std::map<int, Camera> &cameras,
                                           tl::Path database,
                                           int maxImageSize,
                                           bool cuda,
                                           const std::shared_ptr<FeatureExtractor> &featureExtractor)
  : mImages(images),
    mCameras(cameras),
    mDatabase(std::move(database)),
    mMaxImageSize(maxImageSize),
    bUseCuda(cuda),
    mFeatureExtractor(featureExtractor)
{
}

void FeatureExtractorTask::execute(tl::Progress *progressBar)
{

    try {

        tl::Message::info("Feature extraction running");

        colmap::Database database(mDatabase.toString());

        QueueMPMC<internal::queue_data> buffer(50);
        internal::ProducerImp producer(&mImages,
                                       &mCameras,
                                       &database,
                                       mMaxImageSize,
                                       bUseCuda,
                                       &buffer,
                                       this);
        internal::ConsumerImp consumer(&mImages,
                                       mFeatureExtractor.get(),
                                       mDatabase.toString(),
                                       &database,
                                       &buffer,
                                       this,
                                       bUseCuda,
                                       progressBar);

        size_t num_threads = 1;
        std::vector<std::thread> producer_threads(num_threads);
        std::vector<std::thread> consumer_threads(num_threads);

        internal::featextract_done = false;

        size_t size = mImages.size() / num_threads;
        for (size_t i = 0; i < num_threads; ++i) {
            size_t _ini = i * size;
            size_t _end = _ini + size;
            if (i == num_threads - 1) _end = mImages.size();

            producer_threads[i] = std::thread(producer, _ini, _end);
        }

        for (size_t i = 0; i < num_threads; ++i) {
            consumer_threads[i] = std::thread(consumer);
        }

        for (size_t i = 0; i < num_threads; ++i)
            producer_threads[i].join();

        internal::featextract_done = true;

        for (size_t i = 0; i < num_threads; ++i)
            consumer_threads[i].join();

        if (status() != Status::stopping) {
            size_t keypoints = database.NumKeypoints();

            TL_ASSERT(keypoints > 0, "Keypoints not detected");

            mReport.features = static_cast<int>(keypoints);
            mReport.cuda = bUseCuda;
            mReport.time = this->time();

            tl::Message::success("Feature extraction finished in {:.2} minutes", mReport.time / 60.);
            tl::Message::info(" - Total features extracted: {}", keypoints);

        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Feature Extractor error");
    }

}

auto FeatureExtractorTask::report() const -> FeatureExtractorReport
{
    return mReport;
}

} // graphos