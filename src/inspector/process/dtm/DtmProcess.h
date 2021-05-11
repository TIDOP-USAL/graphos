#ifndef INSPECTOR_DTM_PROCESS_H
#define INSPECTOR_DTM_PROCESS_H

#include "inspector/process/ProcessConcurrent.h"
#include "inspector/core/dtm/dtm.h"

#include <tidop/geometry/entities/point.h>

namespace inspector
{

class DenseExport;

class INSPECTOR_EXPORT DtmProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  DtmProcess(const std::shared_ptr<DtmAlgorithm> &dtmAlgorithm,
             const QString &pointCloud,
             const tl::Point3<double> &offset,
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
  QString mPointCloud;
  tl::Point3<double> mOffset;
  QString mDtmFile;
  double mGSD;
  bool mDSM;
};

} // namespace inspector

#endif // INSPECTOR_DTM_PROCESS_H
