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

#include "ExtractFeaturesTask.h"

#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/core/utils.h"
#include "graphos/core/repositories/CameraRepository.h"
#include "graphos/core/repositories/ImageRepository.h"

#include <tidop/core/app/Message.h>
#include <tidop/core/base/Exception.h>
#include <tidop/core/concurrency/QueueMPMC.h>
#include <tidop/rastertools/io/Reader.h>
#include <tidop/geotools/CRSsTools.h>
#include <tidop/geotools/GeoTools.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#include <colmap/scene/database.h>
#include <colmap/scene/camera.h>
#include <colmap/sensor/models.h>
#include <colmap/feature/sift.h>
#include <colmap/feature/utils.h>
#include <colmap/scene/image.h>
#include <colmap/feature/types.h>

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

    ProducerImp(const ImageRepository &imageRepo,
                const std::unordered_map<size_t, uint32_t> &graphosToColmapId,
                int maxImageSize,
                bool useGPU,
                QueueMPMC<queue_data> *buffer,
                ExtractFeaturesTask *featureExtractorTask)
      : mImageRepo(imageRepo),
        mGraphosToColmapId(graphosToColmapId),
        mMaxImageSize(maxImageSize),
        bUseGPU(useGPU),
        mBuffer(buffer),
        mFeatureExtractorTask(featureExtractorTask)
    {
    }

    void operator() (size_t, size_t)
    {
        for (const auto &[index, image] : mImageRepo) {

            if (mFeatureExtractorTask && mFeatureExtractorTask->status() == tl::Task::Status::stopping) {
                featextract_done = true;
                return;
            }

            producer(image);
        }
    }

private:

    void producer(const Image &image)
    {
        try {

            auto image_id = Image::id(image);
            auto image_path = image.path().toString();

            /* Lectura de imagen */

            double scale = 1.;
            cv::Mat mat = readImage(image, scale);

            /* Write queue */

            queue_data data;
            data.mat = mat;
            data.colmap_image_id = static_cast<colmap::image_t>(mGraphosToColmapId.at(image_id));//colmap_image_id;
            data.image_id = image_id;
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

            mat = cv::imread(image.path().toString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_GRAYSCALE | cv::IMREAD_ANYDEPTH);

            if (mat.empty()) {
                featextract_opencv_read = false;
            } else {

                cv::Size size(mat.cols, mat.rows);
                double max_dimension = std::max(size.width, size.height);

                if (mMaxImageSize > 0 && mMaxImageSize < max_dimension) {

                    scale = max_dimension / mMaxImageSize;
                    size.width = tl::roundToInteger(size.width / scale);
                    size.height = tl::roundToInteger(size.height / scale);

                    resizeImage(mat, mat, size, bUseGPU);
                }
            }
        }

        if (!featextract_opencv_read) {

            RasterReader image_reader(image.path().toString());
            TL_ASSERT(image_reader.isOpen(), "The image could not be read");

            double max_dimension = std::max(image_reader.cols(), image_reader.rows());

            if (mMaxImageSize > 0 && mMaxImageSize < max_dimension) {
                scale = mMaxImageSize / max_dimension;
                mat = image_reader.read(scale, scale);
                scale = 1. / scale;
            } else {
                mat = image_reader.read();
            }

            if (mat.channels() >= 3) {
                convertToGray(mat, mat, bUseGPU);
            }

            image_reader.close();
        }

        normalizeImage(mat, mat, bUseGPU);

        return mat;
    }

protected:

    const ImageRepository &mImageRepo;
    std::unordered_map<size_t, uint32_t> mGraphosToColmapId;
    int mMaxImageSize;
    bool bUseGPU;
    QueueMPMC<queue_data> *mBuffer;
    ExtractFeaturesTask *mFeatureExtractorTask;
};



class ConsumerImp
{

public:

    ConsumerImp(const ImageRepository &imageRepo,
                FeatureExtractor *featExtractor,
                std::string databaseFile,
                colmap::Database *database,
                QueueMPMC<queue_data> *buffer,
                ExtractFeaturesTask *featureExtractorTask,
                bool useGPU,
                tl::Progress *progressBar)
      : mImageRepo(imageRepo),
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

