#include "OpenProjectView.h"

namespace graphos
{

OpenProjectViewImp::OpenProjectViewImp(QWidget *parent)
  : OpenProjectView(parent)
{
  this->init();
}

void OpenProjectViewImp::init()
{
  QFileDialog::setWindowTitle(tr("Open Graphos Project"));
  QFileDialog::setNameFilter(tr("Graphos Project (*.xml)"));
  QFileDialog::setFileMode(QFileDialog::ExistingFile);
}

void OpenProjectViewImp::setGraphosProjectsPath(const QString &directory)
{
  QFileDialog::setDirectory(directory);
}

} // namespace graphos