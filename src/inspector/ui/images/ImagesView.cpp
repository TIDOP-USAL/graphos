#include "ImagesView.h"

#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QPushButton>

namespace inspector
{

namespace ui
{


ImagesViewImp::ImagesViewImp(QWidget *parent)
  : ImagesView(parent)
{
  this->init();
}

void ImagesViewImp::init()
{
  QFileDialog::setWindowTitle(tr("Add images"));
  QFileDialog::setFileMode(QFileDialog::ExistingFiles);
  QFileDialog::setNameFilter(tr("Image files (*.tif *.tiff *.jpg *.png);;TIFF (*.tif *.tiff);;png (*.png);;JPEG (*.jpg)"));
}

void ImagesViewImp::setImageDirectory(const QString &directory)
{
  QFileDialog::setDirectory(directory);
}

} // namespace ui

} // namespace inspector
