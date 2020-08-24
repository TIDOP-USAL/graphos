#ifndef INSPECTOR_FEATURE_MATCHING_PROCESS_H
#define INSPECTOR_FEATURE_MATCHING_PROCESS_H

#include "inspector/process/ProcessConcurrent.h"
#include "inspector/core/features/features.h"



namespace colmap
{
struct ExhaustiveMatchingOptions;
struct SiftMatchingOptions;
class ExhaustiveFeatureMatcher;
class Thread;
}

namespace inspector
{

class INSPECTOR_EXPORT FeatureMatchingProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  FeatureMatchingProcess(QString database,
                         bool cuda,
                         bool spatialMatching,
                         const std::shared_ptr<FeatureMatching> &featureMatching);
  ~FeatureMatchingProcess() override;

signals:

  void featureMatchingFinished();

// ProcessConcurrent interface

public:

  std::shared_ptr<FeatureMatching> featureMatching() const;
  void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching);

  QString database() const;
  void setDatabase(const QString &database);

  bool useGPU() const;
  void setUseGPU(bool useGPU);

  void setSpatialMatching(bool spatialMatching);

protected:

  void run() override;

private:

  colmap::Thread *mFeatureMatcher;
  QString mDatabase;
  bool bUseCuda;
  bool bSpatialMatching;
  std::shared_ptr<FeatureMatching> mFeatureMatching;

};

} // namespace inspector

#endif // INSPECTOR_FEATURE_MATCHING_PROCESS_H
