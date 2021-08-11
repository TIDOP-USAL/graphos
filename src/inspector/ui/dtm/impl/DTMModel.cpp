#include "DTMModel.h"

#include "inspector/core/project.h"

#include <QFileInfo>
#include <QFile>
#include <QTextStream>

namespace inspector
{

namespace ui
{
	
DtmModelImp::DtmModelImp(Project *project,
                         QObject *parent)
  : DtmModel(parent),
    mProject(project)
{
  this->init();
}

QString DtmModelImp::projectPath() const
{
  return mProject->projectFolder();
}

QString DtmModelImp::denseModel() const
{
  return mProject->denseModel();
}

QString DtmModelImp::crs() const
{
  return mProject->crs();
}

std::shared_ptr<Dtm> DtmModelImp::dtmMethod() const
{
  return mProject->dtmMethod();
}

void DtmModelImp::setDtmMethod(const std::shared_ptr<Dtm> &dtm)
{
  mProject->setDtmMethod(dtm);
}

QString DtmModelImp::dtmPath() const
{
  return mProject->dtmPath();
}

void DtmModelImp::setDtmPath(const QString &dtmPath)
{
  mProject->setDtmPath(dtmPath);
}

std::array<double, 3> DtmModelImp::offset() const
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

void DtmModelImp::init()
{
}

void DtmModelImp::clear()
{
}

} // namespace ui

} // namespace inspector


