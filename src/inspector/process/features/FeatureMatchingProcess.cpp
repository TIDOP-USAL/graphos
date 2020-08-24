#include "FeatureMatchingProcess.h"

#include <tidop/core/messages.h>

#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/feature/sift.h>
#include <colmap/feature/matching.h>

namespace inspector
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

void inspector::FeatureMatchingProcess::run()
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
    //siftMatchingOptions.max_num_matches = 30000;//mFeatureMatching->maxMatches();
    siftMatchingOptions.use_gpu = bUseCuda;
    //siftMatchingOptions.gpu_index = "0";

    if (bSpatialMatching) {

      colmap::SpatialMatchingOptions spatialMatchingOptions;

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

    colmap::Database database(mDatabase.toStdString());
    if (database.NumMatches() > 0) {
      emit featureMatchingFinished();
    } else {
      emit error(0, "Feature Matching error: No matching points detected.");
      return;
    }

    database.Close();

    uint64_t time = chrono.stop();
    msgInfo("Feature Matching finished [Time: %f seconds]", time / 1000.);

  } catch (std::exception & e) {
    emit error(0, "Feature Matching error");
    msgError(e.what());
  }
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

std::shared_ptr<FeatureMatching> FeatureMatchingProcess::featureMatching() const
{
  return mFeatureMatching;
}

void FeatureMatchingProcess::setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching)
{
  mFeatureMatching = featureMatching;
}

} // inspector
