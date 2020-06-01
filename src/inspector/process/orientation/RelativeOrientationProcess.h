#ifndef INSPECTOR_RELATIVE_ORIENTATION_PROCESS_H
#define INSPECTOR_RELATIVE_ORIENTATION_PROCESS_H

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

class OrientationProcess;

class INSPECTOR_EXPORT RelativeOrientationProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  RelativeOrientationProcess(std::shared_ptr<OrientationProcess> &orientationProcess);
  ~RelativeOrientationProcess() override;

signals:

  void orientationFinished();

// ProcessConcurrent interface

protected:

  void run() override;

private:

  std::shared_ptr<OrientationProcess> mOrientationProcess;

};

} // namespace inspector

#endif // INSPECTOR_RELATIVE_ORIENTATION_PROCESS_H
