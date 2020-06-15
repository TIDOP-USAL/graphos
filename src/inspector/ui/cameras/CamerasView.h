#ifndef INSPECTOR_CAMERAS_VIEW_H
#define INSPECTOR_CAMERAS_VIEW_H

#include "inspector/ui/cameras/Cameras.h"

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

class CamerasViewImp
  : public CamerasView
{

  Q_OBJECT

public:

  explicit CamerasViewImp(QWidget *parent = nullptr);
  ~CamerasViewImp() override = default;

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
  void setCalibCx(const QString &cx) override;
  void setCalibCy(const QString &cy) override;
  void setCalibF(const QString &f) override;
  void setCalibFx(const QString &fx) override;
  void setCalibFy(const QString &fy) override;
  void setCalibK1(const QString &k1) override;
  void setCalibK2(const QString &k2) override;
  void setCalibK3(const QString &k3) override;
  void setCalibK4(const QString &k4) override;
  void setCalibK5(const QString &k5) override;
  void setCalibK6(const QString &k6) override;
  void setCalibP1(const QString &p1) override;
  void setCalibP2(const QString &p2) override;

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
    QGroupBox *mGroupBoxCalibrationParameters;
    QLabel *mLabelF;
    QLineEdit *mLineEditF;
    QLabel *mLabelFx;
    QLineEdit *mLineEditFx;
    QLabel *mLabelFy;
    QLineEdit *mLineEditFy;
    QLabel *mLabelCx;
    QLineEdit *mLineEditCx;
    QLabel *mLabelCy;
    QLineEdit *mLineEditCy;
    QLabel *mLabelK1;
    QLineEdit *mLineEditK1;
    QLabel *mLabelK2;
    QLineEdit *mLineEditK2;
    QLabel *mLabelK3;
    QLineEdit *mLineEditK3;
    QLabel *mLabelK4;
    QLineEdit *mLineEditK4;
    QLabel *mLabelK5;
    QLineEdit *mLineEditK5;
    QLabel *mLabelK6;
    QLineEdit *mLineEditK6;
    QLabel *mLabelP1;
    QLineEdit *mLineEditP1;
    QLabel *mLabelP2;
    QLineEdit *mLineEditP2;
    QLabel *mLabelImages;
    QListWidget *mListWidgetImages;
    QDialogButtonBox *mButtonBox;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERAS_VIEW_H
