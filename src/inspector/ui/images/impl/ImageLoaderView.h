#ifndef INSPECTOR_IMAGE_LOADER_VIEW_H
#define INSPECTOR_IMAGE_LOADER_VIEW_H

#include "inspector/ui/images/ImageLoaderView.h"

class QGroupBox;
class QLineEdit;
class QLabel;
class QListWidget;
class QComboBox;
class QSpinBox;
class QSpinBox;
class QDialogButtonBox;

namespace inspector
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

  void setImageDirectory(const QString &directory) override;

private:

  void init();
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_IMAGE_LOADER_VIEW_H
