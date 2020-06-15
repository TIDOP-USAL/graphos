#include <QtTest>
#include <QCoreApplication>

#include "inspector/core/densification/Smvs.h"

using namespace inspector;

class TestSmvs
  : public QObject
{
  Q_OBJECT

public:

  TestSmvs();
  ~TestSmvs();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_constructor();
  void test_copy_constructor();
  void test_type();
  void test_name();
  void test_inputImageScale_data();
  void test_inputImageScale();
  void test_outputDepthScale_data();
  void test_outputDepthScale();
  void test_shadingBasedOptimization_data();
  void test_shadingBasedOptimization();
  void test_semiGlobalMatching_data();
  void test_semiGlobalMatching();
  void test_surfaceSmoothingFactor_data();
  void test_surfaceSmoothingFactor();

private:

  Smvs *mSmvs;

};


TestSmvs::TestSmvs()
{
  mSmvs = new SmvsProperties();
}

TestSmvs::~TestSmvs()
{
  if (mSmvs){
    delete mSmvs;
    mSmvs = nullptr;
  }
}


void TestSmvs::initTestCase()
{
  QCOMPARE(1, mSmvs->inputImageScale());
  QCOMPARE(2, mSmvs->outputDepthScale());
  QCOMPARE(true, mSmvs->semiGlobalMatching());
  QCOMPARE(1., mSmvs->surfaceSmoothingFactor());
  QCOMPARE(false, mSmvs->shadingBasedOptimization());
}

void TestSmvs::cleanupTestCase()
{
  mSmvs->reset();

  QCOMPARE(1, mSmvs->inputImageScale());
  QCOMPARE(2, mSmvs->outputDepthScale());
  QCOMPARE(true, mSmvs->semiGlobalMatching());
  QCOMPARE(1., mSmvs->surfaceSmoothingFactor());
  QCOMPARE(false, mSmvs->shadingBasedOptimization());
}

void TestSmvs::test_constructor()
{
  SmvsDensifier smvsDensifier(2, 3, true, false, 0.7);
  QCOMPARE(2, smvsDensifier.inputImageScale());
  QCOMPARE(3, smvsDensifier.outputDepthScale());
  QCOMPARE(false, smvsDensifier.semiGlobalMatching());
  QCOMPARE(0.7, smvsDensifier.surfaceSmoothingFactor());
  QCOMPARE(true, smvsDensifier.shadingBasedOptimization());
}

void TestSmvs::test_copy_constructor()
{
  SmvsDensifier smvsDensifier(2, 3, true, false, 0.7);
  SmvsDensifier c(smvsDensifier);
  QCOMPARE(2, c.inputImageScale());
  QCOMPARE(3, c.outputDepthScale());
  QCOMPARE(false, c.semiGlobalMatching());
  QCOMPARE(0.7, c.surfaceSmoothingFactor());
  QCOMPARE(true, c.shadingBasedOptimization());
}

void TestSmvs::test_type()
{
  QCOMPARE(Densification::Method::smvs, mSmvs->method());
}

void TestSmvs::test_name()
{
  QCOMPARE("SMVS", mSmvs->name());
}

void TestSmvs::test_inputImageScale_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("0") << 0 << 0;
  QTest::newRow("6") << 6 << 6;
}

void TestSmvs::test_inputImageScale()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mSmvs->setInputImageScale(value);
  QCOMPARE(result, mSmvs->inputImageScale());
}

void TestSmvs::test_outputDepthScale_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("0") << 0 << 0;
  QTest::newRow("1") << 1 << 1;
  QTest::newRow("2") << 2 << 2;
  QTest::newRow("4") << 4 << 4;
  QTest::newRow("6") << 6 << 6;
}

void TestSmvs::test_outputDepthScale()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mSmvs->setOutputDepthScale(value);
  QCOMPARE(result, mSmvs->outputDepthScale());
}

void TestSmvs::test_shadingBasedOptimization_data()
{
  QTest::addColumn<bool>("value");
  QTest::addColumn<bool>("result");

  QTest::newRow("true") << true << true;
  QTest::newRow("false") << false << false;
}

void TestSmvs::test_shadingBasedOptimization()
{
  QFETCH(bool, value);
  QFETCH(bool, result);

  mSmvs->setShadingBasedOptimization(value);
  QCOMPARE(result, mSmvs->shadingBasedOptimization());
}

void TestSmvs::test_semiGlobalMatching_data()
{
  QTest::addColumn<bool>("value");
  QTest::addColumn<bool>("result");

  QTest::newRow("true") << true << true;
  QTest::newRow("false") << false << false;
}

void TestSmvs::test_semiGlobalMatching()
{
  QFETCH(bool, value);
  QFETCH(bool, result);

  mSmvs->setSemiGlobalMatching(value);
  QCOMPARE(result, mSmvs->semiGlobalMatching());
}

void TestSmvs::test_surfaceSmoothingFactor_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.") << 0. << 0.;
  QTest::newRow("1.") << 1. << 1.;
  QTest::newRow("10.") << 10. << 10.;
}

void TestSmvs::test_surfaceSmoothingFactor()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mSmvs->setSurfaceSmoothingFactor(value);
  QCOMPARE(result, mSmvs->surfaceSmoothingFactor());
}


QTEST_APPLESS_MAIN(TestSmvs)

#include "tst_smvs.moc"
