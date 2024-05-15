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

#include "ImportCamerasView.h"

#include <QIcon>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTableView>
#include <QFileDialog>
#include <QComboBox>
#include <QStandardItemModel>
#include <QSpinBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QCheckBox>

namespace graphos
{

ImportCamerasViewImp::ImportCamerasViewImp(QWidget *parent)
  : ImportCamerasView(parent)
{
    ImportCamerasViewImp::initUI();
    ImportCamerasViewImp::initSignalAndSlots();
}

ImportCamerasViewImp::~ImportCamerasViewImp()
{

}

void ImportCamerasViewImp::onDelimiterChanged()
{
    emit delimiterChanged(this->delimiter());
    emit previewCSV();
}

QString ImportCamerasViewImp::delimiter() const
{
    QString delimiter;
    if (mRadioButtonTab->isChecked()) {
        delimiter = "\t";
    } else if (mRadioButtonComma->isChecked()) {
        delimiter = ",";
    } else if (mRadioButtonSpace->isChecked()) {
        delimiter = " ";
    } else if (mRadioButtonSemicolon->isChecked()) {
        delimiter = ";";
    } else if (mRadioButtonOther->isChecked()) {
        delimiter = mLineEditOther->text();
    }
    return delimiter;
}

void ImportCamerasViewImp::initUI()
{
    this->setObjectName(QStringLiteral("ExportOrientationsView"));
    this->resize(1000, 800);
    this->setMaximumWidth(1200);

    QGridLayout *gridLayout = new QGridLayout();
    this->setLayout(gridLayout);

    mGroupBoxDelimiter = new QGroupBox(this);
    QGridLayout *gridLayoutDelimiter = new QGridLayout(mGroupBoxDelimiter);
    mRadioButtonTab = new QRadioButton(mGroupBoxDelimiter);
    gridLayoutDelimiter->addWidget(mRadioButtonTab, 0, 0, 1, 1);
    mRadioButtonComma = new QRadioButton(mGroupBoxDelimiter);
    gridLayoutDelimiter->addWidget(mRadioButtonComma, 0, 1, 1, 1);
    mRadioButtonSpace = new QRadioButton(mGroupBoxDelimiter);
    gridLayoutDelimiter->addWidget(mRadioButtonSpace, 0, 2, 1, 1);
    mRadioButtonSemicolon = new QRadioButton(mGroupBoxDelimiter);
    gridLayoutDelimiter->addWidget(mRadioButtonSemicolon, 0, 3, 1, 1);
    mRadioButtonOther = new QRadioButton(mGroupBoxDelimiter);
    gridLayoutDelimiter->addWidget(mRadioButtonOther, 0, 4, 1, 1);
    mLineEditOther = new QLineEdit(mGroupBoxDelimiter);
    gridLayoutDelimiter->addWidget(mLineEditOther, 0, 5, 1, 1);
    gridLayout->addWidget(mGroupBoxDelimiter, 0, 0, 1, 3);

    mCheckBoxFieldNamesAuto = new QCheckBox(this);
    gridLayout->addWidget(mCheckBoxFieldNamesAuto, 1, 0, 1, 1);

    mLabelSkipLines = new QLabel(this);
    gridLayout->addWidget(mLabelSkipLines, 2, 0, 1, 1);

    mSpinBoxSkipLines = new QSpinBox(this);
    gridLayout->addWidget(mSpinBoxSkipLines, 2, 1, 1, 1);

    mLabelPreview = new QLabel(this);
    gridLayout->addWidget(mLabelPreview, 3, 0, 1, 1);

    mTableViewImportCameras = new QTableView(this);
    //QHeaderView::ResizeToContents
    //mTableViewImportCameras->setMaximumWidth(600);
    gridLayout->addWidget(mTableViewImportCameras, 4, 0, 1, 3);

    mGroupBoxColumns = new QGroupBox(this);
    QGridLayout *gridLayoutColumns = new QGridLayout(mGroupBoxColumns);

    mLabelImageColumn = new QLabel(mGroupBoxColumns);
    gridLayoutColumns->addWidget(mLabelImageColumn, 0, 0, 1, 1);
    mComboBoxImageColumn = new QComboBox(mGroupBoxColumns);
    gridLayoutColumns->addWidget(mComboBoxImageColumn, 0, 1, 1, 1);

    mGroupBoxCoordinates = new QGroupBox(mGroupBoxColumns);
    QGridLayout *gridLayoutCoordinates = new QGridLayout(mGroupBoxCoordinates);

    mLabelXColumn = new QLabel(mGroupBoxCoordinates);
    gridLayoutCoordinates->addWidget(mLabelXColumn, 0, 0, 1, 1);
    mComboBoxXColumn = new QComboBox(mGroupBoxCoordinates);
    gridLayoutCoordinates->addWidget(mComboBoxXColumn, 0, 1, 1, 1);

    QSpacerItem *horizontalSpacer_xy = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayoutCoordinates->addItem(horizontalSpacer_xy, 0, 2, 1, 1);

    mLabelYColumn = new QLabel(mGroupBoxCoordinates);
    gridLayoutCoordinates->addWidget(mLabelYColumn, 0, 3, 1, 1);
    mComboBoxYColumn = new QComboBox(mGroupBoxCoordinates);
    gridLayoutCoordinates->addWidget(mComboBoxYColumn, 0, 4, 1, 1);

    QSpacerItem *horizontalSpacer_yz = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayoutCoordinates->addItem(horizontalSpacer_yz, 0, 5, 1, 1);

    mLabelZColumn = new QLabel(mGroupBoxCoordinates);
    gridLayoutCoordinates->addWidget(mLabelZColumn, 0, 6, 1, 1);
    mComboBoxZColumn = new QComboBox(mGroupBoxCoordinates);
    gridLayoutCoordinates->addWidget(mComboBoxZColumn, 0, 7, 1, 1);

    mLabelCrs = new QLabel(this);
    gridLayoutCoordinates->addWidget(mLabelCrs, 1, 0, 1, 1);
    mLineEditCrsInput = new QLineEdit(this);
    gridLayoutCoordinates->addWidget(mLineEditCrsInput, 1, 1, 1, 1);

    mLabelCrsOut = new QLabel(this);
    gridLayoutCoordinates->addWidget(mLabelCrsOut, 1, 2, 1, 1);
    mLineEditCrsOutput = new QLineEdit(this);
    gridLayoutCoordinates->addWidget(mLineEditCrsOutput, 1, 3, 1, 1);

    gridLayoutColumns->addWidget(mGroupBoxCoordinates, 1, 0, 1, 3);

    mGroupBoxRotations = new QGroupBox(mGroupBoxColumns);
    gridLayoutRotations = new QGridLayout(mGroupBoxRotations);

    mLabelRotation = new QLabel(mGroupBoxRotations);
    gridLayoutRotations->addWidget(mLabelRotation, 0, 0, 1, 1);

    mComboBoxRotation = new QComboBox(mGroupBoxRotations);
    mComboBoxRotation->addItem(QString());
    mComboBoxRotation->addItem(QString());
    mComboBoxRotation->addItem(QString());

    gridLayoutRotations->addWidget(mComboBoxRotation, 0, 1, 1, 1);

    mStackedWidget = new QStackedWidget(mGroupBoxRotations);
    pageQuaternions = new QWidget();
    auto grid_layout_quaternions = new QGridLayout(pageQuaternions);

    auto horizontal_spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayoutRotations->addItem(horizontal_spacer, 0, 2, 1, 1);

    auto horizontal_layout_quaternions = new QHBoxLayout();
    mLabelQxColumn = new QLabel(mGroupBoxRotations);
    horizontal_layout_quaternions->addWidget(mLabelQxColumn);
    mComboBoxQxColumn = new QComboBox(mGroupBoxRotations);
    horizontal_layout_quaternions->addWidget(mComboBoxQxColumn);

    auto horizontal_spacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontal_layout_quaternions->addItem(horizontal_spacer_3);

    mLabelQyColumn = new QLabel(mGroupBoxRotations);
    horizontal_layout_quaternions->addWidget(mLabelQyColumn);
    mComboBoxQyColumn = new QComboBox(mGroupBoxRotations);
    horizontal_layout_quaternions->addWidget(mComboBoxQyColumn);

    auto horizontal_spacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontal_layout_quaternions->addItem(horizontal_spacer_4);

    mLabelQzColumn = new QLabel(mGroupBoxRotations);
    horizontal_layout_quaternions->addWidget(mLabelQzColumn);

    mComboBoxQzColumn = new QComboBox(mGroupBoxRotations);
    horizontal_layout_quaternions->addWidget(mComboBoxQzColumn);

    auto horizontal_spacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontal_layout_quaternions->addItem(horizontal_spacer_5);

    mLabelQwColumn = new QLabel(mGroupBoxRotations);
    horizontal_layout_quaternions->addWidget(mLabelQwColumn);

    mComboBoxQwColumn = new QComboBox(mGroupBoxRotations);
    horizontal_layout_quaternions->addWidget(mComboBoxQwColumn);

    grid_layout_quaternions->addLayout(horizontal_layout_quaternions, 0, 0, 1, 1);

    mStackedWidget->addWidget(pageQuaternions);

    pageYawPitchRoll = new QWidget();
    auto grid_layout_yaw_pitch_roll = new QGridLayout(pageYawPitchRoll);

    auto horizontal_layout_yaw_pitch_roll = new QHBoxLayout();

    mLabelYawColumn = new QLabel(mGroupBoxRotations);
    horizontal_layout_yaw_pitch_roll->addWidget(mLabelYawColumn);

    mComboBoxYawColumn = new QComboBox(mGroupBoxRotations);
    horizontal_layout_yaw_pitch_roll->addWidget(mComboBoxYawColumn);

    auto horizontal_spacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontal_layout_yaw_pitch_roll->addItem(horizontal_spacer_8);

    mLabelPitchColumn = new QLabel(mGroupBoxRotations);
    horizontal_layout_yaw_pitch_roll->addWidget(mLabelPitchColumn);

    mComboBoxPitchColumn = new QComboBox(mGroupBoxRotations);
    horizontal_layout_yaw_pitch_roll->addWidget(mComboBoxPitchColumn);

    auto horizontal_spacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontal_layout_yaw_pitch_roll->addItem(horizontal_spacer_9);

    mLabelRollColumn = new QLabel(mGroupBoxRotations);
    horizontal_layout_yaw_pitch_roll->addWidget(mLabelRollColumn);

    mComboBoxRollColumn = new QComboBox(mGroupBoxRotations);
    horizontal_layout_yaw_pitch_roll->addWidget(mComboBoxRollColumn);

    grid_layout_yaw_pitch_roll->addLayout(horizontal_layout_yaw_pitch_roll, 0, 0, 1, 1);

    mStackedWidget->addWidget(pageYawPitchRoll);



    pageOmegaPhiKappa = new QWidget();
    auto grid_omega_phi_kappa = new QGridLayout(pageOmegaPhiKappa);

    auto horizontal_layout_omega_phi_kappa = new QHBoxLayout();

    mLabelOmegaColumn = new QLabel(mGroupBoxRotations);
    horizontal_layout_omega_phi_kappa->addWidget(mLabelOmegaColumn);

    mComboBoxOmegaColumn = new QComboBox(mGroupBoxRotations);
    horizontal_layout_omega_phi_kappa->addWidget(mComboBoxOmegaColumn);

    auto horizontal_spacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontal_layout_omega_phi_kappa->addItem(horizontal_spacer_6);

    mLabelPhiColumn = new QLabel(mGroupBoxRotations);
    horizontal_layout_omega_phi_kappa->addWidget(mLabelPhiColumn);

    mComboBoxPhiColumn = new QComboBox(mGroupBoxRotations);
    horizontal_layout_omega_phi_kappa->addWidget(mComboBoxPhiColumn);

    auto horizontal_spacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontal_layout_omega_phi_kappa->addItem(horizontal_spacer_7);

    mLabelKappaColumn = new QLabel(mGroupBoxRotations);
    horizontal_layout_omega_phi_kappa->addWidget(mLabelKappaColumn);

    mComboBoxKappaColumn = new QComboBox(mGroupBoxRotations);
    horizontal_layout_omega_phi_kappa->addWidget(mComboBoxKappaColumn);

    grid_omega_phi_kappa->addLayout(horizontal_layout_omega_phi_kappa, 0, 0, 1, 1);

    mStackedWidget->addWidget(pageOmegaPhiKappa);

    gridLayoutRotations->addWidget(mStackedWidget, 1, 0, 1, 3);

    gridLayoutColumns->addWidget(mGroupBoxRotations, 2, 0, 1, 3);

    auto horizontal_spacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayoutColumns->addItem(horizontal_spacer_2, 0, 2, 1, 1);


    gridLayout->addWidget(mGroupBoxColumns, 5, 0, 1, 3);

    mLabelCameras = new QLabel(this);
    gridLayout->addWidget(mLabelPreview, 6, 0, 1, 1);

    mTableViewFormatCameras = new QTableView(this);
    gridLayout->addWidget(mTableViewFormatCameras, 7, 0, 1, 3);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok | QDialogButtonBox::Help);
    gridLayout->addWidget(mButtonBox, 8, 2, 1, 1);

