#ifndef INSPECTOR_CREATE_PROJECT_MODEL_INTERFACE_H
#define INSPECTOR_CREATE_PROJECT_MODEL_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class CreateProjectModel
  : public IModel
{

  Q_OBJECT

public:

  CreateProjectModel(QObject *parent = nullptr) : IModel(parent) {}
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

//  /*!
//   * \brief Project File Version
//   * The project file version is used to update old projects
//   * \return Project File Version
//   */
//  virtual QString version() const = 0;
//
//  /*!
//   * \brief Check for changes without saving in the project
//   * \return
//   */
//  virtual bool checkUnsavedChanges() const = 0;
//
//  /*!
//   * \brief Check if a project is an old version
//   * \param[in] file Project file
//   * \return
//   */
//  virtual bool checkOldVersion(const QString &file) const = 0;
//
//  virtual QString sparseModel() const = 0;
//  virtual bool isAbsoluteOriented() const = 0;
//  virtual QString denseModel() const = 0;

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

//  virtual void setSparseModel(const QString &sparseModel) = 0;
//  virtual void setDenseModel(const QString &denseModel) = 0;
//
//  /*!
//   * \brief Load a project
//   * \param[in] file Project file
//   */
//  virtual void load(const QString &file) = 0;
//
//  /*!
//   * \brief Save the project
//   */
//  virtual void save() = 0;

  /*!
   * \brief Save a project as ...
   * \param[in] file Project file
   */
  virtual void saveAs(const QString &file) = 0;

//  /*!
//   * \brief Create a copy of a project from an old version with the same name and a suffix with the version
//   * \param[in] file Project file
//   */
//  virtual void oldVersionBackup(const QString &file) const = 0;

};



} // namespace ui

} // namespace photomatch

#endif // INSPECTOR_CREATE_PROJECT_MODEL_INTERFACE_H

