#include <QtTest>

#include "graphos/core/image.h"

using namespace graphos;

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
  CameraPose camera_pose = mImage->cameraPose();
  QCOMPARE(true, camera_pose.isEmpty());
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
  CameraPose camera_pose = mImage->cameraPose();
  QCOMPARE(true, camera_pose.isEmpty());
  QCOMPARE(tl::Point3D(), camera_pose.position());

  /// Copy constructor
  Image image("C:/Users/User01/Documents/Graphos/Projects/images/img002.png");
  image.setCameraId(1);
  CameraPose camera_pose_2;
  camera_pose_2.setPosition(tl::Point3D(0.5, 2.3, 10.2));
  image.setCameraPose(camera_pose_2);
  mImage = new Image(image);
  QCOMPARE(QString("C:/Users/User01/Documents/Graphos/Projects/images/img002.png"), mImage->path());
  QCOMPARE(QString("img002"), mImage->name());
  QCOMPARE(1, mImage->cameraId());
  camera_pose = mImage->cameraPose();
  QCOMPARE(tl::Point3D(0.5, 2.3, 10.2), camera_pose.position());

  /// Move constructor
  mImage = new Image(std::move(image));
  QCOMPARE(QString("C:/Users/User01/Documents/Graphos/Projects/images/img002.png"), mImage->path());
  QCOMPARE(QString("img002"), mImage->name());
  QCOMPARE(1, mImage->cameraId());
  camera_pose = mImage->cameraPose();
  QCOMPARE(tl::Point3D(0.5, 2.3, 10.2), camera_pose.position());

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

QTEST_APPLESS_MAIN(TestImage)

#include "tst_image.moc"
