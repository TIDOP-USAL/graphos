#include "RelativeOrientationProcess.h"

#include "graphos/core/orientation/orientationcolmap.h"
#include "graphos/core/orientation/orientationexport.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>

namespace graphos
{

RelativeOrientationProcess::RelativeOrientationProcess(std::shared_ptr<RelativeOrientationAlgorithm> &orientationAlgorithm)
  : ProcessConcurrent(),
    mRelativeOrientationAlgorithm(orientationAlgorithm)
{

}

RelativeOrientationProcess::~RelativeOrientationProcess()
{
}

void RelativeOrientationProcess::run()
{
  try {

    msgInfo("Starting Orientation");

    tl::Chrono chrono("Orientation process finished");
    chrono.run();

    mRelativeOrientationAlgorithm->run();

    emit orientationFinished();

    chrono.stop();

  } catch (std::exception &e) {
    emit error(0, "Relative Orientation error");
    msgError(e.what());
  } catch (...) {
    emit error(0, "Relative Orientation error");
    msgError("Unknow Exception");
  }
}

} // namespace graphos
