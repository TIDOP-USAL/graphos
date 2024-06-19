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

#include <tidop/core/defs.h>
#include <tidop/core/msg/message.h>

namespace graphos
{


CamerasViewImp::CamerasViewImp(QWidget *parent)
  : CamerasView(parent)
{
    CamerasViewImp::initUI();
    CamerasViewImp::initSignalAndSlots();
}

void CamerasViewImp::onCalibrationImport()
{
    QString selected_filter;
    QString path_name = QFileDialog::getOpenFileName(nullptr,
                                                    tr("Import calibration"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    //tr("Pix4D Camera Calibration (*.cam);;Agisoft Camera Calibration (*.xml);;OpenCV Camera Calibration (*.xml)"),
                                                    tr("Agisoft Camera Calibration (*.xml);;OpenCV Camera Calibration (*.xml)"),
                                                    &selected_filter);

    if (!path_name.isEmpty()) {
        QString format;
        /*if (selectedFilter.compare("Pix4D Camera Calibration (*.cam)") == 0) {
          format = "Pix4D";
        } else */if (selected_filter.compare("Agisoft Camera Calibration (*.xml)") == 0) {
            format = "Agisoft";
        } else if (selected_filter.compare("OpenCV Camera Calibration (*.xml)") == 0) {
            format = "OpenCV";
        } else {
            tl::Message::error("Unsupported format");
        }

        emit calibrationImport(path_name, format);
    }
}

void CamerasViewImp::onCalibrationExport()
{
    QString selected_filter;
    QString path_name = QFileDialog::getSaveFileName(nullptr,
                                                    tr("Export Calibration"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    //tr("Pix4D Camera Calibration (*.cam);;Agisoft Camera Calibration (*.xml);;OpenCV Camera Calibration (*.xml)"),
                                                    tr("Agisoft Camera Calibration (*.xml);;OpenCV Camera Calibration (*.xml)"),
                                                    &selected_filter);

    if (!path_name.isEmpty()) {

        QString format;
        /*if (selectedFilter.compare("Pix4D Camera Calibration (*.cam)") == 0) {
          format = "Pix4D";
        } else */if (selected_filter.compare("Agisoft Camera Calibration (*.xml)") == 0) {
            format = "Agisoft";
        } else if (selected_filter.compare("OpenCV Camera Calibration (*.xml)") == 0) {
            format = "OpenCV";
        } else {
            tl::Message::error("Unsupported format");
        }

        emit calibrationExport(path_name, format);
    }
}

void CamerasViewImp::initUI()
{
    this->setObjectName(QString("CamerasView"));

    this->resize(800, 660);

    QGridLayout *layout = new QGridLayout();
    this->setLayout(layout);

    mLabelCameras = new QLabel(this);
    layout->addWidget(mLabelCameras, 0, 0, 1, 1);

    mListWidgetCameras = new QListWidget(this);
    mListWidgetCameras->setMaximumSize(QSize(250, 16777215));
    layout->addWidget(mListWidgetCameras, 1, 0, 4, 1);

    mGroupBoxCamera = new QGroupBox(this);
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
    mComboBoxType->addItem(QString());
    mComboBoxType->addItem(QString());
    //mComboBoxType->addItem(QString());
    mComboBoxType->addItem(QString());
    mComboBoxType->addItem(QString());
    mComboBoxType->addItem(QString());
    mComboBoxType->addItem(QString());
    mComboBoxType->addItem(QString());
    mComboBoxType->addItem(QString());
    mComboBoxType->addItem(QString());
    gridLayout3->addWidget(mComboBoxType, 7, 1, 1, 5);

    mPushButtonEditCamera = new QPushButton(mGroupBoxCamera);
    mPushButtonEditCamera->setCheckable(true);
    gridLayout3->addWidget(mPushButtonEditCamera, 8, 1);

    layout->addWidget(mGroupBoxCamera, 1, 1, 2, 4);

    mGroupBoxCalibrationParameters = new QGroupBox(this);
    QGridLayout *gridLayout2 = new QGridLayout(mGroupBoxCalibrationParameters);

    QToolBar *toolBar = new QToolBar(this);

    mActionImportCalibration = new QAction(this);
    mActionImportCalibration->setIcon(QIcon::fromTheme("folder"));
    toolBar->addAction(mActionImportCalibration);

    mActionExportCalibration = new QAction(this);
    mActionExportCalibration->setIcon(QIcon::fromTheme("save"));
    toolBar->addAction(mActionExportCalibration);

    //toolBar->addSeparator();

    //QIcon ico;
    //ico.addPixmap(QPixmap(":/ico/24/img/material/24/icons8_lock_24px.png"), QIcon::Normal, QIcon::On);
    //ico.addPixmap(QPixmap(":/ico/24/img/material/24/icons8_padlock_24px.png"), QIcon::Normal, QIcon::Off);
    //mActionFixCalibration = new QAction(this);
    //mActionFixCalibration->setIcon(ico);
    //mActionFixCalibration->setCheckable(true);
    //toolBar->addAction(mActionFixCalibration);

    gridLayout2->addWidget(toolBar, 0, 0, 1, 4);

    mLabelF = new QLabel(mGroupBoxCalibrationParameters);
    mLabelF->setMaximumWidth(50);
    gridLayout2->addWidget(mLabelF, 1, 0, 1, 1);
    mDoubleSpinBoxF = new QDoubleSpinBox(mGroupBoxCalibrationParameters);
    mDoubleSpinBoxF->setRange(0., 100000.);
    mDoubleSpinBoxF->setDecimals(8);
    gridLayout2->addWidget(mDoubleSpinBoxF, 1, 1, 1, 3);

    mLabelFx = new QLabel(mGroupBoxCalibrationParameters);
    mLabelFx->setMaximumWidth(50);
    gridLayout2->addWidget(mLabelFx, 1, 0, 1, 1);
    mDoubleSpinBoxFx = new QDoubleSpinBox(mGroupBoxCalibrationParameters);
    mDoubleSpinBoxFx->setRange(0., 100000.);
    mDoubleSpinBoxFx->setDecimals(8);
    gridLayout2->addWidget(mDoubleSpinBoxFx, 1, 1, 1, 1);

    mLabelFy = new QLabel(mGroupBoxCalibrationParameters);
    mLabelFy->setMaximumWidth(50);
    gridLayout2->addWidget(mLabelFy, 1, 2, 1, 1);
    mDoubleSpinBoxFy = new QDoubleSpinBox(mGroupBoxCalibrationParameters);
    mDoubleSpinBoxFy->setRange(0., 100000.);
    mDoubleSpinBoxFy->setDecimals(8);
    gridLayout2->addWidget(mDoubleSpinBoxFy, 1, 3, 1, 1);

    mLabelCx = new QLabel(mGroupBoxCalibrationParameters);
    mLabelCx->setMaximumWidth(50);
    gridLayout2->addWidget(mLabelCx, 2, 0, 1, 1);
    mDoubleSpinBoxCx = new QDoubleSpinBox(mGroupBoxCalibrationParameters);
    mDoubleSpinBoxCx->setRange(-100000., 100000.);
    mDoubleSpinBoxCx->setDecimals(8);
    gridLayout2->addWidget(mDoubleSpinBoxCx, 2, 1, 1, 1);

    mLabelCy = new QLabel(mGroupBoxCalibrationParameters);
    mLabelCy->setMaximumWidth(50);
    gridLayout2->addWidget(mLabelCy, 2, 2, 1, 1);
    mDoubleSpinBoxCy = new QDoubleSpinBox(mGroupBoxCalibrationParameters);
    mDoubleSpinBoxCy->setRange(-100000., 100000.);
    mDoubleSpinBoxCy->setDecimals(8);
    gridLayout2->addWidget(mDoubleSpinBoxCy, 2, 3, 1, 1);

    mLabelK1 = new QLabel(mGroupBoxCalibrationParameters);
    mLabelK1->setMaximumWidth(50);
    gridLayout2->addWidget(mLabelK1, 4, 0, 1, 1);
    mDoubleSpinBoxK1 = new QDoubleSpinBox(mGroupBoxCalibrationParameters);
    mDoubleSpinBoxK1->setRange(-100000., 100000.);
    mDoubleSpinBoxK1->setDecimals(8);
    gridLayout2->addWidget(mDoubleSpinBoxK1, 4, 1, 1, 1);

    mLabelK2 = new QLabel(mGroupBoxCalibrationParameters);
    mLabelK2->setMaximumWidth(50);
    gridLayout2->addWidget(mLabelK2, 4, 2, 1, 1);
    mDoubleSpinBoxK2 = new QDoubleSpinBox(mGroupBoxCalibrationParameters);
    mDoubleSpinBoxK2->setRange(-100000., 100000.);
    mDoubleSpinBoxK2->setDecimals(8);
    gridLayout2->addWidget(mDoubleSpinBoxK2, 4, 3, 1, 1);

    mLabelK3 = new QLabel(mGroupBoxCalibrationParameters);
    mLabelK3->setMaximumWidth(50);
    gridLayout2->addWidget(mLabelK3, 6, 0, 1, 1);
    mDoubleSpinBoxK3 = new QDoubleSpinBox(mGroupBoxCalibrationParameters);
    mDoubleSpinBoxK3->setRange(-100000., 100000.);
    mDoubleSpinBoxK3->setDecimals(8);
    gridLayout2->addWidget(mDoubleSpinBoxK3, 6, 1, 1, 1);

    mLabelK4 = new QLabel(mGroupBoxCalibrationParameters);
    mLabelK4->setMaximumWidth(50);
    gridLayout2->addWidget(mLabelK4, 6, 2, 1, 1);
    mDoubleSpinBoxK4 = new QDoubleSpinBox(mGroupBoxCalibrationParameters);
    mDoubleSpinBoxK4->setRange(-100000., 100000.);
    mDoubleSpinBoxK4->setDecimals(8);
    gridLayout2->addWidget(mDoubleSpinBoxK4, 6, 3, 1, 1);

    mLabelK5 = new QLabel(mGroupBoxCalibrationParameters);
    mLabelK5->setMaximumWidth(50);
    gridLayout2->addWidget(mLabelK5, 7, 0, 1, 1);
    mDoubleSpinBoxK5 = new QDoubleSpinBox(mGroupBoxCalibrationParameters);
    mDoubleSpinBoxK5->setRange(-100000., 100000.);
    mDoubleSpinBoxK5->setDecimals(8);
    gridLayout2->addWidget(mDoubleSpinBoxK5, 7, 1, 1, 1);

    mLabelK6 = new QLabel(mGroupBoxCalibrationParameters);
    mLabelK6->setMaximumWidth(50);
    gridLayout2->addWidget(mLabelK6, 7, 2, 1, 1);
    mDoubleSpinBoxK6 = new QDoubleSpinBox(mGroupBoxCalibrationParameters);
    mDoubleSpinBoxK6->setRange(-100000., 100000.);
    mDoubleSpinBoxK6->setDecimals(8);
    gridLayout2->addWidget(mDoubleSpinBoxK6, 7, 3, 1, 1);

    mLabelP1 = new QLabel(mGroupBoxCalibrationParameters);
    mLabelP1->setMaximumWidth(50);
    gridLayout2->addWidget(mLabelP1, 8, 0, 1, 1);
    mDoubleSpinBoxP1 = new QDoubleSpinBox(mGroupBoxCalibrationParameters);
    mDoubleSpinBoxP1->setRange(-100000., 100000.);
    mDoubleSpinBoxP1->setDecimals(8);
    gridLayout2->addWidget(mDoubleSpinBoxP1, 8, 1, 1, 1);

    mLabelP2 = new QLabel(mGroupBoxCalibrationParameters);
    mLabelP2->setMaximumWidth(50);
    gridLayout2->addWidget(mLabelP2, 8, 2, 1, 1);
    mDoubleSpinBoxP2 = new QDoubleSpinBox(mGroupBoxCalibrationParameters);
    mDoubleSpinBoxP2->setRange(-100000., 100000.);
    mDoubleSpinBoxP2->setDecimals(8);
    gridLayout2->addWidget(mDoubleSpinBoxP2, 8, 3, 1, 1);

    layout->addWidget(mGroupBoxCalibrationParameters, 4, 1, 1, 4);

    mLabelImages = new QLabel(this);
    layout->addWidget(mLabelImages, 5, 0, 1, 1);

    mListWidgetImages = new QListWidget(this);
    layout->addWidget(mListWidgetImages, 6, 0, 1, 5);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setObjectName(QStringLiteral("buttonBox"));
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Help | QDialogButtonBox::Ok);

    layout->addWidget(mButtonBox, 7, 0, 1, 5);

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

    connect(mActionImportCalibration, &QAction::triggered, this, &CamerasViewImp::onCalibrationImport);
    connect(mActionExportCalibration, &QAction::triggered, this, &CamerasViewImp::onCalibrationExport);

    connect(mDoubleSpinBoxCx, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibCxChange);
    connect(mDoubleSpinBoxCy, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibCyChange);
    connect(mDoubleSpinBoxF, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibFChange);
    connect(mDoubleSpinBoxFx, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibFxChange);
    connect(mDoubleSpinBoxFy, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibFyChange);
    connect(mDoubleSpinBoxK1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibK1Change);
    connect(mDoubleSpinBoxK2, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibK2Change);
    connect(mDoubleSpinBoxK3, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibK3Change);
    connect(mDoubleSpinBoxK4, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibK4Change);
    connect(mDoubleSpinBoxK5, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibK5Change);
    connect(mDoubleSpinBoxK6, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibK6Change);
    connect(mDoubleSpinBoxP1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibP1Change);
    connect(mDoubleSpinBoxP2, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CamerasView::calibP2Change);

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
    const QSignalBlocker blocker9(mDoubleSpinBoxCx);
    const QSignalBlocker blocker10(mDoubleSpinBoxCy);
    const QSignalBlocker blocker11(mDoubleSpinBoxF);
    const QSignalBlocker blocker12(mDoubleSpinBoxFx);
    const QSignalBlocker blocker13(mDoubleSpinBoxFy);
    const QSignalBlocker blocker14(mDoubleSpinBoxK1);
    const QSignalBlocker blocker15(mDoubleSpinBoxK2);
    const QSignalBlocker blocker16(mDoubleSpinBoxK3);
    const QSignalBlocker blocker17(mDoubleSpinBoxK4);
    const QSignalBlocker blocker18(mDoubleSpinBoxK5);
    const QSignalBlocker blocker19(mDoubleSpinBoxK6);
    const QSignalBlocker blocker20(mDoubleSpinBoxP1);
    const QSignalBlocker blocker21(mDoubleSpinBoxP2);

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
    mDoubleSpinBoxCx->setValue(0.);
    mDoubleSpinBoxCy->setValue(0.);
    mDoubleSpinBoxF->setValue(0.);
    mDoubleSpinBoxFx->setValue(0.);
    mDoubleSpinBoxFy->setValue(0.);
    mDoubleSpinBoxK1->setValue(0.);
    mDoubleSpinBoxK2->setValue(0.);
    mDoubleSpinBoxK3->setValue(0.);
    mDoubleSpinBoxK4->setValue(0.);
    mDoubleSpinBoxK5->setValue(0.);
    mDoubleSpinBoxK6->setValue(0.);
    mDoubleSpinBoxP1->setValue(0.);
    mDoubleSpinBoxP2->setValue(0.);

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

    mDoubleSpinBoxF->setEnabled(selected_camera && f_enable);
    mDoubleSpinBoxFx->setEnabled(selected_camera && fx_enable);
    mDoubleSpinBoxFy->setEnabled(selected_camera && fy_enable);
    mDoubleSpinBoxCx->setEnabled(selected_camera && cx_enable);
    mDoubleSpinBoxCy->setEnabled(selected_camera && cy_enable);
    mDoubleSpinBoxK1->setEnabled(selected_camera && k1_enable);
    mDoubleSpinBoxK2->setEnabled(selected_camera && k2_enable);
    mDoubleSpinBoxK3->setEnabled(selected_camera && k3_enable);
    mDoubleSpinBoxK4->setEnabled(selected_camera && k4_enable);
    mDoubleSpinBoxK5->setEnabled(selected_camera && k5_enable);
    mDoubleSpinBoxK6->setEnabled(selected_camera && k6_enable);
    mDoubleSpinBoxP1->setEnabled(selected_camera && p1_enable);
    mDoubleSpinBoxP2->setEnabled(selected_camera && p2_enable);

    mLabelF->setVisible(isFocalVisible());
    mDoubleSpinBoxF->setVisible(isFocalVisible());
    mLabelFx->setVisible(isFocalXYVisible());
    mDoubleSpinBoxFx->setVisible(isFocalXYVisible());
    mLabelFy->setVisible(isFocalXYVisible());
    mDoubleSpinBoxFy->setVisible(isFocalXYVisible());

    mActionImportCalibration->setEnabled(selected_camera);
    mActionExportCalibration->setEnabled(selected_camera);

}

void CamerasViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("CamerasView", "Cameras", nullptr));
    mGroupBoxCalibrationParameters->setTitle(QApplication::translate("CamerasView", "Calibration parameters", nullptr));
    mActionImportCalibration->setText(QApplication::translate("CamerasView", "Import Calibration", nullptr));
    mActionImportCalibration->setStatusTip(QApplication::translate("CamerasView", "Import Calibration", nullptr));
    mActionExportCalibration->setText(QApplication::translate("CamerasView", "Export Calibration", nullptr));
    mActionExportCalibration->setStatusTip(QApplication::translate("CamerasView", "Export Calibration", nullptr));
    mLabelF->setText(QApplication::translate("CamerasView", "f", nullptr));
    mLabelFx->setText(QApplication::translate("CamerasView", "fx", nullptr));
    mLabelFy->setText(QApplication::translate("CamerasView", "fy", nullptr));
    mLabelCx->setText(QApplication::translate("CamerasView", "cx", nullptr));
    mLabelCy->setText(QApplication::translate("CamerasView", "cy", nullptr));
    mLabelK1->setText(QApplication::translate("CamerasView", "k1", nullptr));
    mLabelK2->setText(QApplication::translate("CamerasView", "k2", nullptr));
    mLabelK3->setText(QApplication::translate("CamerasView", "k3", nullptr));
    mLabelK4->setText(QApplication::translate("CamerasView", "k4", nullptr));
    mLabelK5->setText(QApplication::translate("CamerasView", "k5", nullptr));
    mLabelK6->setText(QApplication::translate("CamerasView", "k6", nullptr));
    mLabelP1->setText(QApplication::translate("CamerasView", "p1", nullptr));
    mLabelP2->setText(QApplication::translate("CamerasView", "p2", nullptr));
    mLabelImages->setText(QApplication::translate("CamerasView", "Images:", nullptr));
    mLabelCameras->setText(QApplication::translate("CamerasView", "Cameras:", nullptr));
    mGroupBoxCamera->setTitle(QApplication::translate("CamerasView", "Camera", nullptr));
    mLabelType->setText(QApplication::translate("CamerasView", "Type", nullptr));
    mLabelWidth->setText(QApplication::translate("CamerasView", "Width (pixels)", nullptr));
    mLabelSensorSize->setText(QApplication::translate("CamerasView", "Sensor size (mm)", nullptr));
    mLabelFocal->setText(QApplication::translate("CamerasView", "Focal", nullptr));
    mComboBoxType->setItemText(radial_1, QApplication::translate("CamerasView", "Radial 1", nullptr));
    mComboBoxType->setItemText(radial_2, QApplication::translate("CamerasView", "Radial 2", nullptr));
    mComboBoxType->setItemText(pinhole_1, QApplication::translate("CamerasView", "Pinhole 1", nullptr));
    mComboBoxType->setItemText(pinhole_2, QApplication::translate("CamerasView", "Pinhole 2", nullptr));
    mComboBoxType->setItemText(opencv_1, QApplication::translate("CamerasView", "OpenCV 1", nullptr));
    mComboBoxType->setItemText(opencv_2, QApplication::translate("CamerasView", "OpenCV 2", nullptr));
    mComboBoxType->setItemText(opencv_fisheye, QApplication::translate("CamerasView", "OpenCV Fisheye", nullptr));
    mComboBoxType->setItemText(radial_fisheye_1, QApplication::translate("CamerasView", "Radial Fisheye 1", nullptr));
    mComboBoxType->setItemText(radial_fisheye_2, QApplication::translate("CamerasView", "Radial Fisheye 2", nullptr));

