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

#include "FeatureMatchingProcess.h"

#include "graphos/core/features/matching.h"

#include <tidop/core/messages.h>
#include <tidop/core/exception.h>
#include <tidop/core/chrono.h>

#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/feature/sift.h>
#include <colmap/feature/matching.h>

namespace graphos
{

FeatureMatchingProcess::FeatureMatchingProcess(QString database,
                                               bool cuda,
                                               bool spatialMatching,
                                               const std::shared_ptr<FeatureMatching> &featureMatching)
  : tl::ProcessBase(),
    mFeatureMatcher(nullptr),
    mDatabase(database),
    bUseCuda(cuda),
    bSpatialMatching(spatialMatching),
    mFeatureMatching(featureMatching)
{
}

FeatureMatchingProcess::~FeatureMatchingProcess()
{
  if (mFeatureMatcher){
    delete mFeatureMatcher;
    mFeatureMatcher = nullptr;
  }
}

void FeatureMatchingProcess::execute(tl::Progress *progressBar)
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

    colmap::Database database(mDatabase.toStdString());
    TL_ASSERT(database.NumKeypoints() > 0, "Keypoints not found in the database")

    if (bSpatialMatching) {

      colmap::SpatialMatchingOptions spatialMatchingOptions;
      spatialMatchingOptions.max_num_neighbors = 100;// 500;
      //spatialMatchingOptions.max_distance = 250;
      spatialMatchingOptions.ignore_z = true;
      spatialMatchingOptions.is_gps = false; /// TODO: Comprobar el tipo de sistema de coordenadas

      mFeatureMatcher = new colmap::SpatialFeatureMatcher(spatialMatchingOptions,
                                                          siftMatchingOptions,
                                                          mDatabase.toStdString());
      msgInfo("Spatial Matching");

    } else {

      colmap::ExhaustiveMatchingOptions exhaustiveMatchingOptions;
      //    exhaustiveMatchingOptions.block_size = mFeatureMatching->blockSize();
      mFeatureMatcher = new colmap::ExhaustiveFeatureMatcher(exhaustiveMatchingOptions,
                                                             siftMatchingOptions,
                                                             mDatabase.toStdString());
      msgInfo("Exhaustive Matching");

    }

    mFeatureMatcher->Start();
    mFeatureMatcher->Wait();

    int num_matches = database.NumMatches();
    database.Close();

    TL_ASSERT(num_matches > 0, "Matching points not detected");

    chrono.stop();
    
    if(progressBar) (*progressBar)();

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Feature Matching error");
  }
}

//void FeatureMatchingProcess::stop()
//{
//  //mFeatureMatcher->Stop();
//  //Process::stop();
//}

QString FeatureMatchingProcess::database() const
{
  return mDatabase;
}

void FeatureMatchingProcess::setDatabase(const QString &database)
{
  mDatabase = database;
}

bool FeatureMatchingProcess::useGPU() const
{
  return bUseCuda;
}

void FeatureMatchingProcess::setUseGPU(bool useGPU)
{
  bUseCuda = useGPU;
}

void FeatureMatchingProcess::setSpatialMatching(bool spatialMatching)
{
  bSpatialMatching = spatialMatching;
}

//void FeatureMatchingProcess::onCancel()
//{
//  mFeatureMatcher->Stop();
//}

std::shared_ptr<FeatureMatching> FeatureMatchingProcess::featureMatching() const
{
  return mFeatureMatching;
}

void FeatureMatchingProcess::setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching)
{
  mFeatureMatching = featureMatching;
}

} // graphos
