#include <QtTest>

#include "fake/ProjectFake.h"

using namespace inspector;


class TestProject
  : public QObject
{
  Q_OBJECT

public:

  TestProject();
  ~TestProject();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_default_constructor();
  void test_constructor();
  void test_name_data();
  void test_name();
  void test_description_data();
  void test_description();
  void test_projectFolder_data();
  void test_projectFolder();
  void test_findImage();
  void test_imageId_data();
  void test_imageId();
  void test_imageId_exception();
  void test_imageIterator();
  void test_addImage_deleteImage();
  void test_addCamera();
  void test_findCamera();
  void test_findCameraId();
  void test_database_data();
  void test_database();
  void test_featureExtractor();
  void test_features();

protected:

  Project *mProject;
  Project *mProjectXml;
};

TestProject::TestProject()
  : mProject(new ProjectImp),
    mProjectXml(new ProjectFake)
{

}

TestProject::~TestProject()
{
  if (mProject){
    delete mProject;
    mProject = nullptr;
  }

  if (mProjectXml){
    delete mProjectXml;
    mProjectXml = nullptr;
  }
}

void TestProject::initTestCase()
{
  mProjectXml->load("C:/Users/User01/Documents/inspector/Projects/prj001/prj001.xml");
}

void TestProject::cleanupTestCase()
{
  mProjectXml->clear();

  QCOMPARE(QString(), mProjectXml->name());
  QCOMPARE(QString(), mProjectXml->description());
  QCOMPARE(QString(), mProjectXml->projectFolder());
  QCOMPARE(QString("1.0"), mProjectXml->version());
  QCOMPARE(0, mProjectXml->imagesCount());
  QCOMPARE("", mProjectXml->database());
}

void TestProject::test_default_constructor()
{
  ProjectImp project;
  QCOMPARE(QString(), project.name());
  QCOMPARE(QString(), project.description());
  QCOMPARE(QString(), project.projectFolder());
  QCOMPARE(QString("1.0"), project.version());
  QCOMPARE(0, project.imagesCount());
  QCOMPARE("", project.database());
}

void TestProject::test_constructor()
{
  QCOMPARE(QString("prj001"), mProjectXml->name());
  QCOMPARE(QString("Project example"), mProjectXml->description());
  QCOMPARE(QString("C:/Users/User01/Documents/inspector/Projects/prj001"), mProjectXml->projectFolder());
  QCOMPARE(QString("1.0"), mProjectXml->version());
  QCOMPARE(2, mProjectXml->imagesCount());
  QCOMPARE(QString("C:/Users/User01/Documents/inspector/Projects/prj001/prj001.db"), mProjectXml->database());
}

void TestProject::test_name_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("Proj01") << "Proj01" << "Proj01";
  QTest::newRow("Proj02") << "Proj02" << "Proj02";

}

void TestProject::test_name()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mProject->setName(value);
  QCOMPARE(result, mProject->name());
}

void TestProject::test_description_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("Description1") << "Descripción del proyecto 1" << "Descripción del proyecto 1";
  QTest::newRow("Description2") << "Descripción del proyecto 2" << "Descripción del proyecto 2";
}

void TestProject::test_description()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mProject->setDescription(value);
  QCOMPARE(result, mProject->description());
}

void TestProject::test_projectFolder_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("Folder_prj_01") << "C:\\Users\\User01\\Documents\\inspector\\Projects\\prj001" << "C:\\Users\\User01\\Documents\\inspector\\Projects\\prj001";
  QTest::newRow("Folder_prj_02") << "C:/Users/User01/Documents/inspector/Projects/prj002" << "C:/Users/User01/Documents/inspector/Projects/prj002";
}

void TestProject::test_projectFolder()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mProject->setProjectFolder(value);
  QCOMPARE(result, mProject->projectFolder());
}

void TestProject::test_findImage()
{
  Image img = mProjectXml->findImage("img001");
  QCOMPARE("img001", img.name());
  QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj001/images/img001.png", img.path());
  QCOMPARE(0.5, img.longitudeExif());
  QCOMPARE(2.3, img.latitudeExif());
  QCOMPARE(10.2, img.altitudeExif());

  Image img2 = mProjectXml->findImage("img002");
  QCOMPARE("img002", img2.name());
  QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj001/images/img002.png", img2.path());
  QCOMPARE(0.51, img2.longitudeExif());
  QCOMPARE(2.3, img2.latitudeExif());
  QCOMPARE(10.1, img2.altitudeExif());
}

