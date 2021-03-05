#ifndef INSPECTOR_IMAGE_LOADER_VIEW_INTERFACE_H
#define INSPECTOR_IMAGE_LOADER_VIEW_INTERFACE_H

#include "inspector/interfaces/mvp.h"

#include <QFileDialog>

namespace inspector
{

namespace ui
{

class ImageLoaderView
  : public QFileDialog
{
  Q_OBJECT

public:

  ImageLoaderView(QWidget *parent) : QFileDialog(parent) {}
  ~ImageLoaderView() override = default;

  virtual void setImageDirectory(const QString &directory) = 0;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_IMAGE_LOADER_VIEW_INTERFACE_H
