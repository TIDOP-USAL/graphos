#ifndef INSPECTOR_EXPORT_POINT_CLOUD_VIEW_H
#define INSPECTOR_EXPORT_POINT_CLOUD_VIEW_H

#include "inspector/ui/export/densemodel/ExportPointCloudView.h"

/* class QDialogButtonBox;
class QTreeWidget;
class QTableView;
class QLabel;
class QLineEdit;
class QComboBox;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QRadioButton;
class QSpinBox;
class QStackedWidget;
class QCheckBox; */

namespace inspector
{

namespace ui
{

class ExportPointCloudViewImp
  : public ExportPointCloudView
{

  Q_OBJECT

public:

  ExportPointCloudViewImp(QWidget *parent = nullptr);
  ~ExportPointCloudViewImp() override;

public slots:

  void onDelimiterChanged();

// ExportPointCloudView interface

public:

  QString delimiter() const override;

public slots:

  void void setDelimiter(const QString &delimiter) override;

// IDialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QGroupBox *mGroupBoxDelimiter;
  QRadioButton *mRadioButtonTab;
  QRadioButton *mRadioButtonComma;
  QRadioButton *mRadioButtonSpace;
  QRadioButton *mRadioButtonSemicolon;
  QDialogButtonBox *mButtonBox;


};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_EXPORT_POINT_CLOUD_VIEW_H
