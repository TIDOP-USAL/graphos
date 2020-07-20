#ifndef INSPECTOR_LOAD_IMAGES_PROCESS_H
#define INSPECTOR_LOAD_IMAGES_PROCESS_H

#include "inspector/process/ProcessConcurrent.h"
#include "inspector/core/camera.h"
#include "inspector/core/image.h"

namespace inspector
{

class INSPECTOR_EXPORT LoadImagesProcess
  : public ProcessConcurrent
{

  Q_OBJECT

public:

  LoadImagesProcess(const QStringList &images, 
                    const std::vector<Camera> &cameras);

signals:

  void imageAdded(Image image, Camera camera);

private:

  bool existCamera(const QString &make, const QString &model) const;
  Camera findCamera(const QString &make, const QString &model) const;

// ProcessConcurrent interface

protected:

  void run() override;

protected:

  QStringList mImages;
  std::vector<Camera> mCameras;
};

} // inspector

#endif // LOADIMAGESPROCESS_H
