#ifndef GRAPHOS_LOADER_MODEL_INTERFACE_H
#define GRAPHOS_LOADER_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

class Image;
class Camera;

namespace ui
{


class ImageLoaderModel
  : public Model
{

  Q_OBJECT

public:

  typedef std::vector<Image>::iterator image_iterator;
  typedef std::vector<Image>::const_iterator image_const_iterator;
  typedef std::map<int, Camera>::iterator camera_iterator;
  typedef std::map<int, Camera>::const_iterator camera_const_iterator;
  
public:

  ImageLoaderModel(QObject *parent = nullptr) : Model(parent) {}
  ~ImageLoaderModel() override = default;

  virtual QString projectCRS() const = 0;
  virtual void setProjectCRS(const QString &crs) = 0;

  virtual void addImage(Image &image) = 0;
  virtual size_t imageID(const QString &imageName) const = 0;
  virtual Image findImageByName(const QString &imageName) const = 0;
  virtual Image findImageById(size_t id) const = 0;
  virtual bool updateImage(size_t id, const Image &image) = 0;
  virtual bool removeImage(size_t id) = 0;
  virtual bool removeImage(const Image &image) = 0;
  virtual QString imagesDirectory() const = 0;

  virtual image_iterator begin() = 0;
  virtual image_const_iterator begin() const = 0;
  virtual image_iterator end() = 0;
  virtual image_const_iterator end() const = 0;
  
  virtual int addCamera(const Camera &camera) = 0;
  virtual int cameraID(const Camera &camera) const = 0;
  virtual int cameraID(const QString &make, 
                       const QString &model) const = 0;
  virtual camera_iterator cameraBegin() = 0;
  virtual camera_const_iterator cameraBegin() const = 0;
  virtual camera_iterator cameraEnd() = 0;
  virtual camera_const_iterator cameraEnd() const = 0;
};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_LOADER_MODEL_INTERFACE_H
