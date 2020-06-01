#include "RelativeOrientationProcess.h"

#include "inspector/core/orientation/orientationcolmap.h"

#include <tidop/core/messages.h>

#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/feature/sift.h>
#include <colmap/feature/matching.h>
#include <colmap/controllers/hierarchical_mapper.h>

namespace inspector
{

RelativeOrientationProcess::RelativeOrientationProcess(std::shared_ptr<OrientationProcess> &orientationProcess)
  : ProcessConcurrent(),
    mOrientationProcess(orientationProcess)
{

}

RelativeOrientationProcess::~RelativeOrientationProcess()
{
}

void RelativeOrientationProcess::run()
{
  tl::Chrono chrono;
  chrono.run();

  mOrientationProcess->run();

  emit orientationFinished();

  uint64_t time = chrono.stop();
  msgInfo("[Time: %f seconds]", time/1000.);
}

} // namespace inspector
