#ifndef GRAPHOS_FEATURE_MATCHING_PROCESS_H
#define GRAPHOS_FEATURE_MATCHING_PROCESS_H

#include "graphos/process/ProcessConcurrent.h"
#include "graphos/core/features/features.h"



namespace colmap
{
struct ExhaustiveMatchingOptions;
struct SiftMatchingOptions;
class ExhaustiveFeatureMatcher;
class Thread;
}

namespace graphos
{

class GRAPHOS_EXPORT FeatureMatchingProcess
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

//private slots:
//
//  void onCancel();

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

// Process interface

public slots:

  void stop() override;

private:

  colmap::Thread *mFeatureMatcher;
  QString mDatabase;
  bool bUseCuda;
  bool bSpatialMatching;
  std::shared_ptr<FeatureMatching> mFeatureMatching;

};

} // namespace graphos

#endif // GRAPHOS_FEATURE_MATCHING_PROCESS_H
