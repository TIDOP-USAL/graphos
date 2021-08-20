#include "ImageLoaderView.h"

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

namespace graphos
{

namespace ui
{


ImageLoaderViewImp::ImageLoaderViewImp(QWidget *parent)
  : ImageLoaderView(parent)
{
  this->init();
}

void ImageLoaderViewImp::init()
{
  QFileDialog::setWindowTitle(tr("Add images"));
  QFileDialog::setFileMode(QFileDialog::ExistingFiles);
  QFileDialog::setNameFilter(tr("Image files (*.tif *.tiff *.jpg *.png);;TIFF (*.tif *.tiff);;png (*.png);;JPEG (*.jpg)"));
}

void ImageLoaderViewImp::setImagesDirectory(const QString &directory)
{
  QFileDialog::setDirectory(directory);
}

} // namespace ui

} // namespace graphos
