/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_CAMERAS_VIEW_H
#define GRAPHOS_CAMERAS_VIEW_H

#include "graphos/ui/cameras/CamerasView.h"

class QGroupBox;
class QLineEdit;
class QLabel;
class QListWidget;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QDialogButtonBox;

namespace graphos
{

class CamerasViewImp
  : public CamerasView
{

  Q_OBJECT

public:

  explicit CamerasViewImp(QWidget *parent = nullptr);
  ~CamerasViewImp() override = default;

// DialogView interface

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

//  CamerasViewImp

private:

  bool isPrincipalPointEnable() const;
  bool isFocalEnable() const;
  bool isFocalXYEnable() const;
  bool isK1Enable() const;
  bool isK2Enable() const;
  bool isK3Enable() const;
  bool isK4Enable() const;
  bool isK5Enable() const;
  bool isK6Enable() const;
  bool isP1Enable() const;
  bool isP2Enable() const;
  bool isFocalVisible() const;
  bool isFocalXYVisible() const;

private slots:

  void onCalibrationImport();
  void onCalibrationExport();

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

} // namespace graphos

#endif // GRAPHOS_CAMERAS_VIEW_H
