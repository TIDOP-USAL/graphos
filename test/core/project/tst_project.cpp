#include <QtTest>

#include "inspector/core/project.h"
#include "fake/ProjectControllerFake.h"

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
  void test_findImageId_data();
  void test_findImageId();
  void test_imageIterator();
  void test_addImage_deleteImage();
  void test_database_data();
  void test_database();

protected:

  ProjectController *mProjectController;
  Project *mProject;
  Project *mProjectXml;
};

TestProject::TestProject()
  : mProjectController(new ProjectControllerFake),
    mProject(new ProjectImp),
    mProjectXml(new ProjectImp)
{

}

TestProject::~TestProject()
{
  if (mProjectController){
    delete mProjectController;
    mProjectController = nullptr;
  }

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
  mProjectController->read("C:/Users/User01/Documents/inspector/Projects/prj001/prj001.xml", *mProjectXml);
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
  std::shared_ptr<Image> img = mProjectXml->findImage("C:/Users/User01/Documents/inspector/Projects/prj001/images/img001.png");
  QCOMPARE("img001", img->name());
  QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj001/images/img001.png", img->path());
  QCOMPARE(0.5, img->longitudeExif());
  QCOMPARE(2.3, img->latitudeExif());
  QCOMPARE(10.2, img->altitudeExif());

  std::shared_ptr<Image> img2 = mProjectXml->findImage("C:/Users/User01/Documents/inspector/Projects/prj001/images/img002.png");
  QCOMPARE("img002", img2->name());
  QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj001/images/img002.png", img2->path());
  QCOMPARE(0.51, img2->longitudeExif());
  QCOMPARE(2.3, img2->latitudeExif());
  QCOMPARE(10.1, img2->altitudeExif());
}

void TestProject::test_findImageId_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<size_t>("result");

  QTest::newRow("img001") << "C:/Users/User01/Documents/inspector/Projects/prj001/images/img001.png" << size_t{0};
  QTest::newRow("img002") << "C:/Users/User01/Documents/inspector/Projects/prj001/images/img002.png" << size_t{1};
  QTest::newRow("img003") << "C:/Users/User01/Documents/inspector/Projects/prj001/images/img003.png" << std::numeric_limits<size_t>().max();
}

void TestProject::test_findImageId()
{
  QFETCH(QString, value);
  QFETCH(size_t, result);

  QCOMPARE(result, mProjectXml->findImageId(value));
}

void TestProject::test_imageIterator()
{
  std::shared_ptr<Image> img;
  int i = 0;
  for (auto it = mProjectXml->imageBegin(); it != mProjectXml->imageEnd(); it++, i++){

    if (i == 0){
      QCOMPARE("img001", (*it)->name());
      QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj001/images/img001.png", (*it)->path());
      QCOMPARE(0.5, (*it)->longitudeExif());
      QCOMPARE(2.3, (*it)->latitudeExif());
      QCOMPARE(10.2, (*it)->altitudeExif());
    } else {
      QCOMPARE("img002", (*it)->name());
      QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj001/images/img002.png", (*it)->path());
      QCOMPARE(0.51, (*it)->longitudeExif());
      QCOMPARE(2.3, (*it)->latitudeExif());
      QCOMPARE(10.1, (*it)->altitudeExif());
    }
  }
}

void TestProject::test_addImage_deleteImage()
{
  std::shared_ptr<Image> img(new Image("C:/Users/User01/Documents/inspector/Projects/prj001/images/img003.png"));

  mProjectXml->addImage(img);

  QCOMPARE(3, mProjectXml->imagesCount());

  mProjectXml->deleteImage("C:/Users/User01/Documents/inspector/Projects/prj001/images/img003.png");
  QCOMPARE(2, mProjectXml->imagesCount());
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


QTEST_APPLESS_MAIN(TestProject)

#include "tst_project.moc"
