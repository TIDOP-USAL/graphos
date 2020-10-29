#include "ExportPointCloudModel.h"

#include "inspector/core/project.h"

namespace inspector
{

namespace ui
{

ExportPointCloudModelImp::ExportPointCloudModelImp(Project *project,
                                                   QObject *parent)
  : ExportPointCloudModel(parent),
    mProject(project),
    mCsvFile(""),
    mDelimiter(";"),
	mColor(true),
	mNormals(true)
{
  this->init();
}

void ExportPointCloudModelImp::setCsvFile(const QString &csv)
{
  mCsvFile = csv;
}

void ExportPointCloudModelImp::setDelimiter(const QString &delimiter)
{
  mDelimiter = delimiter;
}

void ExportPointCloudModelImp::enableColor(const QString &active)
{
  mColor = active;
}

void ExportPointCloudModelImp::enableNormals(const QString &active)
{
  mNormals = active;
}

void ExportPointCloudModelImp::init()
{
}

void ExportPointCloudModelImp::clear()
{
  mCsvFile.clear();
  mDelimiter = ";";
  mColor = true;
  mNormals = true;
}

void ExportPointCloudModelImp::exportPointCloud()
{

}

} // namespace ui

} // namespace inspector
