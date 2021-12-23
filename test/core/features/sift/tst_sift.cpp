#include <QtTest>
#include <QCoreApplication>

#include "graphos/core/features/sift.h"

using namespace graphos;

class TestSift
  : public QObject
{
  Q_OBJECT

public:

  TestSift();
  ~TestSift();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_constructor();
  void test_copy_constructor();
  void test_type();
  void test_name();
  void test_featuresNumber_data();
  void test_featuresNumber();
  void test_octaveLayers_data();
  void test_octaveLayers();
  void test_contrastThresholdAuto();
  void test_contrastThreshold_data();
  void test_contrastThreshold();
  void test_edgeThreshold_data();
  void test_edgeThreshold();
  void test_sigma_data();
  void test_sigma();

private:

  Sift *mSift;

};


TestSift::TestSift()
{
  mSift = new SiftProperties();
  mSiftDetectorDescriptor = new SiftDetectorDescriptor(6000,
                             4,
                             9.,
                             2.2);
  mSiftCudaDetectorDescriptor = new SiftCudaDetectorDescriptor(10000,
                             8,
                             11.,
                             1.,
                             0.8);
}

TestSift::~TestSift()
{
  if (mSift){
    delete mSift;
    mSift = nullptr;
  }
}


void TestSift::initTestCase()
{
  QCOMPARE(5000, mSift->featuresNumber());
  QCOMPARE(3, mSift->octaveLayers());
  QCOMPARE(true, mSift->constrastThresholdAuto());
  QCOMPARE(0.02/mSift->octaveLayers(), mSift->contrastThreshold());
  QCOMPARE(10., mSift->edgeThreshold());
  QCOMPARE(1.6, mSift->sigma());
  
  QCOMPARE(6000, mSiftDetectorDescriptor->featuresNumber());
  QCOMPARE(4, mSiftDetectorDescriptor->octaveLayers());
  QCOMPARE(true, mSiftDetectorDescriptor->constrastThresholdAuto());
  QCOMPARE(0.02/mSiftDetectorDescriptor->octaveLayers(), mSiftDetectorDescriptor->contrastThreshold());
  QCOMPARE(9., mSiftDetectorDescriptor->edgeThreshold());
  QCOMPARE(2.2, mSiftDetectorDescriptor->sigma());
  
  QCOMPARE(10000, mSiftCudaDetectorDescriptor->featuresNumber());
  QCOMPARE(8, mSiftCudaDetectorDescriptor->octaveLayers());
  QCOMPARE(false, mSiftCudaDetectorDescriptor->constrastThresholdAuto());
  QCOMPARE(0.8, mSiftCudaDetectorDescriptor->contrastThreshold());
  QCOMPARE(11., mSiftCudaDetectorDescriptor->edgeThreshold());
  QCOMPARE(1.0, mSiftCudaDetectorDescriptor->sigma());
}

void TestSift::cleanupTestCase()
{
  mSift->setFeaturesNumber(500);
  mSift->setOctaveLayers(4);
  mSift->setContrastThreshold(0.5);
  mSift->setEdgeThreshold(20.);
  mSift->setSigma(3.);

  mSift->reset();

  QCOMPARE(5000, mSift->featuresNumber());
  QCOMPARE(3, mSift->octaveLayers());
  QCOMPARE(true, mSift->constrastThresholdAuto());
  QCOMPARE(0.02 / mSift->octaveLayers(), mSift->contrastThreshold());
  QCOMPARE(10., mSift->edgeThreshold());
  QCOMPARE(1.6, mSift->sigma());
}

void TestSift::test_constructor()
{
  SiftCudaDetectorDescriptor siftDetectorDescriptor(500, 4, 20., 3., 0.5);
  QCOMPARE(500, siftDetectorDescriptor.featuresNumber());
  QCOMPARE(4, siftDetectorDescriptor.octaveLayers());
  QCOMPARE(0.5, siftDetectorDescriptor.contrastThreshold());
  QCOMPARE(20., siftDetectorDescriptor.edgeThreshold());
  QCOMPARE(3., siftDetectorDescriptor.sigma());
}

void TestSift::test_copy_constructor()
{
  SiftCudaDetectorDescriptor siftDetectorDescriptor(500, 4, 20., 3., 0.5);
  SiftCudaDetectorDescriptor c(siftDetectorDescriptor);
  QCOMPARE(500, c.featuresNumber());
  QCOMPARE(4, c.octaveLayers());
  QCOMPARE(0.5, c.contrastThreshold());
  QCOMPARE(20., c.edgeThreshold());
  QCOMPARE(3., c.sigma());
}

void TestSift::test_type()
{
  QCOMPARE(Sift::Type::sift, mSift->type());
}

void TestSift::test_name()
{
  QCOMPARE("SIFT", mSift->name());
}

void TestSift::test_featuresNumber_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("500") << 500 << 500;
  QTest::newRow("10000") << 10000 << 10000;
}

void TestSift::test_featuresNumber()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mSift->setFeaturesNumber(value);
  QCOMPARE(result, mSift->featuresNumber());
}

void TestSift::test_octaveLayers_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("0") << 0 << 0;
  QTest::newRow("1") << 1 << 1;
  QTest::newRow("2") << 2 << 2;
  QTest::newRow("4") << 4 << 4;
  QTest::newRow("7") << 7 << 7;
}

void TestSift::test_octaveLayers()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mSift->setOctaveLayers(value);
  QCOMPARE(result, mSift->octaveLayers());
}

void TestSift::test_contrastThresholdAuto()
{
  mSift->setContrastThresholdAuto(true);
  QCOMPARE(true, mSift->constrastThresholdAuto());

  mSift->setContrastThresholdAuto(false);
  QCOMPARE(false, mSift->constrastThresholdAuto());
}

void TestSift::test_contrastThreshold_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.04") << 0.04 << 0.04;
  QTest::newRow("0.1") << 0.1 << 0.1;
  QTest::newRow("0.2") << 0.2 << 0.2;
}

void TestSift::test_contrastThreshold()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mSift->setContrastThresholdAuto(false);
  mSift->setContrastThreshold(value);
  QCOMPARE(result, mSift->contrastThreshold());
}

void TestSift::test_edgeThreshold_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("10.") << 10. << 10.;
  QTest::newRow("1.") << 1. << 1.;
  QTest::newRow("20.") << 20. << 20.;
}

void TestSift::test_edgeThreshold()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mSift->setEdgeThreshold(value);
  QCOMPARE(result, mSift->edgeThreshold());
}

void TestSift::test_sigma_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("10.") << 10. << 10.;
  QTest::newRow("1.") << 1. << 1.;
  QTest::newRow("20.") << 20. << 20.;
}

void TestSift::test_sigma()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mSift->setSigma(value);
  QCOMPARE(result, mSift->sigma());
}


QTEST_APPLESS_MAIN(TestSift)

#include "tst_sift.moc"
