#ifndef GRAPHOS_LOADER_VIEW_INTERFACE_H
#define GRAPHOS_LOADER_VIEW_INTERFACE_H

#include "graphos/interfaces/mvp.h"

#include <QFileDialog>

namespace graphos
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

  virtual void setImagesDirectory(const QString &directory) = 0;

};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_LOADER_VIEW_INTERFACE_H
