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

#include <tidop/core/messages.h>
#include <tidop/core/exception.h>
#include <tidop/core/chrono.h>

#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/feature/sift.h>
#include <colmap/feature/matching.h>
#include <colmap/base/gps.h>
#include <FLANN/flann.h>

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
    mFeatureMatcher(nullptr),
    mDatabase(database),
    bUseCuda(cuda),
    mFeatureMatching(featureMatching)
{
}

FeatureMatchingTask::~FeatureMatchingTask()
{
  if (mFeatureMatcher) {
    delete mFeatureMatcher;
    mFeatureMatcher = nullptr;
  }
}

void FeatureMatchingTask::execute(tl::Progress *progressBar)
{
  try {

    tl::Chrono chrono("Feature Matching finished");
    chrono.run();

    if (mFeatureMatcher) {
      delete mFeatureMatcher;
      mFeatureMatcher = nullptr;
    }

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
    mFeatureMatcher = new colmap::ExhaustiveFeatureMatcher(exhaustiveMatchingOptions,
                                                             siftMatchingOptions,
                                                             mDatabase.toString());

    mFeatureMatcher->Start();
    mFeatureMatcher->Wait();

    int num_matches = database.NumMatches();
    database.Close();

    if (status() == tl::Task::Status::stopping) {
      chrono.reset();
    } else {
      TL_ASSERT(num_matches > 0, "Matching points not detected");
      chrono.stop();
    }

    if (progressBar) (*progressBar)();

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Feature Matching error");
  }
}

void FeatureMatchingTask::stop()
{
  TaskBase::stop();
  mFeatureMatcher->Stop();
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
    mFeatureMatcher(nullptr),
    mDatabase(database),
    bUseCuda(cuda),
    mFeatureMatching(featureMatching)
{
}

SpatialMatchingTask::~SpatialMatchingTask()
{
  if (mFeatureMatcher) {
    delete mFeatureMatcher;
    mFeatureMatcher = nullptr;
  }
}

void SpatialMatchingTask::execute(tl::Progress *progressBar)
{
  try {

    tl::Chrono chrono("Feature Matching finished");
    chrono.run();

    if (mFeatureMatcher) {
      delete mFeatureMatcher;
      mFeatureMatcher = nullptr;
    }

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

    mFeatureMatcher = new colmap::SpatialFeatureMatcher(spatialMatchingOptions,
                                                        siftMatchingOptions,
                                                        mDatabase.toString());


    mFeatureMatcher->Start();
    mFeatureMatcher->Wait();

    int num_matches = database.NumMatches();
    database.Close();

    if (status() == tl::Task::Status::stopping) {
      chrono.reset();
    } else {
      TL_ASSERT(num_matches > 0, "Matching points not detected");
      chrono.stop();
    }

    if (progressBar) (*progressBar)();

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Feature Matching error");
  }
}

void SpatialMatchingTask::stop()
{
  TaskBase::stop();
  mFeatureMatcher->Stop();
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

