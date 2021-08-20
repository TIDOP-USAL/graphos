#ifndef GRAPHOS_LOADER_VIEW_H
#define GRAPHOS_LOADER_VIEW_H

#include "graphos/ui/images/ImageLoaderView.h"

class QGroupBox;
class QLineEdit;
class QLabel;
class QListWidget;
class QComboBox;
class QSpinBox;
class QSpinBox;
class QDialogButtonBox;

namespace graphos
{

namespace ui
{

class ImageLoaderViewImp
  : public ImageLoaderView
{

  Q_OBJECT

public:

  explicit ImageLoaderViewImp(QWidget *parent = nullptr);
  ~ImageLoaderViewImp() override = default;

  void setImagesDirectory(const QString &directory) override;

private:

  void init();
};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_LOADER_VIEW_H
