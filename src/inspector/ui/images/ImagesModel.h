#ifndef INSPECTOR_IMAGES_MODEL_H
#define INSPECTOR_IMAGES_MODEL_H

#include "inspector/ui/images/Images.h"
#include "inspector/core/project.h"

namespace inspector
{

namespace ui
{


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

  void addImage(Image &image) override;
  size_t imageID(const QString &imageName) const override;
  Image findImageByName(const QString &imageName) const override;
  Image findImageById(size_t id) const override;
  bool updateImage(size_t id, const Image &image) override;
  bool removeImage(size_t id) override;
  bool removeImage(const Image &image) override;
  QString imageDirectory() const override;

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
