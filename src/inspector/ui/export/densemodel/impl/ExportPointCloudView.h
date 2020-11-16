#ifndef INSPECTOR_POINT_CLOUD_EXPORT_VIEW_H
#define INSPECTOR_POINT_CLOUD_EXPORT_VIEW_H

#include "inspector/ui/export/densemodel/ExportPointCloudView.h"

class QLabel;
class QComboBox;
class QCheckBox;
class QGridLayout;
class QDialogButtonBox;

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

// ExportPointCloudView interface

public:

//  void addFormatWidget(QWidget *formatWidget) override;
//  QString format() const override;
  bool isColorActive() const override;
  bool isNormalsActive() const override;

public slots:

//  void setCurrentFormat(const QString &format) override;
  void setActiveColor(bool active) override;
  void setActiveNormals(bool active) override;

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

  //QLabel *mLabelFormat;
  //QComboBox *mComboBoxFormat;
  QCheckBox *mCheckBoxColor;
  QCheckBox *mCheckBoxNormals;
  QGridLayout *mGridLayoutFormat;
  QDialogButtonBox *mButtonBox;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_POINT_CLOUD_EXPORT_VIEW_H
