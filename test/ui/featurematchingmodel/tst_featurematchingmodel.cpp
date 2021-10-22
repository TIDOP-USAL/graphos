#include <QtTest>

#include "graphos/ui/featmatch/impl/FeatureMatchingModel.h"
#include "graphos/core/project.h"
#include "graphos/core/features/matching.h"
#include "../test/fake/ProjectFake.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>

#include <map>

using namespace graphos;


class TestFeatureMatchingModel
  : public QObject
{
  Q_OBJECT

public:

  TestFeatureMatchingModel();
  ~TestFeatureMatchingModel();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_featureMatching();
  void test_database_data();
  void test_database();
  void test_spatialMatching();

private:

  Project *project;
  FeatureMatchingModel *mFeatureMatchingModel;
};

TestFeatureMatchingModel::TestFeatureMatchingModel()
  : project(new ProjectFake),
    mFeatureMatchingModel(new FeatureMatchingModelImp(project))
{
}

TestFeatureMatchingModel::~TestFeatureMatchingModel()
{
}

void TestFeatureMatchingModel::initTestCase()
{
  QCOMPARE(nullptr, mFeatureMatchingModel->featureMatching());
  QCOMPARE(QString(""), mFeatureMatchingModel->database());
  QCOMPARE(false, mFeatureMatchingModel->spatialMatching());
}

void TestFeatureMatchingModel::cleanupTestCase()
{
}

void TestFeatureMatchingModel::test_featureMatching()
{
  {
    std::shared_ptr<FeatureMatching> match_properties = std::make_shared<FeatureMatchingProperties>();
    mFeatureMatchingModel->setFeatureMatching(match_properties);
  }

  std::shared_ptr<FeatureMatching> match_properties = mFeatureMatchingModel->featureMatching();
  QCOMPARE(true, std::dynamic_pointer_cast<FeatureMatchingProperties>(match_properties)->crossCheck());
  QCOMPARE(0.8, std::dynamic_pointer_cast<FeatureMatchingProperties>(match_properties)->ratio());
}


void TestFeatureMatchingModel::test_database_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("database.db") << "database.db" << "database.db";
  QTest::newRow("database2.db") << "database2.db" << "database2.db";
}


void TestFeatureMatchingModel::test_database()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  project->setDatabase(value);
  QCOMPARE(result, mFeatureMatchingModel->database());
}

void TestFeatureMatchingModel::test_spatialMatching()
{

  Image image1("C:/Users/User01/Documents/Graphos/Projects/images/img001.png");
  CameraPose camera_pose1(5.,5.,5., tl::math::Quaternion<double>::identity());
  image1.setCameraPose(camera_pose1);
  Image image2("C:/Users/User01/Documents/Graphos/Projects/images/img002.png");
  CameraPose camera_pose2(10.,12.,5., tl::math::Quaternion<double>::identity());
  image2.setCameraPose(camera_pose2);

  project->addImage(image1);
  project->addImage(image2);

  QCOMPARE(true, mFeatureMatchingModel->spatialMatching());

}


QTEST_APPLESS_MAIN(TestFeatureMatchingModel)

#include "tst_featurematchingmodel.moc"
