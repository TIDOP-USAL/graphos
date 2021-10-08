#include <QtTest>
#include <QCoreApplication>

#include "graphos/core/dtm/Csf.h"

using namespace graphos;

class TestCSF
  : public QObject
{
  Q_OBJECT

public:

  TestCSF();
  ~TestCSF();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_sloopSmooth_data();
  void test_sloopSmooth();
  void test_timeStep_data();
  void test_timeStep();
  void test_classThreshold_data();
  void test_classThreshold();
  void test_clothResolution_data();
  void test_clothResolution();
  void test_rigidness_data();
  void test_rigidness();
  void test_iterations_data();
  void test_iterations();

private:

  Csf *mCsf;

};


TestCSF::TestCSF()
{
  mCsf = new Csf();
}

TestCSF::~TestCSF()
{
  if (mCsf){
    delete mCsf;
    mCsf = nullptr;
  }
}


void TestCSF::initTestCase()
{
  QCOMPARE(false, mCsf->sloopSmooth());
  QCOMPARE(0.65, mCsf->timeStep());
  QCOMPARE(0.5, mCsf->classThreshold());
  QCOMPARE(2, mCsf->clothResolution());
  QCOMPARE(2, mCsf->rigidness());
  QCOMPARE(500, mCsf->iterations());
}

void TestCSF::cleanupTestCase()
{
  mCsf->reset();

  QCOMPARE(false, mCsf->sloopSmooth());
  QCOMPARE(0.65, mCsf->timeStep());
  QCOMPARE(0.5, mCsf->classThreshold());
  QCOMPARE(2, mCsf->clothResolution());
  QCOMPARE(2, mCsf->rigidness());
  QCOMPARE(500, mCsf->iterations());
}

void TestCSF::test_sloopSmooth_data()
{
  QTest::addColumn<bool>("value");
  QTest::addColumn<bool>("result");

  QTest::newRow("true") << true << true;
  QTest::newRow("false") << false << false;
}

void TestCSF::test_sloopSmooth()
{
  QFETCH(bool, value);
  QFETCH(bool, result);

  mCsf->setSloopSmooth(value);
  QCOMPARE(result, mCsf->sloopSmooth());
}

void TestCSF::test_timeStep_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow(".3") << .3 << .3;
  QTest::newRow(".4") << .4 << .4;
}

void TestCSF::test_timeStep()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mCsf->setTimeStep(value);
  QCOMPARE(result, mCsf->timeStep());
}

void TestCSF::test_classThreshold_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("1.0") << 1.0 << 1.0;
  QTest::newRow("0.5") << 0.5 << 0.5;
  QTest::newRow("2.0") << 2.0 << 2.0;
}

void TestCSF::test_classThreshold()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mCsf->setClassThreshold(value);
  QCOMPARE(result, mCsf->classThreshold());
}

void TestCSF::test_clothResolution_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("1") << 1 << 1;
  QTest::newRow("2") << 2 << 2;
}

void TestCSF::test_clothResolution()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCsf->setClothResolution(value);
  QCOMPARE(result, mCsf->clothResolution());
}

void TestCSF::test_rigidness_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("1") << 1 << 1;
  QTest::newRow("2") << 2 << 2;
}

void TestCSF::test_rigidness()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCsf->setRigidness(value);
  QCOMPARE(result, mCsf->rigidness());
}

void TestCSF::test_iterations_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("50") << 50 << 50;
  QTest::newRow("100") << 100 << 100;
}

void TestCSF::test_iterations()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCsf->setIterations(value);
  QCOMPARE(result, mCsf->iterations());
}

QTEST_APPLESS_MAIN(TestCSF)

#include "tst_csf.moc"