void TestProject::test_imageId_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<size_t>("result");

  QTest::newRow("img001") << "img001" << size_t{0};
  QTest::newRow("img002") << "img002" << size_t{1};
}

void TestProject::test_imageId()
{
  QFETCH(QString, value);
  QFETCH(size_t, result);

  QCOMPARE(result, mProjectXml->imageId(value));
}

void TestProject::test_imageId_exception()
{
  //QTest::newRow("img003") << "C:/Users/User01/Documents/inspector/Projects/prj001/images/img003.png" << std::numeric_limits<size_t>().max();
  try {
    int id = mProjectXml->imageId("C:/Users/User01/Documents/inspector/Projects/prj001/images/img003.png");
  } catch (std::exception &e) {
    QCOMPARE("Image not found: C:/Users/User01/Documents/inspector/Projects/prj001/images/img003.png", e.what());
  }
}

void TestProject::test_imageIterator()
{
  Image img;
  int i = 0;
  for (auto it = mProjectXml->imageBegin(); it != mProjectXml->imageEnd(); it++, i++){
    img = (*it);
    if (i == 0){
      QCOMPARE("img001", img.name());
      QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj001/images/img001.png", img.path());
      QCOMPARE(0.5, img.longitudeExif());
      QCOMPARE(2.3, img.latitudeExif());
      QCOMPARE(10.2, img.altitudeExif());
    } else {
      QCOMPARE("img002", img.name());
      QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj001/images/img002.png", img.path());
      QCOMPARE(0.51, img.longitudeExif());
      QCOMPARE(2.3, img.latitudeExif());
      QCOMPARE(10.1, img.altitudeExif());
    }
  }
}

void TestProject::test_addImage_deleteImage()
{
  Image img("C:/Users/User01/Documents/inspector/Projects/prj001/images/img003.png");

  mProjectXml->addImage(img);

  QCOMPARE(3, mProjectXml->imagesCount());

  mProjectXml->removeImage("img003");
  QCOMPARE(2, mProjectXml->imagesCount());
}

void TestProject::test_addCamera()
{
  Camera camera("SONY", "ILCE-6000");
  camera.setFocal(3552.23);
  camera.setWidth(5472);
  camera.setHeight(3648);
  mProjectXml->addCamera(camera);
  Camera camera2 = mProjectXml->findCamera("SONY", "ILCE-6000");
  QCOMPARE(3552.23, camera2.focal());
  QCOMPARE(5472, camera2.width());
  QCOMPARE(3648, camera2.height());
}

void TestProject::test_findCamera()
{
  Camera camera = mProjectXml->findCamera("DJI", "FC6310");
  QCOMPARE(3752.23, camera.focal());
  QCOMPARE(5472, camera.width());
  QCOMPARE(3648, camera.height());
  QCOMPARE(12.8333, camera.sensorSize());

  Camera camera2 = mProjectXml->findCamera(1);
  QCOMPARE(3752.23, camera2.focal());
  QCOMPARE(5472, camera2.width());
  QCOMPARE(3648, camera2.height());
  QCOMPARE(12.8333, camera2.sensorSize());
}

void TestProject::test_findCameraId()
{
  int id = mProjectXml->cameraId("DJI", "FC6310");
  QCOMPARE(1, id);
}

void TestProject::test_database_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("database1") << "C:\\Users\\User01\\Documents\\inspector\\Projects\\prj001\\prj001.db" << "C:\\Users\\User01\\Documents\\inspector\\Projects\\prj001\\prj001.db";
  QTest::newRow("database2") << "C:/Users/User01/Documents/inspector/Projects/prj001/database.db" << "C:/Users/User01/Documents/inspector/Projects/prj001/database.db";
}

void TestProject::test_database()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mProject->setDatabase(value);
  QCOMPARE(result, mProject->database());
}

void TestProject::test_featureExtractor()
{

}

void TestProject::test_features()
{
  QString features = mProjectXml->features("img001");
  QCOMPARE("img001@C:/Users/User01/Documents/inspector/Projects/prj001/prj001.db", features);

  features = mProjectXml->features("img002");
  QCOMPARE("img002@C:/Users/User01/Documents/inspector/Projects/prj001/prj001.db", features);
}


QTEST_APPLESS_MAIN(TestProject)

#include "tst_project.moc"
