#include <QtTest>
#include <QCoreApplication>

#include "graphos/core/dtm/invdist.h"

using namespace graphos;

class TestDtmInvDist
  : public QObject
{
  Q_OBJECT

public:

  TestDtmInvDist();
  ~TestDtmInvDist();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_constructor();
  void test_interpolation();
  void test_name();
  void test_power_data();
  void test_power();
  void test_smoothing_data();
  void test_smoothing();
  void test_radius1_data();
  void test_radius1();
  void test_radius2_data();
  void test_radius2();
  void test_angle_data();
  void test_angle();
  void test_maxPoints_data();
  void test_maxPoints();
  void test_minPoints_data();
  void test_minPoints();

private:

  DtmInvDist *mDtmInvDist;

};


TestDtmInvDist::TestDtmInvDist()
{
  mDtmInvDist = new DtmInvDistProperties();
}

TestDtmInvDist::~TestDtmInvDist()
{
  if (mDtmInvDist){
    delete mDtmInvDist;
    mDtmInvDist = nullptr;
  }
}


void TestDtmInvDist::initTestCase()
{
  QCOMPARE(2.0, mDtmInvDist->power());
  QCOMPARE(0.0, mDtmInvDist->smoothing());
  QCOMPARE(0.0, mDtmInvDist->radius1());
  QCOMPARE(0.0, mDtmInvDist->radius2());
  QCOMPARE(0.0, mDtmInvDist->angle());
  QCOMPARE(0, mDtmInvDist->maxPoints());
  QCOMPARE(0, mDtmInvDist->minPoints());
}

void TestDtmInvDist::cleanupTestCase()
{
  mDtmInvDist->reset();

  QCOMPARE(2.0, mDtmInvDist->power());
  QCOMPARE(0.0, mDtmInvDist->smoothing());
  QCOMPARE(0.0, mDtmInvDist->radius1());
  QCOMPARE(0.0, mDtmInvDist->radius2());
  QCOMPARE(0.0, mDtmInvDist->angle());
  QCOMPARE(0, mDtmInvDist->maxPoints());
  QCOMPARE(0, mDtmInvDist->minPoints());
}

void TestDtmInvDist::test_constructor()
{
  DtmInvDistAlgorithm dtmInvDistProperties(2.0, 0.1, 0.2, 0.15, 0.5, 100, 1);
  QCOMPARE(2.0, dtmInvDistProperties.power());
  QCOMPARE(0.1, dtmInvDistProperties.smoothing());
  QCOMPARE(0.2, dtmInvDistProperties.radius1());
  QCOMPARE(0.15, dtmInvDistProperties.radius2());
  QCOMPARE(0.5, dtmInvDistProperties.angle());
  QCOMPARE(100, dtmInvDistProperties.maxPoints());
  QCOMPARE(1, dtmInvDistProperties.minPoints());
}

void TestDtmInvDist::test_interpolation()
{
  QCOMPARE(Dtm::Interpolation::inv_dist, mDtmInvDist->interpolation());
}

void TestDtmInvDist::test_name()
{
  QCOMPARE("INVDIST", mDtmInvDist->name());
}

void TestDtmInvDist::test_power_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("3") << 3. << 3.;
  QTest::newRow("4") << 4. << 4.;
}

void TestDtmInvDist::test_power()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDist->setPower(value);
  QCOMPARE(result, mDtmInvDist->power());
}

void TestDtmInvDist::test_smoothing_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("1.0") << 1.0 << 1.0;
  QTest::newRow("0.5") << 0.5 << 0.5;
  QTest::newRow("2.0") << 2.0 << 2.0;
}

void TestDtmInvDist::test_smoothing()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDist->setSmoothing(value);
  QCOMPARE(result, mDtmInvDist->smoothing());
}

void TestDtmInvDist::test_radius1_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("1.0") << 1.0 << 1.0;
  QTest::newRow("0.5") << 0.5 << 0.5;
  QTest::newRow("2.0") << 2.0 << 2.0;
}

void TestDtmInvDist::test_radius1()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDist->setRadius1(value);
  QCOMPARE(result, mDtmInvDist->radius1());
}

void TestDtmInvDist::test_radius2_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("1.0") << 1.0 << 1.0;
  QTest::newRow("0.5") << 0.5 << 0.5;
  QTest::newRow("2.0") << 2.0 << 2.0;
}

void TestDtmInvDist::test_radius2()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDist->setRadius2(value);
  QCOMPARE(result, mDtmInvDist->radius2());
}

void TestDtmInvDist::test_angle_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.") << 0. << 0.;
  QTest::newRow("0.6") << 0.6 << 0.6;
  QTest::newRow("1.") << 1. << 1.;
}

void TestDtmInvDist::test_angle()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDist->setAngle(value);
  QCOMPARE(result, mDtmInvDist->angle());
}

void TestDtmInvDist::test_maxPoints_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("5") << 5 << 5;
  QTest::newRow("10") << 10 << 10;
  QTest::newRow("20") << 20 << 20;
}

void TestDtmInvDist::test_maxPoints()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mDtmInvDist->setMaxPoints(value);
  QCOMPARE(result, mDtmInvDist->maxPoints());
}

void TestDtmInvDist::test_minPoints_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("2") << 2 << 2;
  QTest::newRow("3") << 3 << 3;
  QTest::newRow("6") << 6 << 6;
}

void TestDtmInvDist::test_minPoints()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mDtmInvDist->setMinPoints(value);
  QCOMPARE(result, mDtmInvDist->minPoints());
}

QTEST_APPLESS_MAIN(TestDtmInvDist)

#include "tst_inv_dist.moc"
