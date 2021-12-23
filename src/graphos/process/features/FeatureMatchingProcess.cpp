#include "FeatureMatchingProcess.h"

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
  : ProcessConcurrent(),
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

void graphos::FeatureMatchingProcess::run()
{
  try {

    msgInfo("Starting Feature Matching");

    tl::Chrono chrono;
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

    
    if (database.NumMatches() > 0) {
      emit featureMatchingFinished();
    } else {
      emit error(0, "Feature Matching error: No matching points detected.");
      return;
    }

    database.Close();

    double time = chrono.stop();
    msgInfo("Feature Matching finished [Time: %f seconds]", time);

  } catch (std::exception & e) {
    emit error(0, "Feature Matching error");
    msgError(e.what());
  }
}

void FeatureMatchingProcess::stop()
{
  //mFeatureMatcher->Stop();
  //Process::stop();
}

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
