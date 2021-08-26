#ifndef GRAPHOS_LOAD_IMAGES_PROCESS_H
#define GRAPHOS_LOAD_IMAGES_PROCESS_H

#include <tidop/geospatial/camera.h>

#include "graphos/process/ProcessConcurrent.h"
#include "graphos/core/image.h"

namespace graphos
{

class GRAPHOS_EXPORT LoadImagesProcess
  : public ProcessConcurrent
{

  Q_OBJECT

public:

  LoadImagesProcess(std::vector<Image> *images, 
                    std::vector<tl::Camera> *cameras,
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
  std::vector<tl::Camera> *mCameras;
  QString mEPSG;
};

} // graphos

#endif // LOADIMAGESPROCESS_H
