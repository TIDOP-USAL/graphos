#include "GeoreferenceView.h"

#include <QFileDialog>
#include <QIcon>
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

namespace inspector
{

namespace ui
{

GeoreferenceViewImp::GeoreferenceViewImp(QWidget *parent)
  : GeoreferenceView(parent),
    mProjectPath("")
{
  this->initUI();
  this->initSignalAndSlots();
}

GeoreferenceViewImp::~GeoreferenceViewImp()
{

}

void GeoreferenceViewImp::initUI()
{
  this->setObjectName(QStringLiteral("ExportOrientationsView"));
  this->setWindowIcon(QIcon(":/ico/app/img/FMELogo.ico"));
  this->resize(400, 200);

  QGridLayout *gridLayout = new QGridLayout();
  this->setLayout(gridLayout);

  mHorizontalLayoutImportCameras = new QHBoxLayout();
  mLineEditImportCameras = new QLineEdit(this);
  mLineEditImportCameras->setEnabled(false);
  mHorizontalLayoutImportCameras->addWidget(mLineEditImportCameras);
  mPushButtonImportCameras = new QPushButton(this);
  mHorizontalLayoutImportCameras->addWidget(mPushButtonImportCameras);
  gridLayout->addLayout(mHorizontalLayoutImportCameras, 0, 0, 1, 2);

  mTableViewImportCameras = new QTableView(this);
  mTableViewImportCameras->setAlternatingRowColors(true);
  gridLayout->addWidget(mTableViewImportCameras, 1, 0, 1, 2);

  mGridLayoutSelectColmns = new QGridLayout();
  mLabelImageColumn = new QLabel(this);
  mGridLayoutSelectColmns->addWidget(mLabelImageColumn, 0, 0, 1, 1);
  mComboBoxImageColumn = new QComboBox(this);
  mGridLayoutSelectColmns->addWidget(mComboBoxImageColumn, 0, 1, 1, 1);
  mLabelXColumn = new QLabel(this);
  mGridLayoutSelectColmns->addWidget(mLabelXColumn, 0, 2, 1, 1);
  mComboBoxXColumn = new QComboBox(this);
  mGridLayoutSelectColmns->addWidget(mComboBoxXColumn, 0, 3, 1, 1);
  mLabelYColumn = new QLabel(this);
  mGridLayoutSelectColmns->addWidget(mLabelYColumn, 0, 4, 1, 1);
  mComboBoxYColumn = new QComboBox(this);
  mGridLayoutSelectColmns->addWidget(mComboBoxYColumn, 0, 5, 1, 1);
  mLabelZColumn = new QLabel(this);
  mGridLayoutSelectColmns->addWidget(mLabelZColumn, 0, 6, 1, 1);
  mComboBoxZColumn = new QComboBox(this);
  mGridLayoutSelectColmns->addWidget(mComboBoxZColumn, 0, 7, 1, 1);
  mLabelQxColumn = new QLabel(this);
  mGridLayoutSelectColmns->addWidget(mLabelQxColumn, 1, 0, 1, 1);
  mComboBoxQxColumn = new QComboBox(this);
  mGridLayoutSelectColmns->addWidget(mComboBoxQxColumn, 1, 1, 1, 1);
  mLabelQyColumn = new QLabel(this);
  mGridLayoutSelectColmns->addWidget(mLabelQyColumn, 1, 2, 1, 1);
  mComboBoxQyColumn = new QComboBox(this);
  mGridLayoutSelectColmns->addWidget(mComboBoxQyColumn, 1, 3, 1, 1);
  mLabelQzColumn = new QLabel(this);
  mGridLayoutSelectColmns->addWidget(mLabelQzColumn, 1, 4, 1, 1);
  mComboBoxQzColumn = new QComboBox(this);
  mGridLayoutSelectColmns->addWidget(mComboBoxQzColumn, 1, 5, 1, 1);
  mLabelQwColumn = new QLabel(this);
  mGridLayoutSelectColmns->addWidget(mLabelQwColumn, 1, 6, 1, 1);
  mComboBoxQwColumn = new QComboBox(this);
  mGridLayoutSelectColmns->addWidget(mComboBoxQwColumn, 1, 7, 1, 1);

  gridLayout->addLayout(mGridLayoutSelectColmns, 2, 0, 1, 2);

  mHorizontalLayoutCRS = new QHBoxLayout();
  mLabelCrsInput = new QLabel(this);
  mHorizontalLayoutCRS->addWidget(mLabelCrsInput);
  mLineEditCrsInput = new QLineEdit(this);
  mHorizontalLayoutCRS->addWidget(mLineEditCrsInput);
  mLabelCrsOutput = new QLabel(this);
  mHorizontalLayoutCRS->addWidget(mLabelCrsOutput);
  mLineEditCrsOutput = new QLineEdit(this);
  mHorizontalLayoutCRS->addWidget(mLineEditCrsOutput);
  gridLayout->addLayout(mHorizontalLayoutCRS, 3, 0, 1, 2);

//  mPushButtonGeoreference = new QPushButton(this);
//  gridLayout->addWidget(mPushButtonGeoreference, 4, 0, 1, 2);

  mTreeWidgetGeoreferencedCameras = new QTreeWidget(this);
  gridLayout->addWidget(mTreeWidgetGeoreferencedCameras, 5, 0, 1, 2);


  mButtonBox = new QDialogButtonBox(this);
  mButtonBox->setOrientation(Qt::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 6, 1, 1, 1);

  this->retranslate();
  this->update();
}

void GeoreferenceViewImp::initSignalAndSlots()
{
  connect(mLineEditImportCameras, &QLineEdit::textChanged,   this, &GeoreferenceViewImp::update);
  connect(mPushButtonImportCameras,  &QAbstractButton::clicked, this, &GeoreferenceViewImp::openFile);
  connect(mComboBoxImageColumn, &QComboBox::currentTextChanged, this, &GeoreferenceView::imageColumnChange);
  connect(mComboBoxImageColumn, &QComboBox::currentTextChanged, this, &GeoreferenceViewImp::update);
  connect(mComboBoxXColumn, &QComboBox::currentTextChanged, this, &GeoreferenceView::xColumnChange);
  connect(mComboBoxYColumn, &QComboBox::currentTextChanged, this, &GeoreferenceView::yColumnChange);
  connect(mComboBoxZColumn, &QComboBox::currentTextChanged, this, &GeoreferenceView::zColumnChange);
  connect(mComboBoxXColumn, &QComboBox::currentTextChanged, this, &GeoreferenceViewImp::update);
  connect(mComboBoxYColumn, &QComboBox::currentTextChanged, this, &GeoreferenceViewImp::update);
  connect(mComboBoxZColumn, &QComboBox::currentTextChanged, this, &GeoreferenceViewImp::update);
  connect(mComboBoxQxColumn, &QComboBox::currentTextChanged, this, &GeoreferenceView::qxColumnChange);
  connect(mComboBoxQyColumn, &QComboBox::currentTextChanged, this, &GeoreferenceView::qyColumnChange);
  connect(mComboBoxQzColumn, &QComboBox::currentTextChanged, this, &GeoreferenceView::qzColumnChange);
  connect(mComboBoxQwColumn, &QComboBox::currentTextChanged, this, &GeoreferenceView::qwColumnChange);
  connect(mLineEditCrsInput, &QLineEdit::textChanged, this, &GeoreferenceViewImp::update);
  connect(mLineEditCrsOutput, &QLineEdit::textChanged, this, &GeoreferenceViewImp::update);

  connect(mButtonBox,                                   &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Ok),     &QAbstractButton::clicked,   this, &QDialog::accept);
  connect(mButtonBox->button(QDialogButtonBox::Help),   &QAbstractButton::clicked,   this, &IDialogView::help);
}

void GeoreferenceViewImp::clear()
{
  QSignalBlocker blocker1(mComboBoxImageColumn);
  QSignalBlocker blocker2(mComboBoxXColumn);
  QSignalBlocker blocker3(mComboBoxYColumn);
  QSignalBlocker blocker4(mComboBoxZColumn);
  QSignalBlocker blocker5(mComboBoxQxColumn);
  QSignalBlocker blocker6(mComboBoxQyColumn);
  QSignalBlocker blocker7(mComboBoxQzColumn);
  QSignalBlocker blocker8(mComboBoxQwColumn);

  mLineEditImportCameras->clear();
  mComboBoxImageColumn->clear();
  mComboBoxXColumn->clear();
  mComboBoxYColumn->clear();
  mComboBoxZColumn->clear();
  mComboBoxQxColumn->clear();
  mComboBoxQyColumn->clear();
  mComboBoxQzColumn->clear();
  mComboBoxQwColumn->clear();
  mLineEditCrsInput->clear();
  mLineEditCrsOutput->clear();
  mProjectPath = "";

  update();
}

void GeoreferenceViewImp::update()
{
  QFileInfo file_info(mLineEditImportCameras->text());
  bool file_exist = file_info.exists();

  QPalette palette;
  if(file_exist){
    palette.setColor(QPalette::Text, Qt::black);
  } else {
    palette.setColor(QPalette::Text, Qt::red);
  }
  mLineEditImportCameras->setPalette(palette);

//  bool bx = mComboBoxXColumn->currentText().compare("--") != 0;
//  bool by = mComboBoxYColumn->currentText().compare("--") != 0;
//  bool bz = mComboBoxZColumn->currentText().compare("--") != 0;

//  mPushButtonGeoreference->setEnabled(file_exist && bx && by && bz);
}

void GeoreferenceViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("GeoreferenceViewImp", "Georeference", nullptr));
  mPushButtonImportCameras->setText(QCoreApplication::translate("GeoreferenceViewImp", "Import Cameras", nullptr));
  mLabelCrsInput->setText(QCoreApplication::translate("GeoreferenceViewImp", "CRS input", nullptr));
  mLabelCrsOutput->setText(QCoreApplication::translate("GeoreferenceViewImp", "CRS output", nullptr));
  //mPushButtonGeoreference->setText(QCoreApplication::translate("GeoreferenceViewImp", "Georeference", nullptr));
  mLabelImageColumn->setText(QCoreApplication::translate("GeoreferenceViewImp", "Image:", nullptr));
  mLabelXColumn->setText(QCoreApplication::translate("GeoreferenceViewImp", "X:", nullptr));
  mLabelYColumn->setText(QCoreApplication::translate("GeoreferenceViewImp", "Y:", nullptr));
  mLabelZColumn->setText(QCoreApplication::translate("GeoreferenceViewImp", "Z:", nullptr));
  mLabelQxColumn->setText(QCoreApplication::translate("GeoreferenceViewImp", "Qx:", nullptr));
  mLabelQyColumn->setText(QCoreApplication::translate("GeoreferenceViewImp", "Qy:", nullptr));
  mLabelQzColumn->setText(QCoreApplication::translate("GeoreferenceViewImp", "Qz:", nullptr));
  mLabelQwColumn->setText(QCoreApplication::translate("GeoreferenceViewImp", "Qw", nullptr));
  mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("GeoreferenceViewImp", "Cancel", nullptr));
  mButtonBox->button(QDialogButtonBox::Ok)->setText(QApplication::translate("GeoreferenceViewImp", "Save", nullptr));
  mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("GeoreferenceViewImp", "Help", nullptr));
}

