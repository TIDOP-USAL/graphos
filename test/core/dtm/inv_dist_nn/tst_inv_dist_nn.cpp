#include <QtTest>
#include <QCoreApplication>

#include "graphos/core/dtm/invdistnn.h"

using namespace graphos;

class TestDtmInvDistNN
  : public QObject
{
  Q_OBJECT

public:

  TestDtmInvDistNN();
  ~TestDtmInvDistNN();

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
  void test_radius_data();
  void test_radius();
  void test_maxPoints_data();
  void test_maxPoints();
  void test_minPoints_data();
  void test_minPoints();

private:

  DtmInvDistNN *mDtmInvDistNN;

};


TestDtmInvDistNN::TestDtmInvDistNN()
{
  mDtmInvDistNN = new DtmInvDistNNProperties();
}

TestDtmInvDistNN::~TestDtmInvDistNN()
{
  if (mDtmInvDistNN){
    delete mDtmInvDistNN;
    mDtmInvDistNN = nullptr;
  }
}


void TestDtmInvDistNN::initTestCase()
{
  QCOMPARE(2.0, mDtmInvDistNN->power());
  QCOMPARE(0.0, mDtmInvDistNN->smoothing());
  QCOMPARE(1.0, mDtmInvDistNN->radius());
  QCOMPARE(12, mDtmInvDistNN->maxPoints());
  QCOMPARE(0, mDtmInvDistNN->minPoints());
}

void TestDtmInvDistNN::cleanupTestCase()
{
  mDtmInvDistNN->reset();

  QCOMPARE(2.0, mDtmInvDistNN->power());
  QCOMPARE(0.0, mDtmInvDistNN->smoothing());
  QCOMPARE(1.0, mDtmInvDistNN->radius());
  QCOMPARE(12, mDtmInvDistNN->maxPoints());
  QCOMPARE(0, mDtmInvDistNN->minPoints());
}

void TestDtmInvDistNN::test_constructor()
{
  DtmInvDistNNAlgorithm dtmInvDistProperties(2.0, 0.1, 0.2, 100, 1);
  QCOMPARE(2.0, dtmInvDistProperties.power());
  QCOMPARE(0.1, dtmInvDistProperties.smoothing());
  QCOMPARE(0.2, dtmInvDistProperties.radius());
  QCOMPARE(100, dtmInvDistProperties.maxPoints());
  QCOMPARE(1, dtmInvDistProperties.minPoints());
}

void TestDtmInvDistNN::test_interpolation()
{
  QCOMPARE(Dtm::Interpolation::inv_distnn, mDtmInvDistNN->interpolation());
}

void TestDtmInvDistNN::test_name()
{
  QCOMPARE("INVDISTNN", mDtmInvDistNN->name());
}

void TestDtmInvDistNN::test_power_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("3") << 3. << 3.;
  QTest::newRow("4") << 4. << 4.;
}

void TestDtmInvDistNN::test_power()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDistNN->setPower(value);
  QCOMPARE(result, mDtmInvDistNN->power());
}

void TestDtmInvDistNN::test_smoothing_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("1.0") << 1.0 << 1.0;
  QTest::newRow("0.5") << 0.5 << 0.5;
  QTest::newRow("2.0") << 2.0 << 2.0;
}

void TestDtmInvDistNN::test_smoothing()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDistNN->setSmoothing(value);
  QCOMPARE(result, mDtmInvDistNN->smoothing());
}

void TestDtmInvDistNN::test_radius_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("1.0") << 1.0 << 1.0;
  QTest::newRow("0.5") << 0.5 << 0.5;
  QTest::newRow("2.0") << 2.0 << 2.0;
}

void TestDtmInvDistNN::test_radius()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDistNN->setRadius(value);
  QCOMPARE(result, mDtmInvDistNN->radius());
}

void TestDtmInvDistNN::test_maxPoints_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("5") << 5 << 5;
  QTest::newRow("10") << 10 << 10;
  QTest::newRow("20") << 20 << 20;
}

void TestDtmInvDistNN::test_maxPoints()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mDtmInvDistNN->setMaxPoints(value);
  QCOMPARE(result, mDtmInvDistNN->maxPoints());
}

void TestDtmInvDistNN::test_minPoints_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("2") << 2 << 2;
  QTest::newRow("3") << 3 << 3;
  QTest::newRow("6") << 6 << 6;
}

void TestDtmInvDistNN::test_minPoints()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mDtmInvDistNN->setMinPoints(value);
  QCOMPARE(result, mDtmInvDistNN->minPoints());
}

QTEST_APPLESS_MAIN(TestDtmInvDistNN)

#include "tst_inv_dist_nn.moc"
