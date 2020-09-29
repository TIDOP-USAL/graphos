#include <QtTest>

#include "inspector/core/project.h"

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
  void test_updateCamera();
  void test_removeCamera();
  void test_findCamera();
  void test_findCamera_exception();
  void test_findCameraId();
  void test_existCamera();
  void test_database_data();
  void test_database();
  void test_featureExtractor();
  void test_features();
  void test_matches();
  void test_orientations();
  void test_sparseModel();
  void test_densification();
  void test_denseModel();

protected:

  Project *mProject;
  Project *mProjectXml;
};

TestProject::TestProject()
  : mProject(new ProjectImp),
    mProjectXml(new ProjectImp)
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
  mProjectXml->load(QString(INSPECTOR_SOURCE_PATH).append("/test/data/project.xml"));
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
  QCOMPARE(QString("Proj02"), mProjectXml->name());
  QCOMPARE(QString("Descripción del proyecto 2"), mProjectXml->description());
  QCOMPARE(QString("C:/Users/User01/Documents/inspector/Projects/prj002"), mProjectXml->projectFolder());
  QCOMPARE(QString("C:/Users/User01/Documents/inspector/Projects/prj002/images"), mProjectXml->imageDirectory());
  QCOMPARE(QString("1.0"), mProjectXml->version());
  QCOMPARE(4, mProjectXml->imagesCount());
  QCOMPARE(QString("C:/Users/User01/Documents/inspector/Projects/prj002/database.db"), mProjectXml->database());
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
  Image img = mProjectXml->findImage("IMG_7209");
  QCOMPARE("IMG_7209", img.name());
  QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj002/images/IMG_7209.jpg", img.path());

  Image img2 = mProjectXml->findImage("IMG_7210");
  QCOMPARE("IMG_7210", img2.name());
  QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj002/images/IMG_7210.jpg", img2.path());
}

void TestProject::test_imageId_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<size_t>("result");

  QTest::newRow("IMG_7209") << "IMG_7209" << size_t{0};
  QTest::newRow("IMG_7210") << "IMG_7210" << size_t{1};
  QTest::newRow("IMG_7211") << "IMG_7211" << size_t{2};
  QTest::newRow("IMG_7212") << "IMG_7212" << size_t{3};
}

void TestProject::test_imageId()
{
  QFETCH(QString, value);
  QFETCH(size_t, result);

  QCOMPARE(result, mProjectXml->imageId(value));
}

void TestProject::test_imageId_exception()
{
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
      QCOMPARE("IMG_7209", img.name());
      QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj002/images/IMG_7209.jpg", img.path());
    } else if (i == 1) {
      QCOMPARE("IMG_7210", img.name());
      QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj002/images/IMG_7210.jpg", img.path());
    } else if (i == 2) {
      QCOMPARE("IMG_7211", img.name());
      QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj002/images/IMG_7211.jpg", img.path());
    } else if (i == 3) {
      QCOMPARE("IMG_7212", img.name());
      QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj002/images/IMG_7212.jpg", img.path());
    }
  }
}

void TestProject::test_addImage_deleteImage()
{
  Image img("C:/Users/esteban/Documents/Inspector/Projects/SanSegundo/images/IMG_7213.png");

  mProjectXml->addImage(img);

  QCOMPARE(5, mProjectXml->imagesCount());

  mProjectXml->removeImage("IMG_7213");
  QCOMPARE(4, mProjectXml->imagesCount());
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

void TestProject::test_updateCamera()
{

}

void TestProject::test_removeCamera()
{
  int id = mProjectXml->cameraId("SONY", "ILCE-6000");
  mProjectXml->removeCamera(id);
  QCOMPARE(false, mProjectXml->existCamera("SONY", "ILCE-6000"));
}

void TestProject::test_findCamera()
{
  Camera camera = mProjectXml->findCamera("Unknown camera", "0");
  QCOMPARE(5835.6, camera.focal());
  QCOMPARE(4863, camera.width());
  QCOMPARE(3221, camera.height());
  QCOMPARE(1, camera.sensorSize());

  Camera camera2 = mProjectXml->findCamera(1);
  QCOMPARE(5835.6, camera2.focal());
  QCOMPARE(4863, camera2.width());
  QCOMPARE(3221, camera2.height());
  QCOMPARE(1, camera2.sensorSize());
}

void TestProject::test_findCamera_exception()
{
  try {
    Camera camera_error = mProjectXml->findCamera("DJI", "FC6310");
  } catch (std::exception &e) {
    QString error = e.what();
    QCOMPARE("Camera not found: DJI FC6310", error);
  }
}

void TestProject::test_findCameraId()
{
  int id = mProjectXml->cameraId("Unknown camera", "0");
  QCOMPARE(1, id);
}

void TestProject::test_existCamera()
{
  QCOMPARE(false, mProjectXml->existCamera("DJI", "FC6310"));
  QCOMPARE(true, mProjectXml->existCamera("Unknown camera", "0"));
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
  std::shared_ptr<Feature> feature_extractor = mProjectXml->featureExtractor();
  QCOMPARE(Feature::Type::sift, feature_extractor->type());
}

void TestProject::test_features()
{
  QString features = mProjectXml->features("IMG_7209");
  QCOMPARE("IMG_7209@C:/Users/User01/Documents/inspector/Projects/prj002/database.db", features);

  features = mProjectXml->features("IMG_7210");
  QCOMPARE("IMG_7210@C:/Users/User01/Documents/inspector/Projects/prj002/database.db", features);
}

void TestProject::test_matches()
{
  std::shared_ptr<FeatureMatching> feature_matching = mProjectXml->featureMatching();
  QCOMPARE(true, feature_matching.get() != nullptr);
}

void TestProject::test_orientations()
{
  PhotoOrientation ori = mProjectXml->photoOrientation("IMG_7209");
  QCOMPARE(5.30029, ori.x);
}

void TestProject::test_sparseModel()
{
  mProject->setSparseModel("C:/Users/esteban/Documents/inspector/Projects/SanSegundo/sparse/0/sparse.ply");
  QCOMPARE("C:/Users/esteban/Documents/inspector/Projects/SanSegundo/sparse/0/sparse.ply", mProject->sparseModel());
}

void TestProject::test_densification()
{
  std::shared_ptr<Densification> dense = mProjectXml->densification();
  QCOMPARE(Densification::Method::cmvs_pmvs, dense->method());
}

void TestProject::test_denseModel()
{
  mProject->setDenseModel("C:/Users/esteban/Documents/inspector/Projects/SanSegundo/dense/pmvs/models/option-all.ply");
  QCOMPARE("C:/Users/esteban/Documents/inspector/Projects/SanSegundo/dense/pmvs/models/option-all.ply", mProject->denseModel());
}

QTEST_APPLESS_MAIN(TestProject)

#include "tst_project.moc"
