#include "ExportOrientationsModel.h"

#include "inspector/core/project.h"


namespace inspector
{

namespace ui
{

ExportOrientationsModelImp::ExportOrientationsModelImp(Project *project,
                                                       QObject *parent)
  : ExportOrientationsModel(parent),
    mProject(project)
{
  this->init();
}


void ExportOrientationsModelImp::init()
{
}

void ExportOrientationsModelImp::clear()
{
}

QString ExportOrientationsModelImp::reconstruction() const
{
  QString mReconstructionPath = mProject->projectFolder() + "/sparse/0";
  return mReconstructionPath;
}


} // namespace ui

} // namespace inspector
