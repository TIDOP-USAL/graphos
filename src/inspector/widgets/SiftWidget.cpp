#include "SiftWidget.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QApplication>

namespace inspector
{

SiftWidgetImp::SiftWidgetImp(QWidget *parent)
  : SiftWidget(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelFeaturesNumber(new QLabel(this)),
    mFeaturesNumber(new QSpinBox(this)),
    mLabelOctaveLayers(new QLabel(this)),
    mOctaveLayers(new QSpinBox(this)),
    mLabelContrastThreshold(new QLabel(this)),
    mContrastThreshold(new QDoubleSpinBox(this)),
    mLabelEdgeThreshold(new QLabel(this)),
    mEdgeThreshold(new QDoubleSpinBox(this)),
    mLabelSigma(new QLabel(this)),
    mSigma(new QDoubleSpinBox(this))
{
  this->initUI();
  this->initSignalAndSlots();
}

SiftWidgetImp::~SiftWidgetImp()
{

}

void SiftWidgetImp::initUI()
{
  this->setObjectName(QStringLiteral("SiftWidgetImp"));
  this->setWindowTitle("SIFT");

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  layout->addWidget(mGroupBox);

  QGridLayout *propertiesLayout = new QGridLayout();
  mGroupBox->setLayout(propertiesLayout);

  propertiesLayout->addWidget(mLabelFeaturesNumber, 0, 0);
  mFeaturesNumber->setRange(0, 50000);
  propertiesLayout->addWidget(mFeaturesNumber, 0, 1);

  propertiesLayout->addWidget(mLabelOctaveLayers, 1, 0);
  mOctaveLayers->setRange(0, 10);
  propertiesLayout->addWidget(mOctaveLayers, 1, 1);

  propertiesLayout->addWidget(mLabelContrastThreshold, 2, 0);
  mContrastThreshold->setRange(0., 10.);  //TODO: ver que el rango tenga sentido
  mContrastThreshold->setDecimals(3);
  mContrastThreshold->setSingleStep(0.01);
  propertiesLayout->addWidget(mContrastThreshold, 2, 1);

  propertiesLayout->addWidget(mLabelEdgeThreshold, 3, 0);
  mEdgeThreshold->setRange(0., 100.);
  propertiesLayout->addWidget(mEdgeThreshold, 3, 1);

  propertiesLayout->addWidget(mLabelSigma, 4, 0);
  mSigma->setRange(0., 100.);
  mSigma->setSingleStep(0.1);
  propertiesLayout->addWidget(mSigma, 4, 1);

  this->retranslate();
  this->clear(); /// set default values
  this->update();
}

void SiftWidgetImp::initSignalAndSlots()
{
  connect(mFeaturesNumber,    SIGNAL(valueChanged(int)),    this, SIGNAL(featuresNumberChange(int)));
  connect(mOctaveLayers,      SIGNAL(valueChanged(int)),    this, SIGNAL(octaveLayersChange(int)));
  connect(mContrastThreshold, SIGNAL(valueChanged(double)), this, SIGNAL(contrastThresholdChange(double)));
  connect(mEdgeThreshold,     SIGNAL(valueChanged(double)), this, SIGNAL(edgeThresholdChange(double)));
  connect(mSigma,             SIGNAL(valueChanged(double)), this, SIGNAL(sigmaChange(double)));
}

void SiftWidgetImp::clear()
{
  const QSignalBlocker blockerFeaturesNumber(mFeaturesNumber);
  const QSignalBlocker blockerOctaveLayers(mOctaveLayers);
  const QSignalBlocker blockerContrastThreshold(mContrastThreshold);
  const QSignalBlocker blockerEdgeThreshold(mEdgeThreshold);
  const QSignalBlocker blockerSigma(mSigma);

  mFeaturesNumber->setValue(10000);     // OpenCV default value = 0
  mOctaveLayers->setValue(3);          // OpenCV default value = 3
  mContrastThreshold->setValue(0.04);  // OpenCV default value = 0.04
  mEdgeThreshold->setValue(10.);
  mSigma->setValue(1.6);
}

void SiftWidgetImp::update()
{

}

void SiftWidgetImp::retranslate()
{
  mGroupBox->setTitle(QApplication::translate("SiftWidget", "SIFT Parameters", nullptr));
  mLabelFeaturesNumber->setText(QApplication::translate("SiftWidget", "Features Number:"));
  mLabelOctaveLayers->setText(QApplication::translate("SiftWidget", "Octave Layers:"));
  mLabelContrastThreshold->setText(QApplication::translate("SiftWidget", "Contrast Threshold:"));
  mLabelEdgeThreshold->setText(QApplication::translate("SiftWidget", "Edge Threshold:"));
  mLabelSigma->setText(QApplication::translate("SiftWidget", "Sigma:"));

//#ifndef QT_NO_WHATSTHIS
//  mFeaturesNumber->setWhatsThis(tr("<html><head/><body><p>The number of best features to retain.</p></body></html>"));
//  mOctaveLayers->setWhatsThis(tr("<html><head/><body><p>The number of layers in each octave. 3 is the value used in D. Lowe paper. The number of octaves is computed automatically from the image resolution.</p></body></html>"));
//  mContrastThreshold->setWhatsThis(tr("<html><head/><body><p>The contrast threshold used to filter out weak features in semi-uniform (low-contrast) regions</p></body></html>"));
//  mEdgeThreshold->setWhatsThis(tr("<html><head/><body><p>The threshold used to filter out edge-like features. Note that the its meaning is different from the contrastThreshold, i.e. the larger the edgeThreshold, the less features are filtered out (more features are retained).</p></body></html>"));
//  mSigma->setWhatsThis(tr("<html><head/><body><p>The sigma of the Gaussian applied to the input image at the octave 0</p></body></html>"));
//#endif // QT_NO_WHATSTHIS
}

int SiftWidgetImp::featuresNumber() const
{
  return mFeaturesNumber->value();
}

int SiftWidgetImp::octaveLayers() const
{
  return mOctaveLayers->value();
}

double SiftWidgetImp::contrastThreshold()
{
  return mContrastThreshold->value();
}

double SiftWidgetImp::edgeThreshold()
{
  return mEdgeThreshold->value();
}

double SiftWidgetImp::sigma()
{
  return mSigma->value();
}

void SiftWidgetImp::setFeaturesNumber(int featuresNumber)
{
  const QSignalBlocker blockerFeaturesNumber(mFeaturesNumber);
  mFeaturesNumber->setValue(featuresNumber);
}

void SiftWidgetImp::setOctaveLayers(int octaveLayers)
{
  const QSignalBlocker blockerFeaturesNumber(mOctaveLayers);
  mOctaveLayers->setValue(octaveLayers);
}

void SiftWidgetImp::setContrastThreshold(double contrastThreshold)
{
  const QSignalBlocker blockerContrastThreshold(mContrastThreshold);
  mContrastThreshold->setValue(contrastThreshold);
}

void SiftWidgetImp::setEdgeThreshold(double edgeThreshold)
{
  const QSignalBlocker blockerEdgeThreshold(mEdgeThreshold);
  mEdgeThreshold->setValue(edgeThreshold);
}

void SiftWidgetImp::setSigma(double sigma)
{
  const QSignalBlocker blockerSigma(mSigma);
  mSigma->setValue(sigma);
}


} // namespace photomatch
