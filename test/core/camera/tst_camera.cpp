#include <QtTest>

#include "inspector/core/camera.h"

using namespace inspector;

class TestCamera
  : public QObject
{
  Q_OBJECT

public:

  TestCamera();
  ~TestCamera();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_default_constructor();
  void test_constructor();
  void test_copy_constructor();
  void test_make_data();
  void test_make();
  void test_model_data();
  void test_model();
  void test_type_data();
  void test_type();
  void test_focal_data();
  void test_focal();
  void test_width_data();
  void test_width();
  void test_height_data();
  void test_height();
  void test_sensorSize_data();
  void test_sensorSize();

protected:

  Camera *mCamera;
};

TestCamera::TestCamera()
  : mCamera(new Camera("DJI", "FC6310"))
{
  mCamera->setType("SIMPLE_RADIAL");
  mCamera->setFocal(3552.23);
  mCamera->setWidth(5472);
  mCamera->setHeight(3648);
  mCamera->setSensorSize(12.8333);
}

TestCamera::~TestCamera()
{
  if (mCamera){
    delete mCamera;
  }
}

void TestCamera::initTestCase()
{
  QCOMPARE(QString("DJI"), mCamera->make());
  QCOMPARE(QString("FC6310"), mCamera->model());
  QCOMPARE(QString("SIMPLE_RADIAL"), mCamera->type());
  QCOMPARE(3552.23, mCamera->focal());
  QCOMPARE(5472, mCamera->width());
  QCOMPARE(3648, mCamera->height());
  QCOMPARE(12.8333, mCamera->sensorSize());
}

void TestCamera::cleanupTestCase()
{

}

void TestCamera::test_default_constructor()
{
  Camera camera;
  QCOMPARE(QString(), camera.make());
  QCOMPARE(QString(), camera.model());
  QCOMPARE(QString("RADIAL"), camera.type());
  QCOMPARE(1., camera.focal());
  QCOMPARE(0, camera.width());
  QCOMPARE(0, camera.height());
  QCOMPARE(1., camera.sensorSize());
}

void TestCamera::test_constructor()
{
  Camera camera("SONY", "ILCE-6000");
  QCOMPARE(QString("SONY"), camera.make());
  QCOMPARE(QString("ILCE-6000"), camera.model());
  QCOMPARE(QString("RADIAL"), camera.type());
  QCOMPARE(1., camera.focal());
  QCOMPARE(0, camera.width());
  QCOMPARE(0, camera.height());
  QCOMPARE(1., camera.sensorSize());
}

void TestCamera::test_copy_constructor()
{
  Camera copy_camera(*mCamera);
  QCOMPARE(QString("DJI"), copy_camera.make());
  QCOMPARE(QString("FC6310"), copy_camera.model());
  QCOMPARE(QString("SIMPLE_RADIAL"), copy_camera.type());
  QCOMPARE(3552.23, copy_camera.focal());
  QCOMPARE(5472, copy_camera.width());
  QCOMPARE(3648, copy_camera.height());
  QCOMPARE(12.8333, copy_camera.sensorSize());
}

void TestCamera::test_make_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("olympus") << "olympus" << "olympus";
  QTest::newRow("canon") << "canon" << "canon";
  QTest::newRow("sony") << "sony" << "sony";
  QTest::newRow("leica") << "leica" << "leica";
}

void TestCamera::test_make()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mCamera->setMake(value);
  QCOMPARE(result, mCamera->make());
}

void TestCamera::test_model_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("omdem1") << "omdem1" << "omdem1";
  QTest::newRow("eos7d") << "eos7d" << "eos7d";
  QTest::newRow("alphaa5000") << "alphaa5000" << "alphaa5000";
  QTest::newRow("dlux5") << "dlux5" << "dlux5";
}

void TestCamera::test_model()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mCamera->setModel(value);
  QCOMPARE(result, mCamera->model());
}

void TestCamera::test_type_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("SIMPLE_RADIAL") << "SIMPLE_RADIAL" << "SIMPLE_RADIAL";
  QTest::newRow("RADIAL") << "RADIAL" << "RADIAL";
  QTest::newRow("FULL_RADIAL") << "FULL_RADIAL" << "FULL_RADIAL";
}

void TestCamera::test_type()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mCamera->setType(value);
  QCOMPARE(result, mCamera->type());
}

void TestCamera::test_focal_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("1.") << 1. << 1.;
  QTest::newRow("3552.23") << 3552.23 << 3552.23;
  QTest::newRow("4500.") << 4500. << 4500.;
}

void TestCamera::test_focal()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mCamera->setFocal(value);
  QCOMPARE(result, mCamera->focal());
}

void TestCamera::test_width_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("4000") << 4000 << 4000;
  QTest::newRow("5000") << 5000 << 5000;
  QTest::newRow("6000") << 6000 << 6000;
}

void TestCamera::test_width()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCamera->setWidth(value);
  QCOMPARE(result, mCamera->width());
}

void TestCamera::test_height_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("2500") << 2500 << 2500;
  QTest::newRow("3000") << 3000 << 3000;
  QTest::newRow("4500") << 4500 << 4500;
}

void TestCamera::test_height()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCamera->setHeight(value);
  QCOMPARE(result, mCamera->height());
}

void TestCamera::test_sensorSize_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("17.3") << 17.3 << 17.3;
  QTest::newRow("22.3") << 22.3 << 22.3;
  QTest::newRow("23.5") << 23.5 << 23.5;
  QTest::newRow("7.85") << 7.85 << 7.85;
}

void TestCamera::test_sensorSize()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mCamera->setSensorSize(value);
  QCOMPARE(result, mCamera->sensorSize());
}

QTEST_APPLESS_MAIN(TestCamera)

#include "tst_camera.moc"
