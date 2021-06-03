#ifndef INSPECTOR_NEW_PROJECT_INTERFACES_H
#define INSPECTOR_NEW_PROJECT_INTERFACES_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{
	
/*!
 * \brief INewProjectView
 */
class NewProjectView
  : public DialogView
{

  Q_OBJECT

public:

  NewProjectView(QWidget *parent) : DialogView(parent) {}
  virtual ~NewProjectView() override = default;

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


/*!
 * \brief The INewProjectPresenter class
 */
class NewProjectPresenter
  : public Presenter
{

  Q_OBJECT

public:

  NewProjectPresenter(){}

  virtual ~NewProjectPresenter() override = default;

signals:

  /*!
   * \brief Señal que se emite cuando se crea el proyecto
   */
  void projectCreate();

protected slots:

  /*!
   * \brief Guarda el proyecto
   */
  virtual void saveProject() = 0;

  /*!
   * \brief Descarta los cambios en el proyecto
   */
  virtual void discartProject() = 0;

  /*!
   * \brief Comprueba si ya existe un proyecto con el mismo nombre
   */
  virtual void checkProjectName() const = 0;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_NEW_PROJECT_INTERFACES_H
