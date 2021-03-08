#include "OpenProjectView.h"

namespace inspector
{

namespace ui
{

OpenProjectViewImp::OpenProjectViewImp(QWidget *parent)
  : OpenProjectView(parent)
{
  this->init();
}

void OpenProjectViewImp::init()
{
  QFileDialog::setWindowTitle(tr("Open Inspector Image Project"));
  QFileDialog::setNameFilter(tr("Inspector Image Project (*.xml)"));
  QFileDialog::setFileMode(QFileDialog::ExistingFile);
}

void OpenProjectViewImp::setInspectorPath(const QString &directory)
{
  QFileDialog::setDirectory(directory);
}

} // namespace ui

} // namespace inspector