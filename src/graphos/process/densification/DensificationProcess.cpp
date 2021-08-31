#include "DensificationProcess.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>

namespace graphos
{

DensificationProcess::DensificationProcess(const std::shared_ptr<Densifier> &densifier,
                                           const QString &reconstructionPath,
                                           const QString &outputPath)
  : ProcessConcurrent(),
    mDensifier(densifier),
    mReconstructionPath(reconstructionPath),
    mOutputPat(outputPath)
{

}

void DensificationProcess::run()
{
  try {

    msgInfo("Starting Densification");

    tl::Chrono chrono;
    chrono.run();

    mDensifier->undistort(mReconstructionPath, mOutputPat);

    mDensifier->densify(mOutputPat);

    double time = chrono.stop();
    msgInfo("[Time: %f seconds]", time);

    emit densificationFinished();

    msgInfo("Densification finished.");

  } catch (std::exception &e) {
    emit error(0, "Densification error");
    msgError(e.what());
  } catch (...) {
    emit error(0, "Densification error");
    msgError("Unhandled exception");
  }
}

QString DensificationProcess::outputPat() const
{
  return mOutputPat;
}

void DensificationProcess::setOutputPat(const QString &outputPat)
{
  mOutputPat = outputPat;
}


QString DensificationProcess::reconstructionPath() const
{
  return mReconstructionPath;
}

void DensificationProcess::setReconstructionPath(const QString &reconstructionPath)
{
  mReconstructionPath = reconstructionPath;
}

} // graphos
