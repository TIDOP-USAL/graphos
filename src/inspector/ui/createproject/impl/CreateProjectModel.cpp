#include "CreateProjectModel.h"

#include "inspector/core/project.h"

namespace inspector
{

namespace ui
{

CreateProjectModelImp::CreateProjectModelImp(Project *project,
                                             QObject *parent)
  : CreateProjectModel(parent),
    mProject(project),
    mPrjFile(""),
    bUnsavedChanges(false)
{
  init();
}

CreateProjectModelImp::~CreateProjectModelImp()
{

}

QString CreateProjectModelImp::projectName() const
{
  return mProject->name();
}

QString CreateProjectModelImp::projectDescription() const
{
  return mProject->description();
}

QString CreateProjectModelImp::projectFolder() const
{
  return mProject->projectFolder();
}

QString CreateProjectModelImp::projectPath() const
{
  return mPrjFile;
}

void CreateProjectModelImp::setProjectName(const QString &name)
{
  mProject->setName(name);
  bUnsavedChanges = true;
}

void CreateProjectModelImp::setProjectDescription(const QString &description)
{
  mProject->setDescription(description);
  bUnsavedChanges = true;
}

void CreateProjectModelImp::setProjectFolder(const QString &dir)
{
  mProject->setProjectFolder(dir);
  bUnsavedChanges = true;
}

void CreateProjectModelImp::setDatabase(const QString &database)
{
  mProject->setDatabase(database);
  bUnsavedChanges = true;
}

void CreateProjectModelImp::saveAs(const QString &file)
{
  mPrjFile = file;
  mProject->save(file);

  bUnsavedChanges = false;
}

void CreateProjectModelImp::init()
{

}

void CreateProjectModelImp::clear()
{
  mProject->clear();
  mPrjFile = "";
}

} // namespace ui

} // namespace inspector
