#include <QtTest>

#include "graphos/ui/featextract/impl/FeatureExtractorModel.h"
#include "graphos/core/project.h"
#include "graphos/core/features/sift.h"
#include "../test/fake/ProjectFake.h"

#include <tidop/geospatial/camera.h>

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>

#include <map>

using namespace graphos;


class TestFeatureExtractorModel
  : public QObject
{
  Q_OBJECT

public:

  TestFeatureExtractorModel();
  ~TestFeatureExtractorModel();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_featureExtractor();
  void test_database_data();
  void test_database();
  void test_addFeatures();
  void test_images();
  void test_cameras();

private:

  Project *project;
  FeatureExtractorModel *mFeatureExtractorModel;
};

TestFeatureExtractorModel::TestFeatureExtractorModel()
  : project(new ProjectFake),
    mFeatureExtractorModel(new FeatureExtractorModelImp(project))
{
}

TestFeatureExtractorModel::~TestFeatureExtractorModel()
{
}

void TestFeatureExtractorModel::initTestCase()
{
  QCOMPARE(nullptr, mFeatureExtractorModel->featureExtractor());
  QCOMPARE(QString(""), mFeatureExtractorModel->database());
  // Depende del fichero de configuración...
  //QCOMPARE(QString(""), mFeatureExtractorModel->useCuda());
  std::vector<Image> images = mFeatureExtractorModel->images();
  QCOMPARE(0, images.size());
  auto cameras = mFeatureExtractorModel->cameras();
  QCOMPARE(true, cameras.begin() == cameras.end());
}

void TestFeatureExtractorModel::cleanupTestCase()
{
}

void TestFeatureExtractorModel::test_featureExtractor()
{
  {
    std::shared_ptr<Feature> feature = std::make_shared<SiftProperties>();
    mFeatureExtractorModel->setFeatureExtractor(feature);
  }

  std::shared_ptr<Feature> feature = mFeatureExtractorModel->featureExtractor();
  QCOMPARE(5000, std::dynamic_pointer_cast<SiftProperties>(feature)->featuresNumber());
  QCOMPARE(3, std::dynamic_pointer_cast<SiftProperties>(feature)->octaveLayers());
}

void TestFeatureExtractorModel::test_database_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("database.db") << "database.db" << "database.db";
  QTest::newRow("database2.db") << "database2.db" << "database2.db";
}

void TestFeatureExtractorModel::test_database()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  project->setDatabase(value);
  QCOMPARE(result, mFeatureExtractorModel->database());
}

void TestFeatureExtractorModel::test_addFeatures()
{
  mFeatureExtractorModel->addFeatures("image001", "c:/features/image001.xml");
  QCOMPARE("c:/features/image001.xml", project->features("image001"));
}

void TestFeatureExtractorModel::test_images()
{
  project->addImage(Image("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png"));
  project->addImage(Image("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img002.png"));

  std::vector<Image> images = mFeatureExtractorModel->images();
  QCOMPARE(2, images.size());
  QCOMPARE("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png", images[0].path());
  QCOMPARE("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img002.png", images[1].path());
}

void TestFeatureExtractorModel::test_cameras()
{
  tl::Camera camera("DJI", "FC6310");
  project->addCamera(camera);

  auto cameras = mFeatureExtractorModel->cameras();
  auto it_camera = cameras.begin();
  QCOMPARE(1, it_camera->first);
  QCOMPARE("DJI", it_camera->second.make());
  QCOMPARE("FC6310", it_camera->second.model());
}


QTEST_APPLESS_MAIN(TestFeatureExtractorModel)

#include "tst_featureextractormodel.moc"
