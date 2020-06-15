#include <QtTest>
#include <QCoreApplication>

 #include "inspector/core/densification/CmvsPmvs.h"

using namespace inspector;

class TestCmvsPmvs
  : public QObject
{
  Q_OBJECT

public:

  TestCmvsPmvs();
  ~TestCmvsPmvs();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_constructor();
  void test_copy_constructor();
  void test_method();
  void test_name();
  void test_useVisibilityInformation_data();
  void test_useVisibilityInformation();
  void test_imagesPerCluster_data();
  void test_imagesPerCluster();
  void test_level_data();
  void test_level();
  void test_cellSize_data();
  void test_cellSize();
  void test_threshold_data();
  void test_threshold();
  void test_windowSize_data();
  void test_windowSize();
  void test_minimunImageNumber_data();
  void test_minimunImageNumber();

private:

  CmvsPmvs *mCmvsPmvs;

};


TestCmvsPmvs::TestCmvsPmvs()
{
  mCmvsPmvs = new CmvsPmvsProperties();
}

TestCmvsPmvs::~TestCmvsPmvs()
{
  if (mCmvsPmvs){
    delete mCmvsPmvs;
    mCmvsPmvs = nullptr;
  }
}


void TestCmvsPmvs::initTestCase()
{
  QCOMPARE(true, mCmvsPmvs->useVisibilityInformation());
  QCOMPARE(100, mCmvsPmvs->imagesPerCluster());
  QCOMPARE(1, mCmvsPmvs->level());
  QCOMPARE(2, mCmvsPmvs->cellSize());
  QCOMPARE(0.7, mCmvsPmvs->threshold());
  QCOMPARE(7, mCmvsPmvs->windowSize());
  QCOMPARE(3, mCmvsPmvs->minimunImageNumber());
}

void TestCmvsPmvs::cleanupTestCase()
{
  mCmvsPmvs->reset();

  QCOMPARE(true, mCmvsPmvs->useVisibilityInformation());
  QCOMPARE(100, mCmvsPmvs->imagesPerCluster());
  QCOMPARE(1, mCmvsPmvs->level());
  QCOMPARE(2, mCmvsPmvs->cellSize());
  QCOMPARE(0.7, mCmvsPmvs->threshold());
  QCOMPARE(7, mCmvsPmvs->windowSize());
  QCOMPARE(3, mCmvsPmvs->minimunImageNumber());
}

void TestCmvsPmvs::test_constructor()
{
  CmvsPmvsDensifier cmvsPmvsDensifier(true, 50, 2, 3, 0.5, 10, 2);
  QCOMPARE(true, cmvsPmvsDensifier.useVisibilityInformation());
  QCOMPARE(50, cmvsPmvsDensifier.imagesPerCluster());
  QCOMPARE(2, cmvsPmvsDensifier.level());
  QCOMPARE(3, cmvsPmvsDensifier.cellSize());
  QCOMPARE(0.5, cmvsPmvsDensifier.threshold());
  QCOMPARE(10, cmvsPmvsDensifier.windowSize());
  QCOMPARE(2, cmvsPmvsDensifier.minimunImageNumber());
}

void TestCmvsPmvs::test_copy_constructor()
{
  CmvsPmvsDensifier cmvsPmvsDensifier(true, 50, 2, 3, 0.5, 10, 2);
  CmvsPmvsDensifier c(cmvsPmvsDensifier);
  QCOMPARE(true, c.useVisibilityInformation());
  QCOMPARE(50, c.imagesPerCluster());
  QCOMPARE(2, c.level());
  QCOMPARE(3, c.cellSize());
  QCOMPARE(0.5, c.threshold());
  QCOMPARE(10, c.windowSize());
  QCOMPARE(2, c.minimunImageNumber());
}

void TestCmvsPmvs::test_method()
{
  QCOMPARE(Densification::Method::cmvs_pmvs, mCmvsPmvs->method());
}

void TestCmvsPmvs::test_name()
{
  QCOMPARE("CMVS/PMVS", mCmvsPmvs->name());
}

void TestCmvsPmvs::test_useVisibilityInformation_data()
{
  QTest::addColumn<bool>("value");
  QTest::addColumn<bool>("result");

  QTest::newRow("false") << false << false;
  QTest::newRow("true") << true << true;
}

void TestCmvsPmvs::test_useVisibilityInformation()
{
  QFETCH(bool, value);
  QFETCH(bool, result);

  mCmvsPmvs->setUseVisibilityInformation(value);
  QCOMPARE(result, mCmvsPmvs->useVisibilityInformation());
}


void TestCmvsPmvs::test_imagesPerCluster_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("1") << 1 << 1;
  QTest::newRow("50") << 50 << 50;
  QTest::newRow("100") << 100 << 100;
  QTest::newRow("1000") << 1000 << 1000;
}

void TestCmvsPmvs::test_imagesPerCluster()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCmvsPmvs->setImagesPerCluster(value);
  QCOMPARE(result, mCmvsPmvs->imagesPerCluster());
}

void TestCmvsPmvs::test_level_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("0") << 0 << 0;
  QTest::newRow("3") << 3 << 3;
  QTest::newRow("6") << 6 << 6;
}

void TestCmvsPmvs::test_level()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCmvsPmvs->setLevel(value);
  QCOMPARE(result, mCmvsPmvs->level());
}

void TestCmvsPmvs::test_cellSize_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("1") << 1 << 1;
  QTest::newRow("3") << 3 << 3;
  QTest::newRow("7") << 7 << 7;
}

void TestCmvsPmvs::test_cellSize()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCmvsPmvs->setCellSize(value);
  QCOMPARE(result, mCmvsPmvs->cellSize());
}

void TestCmvsPmvs::test_threshold_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.") << 0. << 0.;
  QTest::newRow("0.6") << 0.6 << 0.6;
  QTest::newRow("1.") << 1. << 1.;
}

void TestCmvsPmvs::test_threshold()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mCmvsPmvs->setThreshold(value);
  QCOMPARE(result, mCmvsPmvs->threshold());
}

void TestCmvsPmvs::test_windowSize_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("5") << 5 << 5;
  QTest::newRow("10") << 10 << 10;
  QTest::newRow("20") << 20 << 20;
}

void TestCmvsPmvs::test_windowSize()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCmvsPmvs->setWindowSize(value);
  QCOMPARE(result, mCmvsPmvs->windowSize());
}

void TestCmvsPmvs::test_minimunImageNumber_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("2") << 2 << 2;
  QTest::newRow("3") << 3 << 3;
  QTest::newRow("6") << 6 << 6;
}

void TestCmvsPmvs::test_minimunImageNumber()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCmvsPmvs->setMinimunImageNumber(value);
  QCOMPARE(result, mCmvsPmvs->minimunImageNumber());
}

QTEST_APPLESS_MAIN(TestCmvsPmvs)

#include "tst_cmsvs_pmvs.moc"
