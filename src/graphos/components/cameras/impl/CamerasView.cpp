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

#include "CamerasView.h"

#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QToolBar>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTabWidget>

#include <tidop/core/defs.h>
#include <tidop/core/app/Message.h>

namespace graphos
{


CamerasViewImp::CamerasViewImp(QWidget *parent)
  : CamerasView(parent)
{
    CamerasViewImp::initUI();
    CamerasViewImp::initSignalAndSlots();
}

void CamerasViewImp::onPriorCalibrationImport()
{
    QString selected_filter;
    QString path_name = QFileDialog::getOpenFileName(this,
                                                    QApplication::translate("CamerasComponent", "Import calibration", nullptr),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    QApplication::translate("CamerasComponent", "Pix4D Camera Calibration (*.cam);;Agisoft Camera Calibration (*.xml);;OpenCV Camera Calibration (*.xml);;ODM Camera Calibration (*.json)"),
                                                    &selected_filter);

    if (!path_name.isEmpty()) {
        QString format;
        if (selected_filter.compare("Pix4D Camera Calibration (*.cam)") == 0) {
          format = "Pix4D";
        } else if (selected_filter.compare(QApplication::translate("CamerasComponent", "Agisoft Camera Calibration (*.xml)", nullptr)) == 0) {
            format = "Agisoft";
        } else if (selected_filter.compare(QApplication::translate("CamerasComponent", "OpenCV Camera Calibration (*.xml)", nullptr)) == 0) {
            format = "OpenCV";
        } else if (selected_filter.compare(QApplication::translate("CamerasComponent", "ODM Camera Calibration (*.json)", nullptr)) == 0) {
            format = "ODM";
        } else {
            tl::Message::error("Unsupported format");
        }

        emit priorCalibrationImport(path_name, format);
    }
}

void CamerasViewImp::onPriorCalibrationExport()
{
    QString selected_filter;
    QString path_name = QFileDialog::getSaveFileName(this,
                                                    QApplication::translate("CamerasComponent", "Export Calibration", nullptr),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    QApplication::translate("CamerasComponent", "Pix4D Camera Calibration (*.cam);;Agisoft Camera Calibration (*.xml);;OpenCV Camera Calibration (*.xml);;ODM Camera Calibration (*.json)"),
                                                    &selected_filter);

    if (!path_name.isEmpty()) {

        QString format;
        if (selected_filter.compare("Pix4D Camera Calibration (*.cam)") == 0) {
          format = "Pix4D";
        } else if (selected_filter.compare(QApplication::translate("CamerasComponent", "Agisoft Camera Calibration (*.xml)", nullptr)) == 0) {
            format = "Agisoft";
        } else if (selected_filter.compare(QApplication::translate("CamerasComponent", "OpenCV Camera Calibration (*.xml)", nullptr)) == 0) {
            format = "OpenCV";
        } else if (selected_filter.compare(QApplication::translate("CamerasComponent", "ODM Camera Calibration (*.json)", nullptr)) == 0) {
            format = "ODM";
        } else {
            tl::Message::error("Unsupported format");
        }

        emit priorCalibrationExport(path_name, format);
    }
}

void CamerasViewImp::onAdjustCalibrationExport()
{
    QString selected_filter;
    QString path_name = QFileDialog::getSaveFileName(this,
        QApplication::translate("CamerasComponent", "Export Calibration", nullptr),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        QApplication::translate("CamerasComponent", "Pix4D Camera Calibration (*.cam);;Agisoft Camera Calibration (*.xml);;OpenCV Camera Calibration (*.xml);;ODM Camera Calibration (*.json)"),
        &selected_filter);

    if (!path_name.isEmpty()) {

        QString format;
        if (selected_filter.compare("Pix4D Camera Calibration (*.cam)") == 0) {
            format = "Pix4D";
        } else if (selected_filter.compare(QApplication::translate("CamerasComponent", "Agisoft Camera Calibration (*.xml)", nullptr)) == 0) {
            format = "Agisoft";
        } else if (selected_filter.compare(QApplication::translate("CamerasComponent", "OpenCV Camera Calibration (*.xml)", nullptr)) == 0) {
            format = "OpenCV";
        } else if (selected_filter.compare(QApplication::translate("CamerasComponent", "ODM Camera Calibration (*.json)", nullptr)) == 0) {
            format = "ODM";
        } else {
            tl::Message::error("Unsupported format");
        }

        emit adjustCalibrationExport(path_name, format);
    }
}

void CamerasViewImp::initUI()
{
    this->setObjectName(QString("CamerasView"));

    this->resize(800, 660);

    QGridLayout *layout = new QGridLayout();
    this->setLayout(layout);

    mLabelCameras = new QLabel(this);
    mLabelCameras->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    layout->addWidget(mLabelCameras, 0, 0, 1, 1);

    mListWidgetCameras = new QListWidget(this);
    mListWidgetCameras->setMaximumSize(QSize(250, 16777215));
    layout->addWidget(mListWidgetCameras, 1, 0, 2, 1);

    mTabWidget = new QTabWidget(this);
    layout->addWidget(mTabWidget, 0, 1, 3, 1);

    // Tab 1: Camera information + images
    //
    mTabInfo = new QWidget();
    QVBoxLayout *tabInfoLayout = new QVBoxLayout(mTabInfo);

    mGroupBoxCamera = new QGroupBox(mTabInfo);
    QGridLayout *gridLayout3 = new QGridLayout(mGroupBoxCamera);

    mLabelMake = new QLabel(mGroupBoxCamera);
    gridLayout3->addWidget(mLabelMake, 1, 0, 1, 1);
    mLineEditMake = new QLineEdit(mGroupBoxCamera);
    gridLayout3->addWidget(mLineEditMake, 1, 1, 1, 1);

    mLabelModel = new QLabel(mGroupBoxCamera);
    gridLayout3->addWidget(mLabelModel, 1, 3, 1, 1);
    mLineEditModel = new QLineEdit(mGroupBoxCamera);
    gridLayout3->addWidget(mLineEditModel, 1, 4, 1, 1);

    mLabelWidth = new QLabel(mGroupBoxCamera);
    gridLayout3->addWidget(mLabelWidth, 2, 0, 1, 1);
    mSpinBoxWidth = new QSpinBox(mGroupBoxCamera);
    mSpinBoxWidth->setMaximum(100000);
    gridLayout3->addWidget(mSpinBoxWidth, 2, 1, 1, 1);

    mLabelHeight = new QLabel(mGroupBoxCamera);
    gridLayout3->addWidget(mLabelHeight, 2, 3, 1, 1);
    mSpinBoxHeight = new QSpinBox(mGroupBoxCamera);
    mSpinBoxHeight->setMaximum(100000);
    gridLayout3->addWidget(mSpinBoxHeight, 2, 4, 1, 1);

    mLabelSensorSize = new QLabel(mGroupBoxCamera);
    gridLayout3->addWidget(mLabelSensorSize, 5, 0, 1, 1);
    mLineEditSensorSize = new QLineEdit(mGroupBoxCamera);
    gridLayout3->addWidget(mLineEditSensorSize, 5, 1, 1, 5);

    mLabelFocal = new QLabel(mGroupBoxCamera);
    gridLayout3->addWidget(mLabelFocal, 6, 0, 1, 1);
    mLineEditFocal = new QLineEdit(mGroupBoxCamera);
    gridLayout3->addWidget(mLineEditFocal, 6, 1, 1, 5);

    mLabelType = new QLabel(mGroupBoxCamera);
    gridLayout3->addWidget(mLabelType, 7, 0, 1, 1);
    mComboBoxType = new QComboBox(mGroupBoxCamera);
    mComboBoxType->addItem("Radial 1");
    mComboBoxType->addItem("Radial 2");
    mComboBoxType->addItem("Pinhole 1");
    mComboBoxType->addItem("Pinhole 2");
    mComboBoxType->addItem("OpenCV 1");
    mComboBoxType->addItem("OpenCV 2");
    mComboBoxType->addItem("OpenCV Fisheye");
    mComboBoxType->addItem("Radial Fisheye 1");
    mComboBoxType->addItem("Radial Fisheye 2");

    gridLayout3->addWidget(mComboBoxType, 7, 1, 1, 5);

    mPushButtonEditCamera = new QPushButton(mGroupBoxCamera);
    mPushButtonEditCamera->setCheckable(true);
    gridLayout3->addWidget(mPushButtonEditCamera, 8, 1);

    tabInfoLayout->addWidget(mGroupBoxCamera);

    mLabelImages = new QLabel(mTabInfo);
    tabInfoLayout->addWidget(mLabelImages);
    mListWidgetImages = new QListWidget(mTabInfo);
    tabInfoLayout->addWidget(mListWidgetImages);

    mTabWidget->addTab(mTabInfo, "");

    //layout->addWidget(mGroupBoxCamera, 1, 1, 2, 4);

    // Tab 2: Calibrations

    mTabCalibration = new QWidget();
    QVBoxLayout *tabCalibLayout = new QVBoxLayout(mTabCalibration);

    // Prior Calibration

    mGroupBoxPriorCalibration  = new QGroupBox(mTabCalibration);
    QGridLayout *grid_layout_prior_calibration = new QGridLayout(mGroupBoxPriorCalibration );

    QToolBar *toolBar = new QToolBar(this);

    mActionImportPriorCalibration = new QAction(this);
    mActionImportPriorCalibration->setIcon(QIcon::fromTheme("folder"));
    toolBar->addAction(mActionImportPriorCalibration);

    mActionExportPriorCalibration = new QAction(this);
    mActionExportPriorCalibration->setIcon(QIcon::fromTheme("save"));
    toolBar->addAction(mActionExportPriorCalibration);

    grid_layout_prior_calibration->addWidget(toolBar, 0, 0, 1, 4);

    //toolBar->addSeparator();

    //QIcon ico;
    //ico.addPixmap(QPixmap(":/ico/24/img/material/24/icons8_lock_24px.png"), QIcon::Normal, QIcon::On);
    //ico.addPixmap(QPixmap(":/ico/24/img/material/24/icons8_padlock_24px.png"), QIcon::Normal, QIcon::Off);
    //mActionFixCalibration = new QAction(this);
    //mActionFixCalibration->setIcon(ico);
    //mActionFixCalibration->setCheckable(true);
    //toolBar->addAction(mActionFixCalibration);

    grid_layout_prior_calibration->addWidget(toolBar, 0, 0, 1, 4);

    mLabelPriorF = new QLabel(mGroupBoxPriorCalibration );
    mLabelPriorF->setMaximumWidth(50);
    grid_layout_prior_calibration->addWidget(mLabelPriorF, 1, 0, 1, 1);
    mDoubleSpinBoxPriorF = new QDoubleSpinBox(mGroupBoxPriorCalibration );
    mDoubleSpinBoxPriorF->setRange(0., 100000.);
    mDoubleSpinBoxPriorF->setDecimals(8);
    grid_layout_prior_calibration->addWidget(mDoubleSpinBoxPriorF, 1, 1, 1, 3);

    mLabelPriorFx = new QLabel(mGroupBoxPriorCalibration );
    mLabelPriorFx->setMaximumWidth(50);
    grid_layout_prior_calibration->addWidget(mLabelPriorFx, 1, 0, 1, 1);
    mDoubleSpinBoxPriorFx = new QDoubleSpinBox(mGroupBoxPriorCalibration );
    mDoubleSpinBoxPriorFx->setRange(0., 100000.);
    mDoubleSpinBoxPriorFx->setDecimals(8);
    grid_layout_prior_calibration->addWidget(mDoubleSpinBoxPriorFx, 1, 1, 1, 1);

    mLabelPriorFy = new QLabel(mGroupBoxPriorCalibration );
    mLabelPriorFy->setMaximumWidth(50);
    grid_layout_prior_calibration->addWidget(mLabelPriorFy, 1, 2, 1, 1);
    mDoubleSpinBoxPriorFy = new QDoubleSpinBox(mGroupBoxPriorCalibration );
    mDoubleSpinBoxPriorFy->setRange(0., 100000.);
    mDoubleSpinBoxPriorFy->setDecimals(8);
    grid_layout_prior_calibration->addWidget(mDoubleSpinBoxPriorFy, 1, 3, 1, 1);

    mLabelPriorCx = new QLabel(mGroupBoxPriorCalibration );
    mLabelPriorCx->setMaximumWidth(50);
    grid_layout_prior_calibration->addWidget(mLabelPriorCx, 2, 0, 1, 1);
    mDoubleSpinBoxPriorCx = new QDoubleSpinBox(mGroupBoxPriorCalibration );
    mDoubleSpinBoxPriorCx->setRange(-100000., 100000.);
    mDoubleSpinBoxPriorCx->setDecimals(8);
    grid_layout_prior_calibration->addWidget(mDoubleSpinBoxPriorCx, 2, 1, 1, 1);

    mLabelPriorCy = new QLabel(mGroupBoxPriorCalibration );
    mLabelPriorCy->setMaximumWidth(50);
    grid_layout_prior_calibration->addWidget(mLabelPriorCy, 2, 2, 1, 1);
    mDoubleSpinBoxPriorCy = new QDoubleSpinBox(mGroupBoxPriorCalibration );
    mDoubleSpinBoxPriorCy->setRange(-100000., 100000.);
    mDoubleSpinBoxPriorCy->setDecimals(8);
    grid_layout_prior_calibration->addWidget(mDoubleSpinBoxPriorCy, 2, 3, 1, 1);

    mLabelPriorK1 = new QLabel(mGroupBoxPriorCalibration );
    mLabelPriorK1->setMaximumWidth(50);
    grid_layout_prior_calibration->addWidget(mLabelPriorK1, 4, 0, 1, 1);
    mDoubleSpinBoxPriorK1 = new QDoubleSpinBox(mGroupBoxPriorCalibration );
    mDoubleSpinBoxPriorK1->setRange(-100000., 100000.);
    mDoubleSpinBoxPriorK1->setDecimals(8);
    grid_layout_prior_calibration->addWidget(mDoubleSpinBoxPriorK1, 4, 1, 1, 1);

    mLabelPriorK2 = new QLabel(mGroupBoxPriorCalibration );
    mLabelPriorK2->setMaximumWidth(50);
    grid_layout_prior_calibration->addWidget(mLabelPriorK2, 4, 2, 1, 1);
    mDoubleSpinBoxPriorK2 = new QDoubleSpinBox(mGroupBoxPriorCalibration );
    mDoubleSpinBoxPriorK2->setRange(-100000., 100000.);
    mDoubleSpinBoxPriorK2->setDecimals(8);
    grid_layout_prior_calibration->addWidget(mDoubleSpinBoxPriorK2, 4, 3, 1, 1);

    mLabelPriorK3 = new QLabel(mGroupBoxPriorCalibration );
    mLabelPriorK3->setMaximumWidth(50);
    grid_layout_prior_calibration->addWidget(mLabelPriorK3, 6, 0, 1, 1);
    mDoubleSpinBoxPriorK3 = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxPriorK3->setRange(-100000., 100000.);
    mDoubleSpinBoxPriorK3->setDecimals(8);
    grid_layout_prior_calibration->addWidget(mDoubleSpinBoxPriorK3, 6, 1, 1, 1);

    mLabelPriorK4 = new QLabel(mGroupBoxPriorCalibration );
    mLabelPriorK4->setMaximumWidth(50);
    grid_layout_prior_calibration->addWidget(mLabelPriorK4, 6, 2, 1, 1);
    mDoubleSpinBoxPriorK4 = new QDoubleSpinBox(mGroupBoxPriorCalibration );
    mDoubleSpinBoxPriorK4->setRange(-100000., 100000.);
    mDoubleSpinBoxPriorK4->setDecimals(8);
    grid_layout_prior_calibration->addWidget(mDoubleSpinBoxPriorK4, 6, 3, 1, 1);

    mLabelPriorK5 = new QLabel(mGroupBoxPriorCalibration );
    mLabelPriorK5->setMaximumWidth(50);
    grid_layout_prior_calibration->addWidget(mLabelPriorK5, 7, 0, 1, 1);
    mDoubleSpinBoxPriorK5 = new QDoubleSpinBox(mGroupBoxPriorCalibration );
    mDoubleSpinBoxPriorK5->setRange(-100000., 100000.);
    mDoubleSpinBoxPriorK5->setDecimals(8);
    grid_layout_prior_calibration->addWidget(mDoubleSpinBoxPriorK5, 7, 1, 1, 1);

    mLabelPriorK6 = new QLabel(mGroupBoxPriorCalibration );
    mLabelPriorK6->setMaximumWidth(50);
    grid_layout_prior_calibration->addWidget(mLabelPriorK6, 7, 2, 1, 1);
    mDoubleSpinBoxPriorK6 = new QDoubleSpinBox(mGroupBoxPriorCalibration );
    mDoubleSpinBoxPriorK6->setRange(-100000., 100000.);
    mDoubleSpinBoxPriorK6->setDecimals(8);
    grid_layout_prior_calibration->addWidget(mDoubleSpinBoxPriorK6, 7, 3, 1, 1);

    mLabelPriorP1 = new QLabel(mGroupBoxPriorCalibration );
    mLabelPriorP1->setMaximumWidth(50);
    grid_layout_prior_calibration->addWidget(mLabelPriorP1, 8, 0, 1, 1);
    mDoubleSpinBoxPriorP1 = new QDoubleSpinBox(mGroupBoxPriorCalibration );
    mDoubleSpinBoxPriorP1->setRange(-100000., 100000.);
    mDoubleSpinBoxPriorP1->setDecimals(8);
    grid_layout_prior_calibration->addWidget(mDoubleSpinBoxPriorP1, 8, 1, 1, 1);

    mLabelPriorP2 = new QLabel(mGroupBoxPriorCalibration );
    mLabelPriorP2->setMaximumWidth(50);
    grid_layout_prior_calibration->addWidget(mLabelPriorP2, 8, 2, 1, 1);
    mDoubleSpinBoxPriorP2 = new QDoubleSpinBox(mGroupBoxPriorCalibration );
    mDoubleSpinBoxPriorP2->setRange(-100000., 100000.);
    mDoubleSpinBoxPriorP2->setDecimals(8);
    grid_layout_prior_calibration->addWidget(mDoubleSpinBoxPriorP2, 8, 3, 1, 1);

    tabCalibLayout->addWidget(mGroupBoxPriorCalibration );


    // Adjusted Calibration

    mGroupBoxAdjustCalibration = new QGroupBox(mTabCalibration);
    QGridLayout *grid_layout_adjust_calibration = new QGridLayout(mGroupBoxAdjustCalibration);

    QToolBar *toolbar_adjust_calibration = new QToolBar(this);
    //mActionImportCalibration = new QAction(this);
    //mActionImportCalibration->setIcon(QIcon::fromTheme("folder"));
    //toolbar_adjust_calibration->addAction(mActionImportCalibration);

    mActionExportAdjustCalibration = new QAction(this);
    mActionExportAdjustCalibration->setIcon(QIcon::fromTheme("save"));
    toolbar_adjust_calibration->addAction(mActionExportAdjustCalibration);

    grid_layout_adjust_calibration->addWidget(toolbar_adjust_calibration, 0, 0, 1, 4);

    mLabelAdjustF = new QLabel(mGroupBoxPriorCalibration);
    mLabelAdjustF->setMaximumWidth(50);
    grid_layout_adjust_calibration->addWidget(mLabelAdjustF, 1, 0, 1, 1);
    mDoubleSpinBoxAdjustF = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxAdjustF->setRange(0., 100000.);
    mDoubleSpinBoxAdjustF->setDecimals(8);
    mDoubleSpinBoxAdjustF->setEnabled(false);
    grid_layout_adjust_calibration->addWidget(mDoubleSpinBoxAdjustF, 1, 1, 1, 3);

    mLabelAdjustFx = new QLabel(mGroupBoxPriorCalibration);
    mLabelAdjustFx->setMaximumWidth(50);
    grid_layout_adjust_calibration->addWidget(mLabelAdjustFx, 1, 0, 1, 1);
    mDoubleSpinBoxAdjustFx = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxAdjustFx->setRange(0., 100000.);
    mDoubleSpinBoxAdjustFx->setDecimals(8);
    mDoubleSpinBoxAdjustFx->setEnabled(false);
    grid_layout_adjust_calibration->addWidget(mDoubleSpinBoxAdjustFx, 1, 1, 1, 1);

    mLabelAdjustFy = new QLabel(mGroupBoxPriorCalibration);
    mLabelAdjustFy->setMaximumWidth(50);
    grid_layout_adjust_calibration->addWidget(mLabelAdjustFy, 1, 2, 1, 1);
    mDoubleSpinBoxAdjustFy = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxAdjustFy->setRange(0., 100000.);
    mDoubleSpinBoxAdjustFy->setDecimals(8);
    mDoubleSpinBoxAdjustFy->setEnabled(false);
    grid_layout_adjust_calibration->addWidget(mDoubleSpinBoxAdjustFy, 1, 3, 1, 1);

    mLabelAdjustCx = new QLabel(mGroupBoxPriorCalibration);
    mLabelAdjustCx->setMaximumWidth(50);
    grid_layout_adjust_calibration->addWidget(mLabelAdjustCx, 2, 0, 1, 1);
    mDoubleSpinBoxAdjustCx = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxAdjustCx->setRange(-100000., 100000.);
    mDoubleSpinBoxAdjustCx->setDecimals(8);
    mDoubleSpinBoxAdjustCx->setEnabled(false);
    grid_layout_adjust_calibration->addWidget(mDoubleSpinBoxAdjustCx, 2, 1, 1, 1);

    mLabelAdjustCy = new QLabel(mGroupBoxPriorCalibration);
    mLabelAdjustCy->setMaximumWidth(50);
    grid_layout_adjust_calibration->addWidget(mLabelAdjustCy, 2, 2, 1, 1);
    mDoubleSpinBoxAdjustCy = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxAdjustCy->setRange(-100000., 100000.);
    mDoubleSpinBoxAdjustCy->setDecimals(8);
    mDoubleSpinBoxAdjustCy->setEnabled(false);
    grid_layout_adjust_calibration->addWidget(mDoubleSpinBoxAdjustCy, 2, 3, 1, 1);

    mLabelAdjustK1 = new QLabel(mGroupBoxPriorCalibration);
    mLabelAdjustK1->setMaximumWidth(50);
    grid_layout_adjust_calibration->addWidget(mLabelAdjustK1, 4, 0, 1, 1);
    mDoubleSpinBoxAdjustK1 = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxAdjustK1->setRange(-100000., 100000.);
    mDoubleSpinBoxAdjustK1->setDecimals(8);
    mDoubleSpinBoxAdjustK1->setEnabled(false);
    grid_layout_adjust_calibration->addWidget(mDoubleSpinBoxAdjustK1, 4, 1, 1, 1);

    mLabelAdjustK2 = new QLabel(mGroupBoxPriorCalibration);
    mLabelAdjustK2->setMaximumWidth(50);
    grid_layout_adjust_calibration->addWidget(mLabelAdjustK2, 4, 2, 1, 1);
    mDoubleSpinBoxAdjustK2 = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxAdjustK2->setRange(-100000., 100000.);
    mDoubleSpinBoxAdjustK2->setDecimals(8);
    mDoubleSpinBoxAdjustK2->setEnabled(false);
    grid_layout_adjust_calibration->addWidget(mDoubleSpinBoxAdjustK2, 4, 3, 1, 1);

    mLabelAdjustK3 = new QLabel(mGroupBoxPriorCalibration);
    mLabelAdjustK3->setMaximumWidth(50);
    grid_layout_adjust_calibration->addWidget(mLabelAdjustK3, 6, 0, 1, 1);
    mDoubleSpinBoxAdjustK3 = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxAdjustK3->setRange(-100000., 100000.);
    mDoubleSpinBoxAdjustK3->setDecimals(8);
    mDoubleSpinBoxAdjustK3->setEnabled(false);
    grid_layout_adjust_calibration->addWidget(mDoubleSpinBoxAdjustK3, 6, 1, 1, 1);

    mLabelAdjustK4 = new QLabel(mGroupBoxPriorCalibration);
    mLabelAdjustK4->setMaximumWidth(50);
    grid_layout_adjust_calibration->addWidget(mLabelAdjustK4, 6, 2, 1, 1);
    mDoubleSpinBoxAdjustK4 = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxAdjustK4->setRange(-100000., 100000.);
    mDoubleSpinBoxAdjustK4->setDecimals(8);
    mDoubleSpinBoxAdjustK4->setEnabled(false);
    grid_layout_adjust_calibration->addWidget(mDoubleSpinBoxAdjustK4, 6, 3, 1, 1);

    mLabelAdjustK5 = new QLabel(mGroupBoxPriorCalibration);
    mLabelAdjustK5->setMaximumWidth(50);
    grid_layout_adjust_calibration->addWidget(mLabelAdjustK5, 7, 0, 1, 1);
    mDoubleSpinBoxAdjustK5 = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxAdjustK5->setRange(-100000., 100000.);
    mDoubleSpinBoxAdjustK5->setDecimals(8);
    mDoubleSpinBoxAdjustK5->setEnabled(false);
    grid_layout_adjust_calibration->addWidget(mDoubleSpinBoxAdjustK5, 7, 1, 1, 1);

    mLabelAdjustK6 = new QLabel(mGroupBoxPriorCalibration);
    mLabelAdjustK6->setMaximumWidth(50);
    grid_layout_adjust_calibration->addWidget(mLabelAdjustK6, 7, 2, 1, 1);
    mDoubleSpinBoxAdjustK6 = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxAdjustK6->setRange(-100000., 100000.);
    mDoubleSpinBoxAdjustK6->setDecimals(8);
    mDoubleSpinBoxAdjustK6->setEnabled(false);
    grid_layout_adjust_calibration->addWidget(mDoubleSpinBoxAdjustK6, 7, 3, 1, 1);

    mLabelAdjustP1 = new QLabel(mGroupBoxPriorCalibration);
    mLabelAdjustP1->setMaximumWidth(50);
    grid_layout_adjust_calibration->addWidget(mLabelAdjustP1, 8, 0, 1, 1);
    mDoubleSpinBoxAdjustP1 = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxAdjustP1->setRange(-100000., 100000.);
    mDoubleSpinBoxAdjustP1->setDecimals(8);
    mDoubleSpinBoxAdjustP1->setEnabled(false);
    grid_layout_adjust_calibration->addWidget(mDoubleSpinBoxAdjustP1, 8, 1, 1, 1);

    mLabelAdjustP2 = new QLabel(mGroupBoxPriorCalibration);
    mLabelAdjustP2->setMaximumWidth(50);
    grid_layout_adjust_calibration->addWidget(mLabelAdjustP2, 8, 2, 1, 1);
    mDoubleSpinBoxAdjustP2 = new QDoubleSpinBox(mGroupBoxPriorCalibration);
    mDoubleSpinBoxAdjustP2->setRange(-100000., 100000.);
    mDoubleSpinBoxAdjustP2->setDecimals(8);
    mDoubleSpinBoxAdjustP2->setEnabled(false);
    grid_layout_adjust_calibration->addWidget(mDoubleSpinBoxAdjustP2, 8, 3, 1, 1);
    
    tabCalibLayout->addWidget(mGroupBoxAdjustCalibration);

    mTabWidget->addTab(mTabCalibration, "");

    //layout->addWidget(mGroupBoxPriorCalibration , 4, 1, 1, 4);

    //mLabelImages = new QLabel(this);
    //layout->addWidget(mLabelImages, 5, 0, 1, 1);

    //mListWidgetImages = new QListWidget(this);
    //layout->addWidget(mListWidgetImages, 6, 0, 1, 5);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setObjectName(QStringLiteral("buttonBox"));
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Help | QDialogButtonBox::Ok);

