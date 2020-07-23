#ifndef INSPECTOR_IMAGES_VIEW_H
#define INSPECTOR_IMAGES_VIEW_H

#include "inspector/ui/images/Images.h"

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

class ImagesViewImp
  : public ImagesView
{

  Q_OBJECT

public:

  explicit ImagesViewImp(QWidget *parent = nullptr);
  ~ImagesViewImp() override = default;

  void setImageDirectory(const QString &directory) override;

private:

  void init();
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_IMAGES_VIEW_H