    this->retranslate();
    this->clear();
    this->update();
}

void ImportCamerasViewImp::initSignalAndSlots()
{
    connect(mRadioButtonTab, &QRadioButton::released, this, &ImportCamerasViewImp::onDelimiterChanged);
    connect(mRadioButtonComma, &QRadioButton::released, this, &ImportCamerasViewImp::onDelimiterChanged);
    connect(mRadioButtonSpace, &QRadioButton::released, this, &ImportCamerasViewImp::onDelimiterChanged);
    connect(mRadioButtonSemicolon, &QRadioButton::released, this, &ImportCamerasViewImp::onDelimiterChanged);
    connect(mRadioButtonOther, &QRadioButton::released, this, &ImportCamerasViewImp::onDelimiterChanged);
    connect(mRadioButtonOther, &QRadioButton::released, this, &ImportCamerasViewImp::update);
    connect(mLineEditOther, &QLineEdit::textChanged, this, &ImportCamerasViewImp::onDelimiterChanged);
    connect(mCheckBoxFieldNamesAuto, &QCheckBox::clicked, this, &ImportCamerasView::loadFieldNamesFromFirstRow);
    connect(mSpinBoxSkipLines, QOverload<int>::of(&QSpinBox::valueChanged), this, &ImportCamerasView::skipLines);
    connect(mComboBoxImageColumn, &QComboBox::currentTextChanged, this, &ImportCamerasView::imageColumnChange);
    connect(mComboBoxImageColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::imageFieldIdChanged);
    connect(mComboBoxXColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::xFieldIdChanged);
    connect(mComboBoxYColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::yFieldIdChanged);
    connect(mComboBoxZColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::zFieldIdChanged);
    connect(mComboBoxQxColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::qxFieldChange);
    connect(mComboBoxQyColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::qyFieldChange);
    connect(mComboBoxQzColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::qzFieldChange);
    connect(mComboBoxQwColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::qwFieldChange);
    connect(mComboBoxYawColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::yawFieldChange);
    connect(mComboBoxPitchColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::pitchFieldChange);
    connect(mComboBoxRollColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::rollFieldChange);
    connect(mComboBoxOmegaColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::omegaFieldChange);
    connect(mComboBoxPhiColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::phiFieldChange);
    connect(mComboBoxKappaColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::kappaFieldChange);
    connect(mComboBoxRotation, QOverload<int>::of(&QComboBox::currentIndexChanged), mStackedWidget, &QStackedWidget::setCurrentIndex);
    connect(mComboBoxRotation, &QComboBox::currentTextChanged, this, &ImportCamerasView::rotationChange);
    connect(mLineEditCrsInput, &QLineEdit::textChanged, this, &ImportCamerasViewImp::crsInputChanged);
    connect(mLineEditCrsOutput, &QLineEdit::textChanged, this, &ImportCamerasViewImp::crsOutputChanged);
    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this, &QDialog::accept);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void ImportCamerasViewImp::clear()
{
    QSignalBlocker blocker1(mRadioButtonComma);
    QSignalBlocker blocker2(mComboBoxImageColumn);
    QSignalBlocker blocker3(mComboBoxXColumn);
    QSignalBlocker blocker4(mComboBoxYColumn);
    QSignalBlocker blocker5(mComboBoxZColumn);
    QSignalBlocker blocker7(mComboBoxQxColumn);
    QSignalBlocker blocker8(mComboBoxQyColumn);
    QSignalBlocker blocker9(mComboBoxQzColumn);
    QSignalBlocker blocker10(mComboBoxQwColumn);
    QSignalBlocker blocker11(mComboBoxYawColumn);
    QSignalBlocker blocker12(mComboBoxPitchColumn);
    QSignalBlocker blocker13(mComboBoxRollColumn);
    QSignalBlocker blocker14(mComboBoxOmegaColumn);
    QSignalBlocker blocker15(mComboBoxPhiColumn);
    QSignalBlocker blocker16(mComboBoxKappaColumn);
    QSignalBlocker blocker17(mLineEditOther);
    QSignalBlocker blocker18(mLineEditCrsInput);
    QSignalBlocker blocker19(mLineEditCrsOutput);

    mRadioButtonComma->setChecked(true);
    mCheckBoxFieldNamesAuto->setChecked(true);
    mComboBoxImageColumn->clear();
    mComboBoxXColumn->clear();
    mComboBoxYColumn->clear();
    mComboBoxZColumn->clear();
    mComboBoxRotation->setCurrentIndex(0);
    mComboBoxQxColumn->clear();
    mComboBoxQyColumn->clear();
    mComboBoxQzColumn->clear();
    mComboBoxQwColumn->clear();
    mComboBoxYawColumn->clear();
    mComboBoxPitchColumn->clear();
    mComboBoxRollColumn->clear();
    mComboBoxOmegaColumn->clear();
    mComboBoxPhiColumn->clear();
    mComboBoxKappaColumn->clear();
    mLineEditOther->clear();
    mLineEditCrsInput->clear();
    mLineEditCrsOutput->clear();
    mLineEditCrsOutput->setEnabled(true);
    mParseOk = false;
    mValidCrsIn = false;
    mValidCrsOut = false;

    update();
}

