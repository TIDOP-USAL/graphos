#include "CameraPositionsView.h"

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

namespace inspector
{

namespace ui
{

CamerasImportViewImp::CamerasImportViewImp(QWidget *parent)
  : CamerasImportView(parent)
{
  this->initUI();
  this->initSignalAndSlots();
}

CamerasImportViewImp::~CamerasImportViewImp()
{

}

void CamerasImportViewImp::onDelimiterChanged()
{
  emit delimiterChanged(this->delimiter());
  emit previewCSV();
}

QString CamerasImportViewImp::delimiter() const
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

void CamerasImportViewImp::initUI()
{
  this->setObjectName(QStringLiteral("ExportOrientationsView"));
  this->setWindowIcon(QIcon(":/ico/app/img/FMELogo.ico"));
  this->resize(800, 600);
  this->setMaximumWidth(1000);

  QGridLayout *gridLayout = new QGridLayout();
  this->setLayout(gridLayout);

//  mLabelImportRow = new QLabel(this);
//  gridLayout->addWidget(mLabelImportRow, 0, 0, 1, 1);

//  mSpinBoxImportRow = new QSpinBox(this);
//  gridLayout->addWidget(mSpinBoxImportRow, 0, 1, 1, 1);

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

  mLabelPreview = new QLabel(this);
  gridLayout->addWidget(mLabelPreview, 2, 0, 1, 1);

  mTableViewImportCameras = new QTableView(this);
  gridLayout->addWidget(mTableViewImportCameras, 3, 0, 1, 3);

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


  gridLayout->addWidget(mGroupBoxColumns, 4, 0, 1, 3);

  mLabelCameras = new QLabel(this);
  gridLayout->addWidget(mLabelPreview, 5, 0, 1, 1);

  mTableViewFormatCameras = new QTableView(this);
  gridLayout->addWidget(mTableViewFormatCameras, 6, 0, 1, 3);

  mButtonBox = new QDialogButtonBox(this);
  mButtonBox->setOrientation(Qt::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 7, 2, 1, 1);

  this->retranslate();
  this->clear();
  this->update();
}

void CamerasImportViewImp::initSignalAndSlots()
{
  //connect(mLineEditImportCameras, &QLineEdit::textChanged,   this, &CamerasImportViewImp::update);
  //connect(mPushButtonImportGroundControlPoints,  &QAbstractButton::clicked, this, &CamerasImportViewImp::openFile);

  connect(mRadioButtonTab, &QRadioButton::released, this, &CamerasImportViewImp::onDelimiterChanged);
  connect(mRadioButtonComma, &QRadioButton::released, this, &CamerasImportViewImp::onDelimiterChanged);
  connect(mRadioButtonSpace, &QRadioButton::released, this, &CamerasImportViewImp::onDelimiterChanged);
  connect(mRadioButtonSemicolon, &QRadioButton::released, this, &CamerasImportViewImp::onDelimiterChanged);
  connect(mRadioButtonOther, &QRadioButton::released, this, &CamerasImportViewImp::onDelimiterChanged);
  connect(mRadioButtonOther, &QRadioButton::released, this, &CamerasImportViewImp::update);
  connect(mLineEditOther, &QLineEdit::textChanged, this, &CamerasImportViewImp::onDelimiterChanged);
  connect(mCheckBoxFieldNamesAuto, &QCheckBox::clicked, this, &CamerasImportView::loadFieldNamesFromFirstRow);
  connect(mComboBoxImageColumn, &QComboBox::currentTextChanged, this, &CamerasImportView::imageColumnChange);
  connect(mComboBoxImageColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::imageFieldIdChanged);
  //connect(mComboBoxImageColumn, &QComboBox::currentTextChanged, this, &CamerasImportViewImp::update);
  connect(mComboBoxXColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::xFieldIdChanged);
  connect(mComboBoxYColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::yFieldIdChanged);
  connect(mComboBoxZColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::zFieldIdChanged);
  //connect(mComboBoxXColumn, &QComboBox::currentTextChanged, this, &CamerasImportViewImp::update);
  //connect(mComboBoxYColumn, &QComboBox::currentTextChanged, this, &CamerasImportViewImp::update);
  //connect(mComboBoxZColumn, &QComboBox::currentTextChanged, this, &CamerasImportViewImp::update);
  connect(mComboBoxQxColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::qxFieldChange);
  connect(mComboBoxQyColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::qyFieldChange);
  connect(mComboBoxQzColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::qzFieldChange);
  connect(mComboBoxQwColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::qwFieldChange);
  connect(mComboBoxYawColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::yawFieldChange);
  connect(mComboBoxPitchColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::pitchFieldChange);
  connect(mComboBoxRollColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::rollFieldChange);
  connect(mComboBoxOmegaColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::omegaFieldChange);
  connect(mComboBoxPhiColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::phiFieldChange);
  connect(mComboBoxKappaColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CamerasImportView::kappaFieldChange);
  connect(mComboBoxRotation, QOverload<int>::of(&QComboBox::currentIndexChanged), mStackedWidget, &QStackedWidget::setCurrentIndex);
  connect(mComboBoxRotation, &QComboBox::currentTextChanged, this, &CamerasImportView::rotationChange);
  connect(mLineEditCrsInput, &QLineEdit::textChanged, this, &CamerasImportViewImp::crsInputChanged);
  connect(mLineEditCrsOutput, &QLineEdit::textChanged, this, &CamerasImportViewImp::crsOutputChanged);

//  connect(mComboBoxImageColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(uptdate()));
//  connect(mComboBoxXColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(uptdate()));
//  connect(mComboBoxYColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(uptdate()));
//  connect(mComboBoxZColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(uptdate()));

  connect(mButtonBox,                                   &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Ok),     &QAbstractButton::clicked,   this, &QDialog::accept);
  connect(mButtonBox->button(QDialogButtonBox::Help),   &QAbstractButton::clicked,   this, &IDialogView::help);
}

void CamerasImportViewImp::clear()
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
//  mLineEditCrsInput->clear();
//  mLineEditCrsOutput->clear();
  bParseOk = bValidCRS = false;

