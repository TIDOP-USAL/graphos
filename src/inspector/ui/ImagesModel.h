#ifndef INSPECTOR_IMAGES_MODEL_H
#define INSPECTOR_IMAGES_MODEL_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/project.h"

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

  virtual void addImage(const QString &imagePath, size_t cameraId) = 0;
  virtual void addImage(Image &image) = 0;
  virtual size_t imageID(const QString &imageName) const = 0;
  virtual Image findImageByName(const QString &imageName) const = 0;
  virtual Image findImageById(size_t id) const = 0;
  virtual bool updateImage(size_t id, const Image &image) = 0;
  virtual bool removeImage(size_t id) = 0;
  virtual bool removeImage(const Image &image) = 0;

  virtual image_iterator begin() = 0;
  virtual image_const_iterator begin() const = 0;
  virtual image_iterator end() = 0;
  virtual image_const_iterator end() const = 0;

};



class ImagesModelImp
  : public ImagesModel
{

  Q_OBJECT

public:

  ImagesModelImp(Project *project,
                 QObject *parent = nullptr);
  ~ImagesModelImp() override = default;

// ImagesModel interface

public:

  void addImage(const QString &imagePath, size_t cameraId) override;
  void addImage(Image &image) override;
  size_t imageID(const QString &imageName) const override;
  Image findImageByName(const QString &imageName) const override;
  Image findImageById(size_t id) const override;
  bool updateImage(size_t id, const Image &image) override;
  bool removeImage(size_t id) override;
  bool removeImage(const Image &image) override;

  image_iterator begin() override;
  image_const_iterator begin() const override;
  image_iterator end() override;
  image_const_iterator end() const override;

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