    layout->addWidget(mButtonBox, 3, 0, 1, 5);

    this->retranslate();
    this->clear();
    this->update();
}

void CamerasViewImp::initSignalAndSlots()
{
    connect(mListWidgetCameras, &QListWidget::itemSelectionChanged, this, &CamerasViewImp::onSelectionChanged);
    connect(mLineEditMake, &QLineEdit::textEdited, this, &CamerasView::makeChanged);
    connect(mLineEditModel, &QLineEdit::textEdited, this, &CamerasView::modelChanged);
    connect(mLineEditSensorSize, &QLineEdit::textEdited, this, &CamerasView::sensorSizeChange);
    connect(mLineEditFocal, &QLineEdit::textEdited, this, &CamerasView::focalChange);
    connect(mComboBoxType, &QComboBox::currentTextChanged, this, &CamerasView::typeChange);
    connect(mComboBoxType, &QComboBox::currentTextChanged, this, &CamerasViewImp::update);
    connect(mPushButtonEditCamera, &QPushButton::toggled, this, &CamerasViewImp::enableCameraEdition);

    connect(mActionImportPriorCalibration, &QAction::triggered, this, &CamerasViewImp::onPriorCalibrationImport);
    connect(mActionExportPriorCalibration, &QAction::triggered, this, &CamerasViewImp::onPriorCalibrationExport);
    connect(mActionExportAdjustCalibration, &QAction::triggered, this, &CamerasViewImp::onAdjustCalibrationExport);

    connect(mDoubleSpinBoxPriorCx, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibPriorCxChange);
    connect(mDoubleSpinBoxPriorCy, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibPriorCyChange);
    connect(mDoubleSpinBoxPriorF, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibPriorFChange);
    connect(mDoubleSpinBoxPriorFx, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibPriorFxChange);
    connect(mDoubleSpinBoxPriorFy, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibPriorFyChange);
    connect(mDoubleSpinBoxPriorK1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibPriorK1Change);
    connect(mDoubleSpinBoxPriorK2, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibPriorK2Change);
    connect(mDoubleSpinBoxPriorK3, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibPriorK3Change);
    connect(mDoubleSpinBoxPriorK4, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibPriorK4Change);
    connect(mDoubleSpinBoxPriorK5, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibPriorK5Change);
    connect(mDoubleSpinBoxPriorK6, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibPriorK6Change);
    connect(mDoubleSpinBoxPriorP1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibPriorP1Change);
    connect(mDoubleSpinBoxPriorP2, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibPriorP2Change);

    //connect(mDoubleSpinBoxAdjustCx, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibAdjustCxChange);
    //connect(mDoubleSpinBoxAdjustCy, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibAdjustCyChange);
    //connect(mDoubleSpinBoxAdjustF, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibAdjustFChange);
    //connect(mDoubleSpinBoxAdjustFx, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibAdjustFxChange);
    //connect(mDoubleSpinBoxAdjustFy, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibAdjustFyChange);
    //connect(mDoubleSpinBoxAdjustK1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibAdjustK1Change);
    //connect(mDoubleSpinBoxAdjustK2, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibAdjustK2Change);
    //connect(mDoubleSpinBoxAdjustK3, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibAdjustK3Change);
    //connect(mDoubleSpinBoxAdjustK4, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibAdjustK4Change);
    //connect(mDoubleSpinBoxAdjustK5, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibAdjustK5Change);
    //connect(mDoubleSpinBoxAdjustK6, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibAdjustK6Change);
    //connect(mDoubleSpinBoxAdjustP1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibAdjustP1Change);
    //connect(mDoubleSpinBoxAdjustP2, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibAdjustP2Change);

    connect(mButtonBox->button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this, &QDialog::accept);
    connect(mButtonBox->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void CamerasViewImp::clear()
{
    const QSignalBlocker blocker(mListWidgetCameras);
    const QSignalBlocker blocker1(mLineEditMake);
    const QSignalBlocker blocker2(mLineEditModel);
    const QSignalBlocker blocker3(mSpinBoxWidth);
    const QSignalBlocker blocker4(mSpinBoxHeight);
    const QSignalBlocker blocker5(mLineEditSensorSize);
    const QSignalBlocker blocker6(mLineEditFocal);
    const QSignalBlocker blocker7(mSpinBoxWidth);
    const QSignalBlocker blocker8(mSpinBoxHeight);
    const QSignalBlocker blocker9(mDoubleSpinBoxPriorCx);
    const QSignalBlocker blocker10(mDoubleSpinBoxPriorCy);
    const QSignalBlocker blocker11(mDoubleSpinBoxPriorF);
    const QSignalBlocker blocker12(mDoubleSpinBoxPriorFx);
    const QSignalBlocker blocker13(mDoubleSpinBoxPriorFy);
    const QSignalBlocker blocker14(mDoubleSpinBoxPriorK1);
    const QSignalBlocker blocker15(mDoubleSpinBoxPriorK2);
    const QSignalBlocker blocker16(mDoubleSpinBoxPriorK3);
    const QSignalBlocker blocker17(mDoubleSpinBoxPriorK4);
    const QSignalBlocker blocker18(mDoubleSpinBoxPriorK5);
    const QSignalBlocker blocker19(mDoubleSpinBoxPriorK6);
    const QSignalBlocker blocker20(mDoubleSpinBoxPriorP1);
    const QSignalBlocker blocker21(mDoubleSpinBoxPriorP2);
    //const QSignalBlocker blocker22(mDoubleSpinBoxAdjustCx);
    //const QSignalBlocker blocker23(mDoubleSpinBoxAdjustCy);
    //const QSignalBlocker blocker24(mDoubleSpinBoxAdjustF);
    //const QSignalBlocker blocker25(mDoubleSpinBoxAdjustFx);
    //const QSignalBlocker blocker26(mDoubleSpinBoxAdjustFy);
    //const QSignalBlocker blocker27(mDoubleSpinBoxAdjustK1);
    //const QSignalBlocker blocker28(mDoubleSpinBoxAdjustK2);
    //const QSignalBlocker blocker29(mDoubleSpinBoxAdjustK3);
    //const QSignalBlocker blocker30(mDoubleSpinBoxAdjustK4);
    //const QSignalBlocker blocker31(mDoubleSpinBoxAdjustK5);
    //const QSignalBlocker blocker32(mDoubleSpinBoxAdjustK6);
    //const QSignalBlocker blocker33(mDoubleSpinBoxAdjustP1);
    //const QSignalBlocker blocker34(mDoubleSpinBoxAdjustP2);

    mListWidgetCameras->clear();
    mListWidgetImages->clear();

    mLineEditMake->setText("");
    mLineEditModel->setText("");
    mSpinBoxWidth->setValue(0);
    mSpinBoxHeight->setValue(0);
    mLineEditSensorSize->setText("");
    mLineEditFocal->setText("");
    mSpinBoxWidth->setEnabled(false);
    mSpinBoxHeight->setEnabled(false);
    this->enableCameraEdition(false);
    mDoubleSpinBoxPriorCx->setValue(0.);
    mDoubleSpinBoxPriorCy->setValue(0.);
    mDoubleSpinBoxPriorF->setValue(0.);
    mDoubleSpinBoxPriorFx->setValue(0.);
    mDoubleSpinBoxPriorFy->setValue(0.);
    mDoubleSpinBoxPriorK1->setValue(0.);
    mDoubleSpinBoxPriorK2->setValue(0.);
    mDoubleSpinBoxPriorK3->setValue(0.);
    mDoubleSpinBoxPriorK4->setValue(0.);
    mDoubleSpinBoxPriorK5->setValue(0.);
    mDoubleSpinBoxPriorK6->setValue(0.);
    mDoubleSpinBoxPriorP1->setValue(0.);
    mDoubleSpinBoxPriorP2->setValue(0.);

    mDoubleSpinBoxAdjustCx->setValue(0.);
    mDoubleSpinBoxAdjustCy->setValue(0.);
    mDoubleSpinBoxAdjustF->setValue(0.);
    mDoubleSpinBoxAdjustFx->setValue(0.);
    mDoubleSpinBoxAdjustFy->setValue(0.);
    mDoubleSpinBoxAdjustK1->setValue(0.);
    mDoubleSpinBoxAdjustK2->setValue(0.);
    mDoubleSpinBoxAdjustK3->setValue(0.);
    mDoubleSpinBoxAdjustK4->setValue(0.);
    mDoubleSpinBoxAdjustK5->setValue(0.);
    mDoubleSpinBoxAdjustK6->setValue(0.);
    mDoubleSpinBoxAdjustP1->setValue(0.);
    mDoubleSpinBoxAdjustP2->setValue(0.);
}

void CamerasViewImp::update()
{
    bool selected_camera = !mListWidgetCameras->selectedItems().empty();
    bool cx_enable = isPrincipalPointEnable();
    bool cy_enable = isPrincipalPointEnable();
    bool f_enable = isFocalEnable();
    bool fx_enable = isFocalXYEnable();
    bool fy_enable = isFocalXYEnable();
    bool k1_enable = isK1Enable();
    bool k2_enable = isK2Enable();
    bool k3_enable = isK3Enable();
    bool k4_enable = isK4Enable();
    bool k5_enable = isK5Enable();
    bool k6_enable = isK6Enable();
    bool p1_enable = isP1Enable();
    bool p2_enable = isP2Enable();

    mComboBoxType->setEnabled(selected_camera);
    mPushButtonEditCamera->setEnabled(selected_camera);
    mTabCalibration->setEnabled(selected_camera);
    mTabInfo->setEnabled(selected_camera);

    mDoubleSpinBoxPriorF->setEnabled(selected_camera && f_enable);
    mDoubleSpinBoxPriorFx->setEnabled(selected_camera && fx_enable);
    mDoubleSpinBoxPriorFy->setEnabled(selected_camera && fy_enable);
    mDoubleSpinBoxPriorCx->setEnabled(selected_camera && cx_enable);
    mDoubleSpinBoxPriorCy->setEnabled(selected_camera && cy_enable);
    mDoubleSpinBoxPriorK1->setEnabled(selected_camera && k1_enable);
    mDoubleSpinBoxPriorK2->setEnabled(selected_camera && k2_enable);
    mDoubleSpinBoxPriorK3->setEnabled(selected_camera && k3_enable);
    mDoubleSpinBoxPriorK4->setEnabled(selected_camera && k4_enable);
    mDoubleSpinBoxPriorK5->setEnabled(selected_camera && k5_enable);
    mDoubleSpinBoxPriorK6->setEnabled(selected_camera && k6_enable);
    mDoubleSpinBoxPriorP1->setEnabled(selected_camera && p1_enable);
    mDoubleSpinBoxPriorP2->setEnabled(selected_camera && p2_enable);

    mLabelPriorF->setVisible(isFocalVisible());
    mDoubleSpinBoxPriorF->setVisible(isFocalVisible());
    mLabelPriorFx->setVisible(isFocalXYVisible());
    mDoubleSpinBoxPriorFx->setVisible(isFocalXYVisible());
    mLabelPriorFy->setVisible(isFocalXYVisible());
    mDoubleSpinBoxPriorFy->setVisible(isFocalXYVisible());

    //mDoubleSpinBoxAdjustF->setEnabled(selected_camera && f_enable);
    //mDoubleSpinBoxAdjustFx->setEnabled(selected_camera && fx_enable);
    //mDoubleSpinBoxAdjustFy->setEnabled(selected_camera && fy_enable);
    //mDoubleSpinBoxAdjustCx->setEnabled(selected_camera && cx_enable);
    //mDoubleSpinBoxAdjustCy->setEnabled(selected_camera && cy_enable);
    //mDoubleSpinBoxAdjustK1->setEnabled(selected_camera && k1_enable);
    //mDoubleSpinBoxAdjustK2->setEnabled(selected_camera && k2_enable);
    //mDoubleSpinBoxAdjustK3->setEnabled(selected_camera && k3_enable);
    //mDoubleSpinBoxAdjustK4->setEnabled(selected_camera && k4_enable);
    //mDoubleSpinBoxAdjustK5->setEnabled(selected_camera && k5_enable);
    //mDoubleSpinBoxAdjustK6->setEnabled(selected_camera && k6_enable);
    //mDoubleSpinBoxAdjustP1->setEnabled(selected_camera && p1_enable);
    //mDoubleSpinBoxAdjustP2->setEnabled(selected_camera && p2_enable);

    mLabelAdjustF->setVisible(isFocalVisible());
    mDoubleSpinBoxAdjustF->setVisible(isFocalVisible());
    mLabelAdjustFx->setVisible(isFocalXYVisible());
    mDoubleSpinBoxAdjustFx->setVisible(isFocalXYVisible());
    mLabelAdjustFy->setVisible(isFocalXYVisible());
    mDoubleSpinBoxAdjustFy->setVisible(isFocalXYVisible());

    mActionImportPriorCalibration->setEnabled(selected_camera);
    // Hay que comprobar que exista calibración previa
    mActionExportPriorCalibration->setEnabled(selected_camera);
    mActionExportAdjustCalibration->setEnabled(selected_camera);

}

void CamerasViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("CamerasComponent", "Cameras", nullptr));
    mTabWidget->setTabText(0, QApplication::translate("CamerasComponent", "Camera info", nullptr));
    mTabWidget->setTabText(1, QApplication::translate("CamerasComponent", "Calibrations", nullptr));
    mGroupBoxPriorCalibration ->setTitle(QApplication::translate("CamerasComponent", "Prior Calibration", nullptr));
    mGroupBoxAdjustCalibration->setTitle(QApplication::translate("CamerasComponent", "Adjusted Calibration", nullptr));
    mActionImportPriorCalibration->setText(QApplication::translate("CamerasComponent", "Import prior calibration", nullptr));
    mActionImportPriorCalibration->setStatusTip(QApplication::translate("CamerasComponent", "Import prior calibration", nullptr));
    mActionExportPriorCalibration->setText(QApplication::translate("CamerasComponent", "Export prior calibration", nullptr));
    mActionExportPriorCalibration->setStatusTip(QApplication::translate("CamerasComponent", "Export prior calibration", nullptr));
    mActionExportPriorCalibration->setText(QApplication::translate("CamerasComponent", "Export adjusted calibration", nullptr));
    mActionExportPriorCalibration->setStatusTip(QApplication::translate("CamerasComponent", "Export adjusted calibration", nullptr));
    mLabelPriorF->setText(QApplication::translate("CamerasComponent", "f", nullptr));
    mLabelPriorFx->setText(QApplication::translate("CamerasComponent", "fx", nullptr));
    mLabelPriorFy->setText(QApplication::translate("CamerasComponent", "fy", nullptr));
    mLabelPriorCx->setText(QApplication::translate("CamerasComponent", "cx", nullptr));
    mLabelPriorCy->setText(QApplication::translate("CamerasComponent", "cy", nullptr));
    mLabelPriorK1->setText(QApplication::translate("CamerasComponent", "k1", nullptr));
    mLabelPriorK2->setText(QApplication::translate("CamerasComponent", "k2", nullptr));
    mLabelPriorK3->setText(QApplication::translate("CamerasComponent", "k3", nullptr));
    mLabelPriorK4->setText(QApplication::translate("CamerasComponent", "k4", nullptr));
    mLabelPriorK5->setText(QApplication::translate("CamerasComponent", "k5", nullptr));
    mLabelPriorK6->setText(QApplication::translate("CamerasComponent", "k6", nullptr));
    mLabelPriorP1->setText(QApplication::translate("CamerasComponent", "p1", nullptr));
    mLabelPriorP2->setText(QApplication::translate("CamerasComponent", "p2", nullptr));
    mLabelAdjustF->setText(QApplication::translate("CamerasComponent", "f", nullptr));
    mLabelAdjustFx->setText(QApplication::translate("CamerasComponent", "fx", nullptr));
    mLabelAdjustFy->setText(QApplication::translate("CamerasComponent", "fy", nullptr));
    mLabelAdjustCx->setText(QApplication::translate("CamerasComponent", "cx", nullptr));
    mLabelAdjustCy->setText(QApplication::translate("CamerasComponent", "cy", nullptr));
    mLabelAdjustK1->setText(QApplication::translate("CamerasComponent", "k1", nullptr));
    mLabelAdjustK2->setText(QApplication::translate("CamerasComponent", "k2", nullptr));
    mLabelAdjustK3->setText(QApplication::translate("CamerasComponent", "k3", nullptr));
    mLabelAdjustK4->setText(QApplication::translate("CamerasComponent", "k4", nullptr));
    mLabelAdjustK5->setText(QApplication::translate("CamerasComponent", "k5", nullptr));
    mLabelAdjustK6->setText(QApplication::translate("CamerasComponent", "k6", nullptr));
    mLabelAdjustP1->setText(QApplication::translate("CamerasComponent", "p1", nullptr));
    mLabelAdjustP2->setText(QApplication::translate("CamerasComponent", "p2", nullptr));
    mLabelImages->setText(QApplication::translate("CamerasComponent", "Images:", nullptr));
    mLabelCameras->setText(QApplication::translate("CamerasComponent", "Cameras:", nullptr));
    mGroupBoxCamera->setTitle(QApplication::translate("CamerasComponent", "Camera", nullptr));
    mLabelType->setText(QApplication::translate("CamerasComponent", "Type", nullptr));
    mLabelWidth->setText(QApplication::translate("CamerasComponent", "Width (pixels)", nullptr));
    mLabelSensorSize->setText(QApplication::translate("CamerasComponent", "Sensor size (mm)", nullptr));
    mLabelFocal->setText(QApplication::translate("CamerasComponent", "Focal", nullptr));
    //mComboBoxType->setItemText(radial_1, QApplication::translate("CamerasComponent", "Radial 1", nullptr));
    //mComboBoxType->setItemText(radial_2, QApplication::translate("CamerasComponent", "Radial 2", nullptr));
    //mComboBoxType->setItemText(pinhole_1, QApplication::translate("CamerasComponent", "Pinhole 1", nullptr));
    //mComboBoxType->setItemText(pinhole_2, QApplication::translate("CamerasComponent", "Pinhole 2", nullptr));
    //mComboBoxType->setItemText(opencv_1, QApplication::translate("CamerasComponent", "OpenCV 1", nullptr));
    //mComboBoxType->setItemText(opencv_2, QApplication::translate("CamerasComponent", "OpenCV 2", nullptr));
    //mComboBoxType->setItemText(opencv_fisheye, QApplication::translate("CamerasComponent", "OpenCV Fisheye", nullptr));
    //mComboBoxType->setItemText(radial_fisheye_1, QApplication::translate("CamerasComponent", "Radial Fisheye 1", nullptr));
    //mComboBoxType->setItemText(radial_fisheye_2, QApplication::translate("CamerasComponent", "Radial Fisheye 2", nullptr));

    mLabelMake->setText(QApplication::translate("CamerasComponent", "Make", nullptr));
    mLabelModel->setText(QApplication::translate("CamerasComponent", "Model", nullptr));
    mLabelHeight->setText(QApplication::translate("CamerasComponent", "Height (pixels)", nullptr));
    mPushButtonEditCamera->setText(QApplication::translate("CamerasComponent", "Edit Camera", nullptr));

    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("CamerasComponent", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Ok)->setText(QApplication::translate("CamerasComponent", "Ok"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("CamerasComponent", "Help"));
}

auto CamerasViewImp::activeCamera() const -> int
{
    QList<QListWidgetItem *> itemsCameras = mListWidgetCameras->selectedItems();
    if (itemsCameras.size() == 1)
        return itemsCameras.at(0)->data(Qt::UserRole).toInt();
    else
        return -1;
}

void CamerasViewImp::setActiveCamera(int id)
{
    const QSignalBlocker blocker(mListWidgetCameras);
    for (int i = 0; i < mListWidgetCameras->count(); i++) {
        if (mListWidgetCameras->item(i)->data(Qt::UserRole).toInt() == id) {
            mListWidgetCameras->item(i)->setSelected(true);
            break;
        }
    }
}

void CamerasViewImp::enableCameraEdition(bool enable)
{
    mLineEditMake->setEnabled(enable);
    mLineEditModel->setEnabled(enable);
    mLineEditSensorSize->setEnabled(enable);
    mLineEditFocal->setEnabled(enable);
}

void CamerasViewImp::addCamera(int cameraId, const QString &cameraName)
{
    const QSignalBlocker blocker(mListWidgetCameras);
    QListWidgetItem *item = new QListWidgetItem(cameraName);
    item->setData(Qt::UserRole, QVariant(cameraId));
    mListWidgetCameras->addItem(item);
}

void CamerasViewImp::setMake(const QString &make)
{
    mLineEditMake->setText(make);
}

void CamerasViewImp::setModel(const QString &model)
{
    mLineEditModel->setText(model);
}

void CamerasViewImp::setWidth(int width)
{
    mSpinBoxWidth->setValue(width);
}

void CamerasViewImp::setHeight(int height)
{
    mSpinBoxHeight->setValue(height);
}

void CamerasViewImp::setSensorSize(const QString &sensorSize)
{
    mLineEditSensorSize->setText(sensorSize);
}

void CamerasViewImp::setFocal(const QString &focal)
{
    mLineEditFocal->setText(focal);
}

void CamerasViewImp::setType(const QString &type)
{
    const QSignalBlocker blockerType(mComboBoxType);

    mComboBoxType->setCurrentText(type);

    update();
}

void CamerasViewImp::setImages(const QStringList &images)
{
    mListWidgetImages->clear();
    mListWidgetImages->addItems(images);
}

void CamerasViewImp::setPriorCalibCx(double cx)
{
    mDoubleSpinBoxPriorCx->setValue(cx);
}

void CamerasViewImp::setPriorCalibCy(double cy)
{
    mDoubleSpinBoxPriorCy->setValue(cy);
}

void CamerasViewImp::setPriorCalibF(double f)
{
    mDoubleSpinBoxPriorF->setValue(f);
}

void CamerasViewImp::setPriorCalibFx(double fx)
{
    mDoubleSpinBoxPriorFx->setValue(fx);
}

void CamerasViewImp::setPriorCalibFy(double fy)
{
    mDoubleSpinBoxPriorFy->setValue(fy);
}

void CamerasViewImp::setPriorCalibK1(double k1)
{
    mDoubleSpinBoxPriorK1->setValue(k1);
}

void CamerasViewImp::setPriorCalibK2(double k2)
{
    mDoubleSpinBoxPriorK2->setValue(k2);
}

void CamerasViewImp::setPriorCalibK3(double k3)
{
    mDoubleSpinBoxPriorK3->setValue(k3);
}

void CamerasViewImp::setPriorCalibK4(double k4)
{
    mDoubleSpinBoxPriorK4->setValue(k4);
}

void CamerasViewImp::setPriorCalibK5(double k5)
{
    mDoubleSpinBoxPriorK5->setValue(k5);
}

void CamerasViewImp::setPriorCalibK6(double k6)
{
    mDoubleSpinBoxPriorK6->setValue(k6);
}

void CamerasViewImp::setPriorCalibP1(double p1)
{
    mDoubleSpinBoxPriorP1->setValue(p1);
}

void CamerasViewImp::setPriorCalibP2(double p2)
{
    mDoubleSpinBoxPriorP2->setValue(p2);
}

void CamerasViewImp::setAdjustCalibCx(double cx)
{
    mDoubleSpinBoxAdjustCx->setValue(cx);
}

void CamerasViewImp::setAdjustCalibCy(double cy)
{
    mDoubleSpinBoxAdjustCy->setValue(cy);
}

void CamerasViewImp::setAdjustCalibF(double f)
{
    mDoubleSpinBoxAdjustF->setValue(f);
}

void CamerasViewImp::setAdjustCalibFx(double fx)
{
    mDoubleSpinBoxAdjustFx->setValue(fx);
}

void CamerasViewImp::setAdjustCalibFy(double fy)
{
    mDoubleSpinBoxAdjustFy->setValue(fy);
}

void CamerasViewImp::setAdjustCalibK1(double k1)
{
    mDoubleSpinBoxAdjustK1->setValue(k1);
}

void CamerasViewImp::setAdjustCalibK2(double k2)
{
    mDoubleSpinBoxAdjustK2->setValue(k2);
}

void CamerasViewImp::setAdjustCalibK3(double k3)
{
    mDoubleSpinBoxAdjustK3->setValue(k3);
}

void CamerasViewImp::setAdjustCalibK4(double k4)
{
    mDoubleSpinBoxAdjustK4->setValue(k4);
}

void CamerasViewImp::setAdjustCalibK5(double k5)
{
    mDoubleSpinBoxAdjustK5->setValue(k5);
}

void CamerasViewImp::setAdjustCalibK6(double k6)
{
    mDoubleSpinBoxAdjustK6->setValue(k6);
}

void CamerasViewImp::setAdjustCalibP1(double p1)
{
    mDoubleSpinBoxAdjustP1->setValue(p1);
}

void CamerasViewImp::setAdjustCalibP2(double p2)
{
    mDoubleSpinBoxAdjustP2->setValue(p2);
}

void CamerasViewImp::onSelectionChanged()
{
    QList<QListWidgetItem *> itemsCameras = mListWidgetCameras->selectedItems();
    if (itemsCameras.size() == 1) {
        emit cameraChange(itemsCameras.at(0)->data(Qt::UserRole).toInt());
    }
}

auto CamerasViewImp::isPrincipalPointEnable() const -> bool
{
    int camera_index = mComboBoxType->currentIndex();
    return camera_index == radial_1 ||
           camera_index == radial_2 ||
           camera_index == pinhole_1 ||
           camera_index == pinhole_2 ||
           camera_index == opencv_1 ||
           camera_index == opencv_2 ||
           camera_index == opencv_fisheye ||
           camera_index == radial_fisheye_1 ||
           camera_index == radial_fisheye_2;
}

auto CamerasViewImp::isFocalEnable() const -> bool
{
    int camera_index = mComboBoxType->currentIndex();
    return camera_index == radial_1 ||
           camera_index == radial_2 ||
           camera_index == pinhole_1 ||
           camera_index == radial_fisheye_1 ||
           camera_index == radial_fisheye_2;
}

bool CamerasViewImp::isFocalXYEnable() const
{
    int camera_index = mComboBoxType->currentIndex();
    return camera_index == pinhole_2 ||
           camera_index == opencv_1 ||
           camera_index == opencv_2 ||
           camera_index == opencv_fisheye;
}

auto CamerasViewImp::isK1Enable() const -> bool
{
    int camera_index = mComboBoxType->currentIndex();
    return camera_index == radial_1 ||
           camera_index == radial_2 ||
           camera_index == opencv_1 ||
           camera_index == opencv_2 ||
           camera_index == opencv_fisheye ||
           camera_index == radial_fisheye_1 ||
           camera_index == radial_fisheye_2;
}

auto CamerasViewImp::isK2Enable() const -> bool
{
    int camera_index = mComboBoxType->currentIndex();
    return camera_index == radial_2 ||
           camera_index == opencv_1 ||
           camera_index == opencv_2 ||
           camera_index == opencv_fisheye ||
           camera_index == radial_fisheye_2;
}

auto CamerasViewImp::isK3Enable() const -> bool
{
    int camera_index = mComboBoxType->currentIndex();
    return camera_index == opencv_2 ||
           camera_index == opencv_fisheye;
}

auto CamerasViewImp::isK4Enable() const -> bool
{
    int camera_index = mComboBoxType->currentIndex();
    return camera_index == opencv_2 ||
           camera_index == opencv_fisheye;
}

auto CamerasViewImp::isK5Enable() const -> bool
{
    int camera_index = mComboBoxType->currentIndex();
    return camera_index == opencv_2;
}

auto CamerasViewImp::isK6Enable() const -> bool
{
    int camera_index = mComboBoxType->currentIndex();
    return camera_index == opencv_2;
}

auto CamerasViewImp::isP1Enable() const -> bool
{
    int camera_index = mComboBoxType->currentIndex();
    return camera_index == opencv_1 ||
           camera_index == opencv_2;
}

auto CamerasViewImp::isP2Enable() const -> bool
{
    int camera_index = mComboBoxType->currentIndex();
    return camera_index == opencv_1 ||
           camera_index == opencv_2;
}

auto CamerasViewImp::isFocalVisible() const -> bool
{
    int camera_index = mComboBoxType->currentIndex();
    return camera_index == pinhole_1 ||
           camera_index == radial_1 ||
           camera_index == radial_2 ||
           camera_index == radial_fisheye_1 ||
           camera_index == radial_fisheye_2;
}

auto CamerasViewImp::isFocalXYVisible() const -> bool
{
    return !isFocalVisible();
}

} // namespace graphos
