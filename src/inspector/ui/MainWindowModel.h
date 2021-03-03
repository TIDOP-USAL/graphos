#ifndef INSPECTOR_MAINWINDOW_MODEL_H
#define INSPECTOR_MAINWINDOW_MODEL_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/project.h"

namespace inspector
{

namespace ui
{

class MainWindowModel
  : public IModel
{
  Q_OBJECT

public:

  explicit MainWindowModel(Project *project);

  /*!
   * \brief Devuelve la ruta por defecto donde se situan los proyectos
   * \return
   */
  //QString defaultPath() const;

  std::list<std::pair<QString, QString>> exif(const QString &image) const;

  bool isPhotoOriented(const QString &imgName) const;
  PhotoOrientation photoOrientation(const QString &imgName) const;

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
