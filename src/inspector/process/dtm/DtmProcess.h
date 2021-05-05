#ifndef INSPECTOR_DTM_PROCESS_H
#define INSPECTOR_DTM_PROCESS_H

#include "inspector/process/ProcessConcurrent.h"
#include "inspector/core/dtm/dtm.h"



namespace inspector
{

class DenseExport;

class INSPECTOR_EXPORT DtmProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  DtmProcess(const std::shared_ptr<DtmAlgorithm> &dtmAlgorithm,
             const std::shared_ptr<DenseExport> &denseExport,
             const QString &dtmFile,
             double gsd,
             bool dsm);
  ~DtmProcess() override = default;

signals:

  void dtmFinished();

  // ProcessConcurrent interface

protected:

  void run() override;

private:

  std::shared_ptr<DtmAlgorithm> mDtmAlgorithm;
  std::shared_ptr<DenseExport> mDenseExport;
  QString mDenseModel;
  QString mDtmFile;
  double mGSD;
  bool mDSM;
};

} // namespace inspector

#endif // INSPECTOR_DTM_PROCESS_H
