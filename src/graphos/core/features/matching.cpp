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

#include <tidop/core/app/Message.h>
#include <tidop/core/base/Exception.h>
#include <tidop/core/task/Progress.h>

TL_DISABLE_WARNINGS
#include <colmap/feature/sift.h>
#include <colmap/feature/utils.h>
#include <colmap/retrieval/visual_index.h>
#include <colmap/util/cuda.h>
#include <colmap/util/misc.h>
#include <colmap/estimators/two_view_geometry.h>
#include <colmap/controllers/feature_matching_utils.h>

TL_DEFAULT_WARNINGS

namespace graphos
{

FeatureMatching::FeatureMatching()
  : mCrossCheck(true),
    mRatio(0.8),
    mDistance(0.7),
    mMaxError(4.0),
    mConfidence(0.999)
{
}

FeatureMatching::FeatureMatching(const FeatureMatching &featureMatching)
  : mCrossCheck(featureMatching.mCrossCheck),
    mRatio(featureMatching.mRatio),
    mDistance(featureMatching.mDistance),
    mMaxError(featureMatching.mMaxError),
    mConfidence(featureMatching.mConfidence)
{
}

auto FeatureMatching::crossCheck() const -> bool
{
    return mCrossCheck;
}

void FeatureMatching::enableCrossCheck(bool enable)
{
    mCrossCheck = enable;
}

auto FeatureMatching::ratio() const -> double
{
    return mRatio;
}

void FeatureMatching::setRatio(double ratio)
{
    mRatio = ratio;
}

auto FeatureMatching::distance() const -> double
{
    return mDistance;
}

void FeatureMatching::setDistance(double distance)
{
    mDistance = distance;
}

auto FeatureMatching::maxError() const -> double
{
    return mMaxError;
}

void FeatureMatching::setMaxError(double error)
{
    mMaxError = error;
}

auto FeatureMatching::confidence() const -> double
{
    return mConfidence;
}

void FeatureMatching::setConfidence(double error)
{
    mConfidence = error;
}



/* Exhaustive Feature Matching */


FeatureMatchingTask::FeatureMatchingTask(tl::Path database,
                                         bool cuda,
                                         const std::shared_ptr<FeatureMatching> &featureMatching)
  : mDatabase(std::move(database)),
    bUseCuda(cuda),
    mFeatureMatching(featureMatching)
{
}

FeatureMatchingTask::~FeatureMatchingTask() = default;

void FeatureMatchingTask::execute(tl::Progress *progressBar, std::stop_token stopToken)
{
    try {

        auto database = colmap::Database::Open(mDatabase.toUtf8());
        TL_ASSERT(database->NumKeypoints() > 0, "Keypoints not found in the database");

        colmap::FeatureMatchingOptions matching_options;
        matching_options.sift = std::make_shared<colmap::SiftMatchingOptions>();
        matching_options.sift->cross_check = mFeatureMatching->crossCheck();
        matching_options.sift->max_ratio = mFeatureMatching->ratio();
        matching_options.sift->max_distance = mFeatureMatching->distance();
        matching_options.use_gpu = bUseCuda;

        colmap::TwoViewGeometryOptions geometry_options;
        geometry_options.min_num_inliers = 15;
        geometry_options.ransac_options.confidence = mFeatureMatching->confidence();
        geometry_options.ransac_options.max_error = mFeatureMatching->maxError();


        colmap::ExhaustivePairingOptions pairing_options;
        auto cache = std::make_shared<colmap::FeatureMatcherCache>(pairing_options.CacheSize(), database);

        colmap::FeatureMatcherController matcher_(matching_options, geometry_options, cache);
        TL_ASSERT(matcher_.Setup(), "");

        auto pair_generator = std::make_unique<colmap::ExhaustivePairGenerator>(pairing_options, cache);

        while (!pair_generator->HasFinished()) {

            interruptionPoint(stopToken); // Emite una excepción
            pausePoint(stopToken);

            const auto &image_pairs = pair_generator->Next();
            matcher_.Match(image_pairs);

            if (progressBar) (*progressBar)();
        }

        size_t num_matches = database->NumMatches();
        database->Close();

        if (status() != Status::stopping) {

            TL_ASSERT(num_matches > 0, "Matching points not detected");

            mReport.matches = static_cast<int>(num_matches);
            mReport.cuda = bUseCuda;
            mReport.time = this->time();

            tl::Message::success("Feature Matching finished in {:.2} minutes", mReport.time / 60.);
            tl::Message::info(" - Total matches: {}", num_matches);

        }

    } catch (...) {

        TL_THROW_EXCEPTION_WITH_NESTED("Feature Matching error");
    }
}

auto FeatureMatchingTask::database() const -> tl::Path
{
    return mDatabase;
}

void FeatureMatchingTask::setDatabase(const tl::Path &database)
{
    mDatabase = database;
}

auto FeatureMatchingTask::useGPU() const -> bool
{
    return bUseCuda;
}

void FeatureMatchingTask::setUseGPU(bool useGPU)
{
    bUseCuda = useGPU;
}

auto FeatureMatchingTask::featureMatching() const -> std::shared_ptr<FeatureMatching>
{
    return mFeatureMatching;
}

void FeatureMatchingTask::setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching)
{
    mFeatureMatching = featureMatching;
}

