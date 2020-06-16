#include <QtTest>
#include <QCoreApplication>

#include "inspector/core/features/matching.h"

using namespace inspector;

class TestFeatureMatchingProperties
  : public QObject
{
  Q_OBJECT

public:

  TestFeatureMatchingProperties();
  ~TestFeatureMatchingProperties();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_crossCheck();
  void test_ratio_data();
  void test_ratio();
  void test_distance_data();
  void test_distance();
  void test_maxError_data();
  void test_maxError();
  void test_confidence_data();
  void test_confidence();

private:

  FeatureMatching *mFeatureMatching;

};


TestFeatureMatchingProperties::TestFeatureMatchingProperties()
{
  mFeatureMatching = new FeatureMatchingProperties();
}

TestFeatureMatchingProperties::~TestFeatureMatchingProperties()
{
  if (mFeatureMatching){
    delete mFeatureMatching;
    mFeatureMatching = nullptr;
  }
}


void TestFeatureMatchingProperties::initTestCase()
{
  QCOMPARE(true, mFeatureMatching->crossCheck());
  QCOMPARE(0.8, mFeatureMatching->ratio());
  QCOMPARE(0.7, mFeatureMatching->distance());
  QCOMPARE(4.0, mFeatureMatching->maxError());
  QCOMPARE(0.999, mFeatureMatching->confidence());
}

void TestFeatureMatchingProperties::cleanupTestCase()
{

}

void TestFeatureMatchingProperties::test_crossCheck()
{
  mFeatureMatching->enableCrossCheck(false);
  QCOMPARE(false, mFeatureMatching->crossCheck());
  mFeatureMatching->enableCrossCheck(true);
  QCOMPARE(true, mFeatureMatching->crossCheck());
}

void TestFeatureMatchingProperties::test_ratio_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.7") << 0.7 << 0.7;
  QTest::newRow("0.8") << 0.8 << 0.8;
  QTest::newRow("0.9") << 0.9 << 0.9;
}

void TestFeatureMatchingProperties::test_ratio()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mFeatureMatching->setRatio(value);
  QCOMPARE(result, mFeatureMatching->ratio());
}

void TestFeatureMatchingProperties::test_distance_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.9") << 0.9 << 0.9;
  QTest::newRow("1.5") << 1.5 << 1.5;
  QTest::newRow("0.7") << 0.7 << 0.7;
}

void TestFeatureMatchingProperties::test_distance()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mFeatureMatching->setDistance(value);
  QCOMPARE(result, mFeatureMatching->distance());
}

void TestFeatureMatchingProperties::test_maxError_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("2.0") << 2.0 << 2.0;
  QTest::newRow("3.0") << 3.0 << 3.0;
  QTest::newRow("4.0") << 4.0 << 4.0;
}

void TestFeatureMatchingProperties::test_maxError()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mFeatureMatching->setMaxError(value);
  QCOMPARE(result, mFeatureMatching->maxError());
}

void TestFeatureMatchingProperties::test_confidence_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.9") << 0.9 << 0.9;
  QTest::newRow("0.995") << 0.995 << 0.995;
  QTest::newRow("0.999") << 0.999 << 0.999;
}

void TestFeatureMatchingProperties::test_confidence()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mFeatureMatching->setConfidence(value);
  QCOMPARE(result, mFeatureMatching->confidence());
}


QTEST_APPLESS_MAIN(TestFeatureMatchingProperties)

#include "tst_featurematching.moc"
