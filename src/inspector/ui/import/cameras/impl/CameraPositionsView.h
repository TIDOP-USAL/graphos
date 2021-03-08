#ifndef INSPECTOR_CAMERA_POSITIONS_VIEW_H
#define INSPECTOR_CAMERA_POSITIONS_VIEW_H

#include "inspector/ui/import/cameras/CameraPositionsView.h"

class QDialogButtonBox;
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
class QCheckBox;

namespace inspector
{

namespace ui
{

class CamerasImportViewImp
  : public CamerasImportView
{

  Q_OBJECT

public:

  CamerasImportViewImp(QWidget *parent = nullptr);
  ~CamerasImportViewImp() override;

public slots:

  void onDelimiterChanged();

// CamerasImportView interface

public:

  QString delimiter() const override;

public slots:

  void setItemModel(QStandardItemModel *model) override;
  void setItemModelFormatCameras(QStandardItemModel *model) override;
  void setTableHeader(const QStringList &header) override;
  void setImageColumn(const QString &imageColumn) override;
  void setXColumn(const QString &xColumn) override;
  void setYColumn(const QString &yColumn) override;
  void setZColumn(const QString &zColumn) override;
  void setQxColumn(const QString &qxColumn) override;
  void setQyColumn(const QString &qyColumn) override;
  void setQzColumn(const QString &qzColumn) override;
  void setQwColumn(const QString &qwColumn) override;
  void setOmegaColumn(const QString &omegaColumn) override;
  void setPhiColumn(const QString &phiColumn) override;
  void setKappaColumn(const QString &kappaColumn) override;
  void setYawColumn(const QString &yawColumn) override;
  void setPitchColumn(const QString &pitchColumn) override;
  void setRollColumn(const QString &rollColumn) override;
  void setParseOk(bool parseOk) override;
  void setValidInputCRS(bool valid) override;
  void setValidOutputCRS(bool valid) override;

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
  QRadioButton *mRadioButtonOther;
  QLineEdit *mLineEditOther;
//  QLabel *mLabelImportRow;
  QCheckBox *mCheckBoxFieldNamesAuto;
//  QSpinBox *mSpinBoxImportRow;
  QGroupBox *mGroupBoxColumns;
  QLabel *mLabelImageColumn;
  QComboBox *mComboBoxImageColumn;
  QGroupBox *mGroupBoxCoordinates;
  QLabel *mLabelXColumn;
  QComboBox *mComboBoxXColumn;
  QLabel *mLabelYColumn;
  QComboBox *mComboBoxYColumn;
  QLabel *mLabelZColumn;
  QComboBox *mComboBoxZColumn;
  QLabel *mLabelCrs;
  QLineEdit *mLineEditCrsInput;
  QLabel *mLabelCrsOut;
  QLineEdit *mLineEditCrsOutput;
  QGroupBox *mGroupBoxRotations;
  QGridLayout *gridLayoutRotations;
  QLabel *mLabelRotation;
  QComboBox *mComboBoxRotation;
  QStackedWidget *mStackedWidget;
  QWidget *pageQuaternions;
  QWidget *pageYawPitchRoll;
  QWidget *pageOmegaPhiKappa;
  QLabel *mLabelQxColumn;
  QComboBox *mComboBoxQxColumn;
  QLabel *mLabelQyColumn;
  QComboBox *mComboBoxQyColumn;
  QLabel *mLabelQzColumn;
  QComboBox *mComboBoxQzColumn;
  QLabel *mLabelQwColumn;
  QComboBox *mComboBoxQwColumn;
  QLabel *mLabelYawColumn;
  QComboBox *mComboBoxYawColumn;
  QLabel *mLabelPitchColumn;
  QComboBox *mComboBoxPitchColumn;
  QLabel *mLabelRollColumn;
  QComboBox *mComboBoxRollColumn;
  QLabel *mLabelOmegaColumn;
  QComboBox *mComboBoxOmegaColumn;
  QLabel *mLabelPhiColumn;
  QComboBox *mComboBoxPhiColumn;
  QLabel *mLabelKappaColumn;
  QComboBox *mComboBoxKappaColumn;
  QLabel *mLabelPreview;
  QTableView *mTableViewImportCameras;
  QLabel *mLabelCameras;
  QTableView *mTableViewFormatCameras;
  QDialogButtonBox *mButtonBox;
  //QString mProjectPath;
  bool bParseOk;
  bool bValidCRS;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERA_POSITIONS_VIEW_H