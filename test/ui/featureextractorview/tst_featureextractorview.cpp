#include <QtTest>
#include <QCoreApplication>

#include "inspector/ui/featextract/FeatureExtractorView.h"
#include "inspector/widgets/SiftWidget.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>

using namespace inspector;
using namespace ui;

class TestFeatureExtractorView
  : public FeatureExtractorViewImp
{
  Q_OBJECT

public:

  TestFeatureExtractorView();
  ~TestFeatureExtractorView();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_currentDetectorDescriptor();
  void test_maxImageSize_data();
  void test_maxImageSize();
  void test_fullImageSize();

private:

  SiftWidget *mSift;
};

TestFeatureExtractorView::TestFeatureExtractorView()
  : FeatureExtractorViewImp(),
    mSift(new SiftWidgetImp)
{
  QApplication::setActiveWindow(this);
}

TestFeatureExtractorView::~TestFeatureExtractorView()
{
  if (mSift){
    delete mSift;
    mSift = nullptr;
  }
}

void TestFeatureExtractorView::initTestCase()
{
  QCOMPARE("", this->currentDetectorDescriptor());
  QCOMPARE(3000, this->maxImageSize());
  QCOMPARE(false, this->fullImageSize());

  this->addDetectorDescriptor(mSift);
  this->setCurrentDetectorDescriptor(mSift->windowTitle());
}

void TestFeatureExtractorView::cleanupTestCase()
{
  this->clear();

  QCOMPARE(3000, this->maxImageSize());
  QCOMPARE(false, this->fullImageSize());
}

void TestFeatureExtractorView::test_windowTitle()
{
  QCOMPARE("Feature Detector/Descriptor", this->windowTitle());
}

void TestFeatureExtractorView::test_currentDetectorDescriptor()
{
  QCOMPARE("SIFT", this->currentDetectorDescriptor());
}

void TestFeatureExtractorView::test_maxImageSize_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("2500") << 2500 << 2500;
  QTest::newRow("3000") << 3000 << 3000;
  QTest::newRow("3500") << 3500 << 3500;
}

void TestFeatureExtractorView::test_maxImageSize()
{
  QFETCH(int, value);
  QFETCH(int, result);

  this->setMaxImageSize(value);
  QCOMPARE(result, this->maxImageSize());
}

void TestFeatureExtractorView::test_fullImageSize()
{
  this->setFullImageSize(true);
  QCOMPARE(true, this->fullImageSize());

  this->setFullImageSize(false);
  QCOMPARE(false, this->fullImageSize());
}


QTEST_MAIN(TestFeatureExtractorView)

#include "tst_featureextractorview.moc"
