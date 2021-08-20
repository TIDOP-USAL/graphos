#ifndef GRAPHOS_LOADER_PRESENTER_INTERFACE_H
#define GRAPHOS_LOADER_PRESENTER_INTERFACE_H

#include "graphos/ui/process/ProcessPresenter.h"

namespace graphos
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

} // namespace graphos

#endif // GRAPHOS_LOADER_PRESENTER_INTERFACE_H