void ImportCamerasViewImp::update()
{
    mLineEditOther->setEnabled(mRadioButtonOther->isChecked());

    mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(((mValidCrsIn && mValidCrsOut) ||
                                                         (mLineEditCrsInput->text().isEmpty() && mLineEditCrsOutput->text().isEmpty())));
}

void ImportCamerasViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("ImportCamerasViewImp", "Import cameras", nullptr));
    mGroupBoxDelimiter->setTitle(QCoreApplication::translate("ImportCamerasViewImp", "Delimiter", nullptr));
    mRadioButtonTab->setText(QCoreApplication::translate("ImportCamerasViewImp", "Tab", nullptr));
    mRadioButtonComma->setText(QCoreApplication::translate("ImportCamerasViewImp", "Comma", nullptr));
    mRadioButtonSpace->setText(QCoreApplication::translate("ImportCamerasViewImp", "Space", nullptr));
    mRadioButtonSemicolon->setText(QCoreApplication::translate("ImportCamerasViewImp", "Semicolon", nullptr));
    mRadioButtonOther->setText(QCoreApplication::translate("ImportCamerasViewImp", "Other", nullptr));
    mCheckBoxFieldNamesAuto->setText(QCoreApplication::translate("ImportCamerasViewImp", "First record has field names", nullptr));
    mLabelSkipLines->setText(QCoreApplication::translate("ImportCamerasViewImp", "Skip lines", nullptr));
    mGroupBoxColumns->setTitle(QCoreApplication::translate("ImportCamerasViewImp", "Columns", nullptr));
    mLabelImageColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "Image name:", nullptr));
    mGroupBoxCoordinates->setTitle(QCoreApplication::translate("ImportCamerasViewImp", "Coordinates", nullptr));
    mLabelYColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "Y:", nullptr));
    mLabelXColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "X:", nullptr));
    mLabelZColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "Z:", nullptr));
    mLabelCrs->setText(QCoreApplication::translate("ImportCamerasViewImp", "CRS Input:", nullptr));
    mLabelCrsOut->setText(QCoreApplication::translate("ImportCamerasViewImp", "CRS Output:", nullptr));
    mGroupBoxRotations->setTitle(QCoreApplication::translate("ImportCamerasViewImp", "Rotations", nullptr));
    mLabelRotation->setText(QCoreApplication::translate("ImportCamerasViewImp", "Rotation", nullptr));
    mComboBoxRotation->setItemText(0, QCoreApplication::translate("ImportCamerasViewImp", "Quaternions", nullptr));
    mComboBoxRotation->setItemText(1, QCoreApplication::translate("ImportCamerasViewImp", "Yaw, Pitch, Roll", nullptr));
    mComboBoxRotation->setItemText(2, QCoreApplication::translate("ImportCamerasViewImp", "Omega, Phi, Kappa", nullptr));
    mLabelQxColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "Qx:", nullptr));
    mLabelQyColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "Qy:", nullptr));
    mLabelQzColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "Qz:", nullptr));
    mLabelQwColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "Qw:", nullptr));
    mLabelYawColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "Yaw:", nullptr));
    mLabelPitchColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "Pitch:", nullptr));
    mLabelRollColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "Roll:", nullptr));
    mLabelOmegaColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "Omega:", nullptr));
    mLabelPhiColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "Phi:", nullptr));
    mLabelKappaColumn->setText(QCoreApplication::translate("ImportCamerasViewImp", "Kappa:", nullptr));
    mLabelPreview->setText(QCoreApplication::translate("ImportCamerasViewImp", "Preview:", nullptr));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("ImportCamerasViewImp", "Cancel", nullptr));
    mButtonBox->button(QDialogButtonBox::Ok)->setText(QApplication::translate("ImportCamerasViewImp", "Save", nullptr));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("ImportCamerasViewImp", "Help", nullptr));

}

