#ifndef INSPECTOR_IMAGE_LOADER_PRESENTER_INTERFACE_H
#define INSPECTOR_IMAGE_LOADER_PRESENTER_INTERFACE_H

#include "inspector/ui/process/ProcessPresenter.h"

namespace inspector
{

namespace ui
{


class ImageLoaderPresenter
  : public ProcessPresenter
{

  Q_OBJECT

public:

  ImageLoaderPresenter(){}
  virtual ~ImageLoaderPresenter() override = default;

public slots:

  virtual void setImages(const QStringList &files) = 0;

protected slots:

  virtual void addImage(int imageId, int cameraId) = 0;

signals:

  void loadingImages(bool);
  void imageLoaded(QString);
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_IMAGE_LOADER_PRESENTER_INTERFACE_H
