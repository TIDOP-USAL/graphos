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
  void setCalibK1(const QString &k1) override;
  void setCalibK2(const QString &k2) override;
  void setCalibK3(const QString &k3) override;
  void setCalibP1(const QString &p1) override;
  void setCalibP2(const QString &p2) override;

protected slots:

  void onSelectionChanged() override;

protected:

    QGroupBox *mGroupBoxCalibrationParameters;
    QLineEdit *mLineEditCy;
    QLabel *mLabelF;
    QLabel *mLabelK3;
    QLabel *mLabelCx;
    QLineEdit *mLineEditK1;
    QLabel *mLabelP1;
    QLineEdit *mLineEditCx;
    QLineEdit *mLineEditK3;
    QLabel *mLabelK1;
    QLineEdit *mLineEditP1;
    QLabel *mLabelCy;
    QLabel *mLabelK2;
    QLineEdit *mLineEditK2;
    QLabel *mLabelP2;
    QLineEdit *mLineEditP2;
    QLineEdit *mLineEditF;
    QLabel *mLabelImages;
    QListWidget *mListWidgetCameras;
    QListWidget *mListWidgetImages;
    QLabel *mLabelCameras;
    QGroupBox *mGroupBoxCamera;
    QLineEdit *mLineEditFocal;
    QLabel *mLabelType;
    QLabel *mLabelWidth;
    QLabel *mLabelSensorSize;
    QLabel *mLabelFocal;
    QLineEdit *mLineEditSensorSize;
    QComboBox *mComboBoxType;
    QLabel *mLabelMake;
    QLineEdit *mLineEditMake;
    QLabel *mLabelModel;
    QLineEdit *mLineEditModel;
    QSpinBox *mSpinBoxWidth;
    QLabel *mLabelHeight;
    QSpinBox *mSpinBoxHeight;
    QDialogButtonBox *mButtonBox;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERAS_VIEW_H
