#ifndef GRAPHOS_DENSIFICATION_PROCESS_H
#define GRAPHOS_DENSIFICATION_PROCESS_H

#include "graphos/process/ProcessConcurrent.h"
#include "graphos/core/densification/densification.h"



namespace graphos
{

class GRAPHOS_EXPORT DensificationProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  DensificationProcess(const std::shared_ptr<Densifier> &densifier,
                       const QString &reconstructionPath,
                       const QString &outputPath);
  ~DensificationProcess() override = default;

  QString reconstructionPath() const;
  void setReconstructionPath(const QString &reconstructionPath);


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
  QString mOutputPat;
};

} // namespace graphos

#endif // GRAPHOS_DENSIFICATION_PROCESS_H
