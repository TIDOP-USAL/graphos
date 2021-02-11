#ifndef INSPECTOR_GEOREFERENCE_PROCESS_H
#define INSPECTOR_GEOREFERENCE_PROCESS_H

#include "inspector/process/ProcessConcurrent.h"
#include "inspector/core/orientation/gcp.h"

namespace colmap
{
class OptionManager;
struct IncrementalMapperOptions;
class ReconstructionManager;
class IncrementalMapperController;
}

namespace inspector
{

class AbsoluteOrientationAlgorithm;

class INSPECTOR_EXPORT GeoreferenceProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  GeoreferenceProcess(const QString &inputPath,
                      const QString &outputPath,
                      const std::vector<GroundControlPoint> &groundControlPoints);
  ~GeoreferenceProcess() override;

signals:

  void georeferenceFinished();

// ProcessConcurrent interface

protected:

  void run() override;

private:

  QString mInputPath;
  QString mOutputPath;
  std::vector<GroundControlPoint> mGroundControlPoints;
};

} // namespace inspector

#endif // INSPECTOR_GEOREFERENCE_PROCESS_H
