#include "ProjectModel.h"

namespace inspector
{

namespace ui
{

ProjectModelImp::ProjectModelImp(ProjectController *projectIO, Project *project, QObject *parent)
  : ProjectModel(parent),
    mProjectController(projectIO),
    mProject(project),
    mPrjFile(""),
    bUnsavedChanges(false)
{
  init();
}

ProjectModelImp::~ProjectModelImp()
{

}

QString ProjectModelImp::projectName() const
{
  return mProject->name();
}

QString ProjectModelImp::projectDescription() const
{
  return mProject->description();
}

QString ProjectModelImp::projectFolder() const
{
  return mProject->projectFolder();
}

QString ProjectModelImp::projectPath() const
{
  return mPrjFile;
}

QString ProjectModelImp::version() const
{
  return mProject->version();
}

bool ProjectModelImp::checkUnsavedChanges() const
{
  return bUnsavedChanges;
}

bool ProjectModelImp::checkOldVersion(const QString &file) const
{
  return mProjectController->checkOldVersion(file);
}

void ProjectModelImp::oldVersionBackup(const QString &file) const
{
  mProjectController->oldVersionBak(file);
}

void ProjectModelImp::setProjectName(const QString &name)
{
  mProject->setName(name);
  bUnsavedChanges = true;
}

void ProjectModelImp::setProjectDescription(const QString &description)
{
  mProject->setDescription(description);
  bUnsavedChanges = true;
}

void ProjectModelImp::setProjectFolder(const QString &dir)
{
  mProject->setProjectFolder(dir);
  bUnsavedChanges = true;
}

void ProjectModelImp::load(const QString &file)
{
  mPrjFile = file;
  mProjectController->read(file, *mProject);
  bUnsavedChanges = false;
}

void ProjectModelImp::save()
{
  saveAs(mPrjFile);
}

void ProjectModelImp::saveAs(const QString &file)
{
  mPrjFile = file;
  mProjectController->write(file, *mProject);

  bUnsavedChanges = false;
}

void ProjectModelImp::init()
{

}

void ProjectModelImp::clear()
{
  mProject->clear();
  mPrjFile = "";
}

} // namespace ui

} // namespace photomatch