  update();
}

void CamerasImportViewImp::update()
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

//  mPushButtonCameraPositions->setEnabled(file_exist && bx && by && bz);

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

  mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(bParseOk && bValidCRS);
}

void CamerasImportViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("CamerasImportViewImp", "Import cameras", nullptr));
  mGroupBoxDelimiter->setTitle(QCoreApplication::translate("CamerasImportViewImp", "Delimiter", nullptr));
  mRadioButtonTab->setText(QCoreApplication::translate("CamerasImportViewImp", "Tab", nullptr));
  mRadioButtonComma->setText(QCoreApplication::translate("CamerasImportViewImp", "Comma", nullptr));
  mRadioButtonSpace->setText(QCoreApplication::translate("CamerasImportViewImp", "Space", nullptr));
  mRadioButtonSemicolon->setText(QCoreApplication::translate("CamerasImportViewImp", "Semicolon", nullptr));
  mRadioButtonOther->setText(QCoreApplication::translate("CamerasImportViewImp", "Other", nullptr));
  mCheckBoxFieldNamesAuto->setText(QCoreApplication::translate("CamerasImportViewImp", "First record has field names", nullptr));
  //mLabelImportRow->setText(QCoreApplication::translate("CamerasImportViewImp", "From file", nullptr));
  mGroupBoxColumns->setTitle(QCoreApplication::translate("CamerasImportViewImp", "Columns", nullptr));
  mLabelImageColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "Image name:", nullptr));
  mGroupBoxCoordinates->setTitle(QCoreApplication::translate("CamerasImportViewImp", "Coordinates", nullptr));
  mLabelYColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "Y:", nullptr));
  mLabelXColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "X:", nullptr));
  mLabelZColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "Z:", nullptr));
  mLabelCrs->setText(QCoreApplication::translate("CamerasImportViewImp", "CRS Input:", nullptr));
  mLabelCrsOut->setText(QCoreApplication::translate("CamerasImportViewImp", "CRS Output:", nullptr));
  mGroupBoxRotations->setTitle(QCoreApplication::translate("CamerasImportViewImp", "Rotations", nullptr));
  mLabelRotation->setText(QCoreApplication::translate("CamerasImportViewImp", "Rotation", nullptr));
  mComboBoxRotation->setItemText(0, QCoreApplication::translate("CamerasImportViewImp", "Quaternions", nullptr));
  mComboBoxRotation->setItemText(1, QCoreApplication::translate("CamerasImportViewImp", "Yaw, Pitch, Roll", nullptr));
  mComboBoxRotation->setItemText(2, QCoreApplication::translate("CamerasImportViewImp", "Omega, Phi, Kappa", nullptr));
  mLabelQxColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "Qx:", nullptr));
  mLabelQyColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "Qy:", nullptr));
  mLabelQzColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "Qz:", nullptr));
  mLabelQwColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "Qw:", nullptr));
  mLabelYawColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "Yaw:", nullptr));
  mLabelPitchColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "Pitch:", nullptr));
  mLabelRollColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "Roll:", nullptr));
  mLabelOmegaColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "Omega:", nullptr));
  mLabelPhiColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "Phi:", nullptr));
  mLabelKappaColumn->setText(QCoreApplication::translate("CamerasImportViewImp", "Kappa:", nullptr));
  mLabelPreview->setText(QCoreApplication::translate("CamerasImportViewImp", "Preview:", nullptr));
  mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("CamerasImportViewImp", "Cancel", nullptr));
  mButtonBox->button(QDialogButtonBox::Ok)->setText(QApplication::translate("CamerasImportViewImp", "Save", nullptr));
  mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("CamerasImportViewImp", "Help", nullptr));

  //mPushButtonImportGroundControlPoints->setText(QCoreApplication::translate("CamerasImportViewImp", "Import Cameras", nullptr));
  //mPushButtonCameraPositions->setText(QCoreApplication::translate("CamerasImportViewImp", "CameraPositions", nullptr));
}

