#ifndef GRAPHOS_ABSOLUTE_ORIENTATION_PROCESS_H
#define GRAPHOS_ABSOLUTE_ORIENTATION_PROCESS_H

#include "graphos/process/ProcessConcurrent.h"

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

class GRAPHOS_EXPORT AbsoluteOrientationProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  AbsoluteOrientationProcess(std::shared_ptr<AbsoluteOrientationAlgorithm> &orientationAlgorithm);
  ~AbsoluteOrientationProcess() override;

signals:

  void absoluteOrientationFinished();

// ProcessConcurrent interface

protected:

  void run() override;

private:

  std::shared_ptr<AbsoluteOrientationAlgorithm> mAbsoluteOrientationAlgorithm;

};

} // namespace graphos

#endif // GRAPHOS_ABSOLUTE_ORIENTATION_PROCESS_H
