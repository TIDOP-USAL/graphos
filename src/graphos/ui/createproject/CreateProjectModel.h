#ifndef GRAPHOS_CREATE_PROJECT_MODEL_INTERFACE_H
#define GRAPHOS_CREATE_PROJECT_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

namespace ui
{

class CreateProjectModel
  : public Model
{

  Q_OBJECT

public:

  CreateProjectModel(QObject *parent = nullptr) : Model(parent) {}
  ~CreateProjectModel() override = default;

  /*!
   * \brief Returns the name of the project
   * \return Project name
   */
  virtual QString projectName() const = 0;

  /*!
   * \brief Returns the project description
   * \return Project description
   */
  virtual QString projectDescription() const = 0;

  /*!
   * \brief Returns the project directory
   * \return Project directory
   */
  virtual QString projectFolder() const = 0;

  /*!
   * \brief Returns the project file path
   * \return Project directory
   */
  virtual QString projectPath() const = 0;

public slots:

  /*!
   * \brief Set the project name
   * \param[in] name Project's name
   */
  virtual void setProjectName(const QString &name) = 0;

  /*!
   * \brief Set the project description
   * \param[in] description Project description
   */
  virtual void setProjectDescription(const QString &projectDescription) = 0;

  /*!
   * \brief Set the project directory
   * \param[in] path Project Directory
   */
  virtual void setProjectFolder(const QString &dir) = 0;

  virtual void setDatabase(const QString &database) = 0;

  /*!
   * \brief Save a project as ...
   * \param[in] file Project file
   */
  virtual void saveAs(const QString &file) = 0;

};



} // namespace ui

} // namespace photomatch

#endif // GRAPHOS_CREATE_PROJECT_MODEL_INTERFACE_H