            if (const auto &image_graphos = mImageRepo.find(data.image_id)) {
                QString image_name = image_graphos->name();
                mFeatureExtractorTask->features_extracted(data.image_id, image_name + "@" + mDatabaseFile.c_str());
            }

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
        try {

            size_t features_size = keyPoints.size();

            colmap::FeatureKeypoints keypoints_colmap(features_size);

            for (size_t i = 0; i < features_size; i++) {

                keypoints_colmap[i] = colmap::FeatureKeypoint(keyPoints[i].pt.x,
                                                              keyPoints[i].pt.y,
                                                              keyPoints[i].size,
                                                              keyPoints[i].angle);
            }

            colmap::FeatureDescriptorsFloatData descriptors_float(features_size, descriptors.cols);

            if (descriptors.isContinuous()) {
                std::memcpy(descriptors_float.data(), descriptors.ptr<float>(0), features_size * descriptors.cols * sizeof(float));
            } else {
                for (size_t i = 0; i < features_size; i++) {
                    for (size_t j = 0; j < static_cast<size_t>(descriptors.cols); j++) {
                        descriptors_float(i, j) = descriptors.at<float>(static_cast<int>(i), static_cast<int>(j));
                    }
                }
            }

            colmap::SiftExtractionOptions options;
            if (options.normalization == colmap::SiftExtractionOptions::Normalization::L2) {
                colmap::L2NormalizeFeatureDescriptors(&descriptors_float);
            } else if (options.normalization == colmap::SiftExtractionOptions::Normalization::L1_ROOT) {
                colmap::L1RootNormalizeFeatureDescriptors(&descriptors_float);
            } else {
                throw std::runtime_error("Description normalization type not supported");
            }

            colmap::FeatureDescriptors descriptors_colmap;
            descriptors_colmap.data = colmap::FeatureDescriptorsToUnsignedByte(descriptors_float);
            descriptors_colmap.type = colmap::FeatureExtractorType::SIFT;

            {
                std::lock_guard<std::mutex> lck(featextract_mutex);

                mDatabase->WriteKeypoints(image_id, keypoints_colmap);
                mDatabase->WriteDescriptors(image_id, descriptors_colmap);
            }

        } catch (std::exception &e) {
            tl::printException(e);
        }
    }

private:

    const ImageRepository &mImageRepo;
    FeatureExtractor *mFeatExtractor;
    std::string mDatabaseFile;
    colmap::Database *mDatabase;
    QueueMPMC<queue_data> *mBuffer;
    ExtractFeaturesTask *mFeatureExtractorTask;
    bool bUseGPU;
    tl::Progress *mProgressBar;
};

}



/******************************************************************/



ExtractFeaturesTask::ExtractFeaturesTask(const ImageRepository &imageRepo,
                                         const CameraRepository &cameraRepo,
                                         std::string enuCrs,
                                         tl::Path database,
                                         int maxImageSize,
                                         bool cuda,
                                         const std::shared_ptr<FeatureExtractor> &featureExtractor)
  : mImageRepo(imageRepo),
    mCameraRepo(cameraRepo),
    mEnuCrs(std::move(enuCrs)),
    mDatabase(std::move(database)),
    mMaxImageSize(maxImageSize),
    bUseCuda(cuda),
    mFeatureExtractor(featureExtractor)
{
}

