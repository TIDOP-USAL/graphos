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

  LoadImagesProcess(std::vector<Image> *images, 
                    std::vector<Camera> *cameras,
                    const QString &epsg = QString());

signals:

  void imageAdded(int, int);

private:

  bool existCamera(const QString &make, const QString &model) const;
  int findCamera(const QString &make, const QString &model) const;

// ProcessConcurrent interface

protected:

  void run() override;

  double parseFocal(const std::string &focal, double def);

protected:

  std::vector<Image> *mImages;
  std::vector<Camera> *mCameras;
  QString mEPSG;
};

} // inspector

#endif // LOADIMAGESPROCESS_H