auto FeatureMatchingTask::report() const -> FeatureMatchingReport
{
    return mReport;
}


/* Spatial Matching */

SpatialMatchingTask::SpatialMatchingTask(tl::Path database,
                                         bool cuda,
                                         const std::shared_ptr<FeatureMatching> &featureMatching,
                                         bool geodeticCoordinates)
  : mDatabase(std::move(database)),
    bUseCuda(cuda),
    mFeatureMatching(featureMatching),
    mGeodeticCoordinates(geodeticCoordinates)
{
}

SpatialMatchingTask::~SpatialMatchingTask() = default;

void SpatialMatchingTask::execute(tl::Progress *progressBar, std::stop_token stopToken)
{
    try {

        auto database = colmap::Database::Open(mDatabase.toUtf8());
        TL_ASSERT(database->NumKeypoints() > 0, "Keypoints not found in the database");

        colmap::FeatureMatchingOptions matching_options;
        matching_options.sift = std::make_shared<colmap::SiftMatchingOptions>();
        matching_options.sift->cross_check = mFeatureMatching->crossCheck();
        matching_options.sift->max_ratio = mFeatureMatching->ratio();
        matching_options.sift->max_distance = mFeatureMatching->distance();
        matching_options.use_gpu = bUseCuda;

        colmap::TwoViewGeometryOptions geometry_options;
        geometry_options.min_num_inliers = 15;
        geometry_options.ransac_options.confidence = mFeatureMatching->confidence();
        geometry_options.ransac_options.max_error = mFeatureMatching->maxError();

        colmap::SpatialPairingOptions spatial_pairing_options;
        spatial_pairing_options.max_distance = 250;
        spatial_pairing_options.ignore_z = true;
        spatial_pairing_options.max_num_neighbors = 100;

        auto cache = std::make_shared<colmap::FeatureMatcherCache>(spatial_pairing_options.CacheSize(), database);

        colmap::FeatureMatcherController matcher_(matching_options, geometry_options, cache);
        
        TL_ASSERT(matcher_.Setup(), "");

        auto pair_generator = std::make_unique<colmap::SpatialPairGenerator>(spatial_pairing_options, cache);

        while (!pair_generator->HasFinished()) {

            interruptionPoint(stopToken);
            pausePoint(stopToken);

            const auto &image_pairs = pair_generator->Next();
            matcher_.Match(image_pairs);

            if (progressBar) (*progressBar)();
        }

        size_t num_matches = database->NumMatches();
        database->Close();

        if (status() != Status::stopping) {

            TL_ASSERT(num_matches > 0, "Matching points not detected");

            mReport.matches = static_cast<int>(num_matches);
            mReport.cuda = bUseCuda;
            mReport.time = this->time();

            tl::Message::success("Feature Matching finished in {:.2} minutes", mReport.time / 60.);
            tl::Message::info(" - Total matches: {}", num_matches);

        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Feature Matching error");
    }
}

auto SpatialMatchingTask::database() const -> tl::Path
{
    return mDatabase;
}

void SpatialMatchingTask::setDatabase(const tl::Path &database)
{
    mDatabase = database;
}

auto SpatialMatchingTask::useGPU() const -> bool
{
    return bUseCuda;
}

void SpatialMatchingTask::setUseGPU(bool useGPU)
{
    bUseCuda = useGPU;
}

auto SpatialMatchingTask::featureMatching() const -> std::shared_ptr<FeatureMatching>
{
    return mFeatureMatching;
}

void SpatialMatchingTask::setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching)
{
    mFeatureMatching = featureMatching;
}

auto SpatialMatchingTask::report() const -> FeatureMatchingReport
{
    return mReport;
}

} // namespace graphos

