#ifndef INSPECTOR_MAINWINDOW_MODEL_H
#define INSPECTOR_MAINWINDOW_MODEL_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/project.h"
#include "inspector/core/image.h"

namespace inspector
{

namespace ui
{

class MainWindowModel
  : public IModel
{
  Q_OBJECT

public:

  typedef std::vector<Image>::iterator image_iterator;
  typedef std::vector<Image>::const_iterator image_const_iterator;

public:

  explicit MainWindowModel(Project *project);

  /*!
   * \brief Devuelve la ruta por defecto donde se situan los proyectos
   * \return
   */
  //QString defaultPath() const;

  std::list<std::pair<QString, QString>> exif(const QString &image) const;

  bool isPhotoOriented(const QString &imgName) const;
  CameraPose photoOrientation(const QString &imgName) const;

  Image findImageByName(const QString &imageName) const;
  size_t imageID(const QString &imageName) const;
  bool removeImage(size_t id);
  image_iterator imageBegin();
  image_const_iterator imageBegin() const;
  image_iterator imageEnd();
  image_const_iterator imageEnd() const;

signals:

public slots:


// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  QString mPrjDefaultPath;
  Project *mProject;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_MAINWINDOW_MODEL_H
