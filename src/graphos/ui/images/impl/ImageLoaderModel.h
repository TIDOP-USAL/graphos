#ifndef GRAPHOS_LOADER_MODEL_H
#define GRAPHOS_LOADER_MODEL_H

#include "graphos/ui/images/ImageLoaderModel.h"

namespace graphos
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

  QString projectCRS() const override;
  void setProjectCRS(const QString &crs) override;

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

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
};


} // namespace ui

} // namespace graphos

#endif // GRAPHOSS_MODEL_H
