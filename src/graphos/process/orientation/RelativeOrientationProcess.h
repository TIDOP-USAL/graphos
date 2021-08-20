#ifndef GRAPHOS_RELATIVE_ORIENTATION_PROCESS_H
#define GRAPHOS_RELATIVE_ORIENTATION_PROCESS_H

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

class RelativeOrientationAlgorithm;

class GRAPHOS_EXPORT RelativeOrientationProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  RelativeOrientationProcess(std::shared_ptr<RelativeOrientationAlgorithm> &orientationAlgorithm);
  ~RelativeOrientationProcess() override;

signals:

  void orientationFinished();

// ProcessConcurrent interface

protected:

  void run() override;

private:

  std::shared_ptr<RelativeOrientationAlgorithm> mRelativeOrientationAlgorithm;

};

} // namespace graphos

#endif // GRAPHOS_RELATIVE_ORIENTATION_PROCESS_H
