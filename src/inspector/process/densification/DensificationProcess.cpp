#include "DensificationProcess.h"

#include <tidop/core/messages.h>


namespace inspector
{

DensificationProcess::DensificationProcess(const std::shared_ptr<Densifier> &densifier,
                                           const QString &reconstructionPath,
                                           const QString &imagesPath,
                                           const QString &outputPath)
  : ProcessConcurrent(),
    mDensifier(densifier),
    mReconstructionPath(reconstructionPath),
    mImagesPath(imagesPath),
    mOutputPat(outputPath)
{

}

void DensificationProcess::run()
{
  try {

    msgInfo("Starting Densification");

    tl::Chrono chrono;
    chrono.run();

    mDensifier->undistort(mReconstructionPath, mImagesPath, mOutputPat);

    mDensifier->densify(mOutputPat);

    uint64_t time = chrono.stop();
    msgInfo("[Time: %f seconds]", time/1000.);

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

QString DensificationProcess::imagesPath() const
{
  return mImagesPath;
}

void DensificationProcess::setImagesPath(const QString &imagesPath)
{
  mImagesPath = imagesPath;
}

QString DensificationProcess::reconstructionPath() const
{
  return mReconstructionPath;
}

void DensificationProcess::setReconstructionPath(const QString &reconstructionPath)
{
  mReconstructionPath = reconstructionPath;
}

} // inspector
