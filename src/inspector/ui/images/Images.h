#ifndef INSPECTOR_IMAGES_INTERFACES_H
#define INSPECTOR_IMAGES_INTERFACES_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/image.h"
#include "inspector/core/camera.h"
#include "inspector/ui/process/ProcessView.h"
#include "inspector/ui/process/ProcessPresenter.h"

#include <QFileDialog>

namespace inspector
{

namespace ui
{

class ImagesModel
  : public IModel
{

  Q_OBJECT

public:

  typedef std::vector<Image>::iterator image_iterator;
  typedef std::vector<Image>::const_iterator image_const_iterator;

public:

  ImagesModel(QObject *parent = nullptr) : IModel(parent) {}
  virtual ~ImagesModel() override = default;

  //virtual void addImage(const QString &imagePath, size_t cameraId) = 0;
  virtual void addImage(Image &image) = 0;
  virtual size_t imageID(const QString &imageName) const = 0;
  virtual Image findImageByName(const QString &imageName) const = 0;
  virtual Image findImageById(size_t id) const = 0;
  virtual bool updateImage(size_t id, const Image &image) = 0;
  virtual bool removeImage(size_t id) = 0;
  virtual bool removeImage(const Image &image) = 0;
  virtual QString imageDirectory() const = 0;

  virtual image_iterator begin() = 0;
  virtual image_const_iterator begin() const = 0;
  virtual image_iterator end() = 0;
  virtual image_const_iterator end() const = 0;

};


//class ImagesView
//  : public ProgressDialogImp
//{
//
//  Q_OBJECT
//
//public:
//
//  ImagesView(QWidget *parent) : ProgressDialogImp(parent) {}
//  virtual ~ImagesView() {}
//};

class ImagesView
  : public QFileDialog
{
  Q_OBJECT

public:

  ImagesView(QWidget *parent) : QFileDialog(parent) {}
  ~ImagesView() override = default;

  virtual void setImageDirectory(const QString &directory) = 0;

};

class ImagesPresenter
  : public ProcessPresenter
{

  Q_OBJECT

public:

  ImagesPresenter(){}
  virtual ~ImagesPresenter() override = default;

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

#endif // INSPECTOR_IMAGES_INTERFACES_H
