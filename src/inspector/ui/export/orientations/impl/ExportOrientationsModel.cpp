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
  return mProject->reconstructionPath();
}

tl::Point3D ExportOrientationsModelImp::offset() const
{
  tl::Point3D offset(0., 0., 0.);

  std::ifstream ifs;
  ifs.open(mProject->offset().toStdString(), std::ifstream::in);
  if (ifs.is_open()) {

    ifs >> offset.x >> offset.y >> offset.z;

    ifs.close();
  }

  return offset;
}


} // namespace ui

} // namespace inspector
