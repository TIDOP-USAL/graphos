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

#include "graphos/core/features/matching.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/exception.h>
#include <tidop/core/chrono.h>

TL_DISABLE_WARNINGS
#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/feature/sift.h>
#include <colmap/feature/matching.h>
#include <colmap/base/gps.h>
#include <flann/flann.h>
TL_DEFAULT_WARNINGS

namespace graphos
{

FeatureMatchingProperties::FeatureMatchingProperties()
  : mCrossCheck(true),
    mRatio(0.8),
    mDistance(0.7),
    mMaxError(4.0),
    mConfidence(0.999)
{
}

FeatureMatchingProperties::FeatureMatchingProperties(const FeatureMatchingProperties &featureMatching)
  : FeatureMatching(featureMatching),
    mCrossCheck(featureMatching.mCrossCheck),
    mRatio(featureMatching.mRatio),
    mDistance(featureMatching.mDistance),
    mMaxError(featureMatching.mMaxError),
    mConfidence(featureMatching.mConfidence)
{
}

bool FeatureMatchingProperties::crossCheck() const
{
    return mCrossCheck;
}

void FeatureMatchingProperties::enableCrossCheck(bool enable)
{
    mCrossCheck = enable;
}

double FeatureMatchingProperties::ratio() const
{
    return mRatio;
}

void FeatureMatchingProperties::setRatio(double ratio)
{
    mRatio = ratio;
}

double FeatureMatchingProperties::distance() const
{
    return mDistance;
}

void FeatureMatchingProperties::setDistance(double distance)
{
    mDistance = distance;
}

double FeatureMatchingProperties::maxError() const
{
    return mMaxError;
}

void FeatureMatchingProperties::setMaxError(double error)
{
    mMaxError = error;
}

double FeatureMatchingProperties::confidence() const
{
    return mConfidence;
}

void FeatureMatchingProperties::setConfidence(double error)
{
    mConfidence = error;
}



/* Exhaustive Feature Matching */


FeatureMatchingTask::FeatureMatchingTask(const tl::Path &database,
                                         bool cuda,
                                         const std::shared_ptr<FeatureMatching> &featureMatching)
  : tl::TaskBase(),
    mDatabase(database),
    bUseCuda(cuda),
    mFeatureMatching(featureMatching)
{
}

FeatureMatchingTask::~FeatureMatchingTask()
{
}

void FeatureMatchingTask::execute(tl::Progress *progressBar)
{
    try {

        tl::Chrono chrono("Feature Matching finished");
        chrono.run();

        colmap::SiftMatchingOptions siftMatchingOptions;
        siftMatchingOptions.max_error = mFeatureMatching->maxError();
        siftMatchingOptions.cross_check = mFeatureMatching->crossCheck();
        siftMatchingOptions.max_ratio = mFeatureMatching->ratio();
        siftMatchingOptions.max_distance = mFeatureMatching->distance();
        siftMatchingOptions.confidence = mFeatureMatching->confidence();
        siftMatchingOptions.use_gpu = bUseCuda;
        siftMatchingOptions.min_num_inliers = 15;// 100;

        colmap::Database database(mDatabase.toString());
        TL_ASSERT(database.NumKeypoints() > 0, "Keypoints not found in the database");

        colmap::ExhaustiveMatchingOptions exhaustiveMatchingOptions;
        
        {
            colmap::FeatureMatcherCache cache(5 * exhaustiveMatchingOptions.block_size, &database);
            colmap::SiftFeatureMatcher matcher(siftMatchingOptions, &database, &cache);

            TL_ASSERT(matcher.Setup(), "");
            cache.Setup();

            const std::vector<colmap::image_t> image_ids = cache.GetImageIds();

            const size_t block_size = static_cast<size_t>(exhaustiveMatchingOptions.block_size);
            const size_t num_blocks = static_cast<size_t>(std::ceil(static_cast<double>(image_ids.size()) / block_size));
            const size_t num_pairs_per_block = block_size * (block_size - 1) / 2;

            std::vector<std::pair<colmap::image_t, colmap::image_t>> image_pairs;
            image_pairs.reserve(num_pairs_per_block);

            for (size_t start_idx1 = 0; start_idx1 < image_ids.size(); start_idx1 += block_size) {

                size_t end_idx1 = std::min(image_ids.size(), start_idx1 + block_size) - 1;

                for (size_t start_idx2 = 0; start_idx2 < image_ids.size(); start_idx2 += block_size) {

                    size_t end_idx2 = std::min(image_ids.size(), start_idx2 + block_size) - 1;

                    if (status() == tl::Task::Status::stopping) {
                        chrono.reset();
                        return;
                    }

                    //tl::Message::info("Matching block [{}/{}, {}/{}]", start_idx1 / block_size + 1, num_blocks, start_idx2 / block_size + 1, num_blocks);

                    image_pairs.clear();

                    for (size_t idx1 = start_idx1; idx1 <= end_idx1; ++idx1) {

                        size_t block_id1 = idx1 % block_size;
                        auto image_id1 = image_ids[idx1];

                        for (size_t idx2 = start_idx2; idx2 <= end_idx2; ++idx2) {

                            size_t block_id2 = idx2 % block_size;

                            if ((idx1 > idx2 && block_id1 <= block_id2) || (idx1 < idx2 && block_id1 < block_id2)) {  
                                // Avoid duplicate pairs
                                image_pairs.emplace_back(image_id1, image_ids[idx2]);
                            }
                        }
                    }

                    colmap::DatabaseTransaction database_transaction(&database);
                    matcher.Match(image_pairs);

                    if (progressBar) (*progressBar)();

                }
            }

        }

        size_t num_matches = database.NumMatches();
        database.Close();

        if (status() == tl::Task::Status::stopping) {
            chrono.reset();
        } else {
            TL_ASSERT(num_matches > 0, "Matching points not detected");
            tl::Message::success("Feature Matching finished in {:.2} minutes", chrono.stop() / 60.);
            tl::Message::info(" - Total matches: {}", num_matches);
        }

    } catch (...) {

        TL_THROW_EXCEPTION_WITH_NESTED("Feature Matching error");
    }
}

tl::Path FeatureMatchingTask::database() const
{
    return mDatabase;
}

void FeatureMatchingTask::setDatabase(const tl::Path &database)
{
    mDatabase = database;
}

bool FeatureMatchingTask::useGPU() const
{
    return bUseCuda;
}

void FeatureMatchingTask::setUseGPU(bool useGPU)
{
    bUseCuda = useGPU;
}

std::shared_ptr<FeatureMatching> FeatureMatchingTask::featureMatching() const
{
    return mFeatureMatching;
}

void FeatureMatchingTask::setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching)
{
    mFeatureMatching = featureMatching;
}



