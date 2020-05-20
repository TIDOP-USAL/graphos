#include "FeatureExtractorView.h"

#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QApplication>

namespace inspector
{
	
namespace ui
{

FeatureExtractorViewImp::FeatureExtractorViewImp(QWidget *parent)
  : FeatureExtractorView(parent),
    mButtonBox(new QDialogButtonBox(this))
{
  this->initUI();
  this->initSignalAndSlots();
}

FeatureExtractorViewImp::~FeatureExtractorViewImp()
{

}

void FeatureExtractorViewImp::onCheckBoxFullImageChange()
{
  mSpinBoxMaxImageSize->setEnabled(!mCheckBoxFullImage->isChecked());
}

void FeatureExtractorViewImp::addDetectorDescriptor(QWidget *detectorExtractor)
{
  mComboBoxDetectorDescriptor->addItem(detectorExtractor->windowTitle());
  mGridLayoutDetectorDescriptor->addWidget(detectorExtractor, 0, 0, 1, 2);
  detectorExtractor->setVisible(false);

  this->update();
}

QString FeatureExtractorViewImp::currentDetectorDescriptor() const
{
  return mComboBoxDetectorDescriptor->currentText();
}

int FeatureExtractorViewImp::maxImageSize() const
{
  return mSpinBoxMaxImageSize->value();
}

bool FeatureExtractorViewImp::fullImageSize() const
{
  return mCheckBoxFullImage->isChecked();
}

void FeatureExtractorViewImp::setCurrentDetectorDescriptor(const QString &detectorExtractor)
{
  const QSignalBlocker blocker(mComboBoxDetectorDescriptor);
  mComboBoxDetectorDescriptor->setCurrentText(detectorExtractor);
  for (int idx = 0; idx < mGridLayoutDetectorDescriptor->count(); idx++){
    QLayoutItem * const item = mGridLayoutDetectorDescriptor->itemAt(idx);
    if(dynamic_cast<QWidgetItem *>(item)){
      if (item->widget()->windowTitle().compare(detectorExtractor) == 0)
        item->widget()->setVisible(true);
      else
        item->widget()->setVisible(false);
    }
    }
}

void FeatureExtractorViewImp::setMaxImageSize(int imageSize)
{
  mSpinBoxMaxImageSize->setValue(imageSize);
}

void FeatureExtractorViewImp::setFullImageSize(bool fullImageSize)
{
  mCheckBoxFullImage->setChecked(fullImageSize);
}

void FeatureExtractorViewImp::initUI()
{
  this->setWindowIcon(QIcon(":/ico/app/img/FMELogo.ico"));
  this->resize(350, 300);

  QGridLayout *gridLayout = new QGridLayout();
  this->setLayout(gridLayout);

  mCheckBoxFullImage = new QCheckBox(this);
  mCheckBoxFullImage->setChecked(false);
  gridLayout->addWidget(mCheckBoxFullImage, 0, 0, 1, 1);

  gridLayout->addWidget(new QLabel("Max Image Size:"), 1, 0, 1, 1);
  mSpinBoxMaxImageSize = new QSpinBox(this);
  mSpinBoxMaxImageSize->setRange(100, 100000);
  mSpinBoxMaxImageSize->setValue(2000);
  gridLayout->addWidget(mSpinBoxMaxImageSize, 1, 1, 1, 1);

  mLabelDetectorDescriptor = new QLabel(this);
  gridLayout->addWidget(mLabelDetectorDescriptor, 2, 0, 1, 1);
  mComboBoxDetectorDescriptor = new QComboBox(this);
  gridLayout->addWidget(mComboBoxDetectorDescriptor, 2, 1, 1, 1);

  QWidget *widgetFeatureExtractor = new QWidget();
  mGridLayoutDetectorDescriptor = new QGridLayout(widgetFeatureExtractor);
  mGridLayoutDetectorDescriptor->setContentsMargins(0, 0, 0, 0);
  gridLayout->addWidget(widgetFeatureExtractor, 3, 0, 1, 2);

  gridLayout->addItem(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding), 2, 0, 1, 2);

  mButtonBox->setOrientation(Qt::Orientation::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 4, 0, 1, 2);

  this->retranslate();
  this->update();
}

void FeatureExtractorViewImp::initSignalAndSlots()
{
  connect(mCheckBoxFullImage,   SIGNAL(clicked(bool)),               this, SLOT(onCheckBoxFullImageChange()));
  connect(mComboBoxDetectorDescriptor,     SIGNAL(currentTextChanged(QString)), this, SIGNAL(keypointDetectorChange(QString)));

  connect(mButtonBox,                                    SIGNAL(rejected()),      this, SLOT(reject()));
  connect(mButtonBox->button(QDialogButtonBox::Apply),   SIGNAL(clicked(bool)),   this, SIGNAL(run()));
  connect(mButtonBox->button(QDialogButtonBox::Help),    SIGNAL(clicked(bool)),   this, SIGNAL(help()));
}

void FeatureExtractorViewImp::clear()
{
//  mCurrentKeypointDetector.clear();
//  mCurrentDescriptorExtractor.clear();
}

void FeatureExtractorViewImp::update()
{
  int detectorDescriptorCount = mComboBoxDetectorDescriptor->count();
  mLabelDetectorDescriptor->setVisible(detectorDescriptorCount > 1);
  mComboBoxDetectorDescriptor->setVisible(detectorDescriptorCount > 1);
}

void FeatureExtractorViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("FeatureExtractorView", "Feature Detector/Descriptor"));
  mCheckBoxFullImage->setText(QApplication::translate("FeatureExtractorView", "Full Image Size"));
  mLabelDetectorDescriptor->setText(QApplication::translate("FeatureExtractorView", "Keypoint Detector/Descriptor:"));
  mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("FeatureExtractorView", "Cancel"));
  mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("FeatureExtractorView", "Run"));
  mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("FeatureExtractorView", "Help"));
}

} // namespace ui

} // namespace inspector
