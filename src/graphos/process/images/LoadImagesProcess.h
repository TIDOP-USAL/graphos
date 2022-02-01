#ifndef GRAPHOS_LOAD_IMAGES_PROCESS_H
#define GRAPHOS_LOAD_IMAGES_PROCESS_H

#include "graphos/process/ProcessConcurrent.h"
#include "graphos/core/image.h"

namespace tl
{
class ImageReader;

namespace geospatial
{
class Crs;
}

}

namespace graphos
{

//class DatabaseCameras;
class Camera;

class GRAPHOS_EXPORT LoadImagesProcess
  : public ProcessConcurrent
{

  Q_OBJECT

public:

  LoadImagesProcess(std::vector<Image> *images, 
                    std::vector<Camera> *cameras,
                    const QString &epsg = QString());
  ~LoadImagesProcess() override;

signals:

  void imageAdded(int, int);

private:

  bool existCamera(const QString &make, const QString &model) const;
  int findCamera(const QString &make, const QString &model) const;
  void loadImage(size_t imageId);
  int loadCamera(tl::ImageReader *imageReader);

// ProcessConcurrent interface

protected:

  void run() override;

  double parseFocal(const std::string &focal, double def);

protected:

  std::vector<Image> *mImages;
  std::vector<Camera> *mCameras;
  QString mEPSG;
  std::shared_ptr<tl::geospatial::Crs> mCrsIn;
  std::shared_ptr<tl::geospatial::Crs> mCrsOut;
  //DatabaseCameras *mDatabaseCameras;
  QString mDatabaseCamerasPath;
};

} // graphos

#endif // LOADIMAGESPROCESS_H
