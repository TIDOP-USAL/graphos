/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_CAMERAS_VIEW_H
#define GRAPHOS_CAMERAS_VIEW_H

#include "graphos/components/cameras/CamerasView.h"

class QGroupBox;
class QLineEdit;
class QLabel;
class QListWidget;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QDialogButtonBox;
class QTabWidget;

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

    auto activeCamera() const -> int override;
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
    void setPriorCalibCx(double cx) override;
    void setPriorCalibCy(double cy) override;
    void setPriorCalibF(double f) override;
    void setPriorCalibFx(double fx) override;
    void setPriorCalibFy(double fy) override;
    void setPriorCalibK1(double k1) override;
    void setPriorCalibK2(double k2) override;
    void setPriorCalibK3(double k3) override;
    void setPriorCalibK4(double k4) override;
    void setPriorCalibK5(double k5) override;
    void setPriorCalibK6(double k6) override;
    void setPriorCalibP1(double p1) override;
    void setPriorCalibP2(double p2) override;
    void setAdjustCalibCx(double cx) override;
    void setAdjustCalibCy(double cy) override;
    void setAdjustCalibF(double f) override;
    void setAdjustCalibFx(double fx) override;
    void setAdjustCalibFy(double fy) override;
    void setAdjustCalibK1(double k1) override;
    void setAdjustCalibK2(double k2) override;
    void setAdjustCalibK3(double k3) override;
    void setAdjustCalibK4(double k4) override;
    void setAdjustCalibK5(double k5) override;
    void setAdjustCalibK6(double k6) override;
    void setAdjustCalibP1(double p1) override;
    void setAdjustCalibP2(double p2) override;

protected slots:

    void onSelectionChanged() override;

//  CamerasViewImp

private:

    auto isPrincipalPointEnable() const -> bool;
    auto isFocalEnable() const -> bool;
    auto isFocalXYEnable() const -> bool;
    auto isK1Enable() const -> bool;
    auto isK2Enable() const -> bool;
    auto isK3Enable() const -> bool;
    auto isK4Enable() const -> bool;
    auto isK5Enable() const -> bool;
    auto isK6Enable() const -> bool;
    auto isP1Enable() const -> bool;
    auto isP2Enable() const -> bool;
    auto isFocalVisible() const -> bool;
    auto isFocalXYVisible() const -> bool;

private slots:

    void onPriorCalibrationImport();
    void onPriorCalibrationExport();
    void onAdjustCalibrationExport();

protected:

    QLabel *mLabelCameras;
    QListWidget *mListWidgetCameras;
    QTabWidget *mTabWidget;
    QWidget *mTabInfo;
    QWidget *mTabCalibration;
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
    QGroupBox *mGroupBoxPriorCalibration;
    QGroupBox *mGroupBoxAdjustCalibration;
    QAction *mActionImportPriorCalibration;
    QAction *mActionExportPriorCalibration;
    QAction *mActionExportAdjustCalibration;
    //QAction *mActionFixCalibration;
    QLabel *mLabelPriorF;
    QDoubleSpinBox *mDoubleSpinBoxPriorF;
    QLabel *mLabelPriorFx;
    QDoubleSpinBox *mDoubleSpinBoxPriorFx;
    QLabel *mLabelPriorFy;
    QDoubleSpinBox *mDoubleSpinBoxPriorFy;
    QLabel *mLabelPriorCx;
    QDoubleSpinBox *mDoubleSpinBoxPriorCx;
    QLabel *mLabelPriorCy;
    QDoubleSpinBox *mDoubleSpinBoxPriorCy;
    QLabel *mLabelPriorK1;
    QDoubleSpinBox *mDoubleSpinBoxPriorK1;
    QLabel *mLabelPriorK2;
    QDoubleSpinBox *mDoubleSpinBoxPriorK2;
    QLabel *mLabelPriorK3;
    QDoubleSpinBox *mDoubleSpinBoxPriorK3;
    QLabel *mLabelPriorK4;
    QDoubleSpinBox *mDoubleSpinBoxPriorK4;
    QLabel *mLabelPriorK5;
    QDoubleSpinBox *mDoubleSpinBoxPriorK5;
    QLabel *mLabelPriorK6;
    QDoubleSpinBox *mDoubleSpinBoxPriorK6;
    QLabel *mLabelPriorP1;
    QDoubleSpinBox *mDoubleSpinBoxPriorP1;
    QLabel *mLabelPriorP2;
    QDoubleSpinBox *mDoubleSpinBoxPriorP2;
    QLabel *mLabelAdjustF;
    QDoubleSpinBox *mDoubleSpinBoxAdjustF;
    QLabel *mLabelAdjustFx;
    QDoubleSpinBox *mDoubleSpinBoxAdjustFx;
    QLabel *mLabelAdjustFy;
    QDoubleSpinBox *mDoubleSpinBoxAdjustFy;
    QLabel *mLabelAdjustCx;
    QDoubleSpinBox *mDoubleSpinBoxAdjustCx;
    QLabel *mLabelAdjustCy;
    QDoubleSpinBox *mDoubleSpinBoxAdjustCy;
    QLabel *mLabelAdjustK1;
    QDoubleSpinBox *mDoubleSpinBoxAdjustK1;
    QLabel *mLabelAdjustK2;
    QDoubleSpinBox *mDoubleSpinBoxAdjustK2;
    QLabel *mLabelAdjustK3;
    QDoubleSpinBox *mDoubleSpinBoxAdjustK3;
    QLabel *mLabelAdjustK4;
    QDoubleSpinBox *mDoubleSpinBoxAdjustK4;
    QLabel *mLabelAdjustK5;
    QDoubleSpinBox *mDoubleSpinBoxAdjustK5;
    QLabel *mLabelAdjustK6;
    QDoubleSpinBox *mDoubleSpinBoxAdjustK6;
    QLabel *mLabelAdjustP1;
    QDoubleSpinBox *mDoubleSpinBoxAdjustP1;
    QLabel *mLabelAdjustP2;
    QDoubleSpinBox *mDoubleSpinBoxAdjustP2;
    QLabel *mLabelImages;
    QListWidget *mListWidgetImages;
    QDialogButtonBox *mButtonBox;

};

} // namespace graphos

#endif // GRAPHOS_CAMERAS_VIEW_H
