#include "AbsoluteOrientationProcess.h"

#include "inspector/core/orientation/orientationcolmap.h"
#include "inspector/core/orientation/orientationexport.h"

#include <tidop/core/messages.h>

#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/feature/sift.h>
#include <colmap/feature/matching.h>
#include <colmap/controllers/hierarchical_mapper.h>

namespace inspector
{

AbsoluteOrientationProcess::AbsoluteOrientationProcess(std::shared_ptr<AbsoluteOrientationAlgorithm> &orientationAlgorithm)
  : ProcessConcurrent(),
    mAbsoluteOrientationAlgorithm(orientationAlgorithm)
{

}

AbsoluteOrientationProcess::~AbsoluteOrientationProcess()
{
}

void AbsoluteOrientationProcess::run()
{
  try {

    msgInfo("Starting Orientation");

    tl::Chrono chrono;
    chrono.run();

    mAbsoluteOrientationAlgorithm->run();

    emit absoluteOrientationFinished();

    uint64_t time = chrono.stop();
    msgInfo("Absolute Orientation process finished [Time: %f seconds]", time/1000.);

  } catch (std::exception &e) {
    emit error(0, "Absolute Orientation error");
    msgError(e.what());
  }
}

} // namespace inspector
