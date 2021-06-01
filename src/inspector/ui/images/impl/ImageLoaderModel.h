#ifndef INSPECTOR_IMAGE_LOADER_MODEL_H
#define INSPECTOR_IMAGE_LOADER_MODEL_H

#include "inspector/ui/images/ImageLoaderModel.h"

namespace inspector
{

class Project;

namespace ui
{


class ImageLoaderModelImp
  : public ImageLoaderModel
{

  Q_OBJECT

public:

  ImageLoaderModelImp(Project *project,
                      QObject *parent = nullptr);
  ~ImageLoaderModelImp() override = default;

// ImagesModel interface

public:

  void addImage(Image &image) override;
  size_t imageID(const QString &imageName) const override;
  Image findImageByName(const QString &imageName) const override;
  Image findImageById(size_t id) const override;
  bool updateImage(size_t id, const Image &image) override;
  bool removeImage(size_t id) override;
  bool removeImage(const Image &image) override;
  QString imagesDirectory() const override;

  image_iterator begin() override;
  image_const_iterator begin() const override;
  image_iterator end() override;
  image_const_iterator end() const override;

  int addCamera(const Camera &camera) override;
  int cameraID(const Camera &camera) const override;
  int cameraID(const QString &make, 
               const QString &model) const override;
  camera_iterator cameraBegin() override;
  camera_const_iterator cameraBegin() const override;
  camera_iterator cameraEnd() override;
  camera_const_iterator cameraEnd() const override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_IMAGES_MODEL_H
