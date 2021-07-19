#ifndef INSPECTOR_CREATE_PROJECT_VIEW_INSPECTOR_H
#define INSPECTOR_CREATE_PROJECT_VIEW_INSPECTOR_H

#include "inspector/interfaces/mvp.h"


namespace inspector
{

namespace ui
{

class CreateProjectView
  : public DialogView
{

  Q_OBJECT

public:

  CreateProjectView(QWidget *parent) : DialogView(parent) {}
  virtual ~CreateProjectView() override = default;

  /*!
   * \brief Devuelve el nombre del proyecto
   * \return Nombre del proyecto
   */
  virtual QString projectName() const = 0;

  /*!
   * \brief Devuelve la ruta del proyecto
   * \return Ruta del proyecto
   */
  virtual QString projectPath() const = 0;

  /*!
   * \brief Establece la ruta del proyecto
   * \param path Ruta del proyecto
   */
  virtual void setProjectPath(const QString &path) = 0;

  /*!
   * \brief Devuelve la descripción del proyecto
   * \return Descripción del proyecto
   */
  virtual QString projectDescription() const = 0;

  virtual bool createProjectFolderEnable() const = 0;
  virtual void setExistingProject(bool prjExist) = 0;

protected slots:

  virtual void onClickButtonSelectPath() = 0;

signals:

  void projectNameChange();
};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_CREATE_PROJECT_VIEW_INSPECTOR_H
