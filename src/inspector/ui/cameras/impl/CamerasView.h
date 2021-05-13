#ifndef INSPECTOR_CAMERAS_VIEW_H
#define INSPECTOR_CAMERAS_VIEW_H

#include "inspector/ui/cameras/CamerasView.h"

class QGroupBox;
class QLineEdit;
class QLabel;
class QListWidget;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QDialogButtonBox;

namespace inspector
{

namespace ui
{

class CamerasViewImp
  : public CamerasView
{

  Q_OBJECT

public:

  explicit CamerasViewImp(QWidget *parent = nullptr);
  ~CamerasViewImp() override = default;

private slots:

  void onCalibrationImport();
  void onCalibrationExport();

// IDialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

// CamerasView interface

public:

  int activeCamera() const override;
  void setActiveCamera(int id) override;
  void enableCameraEdition(bool enable) override;
  void addCamera(int cameraId, const QString &cameraName) override;
  void setMake(const QString &make) override;
  void setModel(const QString &model) override;
  void setWidth(int width) override;
  void setHeight(int height) override;
  void setSensorSize(const QString &sensorSize) override;
  void setFocal(const QString &focal) override;
  void setType(const QString &type) override;
  void setImages(const QStringList &images) override;
  void setCalibCx(double cx) override;
  void setCalibCy(double cy) override;
  void setCalibF(double f) override;
  void setCalibFx(double fx) override;
  void setCalibFy(double fy) override;
  void setCalibK1(double k1) override;
  void setCalibK2(double k2) override;
  void setCalibK3(double k3) override;
  void setCalibK4(double k4) override;
  void setCalibK5(double k5) override;
  void setCalibK6(double k6) override;
  void setCalibP1(double p1) override;
  void setCalibP2(double p2) override;

protected slots:

  void onSelectionChanged() override;

protected:

    QLabel *mLabelCameras;
    QListWidget *mListWidgetCameras;
    QGroupBox *mGroupBoxCamera;
    QLabel *mLabelMake;
    QLineEdit *mLineEditMake;
    QLabel *mLabelModel;
    QLineEdit *mLineEditModel;
    QLabel *mLabelWidth;
    QSpinBox *mSpinBoxWidth;
    QLabel *mLabelHeight;
    QSpinBox *mSpinBoxHeight;
    QLabel *mLabelSensorSize;
    QLineEdit *mLineEditSensorSize;
    QLabel *mLabelFocal;
    QLineEdit *mLineEditFocal;
    QLabel *mLabelType;
    QComboBox *mComboBoxType;
    QPushButton *mPushButtonEditCamera;
    QGroupBox *mGroupBoxCalibrationParameters;
    QAction *mActionImportCalibration;
    QAction *mActionExportCalibration;
    QAction *mActionFixCalibration;
    QLabel *mLabelF;
    QDoubleSpinBox *mDoubleSpinBoxF;
    QLabel *mLabelFx;
    QDoubleSpinBox *mDoubleSpinBoxFx;
    QLabel *mLabelFy;
    QDoubleSpinBox *mDoubleSpinBoxFy;
    QLabel *mLabelCx;
    QDoubleSpinBox *mDoubleSpinBoxCx;
    QLabel *mLabelCy;
    QDoubleSpinBox *mDoubleSpinBoxCy;
    QLabel *mLabelK1;
    QDoubleSpinBox *mDoubleSpinBoxK1;
    QLabel *mLabelK2;
    QDoubleSpinBox *mDoubleSpinBoxK2;
    QLabel *mLabelK3;
    QDoubleSpinBox *mDoubleSpinBoxK3;
    QLabel *mLabelK4;
    QDoubleSpinBox *mDoubleSpinBoxK4;
    QLabel *mLabelK5;
    QDoubleSpinBox *mDoubleSpinBoxK5;
    QLabel *mLabelK6;
    QDoubleSpinBox *mDoubleSpinBoxK6;
    QLabel *mLabelP1;
    QDoubleSpinBox *mDoubleSpinBoxP1;
    QLabel *mLabelP2;
    QDoubleSpinBox *mDoubleSpinBoxP2;
    QLabel *mLabelImages;
    QListWidget *mListWidgetImages;
    QDialogButtonBox *mButtonBox;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERAS_VIEW_H
