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

public:

  LoadImagesProcess(const QString &image);

  QString image() const;
  void setImage(const QString &image);

signals:

  void addCamera(Camera camera);
  void addImage(Image image);

// ProcessConcurrent interface

protected:

  void run() override;

protected:

  QString mImage;
};

} // inspector

#endif // LOADIMAGESPROCESS_H
