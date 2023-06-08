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
  this->initUI();
  this->initSignalAndSlots();
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
  if (mRadioButtonTab->isChecked()){
    delimiter = "\t";
  } else if (mRadioButtonComma->isChecked()){
    delimiter = ",";
  } else if (mRadioButtonSpace->isChecked()){
    delimiter = " ";
  } else if (mRadioButtonSemicolon->isChecked()){
    delimiter = ";";
  } else if (mRadioButtonOther->isChecked()){
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
  QGridLayout *gridLayoutQuaternions = new QGridLayout(pageQuaternions);

  QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  gridLayoutRotations->addItem(horizontalSpacer, 0, 2, 1, 1);

  QHBoxLayout *horizontalLayoutQuaternions = new QHBoxLayout();
  mLabelQxColumn = new QLabel(mGroupBoxRotations);
  horizontalLayoutQuaternions->addWidget(mLabelQxColumn);
  mComboBoxQxColumn = new QComboBox(mGroupBoxRotations);
  horizontalLayoutQuaternions->addWidget(mComboBoxQxColumn);

  QSpacerItem *horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  horizontalLayoutQuaternions->addItem(horizontalSpacer_3);

  mLabelQyColumn = new QLabel(mGroupBoxRotations);
  horizontalLayoutQuaternions->addWidget(mLabelQyColumn);
  mComboBoxQyColumn = new QComboBox(mGroupBoxRotations);
  horizontalLayoutQuaternions->addWidget(mComboBoxQyColumn);

  QSpacerItem *horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  horizontalLayoutQuaternions->addItem(horizontalSpacer_4);

  mLabelQzColumn = new QLabel(mGroupBoxRotations);
  horizontalLayoutQuaternions->addWidget(mLabelQzColumn);

  mComboBoxQzColumn = new QComboBox(mGroupBoxRotations);
  horizontalLayoutQuaternions->addWidget(mComboBoxQzColumn);

  QSpacerItem *horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  horizontalLayoutQuaternions->addItem(horizontalSpacer_5);

  mLabelQwColumn = new QLabel(mGroupBoxRotations);
  horizontalLayoutQuaternions->addWidget(mLabelQwColumn);

  mComboBoxQwColumn = new QComboBox(mGroupBoxRotations);
  horizontalLayoutQuaternions->addWidget(mComboBoxQwColumn);

  gridLayoutQuaternions->addLayout(horizontalLayoutQuaternions, 0, 0, 1, 1);

  mStackedWidget->addWidget(pageQuaternions);

  pageYawPitchRoll = new QWidget();
  QGridLayout *gridLayoutYawPitchRoll = new QGridLayout(pageYawPitchRoll);

  QHBoxLayout *horizontalLayoutYawPitchRoll = new QHBoxLayout();

  mLabelYawColumn = new QLabel(mGroupBoxRotations);
  horizontalLayoutYawPitchRoll->addWidget(mLabelYawColumn);

  mComboBoxYawColumn = new QComboBox(mGroupBoxRotations);
  horizontalLayoutYawPitchRoll->addWidget(mComboBoxYawColumn);

  QSpacerItem *horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  horizontalLayoutYawPitchRoll->addItem(horizontalSpacer_8);

  mLabelPitchColumn = new QLabel(mGroupBoxRotations);
  horizontalLayoutYawPitchRoll->addWidget(mLabelPitchColumn);

  mComboBoxPitchColumn = new QComboBox(mGroupBoxRotations);
  horizontalLayoutYawPitchRoll->addWidget(mComboBoxPitchColumn);

  QSpacerItem *horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

  horizontalLayoutYawPitchRoll->addItem(horizontalSpacer_9);

  mLabelRollColumn = new QLabel(mGroupBoxRotations);
  horizontalLayoutYawPitchRoll->addWidget(mLabelRollColumn);

  mComboBoxRollColumn = new QComboBox(mGroupBoxRotations);
  horizontalLayoutYawPitchRoll->addWidget(mComboBoxRollColumn);

  gridLayoutYawPitchRoll->addLayout(horizontalLayoutYawPitchRoll, 0, 0, 1, 1);

  mStackedWidget->addWidget(pageYawPitchRoll);



  pageOmegaPhiKappa = new QWidget();
  QGridLayout *gridOmegaPhiKappa = new QGridLayout(pageOmegaPhiKappa);

  QHBoxLayout *horizontalLayoutOmegaPhiKappa = new QHBoxLayout();

  mLabelOmegaColumn = new QLabel(mGroupBoxRotations);
  horizontalLayoutOmegaPhiKappa->addWidget(mLabelOmegaColumn);

  mComboBoxOmegaColumn = new QComboBox(mGroupBoxRotations);
  horizontalLayoutOmegaPhiKappa->addWidget(mComboBoxOmegaColumn);

  QSpacerItem *horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  horizontalLayoutOmegaPhiKappa->addItem(horizontalSpacer_6);

  mLabelPhiColumn = new QLabel(mGroupBoxRotations);
  horizontalLayoutOmegaPhiKappa->addWidget(mLabelPhiColumn);

  mComboBoxPhiColumn = new QComboBox(mGroupBoxRotations);
  horizontalLayoutOmegaPhiKappa->addWidget(mComboBoxPhiColumn);

  QSpacerItem *horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  horizontalLayoutOmegaPhiKappa->addItem(horizontalSpacer_7);

  mLabelKappaColumn = new QLabel(mGroupBoxRotations);
  horizontalLayoutOmegaPhiKappa->addWidget(mLabelKappaColumn);

  mComboBoxKappaColumn = new QComboBox(mGroupBoxRotations);
  horizontalLayoutOmegaPhiKappa->addWidget(mComboBoxKappaColumn);

  gridOmegaPhiKappa->addLayout(horizontalLayoutOmegaPhiKappa, 0, 0, 1, 1);

  mStackedWidget->addWidget(pageOmegaPhiKappa);

  gridLayoutRotations->addWidget(mStackedWidget, 1, 0, 1, 3);

  gridLayoutColumns->addWidget(mGroupBoxRotations, 2, 0, 1, 3);

  QSpacerItem *horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  gridLayoutColumns->addItem(horizontalSpacer_2, 0, 2, 1, 1);


  gridLayout->addWidget(mGroupBoxColumns, 5, 0, 1, 3);

  mLabelCameras = new QLabel(this);
  gridLayout->addWidget(mLabelPreview, 5, 0, 1, 1);

  mTableViewFormatCameras = new QTableView(this);
  gridLayout->addWidget(mTableViewFormatCameras, 6, 0, 1, 3);

  mButtonBox = new QDialogButtonBox(this);
  mButtonBox->setOrientation(Qt::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 8, 2, 1, 1);

  this->retranslate();
  this->clear();
  this->update();
}

