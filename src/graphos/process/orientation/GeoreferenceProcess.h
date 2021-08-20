#ifndef GRAPHOS_GEOREFERENCE_PROCESS_H
#define GRAPHOS_GEOREFERENCE_PROCESS_H

#include "graphos/process/ProcessConcurrent.h"
#include "graphos/core/orientation/gcp.h"

namespace colmap
{
class OptionManager;
struct IncrementalMapperOptions;
class ReconstructionManager;
class IncrementalMapperController;
}

namespace graphos
{

class AbsoluteOrientationAlgorithm;

class GRAPHOS_EXPORT GeoreferenceProcess
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

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_PROCESS_H
