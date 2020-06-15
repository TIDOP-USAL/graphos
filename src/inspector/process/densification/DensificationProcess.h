#ifndef INSPECTOR_DENSIFICATION_PROCESS_H
#define INSPECTOR_DENSIFICATION_PROCESS_H

#include "inspector/process/ProcessConcurrent.h"
#include "inspector/core/densification/densification.h"



namespace inspector
{

class INSPECTOR_EXPORT DensificationProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  DensificationProcess(const std::shared_ptr<Densifier> &densifier,
                       const QString &reconstructionPath,
                       const QString &imagesPath,
                       const QString &outputPath);
  ~DensificationProcess() override = default;

  QString reconstructionPath() const;
  void setReconstructionPath(const QString &reconstructionPath);

  QString imagesPath() const;
  void setImagesPath(const QString &imagesPath);

  QString outputPat() const;
  void setOutputPat(const QString &outputPat);

signals:

  void densificationFinished();

  // ProcessConcurrent interface

protected:

  void run() override;

private:

  std::shared_ptr<Densifier> mDensifier;
  QString mReconstructionPath;
  QString mImagesPath;
  QString mOutputPat;
};

} // namespace inspector

#endif // INSPECTOR_DENSIFICATION_PROCESS_H