void ImportCamerasViewImp::setTableHeader(const QStringList &header)
{
    QSignalBlocker blocker2(mComboBoxImageColumn);
    QSignalBlocker blocker3(mComboBoxXColumn);
    QSignalBlocker blocker4(mComboBoxYColumn);
    QSignalBlocker blocker5(mComboBoxZColumn);
    QSignalBlocker blocker7(mComboBoxQxColumn);
    QSignalBlocker blocker8(mComboBoxQyColumn);
    QSignalBlocker blocker9(mComboBoxQzColumn);
    QSignalBlocker blocker10(mComboBoxQwColumn);
    QSignalBlocker blocker11(mComboBoxYawColumn);
    QSignalBlocker blocker12(mComboBoxPitchColumn);
    QSignalBlocker blocker13(mComboBoxRollColumn);
    QSignalBlocker blocker14(mComboBoxOmegaColumn);
    QSignalBlocker blocker15(mComboBoxPhiColumn);
    QSignalBlocker blocker16(mComboBoxKappaColumn);

    mComboBoxImageColumn->clear();
    mComboBoxXColumn->clear();
    mComboBoxYColumn->clear();
    mComboBoxZColumn->clear();
    mComboBoxQxColumn->clear();
    mComboBoxQyColumn->clear();
    mComboBoxQzColumn->clear();
    mComboBoxQwColumn->clear();
    mComboBoxYawColumn->clear();
    mComboBoxPitchColumn->clear();
    mComboBoxRollColumn->clear();
    mComboBoxOmegaColumn->clear();
    mComboBoxPhiColumn->clear();
    mComboBoxKappaColumn->clear();

    QStringList reg(header);
    reg.push_front("--");
    mComboBoxImageColumn->addItems(reg);
    mComboBoxXColumn->addItems(reg);
    mComboBoxYColumn->addItems(reg);
    mComboBoxZColumn->addItems(reg);
    mComboBoxQxColumn->addItems(reg);
    mComboBoxQyColumn->addItems(reg);
    mComboBoxQzColumn->addItems(reg);
    mComboBoxQwColumn->addItems(reg);
    mComboBoxYawColumn->addItems(reg);
    mComboBoxPitchColumn->addItems(reg);
    mComboBoxRollColumn->addItems(reg);
    mComboBoxOmegaColumn->addItems(reg);
    mComboBoxPhiColumn->addItems(reg);
    mComboBoxKappaColumn->addItems(reg);

}

