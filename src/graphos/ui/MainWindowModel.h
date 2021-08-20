#ifndef GRAPHOS_MAINWINDOW_MODEL_H
#define GRAPHOS_MAINWINDOW_MODEL_H

#include "graphos/interfaces/mvp.h"
#include "graphos/core/project.h"
#include "graphos/core/image.h"

namespace graphos
{

namespace ui
{

class MainWindowModel
  : public Model
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
  //bool removeImage(size_t id);
  bool removeImage(const QString &imageName);
  void removeImages(const QStringList &images);
  image_iterator imageBegin();
  image_const_iterator imageBegin() const;
  image_iterator imageEnd();
  image_const_iterator imageEnd() const;

signals:

public slots:


// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  QString mPrjDefaultPath;
  Project *mProject;
};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_MAINWINDOW_MODEL_H