    mLabelMake->setText(QApplication::translate("CamerasView", "Make", nullptr));
    mLabelModel->setText(QApplication::translate("CamerasView", "Model", nullptr));
    mLabelHeight->setText(QApplication::translate("CamerasView", "Height (pixels)", nullptr));
    mPushButtonEditCamera->setText(QApplication::translate("CamerasView", "Edit Camera", nullptr));

    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("CamerasView", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Ok)->setText(QApplication::translate("CamerasView", "Ok"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("CamerasView", "Help"));
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

void CamerasViewImp::setCalibCx(double cx)
{
    mDoubleSpinBoxCx->setValue(cx);
}

void CamerasViewImp::setCalibCy(double cy)
{
    mDoubleSpinBoxCy->setValue(cy);
}

void CamerasViewImp::setCalibF(double f)
{
    mDoubleSpinBoxF->setValue(f);
}

void CamerasViewImp::setCalibFx(double fx)
{
    mDoubleSpinBoxFx->setValue(fx);
}

void CamerasViewImp::setCalibFy(double fy)
{
    mDoubleSpinBoxFy->setValue(fy);
}

void CamerasViewImp::setCalibK1(double k1)
{
    mDoubleSpinBoxK1->setValue(k1);
}

void CamerasViewImp::setCalibK2(double k2)
{
    mDoubleSpinBoxK2->setValue(k2);
}

void CamerasViewImp::setCalibK3(double k3)
{
    mDoubleSpinBoxK3->setValue(k3);
}

void CamerasViewImp::setCalibK4(double k4)
{
    mDoubleSpinBoxK4->setValue(k4);
}

void CamerasViewImp::setCalibK5(double k5)
{
    mDoubleSpinBoxK5->setValue(k5);
}

void CamerasViewImp::setCalibK6(double k6)
{
    mDoubleSpinBoxK6->setValue(k6);
}

void CamerasViewImp::setCalibP1(double p1)
{
    mDoubleSpinBoxP1->setValue(p1);
}

void CamerasViewImp::setCalibP2(double p2)
{
    mDoubleSpinBoxP2->setValue(p2);
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
