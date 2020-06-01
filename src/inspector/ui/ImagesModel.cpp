#include "ImagesModel.h"

#include <tidop/core/messages.h>

#include <QFileInfo>
#include <QDir>

namespace inspector
{

namespace ui
{

ImagesModelImp::ImagesModelImp(Project *project,
                               QObject *parent)
  : ImagesModel(parent),
    mProject(project)
{
  init();
}

void ImagesModelImp::addImage(Image &image)
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

size_t ImagesModelImp::imageID(const QString &imageName) const
{
  return mProject->imageId(imageName);
}

Image ImagesModelImp::findImageByName(const QString &imageName) const
{
  return mProject->findImageByName(imageName);
}

Image ImagesModelImp::findImageById(size_t id) const
{
  return mProject->findImageById(id);
}

bool ImagesModelImp::updateImage(size_t id, const Image &image)
{
  return mProject->updateImage(id, image);
}

bool ImagesModelImp::removeImage(size_t id)
{
  return mProject->removeImage(id);
}

bool ImagesModelImp::removeImage(const Image &image)
{
  size_t id_image = imageID(image.name());
  return mProject->removeImage(id_image);
}

ImagesModel::image_iterator ImagesModelImp::begin()
{
  return mProject->imageBegin();
}

ImagesModel::image_const_iterator ImagesModelImp::begin() const
{
  return mProject->imageBegin();
}

ImagesModel::image_iterator ImagesModelImp::end()
{
  return mProject->imageEnd();
}

ImagesModel::image_const_iterator ImagesModelImp::end() const
{
  return mProject->imageEnd();
}

void ImagesModelImp::init()
{
}

void ImagesModelImp::clear()
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