void ExtractFeaturesTask::execute(tl::Progress *progressBar, std::stop_token stopToken)
{

    try {

        tl::Message::info("Feature extraction running");

        TL_ASSERT(!mImageRepo.empty(), "Cannot extract features: Image repository is empty.");
        TL_ASSERT(!mCameraRepo.empty(), "Cannot extract features: Camera repository is empty.");
        TL_ASSERT(mFeatureExtractor != nullptr, "Cannot extract features: Feature extractor is null.");

        setupDatabaseAndMappings();

        auto database = colmap::Database::Open(mDatabase.toUtf8());

        QueueMPMC<internal::queue_data> buffer(50);
        internal::ProducerImp producer(mImageRepo,
                                       mGraphosToColmapId,
                                       mMaxImageSize,
                                       bUseCuda,
                                       &buffer,
                                       this);
        internal::ConsumerImp consumer(mImageRepo,
                                       mFeatureExtractor.get(),
                                       mDatabase.toUtf8(),
                                       database.get(),
                                       &buffer,
                                       this,
                                       bUseCuda,
                                       progressBar);

        size_t num_threads = 1;
        std::vector<std::thread> producer_threads(num_threads);
        std::vector<std::thread> consumer_threads(num_threads);

        internal::featextract_done = false;

        size_t total_images = mImageRepo.size();
        size_t chunk_size = mImageRepo.size() / num_threads;

        for (size_t i = 0; i < num_threads; ++i) {
            size_t _ini = i * chunk_size;
            size_t _end = (i == num_threads - 1) ? total_images : _ini + chunk_size;

            producer_threads[i] = std::thread(producer, _ini, _end);
        }

        for (size_t i = 0; i < num_threads; ++i) {
            consumer_threads[i] = std::thread(consumer);
        }

        for (auto &t : producer_threads) t.join();

        internal::featextract_done = true;

        for (auto &t : consumer_threads) t.join();

        if (status() != Status::stopping) {
            size_t keypoints = database->NumKeypoints();

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

auto ExtractFeaturesTask::report() const -> FeatureExtractorReport
{
    return mReport;
}

void ExtractFeaturesTask::setupDatabaseAndMappings()
{
    auto database = colmap::Database::Open(mDatabase.toUtf8());
    database->BeginTransaction();

    GeoTools *geo_tools = nullptr;
    if (!mEnuCrs.empty()) 
        geo_tools = tl::GeoTools::getInstance();

    for (const auto &[image_id, image] : mImageRepo) {

        std::string image_path = image.path().toString();
        colmap::image_t colmap_image_id{};

        if (!database->ExistsImageWithName(image_path)) {

            colmap::camera_t camera_id = static_cast<colmap::camera_t>(image.cameraId());

            if (!database->ExistsCamera(camera_id)) {

                const Camera *camera = mCameraRepo.find(image.cameraId());
                TL_ASSERT(camera, "Camera not found for image: {}", image_path);

                QString colmap_camera_type = cameraToColmapType(*camera);
                TL_ASSERT(colmap::ExistsCameraModelWithName(colmap_camera_type.toStdString()), "Unknown COLMAP camera model: {}", colmap_camera_type.toStdString());

                auto camera_model_id = colmap::CameraModelNameToId(colmap_camera_type.toStdString());
                if (camera_model_id == colmap::CameraModelId::kInvalid) throw std::runtime_error("Camera model unknow");

                size_t width = static_cast<size_t>(camera->width());
                size_t height = static_cast<size_t>(camera->height());

                colmap::Camera camera_colmap;
                camera_colmap.model_id = camera_model_id;
                camera_colmap.width = width;
                camera_colmap.height = height;

                if (camera->priorCalibration()) {
                    camera_colmap.params = camera->priorCalibration()->toVector();
                    camera_colmap.has_prior_focal_length = true;
                } else {
                    camera_colmap.has_prior_focal_length = false;
                    size_t num_params = colmap::CameraModelNumParams(camera_model_id);
                    camera_colmap.params.assign(num_params, 0.0);

                    const auto focal_length_idxs = colmap::CameraModelFocalLengthIdxs(camera_model_id);
                    double focal_length = camera->focal() > 0. ? camera->focal() : 1.2 * std::max(width, height);

                    for (const auto idx : focal_length_idxs) {
                        camera_colmap.params[idx] = focal_length;
                    }

                    const auto principal_point_idxs = colmap::CameraModelPrincipalPointIdxs(camera_model_id);
                    if (principal_point_idxs.size() == 2) {
                        camera_colmap.params[principal_point_idxs[0]] = camera_colmap.width / 2.0;
                        camera_colmap.params[principal_point_idxs[1]] = camera_colmap.height / 2.0;
                    }
                }

                camera_id = database->WriteCamera(camera_colmap);
            }

            colmap::Image image_colmap;
            image_colmap.SetName(image_path);
            image_colmap.SetCameraId(camera_id);
            colmap_image_id = database->WriteImage(image_colmap, false);

            auto position = image.cameraPose().position();
            
            if (position != tl::Point3d()) {

                auto crs = image.cameraPose().crs().toStdString();
                if (geo_tools) {
                    geo_tools->ptrCRSsTools()->crsOperation(crs, mEnuCrs, position.x(), position.y(), position.z());
                }

                colmap::PosePrior pose_prior;
                pose_prior.corr_data_id = colmap::data_t(colmap::sensor_t(colmap::SensorType::CAMERA, camera_id), colmap_image_id);
                pose_prior.position = Eigen::Vector3d(position.x(), position.y(), position.z());
                pose_prior.coordinate_system = colmap::PosePrior::CoordinateSystem::CARTESIAN;

                tl::Quaternion<double> q = image.cameraPose().quaternion();
                if (q != tl::Quaternion<double>::zero()) {
                    Eigen::Quaterniond q_eigen(q.w(), q.x(), q.y(), q.z());
                    // El vector gravedad [0, 0, 1] transformado al sistema local de la cámara
                    pose_prior.gravity = q_eigen.conjugate() * Eigen::Vector3d(0.0, 0.0, 1.0);
                }

                auto acc = image.cameraPose().accuracy();
                Eigen::Vector3d accuracy(acc.x(), acc.y(), acc.z());
                auto rtk_flag = image.cameraPose().rtkFlag();
                if (rtk_flag == 50) { // RTK Fix
                    accuracy = Eigen::Vector3d(0.01, 0.01, 0.03);
                } else if (rtk_flag == 34) { // RTK Float
                    accuracy = Eigen::Vector3d(0.2, 0.2, 0.5);
                } else if (rtk_flag == 16) { // Single/Standalone GPS
                    accuracy = Eigen::Vector3d(10.0, 10.0, 10.0);
                }

                Eigen::Matrix3d cov = Eigen::Matrix3d::Zero();
                cov(0, 0) = accuracy.x() * accuracy.x(); // sigma_x^2
                cov(1, 1) = accuracy.y() * accuracy.y(); // sigma_y^2
                cov(2, 2) = accuracy.z() * accuracy.z(); // sigma_z^2
                pose_prior.position_covariance = cov;

                database->WritePosePrior(pose_prior);
            }

        } else {

            auto image_colmap = database->ReadImageWithName(image_path);

            // Si no existe lanzar error
            if (image_colmap)
                colmap_image_id = image_colmap.value().ImageId();

        }

        mGraphosToColmapId[image_id] = static_cast<uint32_t>(colmap_image_id);
    }

    database->EndTransaction();
}

} // graphos