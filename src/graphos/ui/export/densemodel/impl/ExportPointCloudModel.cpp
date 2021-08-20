#include "ExportPointCloudModel.h"

#include "graphos/core/project.h"

#include <QFile>
#include <QTextStream>

namespace graphos
{

namespace ui
{

ExportPointCloudModelImp::ExportPointCloudModelImp(Project *project,
                                                   QObject *parent)
  : ExportPointCloudModel(parent),
    mProject(project)
{
  this->init();
}

std::array<double, 3> ExportPointCloudModelImp::offset() const
{
  std::array<double, 3> offset;
  offset.fill(0.);

  try {
    QString path = mProject->reconstructionPath();
    path.append("/offset.txt");
    QFile file(path);
    if (file.open(QFile::ReadOnly | QFile::Text)){
      QTextStream stream(&file);
      QString line = stream.readLine();
      QStringList reg = line.split(" ");
      offset[0] = reg[0].toDouble();
      offset[1] = reg[1].toDouble();
      offset[2] = reg[2].toDouble();
      file.close();
    }
  } catch (...) {

  }

  return offset;
}

QString ExportPointCloudModelImp::denseModel() const
{
  return mProject->denseModel();
}

void ExportPointCloudModelImp::init()
{
}

void ExportPointCloudModelImp::clear()
{

}


} // namespace ui

} // namespace graphos
