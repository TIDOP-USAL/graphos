#include "AbsoluteOrientationProcess.h"

#include "graphos/core/orientation/orientationcolmap.h"
#include "graphos/core/orientation/orientationexport.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>

namespace graphos
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

    msgInfo("Starting Absolute Orientation");

    tl::Chrono chrono("Absolute Orientation process finished");
    chrono.run();

    mAbsoluteOrientationAlgorithm->run();

    emit absoluteOrientationFinished();

    chrono.stop();

  } catch (std::exception &e) {
    emit error(0, "Absolute Orientation error");
    msgError(e.what());
  } catch (...) {
    emit error(0, "Absolute Orientation error");
    msgError("Unknow Exception");
  }
}

} // namespace graphos