void ImportCamerasViewImp::initSignalAndSlots()
{
  //connect(mLineEditImportCameras, &QLineEdit::textChanged,   this, &ImportCamerasViewImp::update);
  //connect(mPushButtonImportGroundControlPoints,  &QAbstractButton::clicked, this, &ImportCamerasViewImp::openFile);

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
  //connect(mComboBoxImageColumn, &QComboBox::currentTextChanged, this, &ImportCamerasViewImp::update);
  connect(mComboBoxXColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::xFieldIdChanged);
  connect(mComboBoxYColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::yFieldIdChanged);
  connect(mComboBoxZColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportCamerasView::zFieldIdChanged);
  //connect(mComboBoxXColumn, &QComboBox::currentTextChanged, this, &ImportCamerasViewImp::update);
  //connect(mComboBoxYColumn, &QComboBox::currentTextChanged, this, &ImportCamerasViewImp::update);
  //connect(mComboBoxZColumn, &QComboBox::currentTextChanged, this, &ImportCamerasViewImp::update);
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

//  connect(mComboBoxImageColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(uptdate()));
//  connect(mComboBoxXColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(uptdate()));
//  connect(mComboBoxYColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(uptdate()));
//  connect(mComboBoxZColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(uptdate()));

  connect(mButtonBox,                                   &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Ok),     &QAbstractButton::clicked,   this, &QDialog::accept);
  connect(mButtonBox->button(QDialogButtonBox::Help),   &QAbstractButton::clicked,   this, &DialogView::help);
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
  bParseOk = false;
  bValidCrsIn = false;
  bValidCrsOut = false;

  update();
}

