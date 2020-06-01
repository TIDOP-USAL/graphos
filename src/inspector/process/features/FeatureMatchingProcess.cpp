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
                                               const std::shared_ptr<FeatureMatching> &featureMatching)
  : ProcessConcurrent(),
    mExhaustiveFeatureMatcher(nullptr),
    mDatabase(database),
    bUseCuda(cuda),
    mFeatureMatching(featureMatching)
{
}

FeatureMatchingProcess::~FeatureMatchingProcess()
{
  if (mExhaustiveFeatureMatcher){
    delete mExhaustiveFeatureMatcher;
    mExhaustiveFeatureMatcher = nullptr;
  }
}

void inspector::FeatureMatchingProcess::run()
{
  try {

    tl::Chrono chrono;
    chrono.run();

    if (mExhaustiveFeatureMatcher){
      delete mExhaustiveFeatureMatcher;
      mExhaustiveFeatureMatcher = nullptr;
    }

    colmap::ExhaustiveMatchingOptions exhaustiveMatchingOptions;
//    exhaustiveMatchingOptions.block_size = mFeatureMatching->blockSize();
    colmap::SiftMatchingOptions siftMatchingOptions;
    siftMatchingOptions.max_error = mFeatureMatching->maxError();
    siftMatchingOptions.cross_check = mFeatureMatching->crossCheck();
    siftMatchingOptions.max_ratio = mFeatureMatching->ratio();
    siftMatchingOptions.max_distance = mFeatureMatching->distance();
    siftMatchingOptions.confidence = mFeatureMatching->confidence();
//    siftMatchingOptions.max_num_matches = mFeatureMatching->maxMatches();
    siftMatchingOptions.use_gpu = bUseCuda;

    mExhaustiveFeatureMatcher = new colmap::ExhaustiveFeatureMatcher(exhaustiveMatchingOptions,
                                                                     siftMatchingOptions,
                                                                     mDatabase.toStdString());

    mExhaustiveFeatureMatcher->Start();
    mExhaustiveFeatureMatcher->Wait();

    colmap::Database database(mDatabase.toStdString());
    if (database.NumMatches() > 0)
      emit featureMatchingFinished();

    database.Close();

    uint64_t time = chrono.stop();
    msgInfo("[Time: %f seconds]", time/1000.);

  } catch (std::exception &e) {
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

std::shared_ptr<FeatureMatching> FeatureMatchingProcess::featureMatching() const
{
  return mFeatureMatching;
}

void FeatureMatchingProcess::setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching)
{
  mFeatureMatching = featureMatching;
}

} // inspector