void ImportCamerasViewImp::setItemModel(QStandardItemModel *model)
{
    mTableViewImportCameras->setModel(model);
}

void ImportCamerasViewImp::setItemModelFormatCameras(QStandardItemModel *model)
{
    mTableViewFormatCameras->setModel(model);
}

void ImportCamerasViewImp::setImageColumn(const QString &imageColumn)
{
    mComboBoxImageColumn->setCurrentText(imageColumn);
    update();
}

void ImportCamerasViewImp::setXColumn(const QString &xColumn)
{
    mComboBoxXColumn->setCurrentText(xColumn);
    update();
}

void ImportCamerasViewImp::setYColumn(const QString &yColumn)
{
    mComboBoxYColumn->setCurrentText(yColumn);
    update();
}

void ImportCamerasViewImp::setZColumn(const QString &zColumn)
{
    mComboBoxZColumn->setCurrentText(zColumn);
    update();
}

void ImportCamerasViewImp::setQxColumn(const QString &qxColumn)
{
    mComboBoxQxColumn->setCurrentText(qxColumn);
    update();
}

void ImportCamerasViewImp::setQyColumn(const QString &qyColumn)
{
    mComboBoxQyColumn->setCurrentText(qyColumn);
    update();
}

void ImportCamerasViewImp::setQzColumn(const QString &qzColumn)
{
    mComboBoxQzColumn->setCurrentText(qzColumn);
    update();
}