/* Spatial Matching */

SpatialMatchingTask::SpatialMatchingTask(const tl::Path &database,
                                         bool cuda,
                                         const std::shared_ptr<FeatureMatching> &featureMatching)
  : tl::TaskBase(),
    mDatabase(database),
    bUseCuda(cuda),
    mFeatureMatching(featureMatching)
{
}

SpatialMatchingTask::~SpatialMatchingTask()
{
}

void SpatialMatchingTask::execute(tl::Progress *progressBar)
{
    try {

        tl::Chrono chrono;
        chrono.run();

        colmap::SiftMatchingOptions siftMatchingOptions;
        siftMatchingOptions.max_error = mFeatureMatching->maxError();
        siftMatchingOptions.cross_check = mFeatureMatching->crossCheck();
        siftMatchingOptions.max_ratio = mFeatureMatching->ratio();
        siftMatchingOptions.max_distance = mFeatureMatching->distance();
        siftMatchingOptions.confidence = mFeatureMatching->confidence();
        siftMatchingOptions.use_gpu = bUseCuda;
        siftMatchingOptions.min_num_inliers = 15;// 100;

        colmap::Database database(mDatabase.toString());
        TL_ASSERT(database.NumKeypoints() > 0, "Keypoints not found in the database");

        colmap::SpatialMatchingOptions spatialMatchingOptions;
        spatialMatchingOptions.max_num_neighbors = 100;// 500;
        //spatialMatchingOptions.max_distance = 250;
        spatialMatchingOptions.ignore_z = true;
        spatialMatchingOptions.is_gps = false; /// TODO: Comprobar el tipo de sistema de coordenadas

        // Extraido de colmap para incluir barra de progreso
        {
            
            colmap::FeatureMatcherCache cache(5 * spatialMatchingOptions.max_num_neighbors, &database);
            colmap::SiftFeatureMatcher matcher(siftMatchingOptions, &database, &cache);

            TL_ASSERT(matcher.Setup(), "");
            cache.Setup();

            const std::vector<colmap::image_t> image_ids = cache.GetImageIds();

            //////////////////////////////////////////////////////////////////////////////
            // Spatial indexing
            //////////////////////////////////////////////////////////////////////////////

            tl::Message::info("Indexing images");

            colmap::GPSTransform gps_transform;

            size_t num_locations = 0;
            Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> location_matrix(image_ids.size(), 3);

            std::vector<size_t> location_idxs;
            location_idxs.reserve(image_ids.size());
            std::vector<size_t> not_location_idxs;
            not_location_idxs.reserve(image_ids.size());

            /// Compute offset
            Eigen::Vector3d offset(0., 0., 0.);
            for (size_t i = 0, j=0; i < image_ids.size(); ++i) {

                auto image_id = image_ids[i];
                const auto &image = cache.GetImage(image_id);

                if (image.HasTvecPrior()) {
                    offset += (image.TvecPrior() - offset) / (++j);
                }

            }

            std::vector<Eigen::Vector3d> ells(1);

            for (size_t i = 0; i < image_ids.size(); ++i) {

                auto image_id = image_ids[i];
                const auto &image = cache.GetImage(image_id);

                if (!image.HasTvecPrior() || (image.TvecPrior(0) == 0 && image.TvecPrior(1) == 0 && spatialMatchingOptions.ignore_z) ||
                    (image.TvecPrior(0) == 0 && image.TvecPrior(1) == 0 && image.TvecPrior(2) == 0 && !spatialMatchingOptions.ignore_z)) {
                    not_location_idxs.push_back(i);
                    continue;
                }

                location_idxs.push_back(i);

                location_matrix(num_locations, 0) = static_cast<float>(image.TvecPrior(0) - offset.x());
                location_matrix(num_locations, 1) = static_cast<float>(image.TvecPrior(1) - offset.y());
                location_matrix(num_locations, 2) = static_cast<float>(spatialMatchingOptions.ignore_z ? 0 : image.TvecPrior(2) - offset.z());

                num_locations += 1;
            }

            //////////////////////////////////////////////////////////////////////////////
            // Building spatial index
            //////////////////////////////////////////////////////////////////////////////

            tl::Message::info("Building search index");

            flann::Matrix<float> locations(location_matrix.data(), num_locations, location_matrix.cols());

            flann::LinearIndexParams index_params;
            flann::LinearIndex<flann::L2<float>> search_index(index_params);
            search_index.buildIndex(locations);

            //////////////////////////////////////////////////////////////////////////////
            // Searching spatial index
            //////////////////////////////////////////////////////////////////////////////

            tl::Message::info("Searching for nearest neighbors");

            int knn = std::min<int>(spatialMatchingOptions.max_num_neighbors, num_locations);

            Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> index_matrix(num_locations, knn);
            flann::Matrix<size_t> indices(index_matrix.data(), num_locations, knn);

            Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> distance_matrix(num_locations, knn);
            flann::Matrix<float> distances(distance_matrix.data(), num_locations, knn);

            flann::SearchParams search_params(flann::FLANN_CHECKS_AUTOTUNED);
            search_params.cores = std::thread::hardware_concurrency() <= 0 ? 1 : std::thread::hardware_concurrency();

            search_index.knnSearch(locations, indices, distances, knn, search_params);

            //////////////////////////////////////////////////////////////////////////////
            // Matching
            //////////////////////////////////////////////////////////////////////////////

            float max_distance = static_cast<float>(spatialMatchingOptions.max_distance * spatialMatchingOptions.max_distance);

            std::vector<std::pair<colmap::image_t, colmap::image_t>> image_pairs;
            image_pairs.reserve(knn);

            for (size_t i = 0; i < num_locations; ++i) {
                
                if (status() == tl::Task::Status::stopping) {
                    chrono.reset();
                    return;
                }

                image_pairs.clear();

                size_t idx = location_idxs[i];
                colmap::image_t image_id = image_ids.at(idx);

                for (int j = 0; j < knn; ++j) {
                    // Check if query equals result.
                    if (index_matrix(i, j) == i) {
                        continue;
                    }

                    // Since the nearest neighbors are sorted by distance, we can break.
                    if (distance_matrix(i, j) > max_distance) {
                        break;
                    }
                    
                    size_t nn_idx = location_idxs.at(index_matrix(i, j));
                    colmap::image_t nn_image_id = image_ids.at(nn_idx);
                    image_pairs.emplace_back(image_id, nn_image_id);
                }

                colmap::DatabaseTransaction database_transaction(&database);
                matcher.Match(image_pairs);

                if (progressBar) (*progressBar)();

            }
            
            image_pairs.reserve(image_ids.size());
            
            for (size_t i = 0; i < not_location_idxs.size(); ++i) {
                
                if (status() == tl::Task::Status::stopping) {
                    chrono.reset();
                    return;
                }
            
                image_pairs.clear();
                
                auto image_id = not_location_idxs[i];
             
                for (size_t j = 0; j < image_ids.size(); ++j) {

                    if (image_id == image_ids[j]) continue;

                    auto image_id_pair = image_ids[j];
                    image_pairs.emplace_back(image_id, image_id_pair);
                }
                         
                colmap::DatabaseTransaction database_transaction(&database);
                matcher.Match(image_pairs);
            
                if (progressBar) (*progressBar)();
            }

        }

        size_t num_matches = database.NumMatches();
        database.Close();

        if (status() == tl::Task::Status::stopping) {
            chrono.reset();
        } else {
            TL_ASSERT(num_matches > 0, "Matching points not detected");

            tl::Message::success("Feature Matching finished in {:.2} minutes", chrono.stop() / 60.);
            tl::Message::info(" - Total matches: {}", num_matches);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Feature Matching error");
    }
}

tl::Path SpatialMatchingTask::database() const
{
    return mDatabase;
}

void SpatialMatchingTask::setDatabase(const tl::Path &database)
{
    mDatabase = database;
}

bool SpatialMatchingTask::useGPU() const
{
    return bUseCuda;
}

void SpatialMatchingTask::setUseGPU(bool useGPU)
{
    bUseCuda = useGPU;
}

std::shared_ptr<FeatureMatching> SpatialMatchingTask::featureMatching() const
{
    return mFeatureMatching;
}

void SpatialMatchingTask::setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching)
{
    mFeatureMatching = featureMatching;
}

} // namespace graphos

