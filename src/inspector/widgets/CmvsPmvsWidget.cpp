#include "CmvsPmvsWidget.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QApplication>

namespace inspector
{

CmvsPmvsWidgetImp::CmvsPmvsWidgetImp(QWidget *parent)
  : CmvsPmvsWidget(parent),
    mGroupBox(new QGroupBox(this)),
    mCheckBoxUseVisibilityInformation(new QCheckBox(this)),
    mLabelImagesPerCluster(new QLabel(this)),
    mSpinBoxImagesPerCluster(new QSpinBox(this)),
    mLabelLevel(new QLabel(this)),
    mSpinBoxLevel(new QSpinBox(this)),
    mLabelCellSize(new QLabel(this)),
    mSpinBoxCellSize(new QSpinBox(this)),
    mLabelThreshold(new QLabel(this)),
    mDoubleSpinBoxThreshold(new QDoubleSpinBox(this)),
    mLabelWindowSize(new QLabel(this)),
    mSpinBoxWindowSize(new QSpinBox(this)),
    mLabelMinimunImageNumber(new QLabel(this)),
    mSpinBoxMinimunImageNumber(new QSpinBox(this))
{
  this->initUI();
  this->initSignalAndSlots();
}

CmvsPmvsWidgetImp::~CmvsPmvsWidgetImp()
{

}

bool CmvsPmvsWidgetImp::useVisibilityInformation() const
{
  return mCheckBoxUseVisibilityInformation->isChecked();
}

int CmvsPmvsWidgetImp::imagesPerCluster() const
{
  return mSpinBoxImagesPerCluster->value();
}

int CmvsPmvsWidgetImp::level() const
{
  return mSpinBoxLevel->value();
}

int CmvsPmvsWidgetImp::cellSize() const
{
  return mSpinBoxCellSize->value();
}

double CmvsPmvsWidgetImp::threshold() const
{
  return mDoubleSpinBoxThreshold->value();
}

int CmvsPmvsWidgetImp::windowSize() const
{
  return mSpinBoxWindowSize->value();
}

int CmvsPmvsWidgetImp::minimunImageNumber() const
{
  return mSpinBoxMinimunImageNumber->value();
}

void CmvsPmvsWidgetImp::setUseVisibilityInformation(bool useVisibilityInformation)
{
  mCheckBoxUseVisibilityInformation->setChecked(useVisibilityInformation);
}

void CmvsPmvsWidgetImp::setImagesPerCluster(int imagesPerCluster)
{
  const QSignalBlocker blockerImagesPerCluster(mSpinBoxImagesPerCluster);
  mSpinBoxImagesPerCluster->setValue(imagesPerCluster);
}

void CmvsPmvsWidgetImp::setLevel(int level)
{
  const QSignalBlocker blockerLevel(mSpinBoxLevel);
  mSpinBoxLevel->setValue(level);
}

void CmvsPmvsWidgetImp::setCellSize(int cellSize)
{
  const QSignalBlocker blockerCellSize(mSpinBoxCellSize);
  mSpinBoxCellSize->setValue(cellSize);
}

void CmvsPmvsWidgetImp::setThreshold(double threshold)
{
  const QSignalBlocker blockerThreshold(mDoubleSpinBoxThreshold);
  mDoubleSpinBoxThreshold->setValue(threshold);
}

void CmvsPmvsWidgetImp::setWindowSize(int windowSize)
{
  const QSignalBlocker blockerWindowSize(mSpinBoxWindowSize);
  mSpinBoxWindowSize->setValue(windowSize);
}

void CmvsPmvsWidgetImp::setMinimunImageNumber(int minimunImageNumber)
{
  const QSignalBlocker blockerMinimunImageNumber(mSpinBoxMinimunImageNumber);
  mSpinBoxMinimunImageNumber->setValue(minimunImageNumber);
}

void CmvsPmvsWidgetImp::update()
{

}

void CmvsPmvsWidgetImp::retranslate()
{
  mGroupBox->setTitle(QApplication::translate("CmvsPmvsWidgetImp", "CMVS/PMVS", nullptr));
  mCheckBoxUseVisibilityInformation->setText(QApplication::translate("CmvsPmvsWidgetImp", "Use Visibility Information:"));
  mLabelImagesPerCluster->setText(QApplication::translate("CmvsPmvsWidgetImp", "Images Per Cluster:"));
  mLabelLevel->setText(QApplication::translate("CmvsPmvsWidgetImp", "Level:"));
  mLabelCellSize->setText(QApplication::translate("CmvsPmvsWidgetImp", "Cell Size:"));
  mLabelThreshold->setText(QApplication::translate("CmvsPmvsWidgetImp", "Threshold:"));
  mLabelWindowSize->setText(QApplication::translate("CmvsPmvsWidgetImp", "Window Size:"));
  mLabelMinimunImageNumber->setText(QApplication::translate("CmvsPmvsWidgetImp", "Minimun Image Number:"));
}

void CmvsPmvsWidgetImp::clear()
{
  const QSignalBlocker blockerImagesPerCluster(mSpinBoxImagesPerCluster);
  const QSignalBlocker blockerLevel(mSpinBoxLevel);
  const QSignalBlocker blockerCellSize(mSpinBoxCellSize);
  const QSignalBlocker blockerThreshold(mDoubleSpinBoxThreshold);
  const QSignalBlocker blockerWindowSize(mSpinBoxWindowSize);
  const QSignalBlocker blockerMinimunImageNumber(mSpinBoxMinimunImageNumber);

  mCheckBoxUseVisibilityInformation->setChecked(true);
  mSpinBoxImagesPerCluster->setValue(100);
  mSpinBoxLevel->setValue(1);
  mSpinBoxCellSize->setValue(2);
  mDoubleSpinBoxThreshold->setValue(0.7);
  mSpinBoxWindowSize->setValue(7);
  mSpinBoxMinimunImageNumber->setValue(3);
}

void CmvsPmvsWidgetImp::initUI()
{
  this->setWindowTitle("CMVS/PMVS");

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  layout->addWidget(mGroupBox);

  QGridLayout *propertiesLayout = new QGridLayout();
  mGroupBox->setLayout(propertiesLayout);

  propertiesLayout->addWidget(mCheckBoxUseVisibilityInformation, 0, 0, 1, 2);

  propertiesLayout->addWidget(mLabelImagesPerCluster, 1, 0);
  mSpinBoxImagesPerCluster->setRange(1, 1000);
  propertiesLayout->addWidget(mSpinBoxImagesPerCluster, 1, 1);

  propertiesLayout->addWidget(mLabelLevel, 2, 0);
  mSpinBoxLevel->setRange(0, 6);
  propertiesLayout->addWidget(mSpinBoxLevel, 2, 1);

  propertiesLayout->addWidget(mLabelCellSize, 3, 0);
  mSpinBoxCellSize->setRange(1, 7);
  propertiesLayout->addWidget(mSpinBoxCellSize, 3, 1);  

  propertiesLayout->addWidget(mLabelThreshold, 4, 0);
  mDoubleSpinBoxThreshold->setRange(0.0, 1.0);
  mDoubleSpinBoxThreshold->setDecimals(2);
  mDoubleSpinBoxThreshold->setSingleStep(0.1);
  propertiesLayout->addWidget(mDoubleSpinBoxThreshold, 4, 1);  

  propertiesLayout->addWidget(mLabelWindowSize, 5, 0);
  mSpinBoxWindowSize->setRange(5, 20);
  propertiesLayout->addWidget(mSpinBoxWindowSize, 5, 1); 
    
  propertiesLayout->addWidget(mLabelMinimunImageNumber, 6, 0);
  mSpinBoxMinimunImageNumber->setRange(2, 6);
  propertiesLayout->addWidget(mSpinBoxMinimunImageNumber, 6, 1); 

  this->retranslate();
  this->clear(); /// set default values
  this->update();
}

void CmvsPmvsWidgetImp::initSignalAndSlots()
{
  connect(mCheckBoxUseVisibilityInformation, &QAbstractButton::clicked,                            this, &CmvsPmvsWidget::useVisibilityInformationChanged);
  connect(mSpinBoxImagesPerCluster,          QOverload<int>::of(&QSpinBox::valueChanged),          this, &CmvsPmvsWidget::imagesPerClusterChanged);
  connect(mSpinBoxLevel,                     QOverload<int>::of(&QSpinBox::valueChanged),          this, &CmvsPmvsWidget::levelChanged);
  connect(mSpinBoxCellSize,                  QOverload<int>::of(&QSpinBox::valueChanged),          this, &CmvsPmvsWidget::cellSizeChanged);
  connect(mDoubleSpinBoxThreshold,           QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CmvsPmvsWidget::thresholdChanged);
  connect(mSpinBoxWindowSize,                QOverload<int>::of(&QSpinBox::valueChanged),          this, &CmvsPmvsWidget::windowSizeChanged);
  connect(mSpinBoxMinimunImageNumber,        QOverload<int>::of(&QSpinBox::valueChanged),          this, &CmvsPmvsWidget::minimunImageNumberChanged);
}


} // namespace inspector
