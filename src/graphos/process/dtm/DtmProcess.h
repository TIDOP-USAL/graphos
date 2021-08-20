#ifndef GRAPHOS_DTM_PROCESS_H
#define GRAPHOS_DTM_PROCESS_H

#include "graphos/process/ProcessConcurrent.h"
#include "graphos/core/dtm/dtm.h"

#include <tidop/geometry/entities/point.h>

namespace graphos
{

class DenseExport;

class GRAPHOS_EXPORT DtmProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  DtmProcess(const std::shared_ptr<DtmAlgorithm> &dtmAlgorithm,
             const QString &pointCloud,
             const tl::Point3<double> &offset,
             const QString &dtmFile,
             double gsd,
             bool dsm,
             const QString &crs);
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
  QString mCrs;
};

} // namespace graphos

#endif // GRAPHOS_DTM_PROCESS_H
