#ifndef INSPECTOR_ABSOLUTE_ORIENTATION_PROCESS_H
#define INSPECTOR_ABSOLUTE_ORIENTATION_PROCESS_H

#include "inspector/process/ProcessConcurrent.h"

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

class INSPECTOR_EXPORT AbsoluteOrientationProcess
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

} // namespace inspector

#endif // INSPECTOR_ABSOLUTE_ORIENTATION_PROCESS_H