//void CamerasImportViewImp::openFile()
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

//QString CamerasImportViewImp::orientationFile() const
//{
//  return mLineEditImportCameras->text();
//}

void CamerasImportViewImp::setTableHeader(const QStringList &header)
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

void CamerasImportViewImp::setItemModel(QStandardItemModel *model)
{
  mTableViewImportCameras->setModel(model);
}

void CamerasImportViewImp::setItemModelFormatCameras(QStandardItemModel *model)
{
    //mItemModelCameras->setColumnCount(4);
  //mItemModelCameras->setHorizontalHeaderLabels(QStringList() << "Image" << "X" << "Y" << "Z");

  mTableViewFormatCameras->setModel(model);
}

void CamerasImportViewImp::setImageColumn(const QString &imageColumn)
{
  //QSignalBlocker blocker(mComboBoxImageColumn);
  mComboBoxImageColumn->setCurrentText(imageColumn);
  update();
}

void CamerasImportViewImp::setXColumn(const QString &xColumn)
{
  //QSignalBlocker blocker(mComboBoxXColumn);
  mComboBoxXColumn->setCurrentText(xColumn);
  update();
}

void CamerasImportViewImp::setYColumn(const QString &yColumn)
{
  //QSignalBlocker blocker(mComboBoxYColumn);
  mComboBoxYColumn->setCurrentText(yColumn);
  update();
}

void CamerasImportViewImp::setZColumn(const QString &zColumn)
{
  //QSignalBlocker blocker(mComboBoxZColumn);
  mComboBoxZColumn->setCurrentText(zColumn);
  update();
}

void CamerasImportViewImp::setQxColumn(const QString &qxColumn)
{
  QSignalBlocker blocker(mComboBoxQxColumn);
  mComboBoxQxColumn->setCurrentText(qxColumn);
  update();
}

void CamerasImportViewImp::setQyColumn(const QString &qyColumn)
{
  QSignalBlocker blocker(mComboBoxQyColumn);
  mComboBoxQyColumn->setCurrentText(qyColumn);
  update();
}

void CamerasImportViewImp::setQzColumn(const QString &qzColumn)
{
  QSignalBlocker blocker7(mComboBoxQzColumn);
  mComboBoxQzColumn->setCurrentText(qzColumn);
  update();
}

void CamerasImportViewImp::setQwColumn(const QString &qwColumn)
{
  QSignalBlocker blocker(mComboBoxQwColumn);
  mComboBoxQwColumn->setCurrentText(qwColumn);
  update();
}

void CamerasImportViewImp::setOmegaColumn(const QString &omegaColumn)
{
  QSignalBlocker blocker(mComboBoxOmegaColumn);
  mComboBoxOmegaColumn->setCurrentText(omegaColumn);
  update();
}

void CamerasImportViewImp::setPhiColumn(const QString &phiColumn)
{
  QSignalBlocker blocker(mComboBoxPhiColumn);
  mComboBoxPhiColumn->setCurrentText(phiColumn);
  update();
}

void CamerasImportViewImp::setKappaColumn(const QString &kappaColumn)
{
  QSignalBlocker blocker(mComboBoxKappaColumn);
  mComboBoxKappaColumn->setCurrentText(kappaColumn);
  update();
}

void CamerasImportViewImp::setYawColumn(const QString &yawColumn)
{
  QSignalBlocker blocker(mComboBoxYawColumn);
  mComboBoxYawColumn->setCurrentText(yawColumn);
  update();
}

void CamerasImportViewImp::setPitchColumn(const QString &pitchColumn)
{
  QSignalBlocker blocker(mComboBoxPitchColumn);
  mComboBoxPitchColumn->setCurrentText(pitchColumn);
  update();
}

void CamerasImportViewImp::setRollColumn(const QString &rollColumn)
{
  QSignalBlocker blocker(mComboBoxRollColumn);
  mComboBoxRollColumn->setCurrentText(rollColumn);
  update();
}

void CamerasImportViewImp::setParseOk(bool parseOk)
{
  bParseOk = parseOk;
  update();
}

void CamerasImportViewImp::setValidInputCRS(bool valid)
{
  bValidCRS = valid;
  update();
}

void CamerasImportViewImp::setValidOutputCRS(bool valid)
{

  update();
}

} // namespace ui

} // namespace inspector


