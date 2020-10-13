#include <QtTest>

#include "inspector/core/image.h"

using namespace inspector;

class TestImage
  : public QObject
{
  Q_OBJECT

public:

  TestImage();
  ~TestImage();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_constructor();
  void test_path_data();
  void test_path();
  void test_name_data();
  void test_name();
  void test_cameraId_data();
  void test_cameraId();
  void test_longitudeExif_data();
  void test_longitudeExif();
  void test_latitudeExif_data();
  void test_latitudeExif();
  void test_altitudeExif_data();
  void test_altitudeExif();

protected:

  Image *mImage;
};

TestImage::TestImage()
  : mImage(new Image)
{
}

TestImage::~TestImage()
{
  if (mImage){
    delete mImage;
  }
}

void TestImage::initTestCase()
{
  QCOMPARE(QString(), mImage->path());
  QCOMPARE(QString(), mImage->name());
  QCOMPARE(0, mImage->cameraId());
  CameraPosition cameraPosition = mImage->cameraPosition();
  QCOMPARE(true, cameraPosition.isEmpty());
  QCOMPARE(TL_DOUBLE_MAX, cameraPosition.x());
  QCOMPARE(TL_DOUBLE_MAX, cameraPosition.y());
  QCOMPARE(TL_DOUBLE_MAX, cameraPosition.z());
}

void TestImage::cleanupTestCase()
{

}

void TestImage::test_constructor()
{

  mImage = new Image("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png");
  QCOMPARE(QString("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png"), mImage->path());
  QCOMPARE(QString("img001"), mImage->name());
  QCOMPARE(0, mImage->cameraId());
  CameraPosition cameraPosition = mImage->cameraPosition();
  QCOMPARE(true, cameraPosition.isEmpty());
  QCOMPARE(TL_DOUBLE_MAX, cameraPosition.x());
  QCOMPARE(TL_DOUBLE_MAX, cameraPosition.y());
  QCOMPARE(TL_DOUBLE_MAX, cameraPosition.z());

  /// Copy constructor
  Image image("C:/Users/User01/Documents/Graphos/Projects/images/img002.png");
  image.setCameraId(1);
  CameraPosition cameraPosition2;
  cameraPosition2.setX(0.5);
  cameraPosition2.setY(2.3);
  cameraPosition2.setZ(10.2);
  image.setCameraPosition(cameraPosition2);
  mImage = new Image(image);
  QCOMPARE(QString("C:/Users/User01/Documents/Graphos/Projects/images/img002.png"), mImage->path());
  QCOMPARE(QString("img002"), mImage->name());
  QCOMPARE(1, mImage->cameraId());
  cameraPosition = mImage->cameraPosition();
  QCOMPARE(0.5, cameraPosition.x());
  QCOMPARE(2.3, cameraPosition.y());
  QCOMPARE(10.2, cameraPosition.z());

  /// Move constructor
  mImage = new Image(std::move(image));
  QCOMPARE(QString("C:/Users/User01/Documents/Graphos/Projects/images/img002.png"), mImage->path());
  QCOMPARE(QString("img002"), mImage->name());
  QCOMPARE(1, mImage->cameraId());
  cameraPosition = mImage->cameraPosition();
  QCOMPARE(0.5, cameraPosition.x());
  QCOMPARE(2.3, cameraPosition.y());
  QCOMPARE(10.2, cameraPosition.z());

  QCOMPARE(QString(""), image.path());
  QCOMPARE(QString(""), image.name());
  QCOMPARE(TL_DOUBLE_MAX, image.cameraPosition().x());
  QCOMPARE(TL_DOUBLE_MAX, image.cameraPosition().y());
  QCOMPARE(TL_DOUBLE_MAX, image.cameraPosition().z());
}

void TestImage::test_path_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png")
             << "C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png"
             << "C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png";
  QTest::newRow("C:/Users/User01/Documents/Graphos/Projects/images/img001.png")
             << "C:/Users/User01/Documents/Graphos/Projects/images/img001.png"
             << "C:/Users/User01/Documents/Graphos/Projects/images/img001.png";
}

void TestImage::test_path()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mImage->setPath(value);
  QCOMPARE(result, mImage->path());
}

void TestImage::test_name_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png")
             << "C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png"
             << "img001";
  QTest::newRow("C:/Users/User01/Documents/Graphos/Projects/images/img001.png")
             << "C:/Users/User01/Documents/Graphos/Projects/images/img001.png"
             << "img001";
}

void TestImage::test_name()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mImage->setPath(value);
  QCOMPARE(result, mImage->name());
}

void TestImage::test_cameraId_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("1") << 1 << 1;
  QTest::newRow("2") << 2 << 2;
  QTest::newRow("3") << 3 << 3;
}

void TestImage::test_cameraId()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mImage->setCameraId(value);
  QCOMPARE(result, mImage->cameraId());
}

void TestImage::test_longitudeExif_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow(".0") << .0 << .0;
  QTest::newRow("10.0") << 10.0 << 10.0;
  QTest::newRow("-24.234234") << -24.234234 << -24.234234;
  QTest::newRow("125.45510") << 125.45510 << 125.45510;
  QTest::newRow("-55.40") << -55.40 << -55.40;
}

void TestImage::test_longitudeExif()
{
  QFETCH(double, value);
  QFETCH(double, result);

  CameraPosition cameraPosition;

  cameraPosition.setX(value);
  QCOMPARE(result, cameraPosition.x());
}

void TestImage::test_latitudeExif_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.0") << 0.0 << 0.0;
  QTest::newRow("23.4563") << 23.4563 << 23.4563;
  QTest::newRow("90.0") << 90.0 << 90.0;
  QTest::newRow("-90.0") << -90.0 << -90.0;
}

void TestImage::test_latitudeExif()
{
  QFETCH(double, value);
  QFETCH(double, result);

  CameraPosition cameraPosition;
  cameraPosition.setY(value);
  QCOMPARE(result, cameraPosition.y());
}

void TestImage::test_altitudeExif_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow(".0") << .0 << .0;
  QTest::newRow("-10.0") << -10.0 << -10.0;
  QTest::newRow("2050.67") << 2050.67 << 2050.67;
}

void TestImage::test_altitudeExif()
{
  QFETCH(double, value);
  QFETCH(double, result);

  CameraPosition cameraPosition;
  cameraPosition.setZ(value);
  QCOMPARE(result, cameraPosition.z());
}

QTEST_APPLESS_MAIN(TestImage)

#include "tst_image.moc"