void ImportCamerasViewImp::update()
{
//  QFileInfo file_info(mLineEditImportCameras->text());
//  bool file_exist = file_info.exists();

//  QPalette palette;
//  if(file_exist){
//    palette.setColor(QPalette::Text, Qt::black);
//  } else {
//    palette.setColor(QPalette::Text, Qt::red);
//  }
//  mLineEditImportCameras->setPalette(palette);

//  bool bx = mComboBoxXColumn->currentText().compare("--") != 0;
//  bool by = mComboBoxYColumn->currentText().compare("--") != 0;
//  bool bz = mComboBoxZColumn->currentText().compare("--") != 0;

//  mPushButtonImportCameras->setEnabled(file_exist && bx && by && bz);

//  QString rotation = mComboBoxRotation->currentText();
//  bool bQuaternions = rotation.compare("Quaternions") == 0;
//  bool bYawPitchRoll = rotation.compare("Yaw, Pitch, Roll") == 0;
//  bool bOmegaPhiKappa = rotation.compare("Omega, Phi, Kappa") == 0;

//  mComboBoxQxColumn->setVisible(bQuaternions);
//  mComboBoxQyColumn->setVisible(bQuaternions);
//  mComboBoxQzColumn->setVisible(bQuaternions);
//  mComboBoxQwColumn->setVisible(bQuaternions);
//  mComboBoxYawColumn->setVisible(bYawPitchRoll);
//  mComboBoxPitchColumn->setVisible(bYawPitchRoll);
//  mComboBoxRollColumn->setVisible(bYawPitchRoll);
//  mComboBoxOmegaColumn->setVisible(bOmegaPhiKappa);
//  mComboBoxPhiColumn->setVisible(bOmegaPhiKappa);
//  mComboBoxKappaColumn->setVisible(bOmegaPhiKappa);
//  mLabelQxColumn->setVisible(bQuaternions);
//  mLabelQyColumn->setVisible(bQuaternions);
//  mLabelQzColumn->setVisible(bQuaternions);
//  mLabelQwColumn->setVisible(bQuaternions);
//  mLabelYawColumn->setVisible(bYawPitchRoll);
//  mLabelPitchColumn->setVisible(bYawPitchRoll);
//  mLabelRollColumn->setVisible(bYawPitchRoll);
//  mLabelOmegaColumn->setVisible(bOmegaPhiKappa);
//  mLabelPhiColumn->setVisible(bOmegaPhiKappa);
//  mLabelKappaColumn->setVisible(bOmegaPhiKappa);

  mLineEditOther->setEnabled(mRadioButtonOther->isChecked());

//  bool bSave = false;

//  bool checkX = false;
//  mComboBoxXColumn->currentText().toDouble(&checkX);
//  bool checkY = false;
//  mComboBoxYColumn->currentText().toDouble(&checkY);
//  bool checkZ = false;
//  mComboBoxZColumn->currentText().toDouble(&checkZ);

//  bSave = checkX && checkY && checkZ;
//  mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(bSave);

  mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(/*bParseOk && */
                                                       ((bValidCrsIn && bValidCrsOut) || 
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

  //mPushButtonImportGroundControlPoints->setText(QCoreApplication::translate("ImportCamerasViewImp", "Import Cameras", nullptr));
  //mPushButtonImportCameras->setText(QCoreApplication::translate("ImportCamerasViewImp", "ImportCameras", nullptr));
}

//void ImportCamerasViewImp::openFile()
//{
//  QString file = QFileDialog::getOpenFileName(Q_NULLPTR,
//                                              tr("Orientation cameras file"),
//                                              mProjectPath,
//                                              tr("Comma-separated values (*.csv)"));
//  if (!file.isEmpty()){
//    mLineEditImportCameras->setText(file);

//    emit loadCSV(file, ",");
//  }
//}

//QString ImportCamerasViewImp::orientationFile() const
//{
//  return mLineEditImportCameras->text();
//}

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

//  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(mTableViewGroundControlPoints->model());
//  model->setColumnCount(header.size());
//  model->setHorizontalHeaderLabels(header);
}

void ImportCamerasViewImp::setItemModel(QStandardItemModel *model)
{
  mTableViewImportCameras->setModel(model);
}

void ImportCamerasViewImp::setItemModelFormatCameras(QStandardItemModel *model)
{
    //mItemModelCameras->setColumnCount(4);
  //mItemModelCameras->setHorizontalHeaderLabels(QStringList() << "Image" << "X" << "Y" << "Z");

  mTableViewFormatCameras->setModel(model);
}

void ImportCamerasViewImp::setImageColumn(const QString &imageColumn)
{
  //QSignalBlocker blocker(mComboBoxImageColumn);
  mComboBoxImageColumn->setCurrentText(imageColumn);
  update();
}

void ImportCamerasViewImp::setXColumn(const QString &xColumn)
{
  //QSignalBlocker blocker(mComboBoxXColumn);
  mComboBoxXColumn->setCurrentText(xColumn);
  update();
}

void ImportCamerasViewImp::setYColumn(const QString &yColumn)
{
  //QSignalBlocker blocker(mComboBoxYColumn);
  mComboBoxYColumn->setCurrentText(yColumn);
  update();
}

void ImportCamerasViewImp::setZColumn(const QString &zColumn)
{
  //QSignalBlocker blocker(mComboBoxZColumn);
  mComboBoxZColumn->setCurrentText(zColumn);
  update();
}

void ImportCamerasViewImp::setQxColumn(const QString &qxColumn)
{
  //QSignalBlocker blocker(mComboBoxQxColumn);
  mComboBoxQxColumn->setCurrentText(qxColumn);
  update();
}

void ImportCamerasViewImp::setQyColumn(const QString &qyColumn)
{
  //QSignalBlocker blocker(mComboBoxQyColumn);
  mComboBoxQyColumn->setCurrentText(qyColumn);
  update();
}

void ImportCamerasViewImp::setQzColumn(const QString &qzColumn)
{
  //QSignalBlocker blocker7(mComboBoxQzColumn);
  mComboBoxQzColumn->setCurrentText(qzColumn);
  update();
}

void ImportCamerasViewImp::setQwColumn(const QString &qwColumn)
{
  //QSignalBlocker blocker(mComboBoxQwColumn);
  mComboBoxQwColumn->setCurrentText(qwColumn);
  update();
}

void ImportCamerasViewImp::setOmegaColumn(const QString &omegaColumn)
{
  //QSignalBlocker blocker(mComboBoxOmegaColumn);
  mComboBoxOmegaColumn->setCurrentText(omegaColumn);
  update();
}

void ImportCamerasViewImp::setPhiColumn(const QString &phiColumn)
{
  //QSignalBlocker blocker(mComboBoxPhiColumn);
  mComboBoxPhiColumn->setCurrentText(phiColumn);
  update();
}

void ImportCamerasViewImp::setKappaColumn(const QString &kappaColumn)
{
  //QSignalBlocker blocker(mComboBoxKappaColumn);
  mComboBoxKappaColumn->setCurrentText(kappaColumn);
  update();
}

void ImportCamerasViewImp::setYawColumn(const QString &yawColumn)
{
  //QSignalBlocker blocker(mComboBoxYawColumn);
  mComboBoxYawColumn->setCurrentText(yawColumn);
  update();
}

void ImportCamerasViewImp::setPitchColumn(const QString &pitchColumn)
{
  //QSignalBlocker blocker(mComboBoxPitchColumn);
  mComboBoxPitchColumn->setCurrentText(pitchColumn);
  update();
}

void ImportCamerasViewImp::setRollColumn(const QString &rollColumn)
{
  //QSignalBlocker blocker(mComboBoxRollColumn);
  mComboBoxRollColumn->setCurrentText(rollColumn);
  update();
}

void ImportCamerasViewImp::setParseOk(bool parseOk)
{
  bParseOk = parseOk;
  update();
}

void ImportCamerasViewImp::setValidInputCRS(bool valid)
{
  bValidCrsIn = valid;
  update();
}

void ImportCamerasViewImp::setValidOutputCRS(bool valid)
{
  bValidCrsOut = valid;
  update();
}

void ImportCamerasViewImp::setOutputCRS(const QString &crs)
{
  if (!crs.isEmpty()) {
    QSignalBlocker blocker(mLineEditCrsOutput);
    mLineEditCrsOutput->setText(crs);
    mLineEditCrsOutput->setEnabled(false); // CRS de proyecto. No editable
    bValidCrsOut = true;
  } else bValidCrsOut = false;
}

} // namespace graphos


