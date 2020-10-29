#include "CameraPositionsView.h"

/* #include <QIcon>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTreeWidget>
#include <QTableView>
#include <QFileDialog>
#include <QComboBox>
#include <QStandardItemModel>
#include <QSpinBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QCheckBox> */

namespace inspector
{

namespace ui
{

ExportPointCloudViewImp::ExportPointCloudViewImp(QWidget *parent)
  : ExportPointCloudView(parent)
{
  this->initUI();
  this->initSignalAndSlots();
}

ExportPointCloudViewImp::~ExportPointCloudViewImp()
{

}

void ExportPointCloudViewImp::onDelimiterChanged()
{
  emit delimiterChanged(this->delimiter());
}

QString ExportPointCloudViewImp::delimiter() const
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

void ExportPointCloudViewImp::initUI()
{
/*   this->setObjectName(QStringLiteral("ExportOrientationsView"));
  this->setWindowIcon(QIcon(":/ico/app/img/FMELogo.ico"));
  this->resize(800, 600);

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
  gridLayout->addWidget(mTableViewFormatCameras, 6, 0, 1, 3); */

  mButtonBox = new QDialogButtonBox(this);
  mButtonBox->setOrientation(Qt::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 7, 2, 1, 1);

  this->retranslate();
  this->clear();
  this->update();
}

void ExportPointCloudViewImp::initSignalAndSlots()
{

  connect(mRadioButtonTab, &QRadioButton::released, this, &ExportPointCloudViewImp::onDelimiterChanged);
  connect(mRadioButtonComma, &QRadioButton::released, this, &ExportPointCloudViewImp::onDelimiterChanged);
  connect(mRadioButtonSpace, &QRadioButton::released, this, &ExportPointCloudViewImp::onDelimiterChanged);
  connect(mRadioButtonSemicolon, &QRadioButton::released, this, &ExportPointCloudViewImp::onDelimiterChanged);

  connect(mButtonBox,                                   &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Ok),     &QAbstractButton::clicked,   this, &QDialog::accept);
  connect(mButtonBox->button(QDialogButtonBox::Help),   &QAbstractButton::clicked,   this, &IDialogView::help);
}

void ExportPointCloudViewImp::clear()
{
  QSignalBlocker blocker1(mRadioButtonComma);

  mRadioButtonComma->setChecked(true);

  update();
}

void ExportPointCloudViewImp::update()
{

}

void ExportPointCloudViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("ExportPointCloudViewImp", "Export Point Cloud", nullptr));
  mGroupBoxDelimiter->setTitle(QCoreApplication::translate("ExportPointCloudViewImp", "Delimiter", nullptr));
  mRadioButtonTab->setText(QCoreApplication::translate("ExportPointCloudViewImp", "Tab", nullptr));
  mRadioButtonComma->setText(QCoreApplication::translate("ExportPointCloudViewImp", "Comma", nullptr));
  mRadioButtonSpace->setText(QCoreApplication::translate("ExportPointCloudViewImp", "Space", nullptr));
  mRadioButtonSemicolon->setText(QCoreApplication::translate("ExportPointCloudViewImp", "Semicolon", nullptr));
}


void ExportPointCloudViewImp::setDelimiter(const QString &qyColumn)
{

}


} // namespace ui

} // namespace inspector


