#include <QtTest>

#include "graphos/core/orientation/poses.h"

using namespace graphos;

class TestCameraPose
  : public QObject
{
  Q_OBJECT

public:

  TestCameraPose();
  ~TestCameraPose();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_constructor();
  void test_crs_data();
  void test_crs();
  void test_source_data();
  void test_source();

protected:

  CameraPose *mCameraPose;
};

TestCameraPose::TestCameraPose()
  : mCameraPose(new CameraPose)
{
}

TestCameraPose::~TestCameraPose()
{
  if (mCameraPose){
    delete mCameraPose;
  }
}

void TestCameraPose::initTestCase()
{
  QCOMPARE(QString(), mCameraPose->crs());
  QCOMPARE(QString(), mCameraPose->source());
}

void TestCameraPose::cleanupTestCase()
{
}

void TestCameraPose::test_constructor()
{

}

void TestCameraPose::test_crs_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("EPSG:25830")
             << "EPSG:25830"
             << "EPSG:25830";
  QTest::newRow("EPSG:4258")
             << "EPSG:4258"
             << "EPSG:4258";
}

void TestCameraPose::test_crs()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mCameraPose->setCrs(value);
  QCOMPARE(result, mCameraPose->crs());
}

void TestCameraPose::test_source_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("C:\\Users\\User01\\Documents\\Graphos\\Projects\\cameras.txt")
             << "C:\\Users\\User01\\Documents\\Graphos\\Projects\\cameras.txt"
             << "C:\\Users\\User01\\Documents\\Graphos\\Projects\\cameras.txt";
  QTest::newRow("C:\\Users\\User01\\Documents\\Graphos\\Projects\\cameras.csv")
             << "C:\\Users\\User01\\Documents\\Graphos\\Projects\\cameras.csv"
             << "C:\\Users\\User01\\Documents\\Graphos\\Projects\\cameras.csv";
}

void TestCameraPose::test_source()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mCameraPose->setSource(value);
  QCOMPARE(result, mCameraPose->source());
}

QTEST_APPLESS_MAIN(TestCameraPose)

#include "tst_poses.moc"
