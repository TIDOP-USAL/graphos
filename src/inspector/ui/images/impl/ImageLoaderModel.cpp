#include "ImageLoaderModel.h"

#include <tidop/core/messages.h>

#include "inspector/core/project.h"

#include <QFileInfo>
#include <QDir>

namespace inspector
{

namespace ui
{

ImageLoaderModelImp::ImageLoaderModelImp(Project *project,
                                         QObject *parent)
  : ImageLoaderModel(parent),
    mProject(project)
{
  init();
}

void ImageLoaderModelImp::addImage(Image &image)
{
  QString image_directory = mProject->imageDirectory();
  QFileInfo file(image.path());
  if (image_directory.isEmpty()){
    image_directory = file.dir().absolutePath();
    mProject->setImageDirectory(image_directory);
  } else {
    QString current_image_directory = file.dir().absolutePath();
    if (current_image_directory.compare(image_directory) != 0) {
      throw std::runtime_error(std::string("image"));
    }
  }

  mProject->addImage(image);
}

size_t ImageLoaderModelImp::imageID(const QString &imageName) const
{
  return mProject->imageId(imageName);
}

Image ImageLoaderModelImp::findImageByName(const QString &imageName) const
{
  return mProject->findImageByName(imageName);
}

Image ImageLoaderModelImp::findImageById(size_t id) const
{
  return mProject->findImageById(id);
}

bool ImageLoaderModelImp::updateImage(size_t id, const Image &image)
{
  return mProject->updateImage(id, image);
}

bool ImageLoaderModelImp::removeImage(size_t id)
{
  return mProject->removeImage(id);
}

bool ImageLoaderModelImp::removeImage(const Image &image)
{
  size_t id_image = imageID(image.name());
  return mProject->removeImage(id_image);
}

QString ImageLoaderModelImp::imageDirectory() const
{
  QString image_directory = mProject->imageDirectory();
  if (image_directory.isEmpty()) {
    image_directory = mProject->projectFolder();
  }
  return image_directory;
}

ImageLoaderModel::image_iterator ImageLoaderModelImp::begin()
{
  return mProject->imageBegin();
}

ImageLoaderModel::image_const_iterator ImageLoaderModelImp::begin() const
{
  return mProject->imageBegin();
}

ImageLoaderModel::image_iterator ImageLoaderModelImp::end()
{
  return mProject->imageEnd();
}

ImageLoaderModel::image_const_iterator ImageLoaderModelImp::end() const
{
  return mProject->imageEnd();
}

int ImageLoaderModelImp::addCamera(const Camera &camera)
{
  return mProject->addCamera(camera);
}

int ImageLoaderModelImp::cameraID(const Camera &camera) const
{
  return cameraID(camera.make(), camera.model());
}

int ImageLoaderModelImp::cameraID(const QString &make, 
                                  const QString &model) const
{
  int id_camera = 0;
  for (auto it = cameraBegin(); it != cameraEnd(); it++){
    QString camera_make = it->second.make();
    QString camera_model = it->second.model();
    if (make.compare(camera_make) == 0 &&
        model.compare(camera_model) == 0){
      id_camera = it->first;
      break;
    }
  }
  return id_camera;
}

ImageLoaderModel::camera_iterator ImageLoaderModelImp::cameraBegin()
{
  return mProject->cameraBegin();
}

ImageLoaderModel::camera_const_iterator ImageLoaderModelImp::cameraBegin() const
{
  return mProject->cameraBegin();
}

ImageLoaderModel::camera_iterator ImageLoaderModelImp::cameraEnd()
{
  return mProject->cameraEnd();
}

ImageLoaderModel::camera_const_iterator ImageLoaderModelImp::cameraEnd() const
{
  return mProject->cameraEnd();
}

void ImageLoaderModelImp::init()
{
}

void ImageLoaderModelImp::clear()
{
  mProject->imagesCount();
  for (auto it = begin(); it != end();){
    Image image = *it;
    removeImage(image);
    it = begin();
  }
}

} // namespace ui

} // namespace inspector
