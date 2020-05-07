#ifndef INSPECTOR_CREATE_PROJECT_INTERFACES_H
#define INSPECTOR_CREATE_PROJECT_INTERFACES_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{


class CreateProjectModel
  : public IModel
{

  Q_OBJECT

public:

  CreateProjectModel() {}

  ~CreateProjectModel() override = default;
  
  /*!
   * \brief Se crea un nuevo proyecto
   */
  virtual void create() = 0;

  /*!
   * \brief Se guarda el proyecto
   */
  virtual void save(const QString &file) = 0;

  /*!
   * \brief Establece el nombre del proyecto
   * \param name Nombre
   */
  virtual void setProjectName(const QString &name) = 0;

  /*!
   * \brief Establece la ruta del proyecto
   * \param path Ruta del proyecto
   */
  virtual void setProjectPath(const QString &path) = 0;

  /*!
   * \brief Establece la descripción del proyecto
   * \param description Descripción del proyecto
   */
  virtual void setProjectDescription(const QString &description) = 0;

  virtual void setDatabase(const QString &database) = 0;


// IModel interface

private:

  void init() override
  {
  }

public slots:

  void clear() override
  {
  }
};



class CreateProjectView
  : public IDialogView
{

  Q_OBJECT

public:

  CreateProjectView(QWidget *parent) : IDialogView(parent) {}
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

  //virtual bool createProjectFolder() const = 0;

//protected slots:

  //virtual void onClickButtonSelectPath() = 0;

};



class CreateProjectPresenter
  : public IPresenter
{

  Q_OBJECT

public:

  CreateProjectPresenter(){}

  virtual ~CreateProjectPresenter() override = default;

signals:

  /*!
   * \brief Señal que se emite cuando se crea el proyecto
   */
  void projectCreated();

protected slots:

  /*!
   * \brief Guarda el proyecto
   */
  virtual void saveProject() = 0;

  /*!
   * \brief Descarta los cambios en el proyecto
   */
  virtual void discartProject() = 0;

};

} // namespace inspector

#endif // INSPECTOR_CREATE_PROJECT_INTERFACES_H