void ImportCamerasViewImp::setQwColumn(const QString &qwColumn)
{
    mComboBoxQwColumn->setCurrentText(qwColumn);
    update();
}

void ImportCamerasViewImp::setOmegaColumn(const QString &omegaColumn)
{
    mComboBoxOmegaColumn->setCurrentText(omegaColumn);
    update();
}

void ImportCamerasViewImp::setPhiColumn(const QString &phiColumn)
{
    mComboBoxPhiColumn->setCurrentText(phiColumn);
    update();
}

void ImportCamerasViewImp::setKappaColumn(const QString &kappaColumn)
{
    mComboBoxKappaColumn->setCurrentText(kappaColumn);
    update();
}

void ImportCamerasViewImp::setYawColumn(const QString &yawColumn)
{
    mComboBoxYawColumn->setCurrentText(yawColumn);
    update();
}

void ImportCamerasViewImp::setPitchColumn(const QString &pitchColumn)
{
    mComboBoxPitchColumn->setCurrentText(pitchColumn);
    update();
}

void ImportCamerasViewImp::setRollColumn(const QString &rollColumn)
{
    mComboBoxRollColumn->setCurrentText(rollColumn);
    update();
}

void ImportCamerasViewImp::setParseOk(bool parseOk)
{
    mParseOk = parseOk;
    update();
}

void ImportCamerasViewImp::setValidInputCRS(bool valid)
{
    mValidCrsIn = valid;
    update();
}

void ImportCamerasViewImp::setValidOutputCRS(bool valid)
{
    mValidCrsOut = valid;
    update();
}

void ImportCamerasViewImp::setOutputCRS(const QString &crs)
{
    if (!crs.isEmpty()) {
        QSignalBlocker blocker(mLineEditCrsOutput);
        mLineEditCrsOutput->setText(crs);
        mLineEditCrsOutput->setEnabled(false); // CRS de proyecto. No editable
        mValidCrsOut = true;
    } else mValidCrsOut = false;
}

} // namespace graphos