void GeoreferenceViewImp::openFile()
{
  QString file = QFileDialog::getOpenFileName(Q_NULLPTR,
                                              tr("Orientation cameras file"),
                                              mProjectPath,
                                              tr("Comma-separated values (*.csv)"));
  if (!file.isEmpty()){
    mLineEditImportCameras->setText(file);

    emit loadCSV(file, ",");
  }
}

void GeoreferenceViewImp::setProjectPath(const QString &path)
{
  mProjectPath = path;
}

QString GeoreferenceViewImp::orientationFile() const
{
  return mLineEditImportCameras->text();
}

void GeoreferenceViewImp::setTableHeader(const QStringList &header)
{
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

  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(mTableViewImportCameras->model());
  model->setColumnCount(header.size());
  model->setHorizontalHeaderLabels(header);
}

void GeoreferenceViewImp::setItemModel(QStandardItemModel *model)
{
  mTableViewImportCameras->setModel(model);
}

void GeoreferenceViewImp::setImageColumn(const QString &imageColumn)
{
  QSignalBlocker blocker1(mComboBoxImageColumn);
  mComboBoxImageColumn->setCurrentText(imageColumn);
  update();
}

void GeoreferenceViewImp::setXColumn(const QString &xColumn)
{
  QSignalBlocker blocker2(mComboBoxXColumn);
  mComboBoxXColumn->setCurrentText(xColumn);
  update();
}

