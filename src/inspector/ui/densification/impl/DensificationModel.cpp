#include "DensificationModel.h"

#include "inspector/core/project.h"
#include "inspector/core/densification/densification.h"


#include <tidop/core/console.h>

#include <QSettings>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace inspector
{


namespace ui
{


DensificationModelImp::DensificationModelImp(Project *project,
                                             QObject *parent)
  : DensificationModel(parent),
    mProject(project)
{
  init();
}

DensificationModelImp::~DensificationModelImp()
{

}

void DensificationModelImp::init()
{
}

void DensificationModelImp::clear()
{
}

std::shared_ptr<Densification> DensificationModelImp::densification() const
{
  return mProject->densification();
}

QString DensificationModelImp::projectFolder() const
{
  return mProject->projectFolder();
}

QString DensificationModelImp::imageDirectory() const
{
  return mProject->imageDirectory();
}

QString DensificationModelImp::reconstructionPath() const
{
  return mProject->reconstructionPath();
}

bool DensificationModelImp::useCuda() const
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TIDOP", "Inspector");
  return settings.value("General/UseCuda", true).toBool();
}

void DensificationModelImp::setDensification(const std::shared_ptr<Densification> &densification)
{
  mProject->setDensification(densification);
}

void DensificationModelImp::setDenseModel(const QString &denseModel)
{
  mProject->setDenseModel(denseModel);
}

} // End namespace ui

} // End namespace inspector