#ifndef INSPECTOR_PROJECT_MODEL_H
#define INSPECTOR_PROJECT_MODEL_H

#include <QObject>

#include "inspector/core/project.h"
#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class ProjectModel
  : public IModel
{

  Q_OBJECT

public:

  ProjectModel(QObject *parent = nullptr) : IModel(parent) {}
  ~ProjectModel() override = default;

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

  /*!
   * \brief Project File Version
   * The project file version is used to update old projects
   * \return Project File Version
   */
  virtual QString version() const = 0;

  /*!
   * \brief Check for changes without saving in the project
   * \return
   */
  virtual bool checkUnsavedChanges() const = 0;

  /*!
   * \brief Check if a project is an old version
   * \param[in] file Project file
   * \return
   */
  virtual bool checkOldVersion(const QString &file) const = 0;

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

  /*!
   * \brief Load a project
   * \param[in] file Project file
   */
  virtual void load(const QString &file) = 0;

  /*!
   * \brief Save the project
   */
  virtual void save() = 0;

  /*!
   * \brief Save a project as ...
   * \param[in] file Project file
   */
  virtual void saveAs(const QString &file) = 0;

  /*!
   * \brief Create a copy of a project from an old version with the same name and a suffix with the version
   * \param[in] file Project file
   */
  virtual void oldVersionBackup(const QString &file) const = 0;

};



class ProjectModelImp
  : public ProjectModel
{

  Q_OBJECT

public:

  explicit ProjectModelImp(ProjectController *projectIO,
                           Project *project,
                           QObject *parent = nullptr);
  ~ProjectModelImp() override;

signals:



// IProject interface

public:

  QString projectName() const override;
  QString projectDescription() const override;
  QString projectFolder() const override;
  QString projectPath() const override;
  QString version() const override;
  bool checkUnsavedChanges() const override;
  bool checkOldVersion(const QString &file) const override;
  void oldVersionBackup(const QString &file) const override;

public slots:

  void setProjectName(const QString &name) override;
  void setProjectDescription(const QString &projectDescription) override;
  void setProjectFolder(const QString &dir) override;
  void load(const QString &file) override;
  void save() override;
  void saveAs(const QString &file) override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  ProjectController *mProjectController;
  Project *mProject;
  QString mPrjFile;
  bool bUnsavedChanges;

};

} // namespace ui

} // namespace photomatch

#endif // PHOTOMATCH_PROJECT_MODEL_H

