#include "OpenProjectModel.h"

#include "graphos/core/project.h"

#include <QStandardPaths>

namespace graphos
{

namespace ui
{

OpenProjectModelImp::OpenProjectModelImp(Project *project,
                                         QObject *parent)
  : OpenProjectModel(parent),
    mProject(project)
{
}

OpenProjectModelImp::~OpenProjectModelImp()
{
  init();
}

QString OpenProjectModelImp::inspectorDirectory() const
{
  QString inspector_directory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  inspector_directory.append("/graphos/Projects");
  return inspector_directory;
}

void OpenProjectModelImp::saveProject()
{
  mProject->save(mProject->projectPath());
}

void OpenProjectModelImp::loadProject(const QString &projectPath)
{
  mProject->load(projectPath);
}

void OpenProjectModelImp::init()
{
}

void OpenProjectModelImp::clear()
{
  mProject->clear();
}

} // namespace ui

} // namespace graphos