void GeoreferenceViewImp::setYColumn(const QString &yColumn)
{
  QSignalBlocker blocker3(mComboBoxYColumn);
  mComboBoxYColumn->setCurrentText(yColumn);
  update();
}

void GeoreferenceViewImp::setZColumn(const QString &zColumn)
{
  QSignalBlocker blocker4(mComboBoxZColumn);
  mComboBoxZColumn->setCurrentText(zColumn);
  update();
}

void GeoreferenceViewImp::setQxColumn(const QString &qxColumn)
{
  QSignalBlocker blocker5(mComboBoxQxColumn);
  mComboBoxQxColumn->setCurrentText(qxColumn);
  update();
}

void GeoreferenceViewImp::setQyColumn(const QString &qyColumn)
{
  QSignalBlocker blocker6(mComboBoxQyColumn);
  mComboBoxQyColumn->setCurrentText(qyColumn);
  update();
}

void GeoreferenceViewImp::setQzColumn(const QString &qzColumn)
{
  QSignalBlocker blocker7(mComboBoxQzColumn);
  mComboBoxQzColumn->setCurrentText(qzColumn);
  update();
}

void GeoreferenceViewImp::setQwColumn(const QString &qwColumn)
{
  QSignalBlocker blocker8(mComboBoxQwColumn);
  mComboBoxQwColumn->setCurrentText(qwColumn);
  update();
}

} // namespace ui

} // namespace inspector


