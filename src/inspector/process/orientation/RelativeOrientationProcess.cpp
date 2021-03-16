#include "RelativeOrientationProcess.h"

#include "inspector/core/orientation/orientationcolmap.h"
#include "inspector/core/orientation/orientationexport.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>

namespace inspector
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

    tl::Chrono chrono;
    chrono.run();

    mRelativeOrientationAlgorithm->run();

    emit orientationFinished();

    uint64_t time = chrono.stop();
    msgInfo("Orientation process finished [Time: %f seconds]", time/1000.);

  } catch (std::exception &e) {
    emit error(0, "Relative Orientation error");
    msgError(e.what());
  }
}

} // namespace inspector
