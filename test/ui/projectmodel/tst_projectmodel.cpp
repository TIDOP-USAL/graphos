#include <QtTest>

#include "inspector/core/project.h"
#include "inspector/ui/ProjectModel.h"
#include "fake/ProjectFake.h"

using namespace inspector;
using namespace ui;

class TestProjectModel : public QObject
{
  Q_OBJECT

public:

  TestProjectModel();
  ~TestProjectModel();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void testConstructor();
  void test_name_data();
  void test_name();
  void test_description_data();
  void test_description();
  void test_projectFolder_data();
  void test_projectFolder();
  void test_save();

protected:

  Project *mProject;
  ProjectModel *mProjectModel;
};

TestProjectModel::TestProjectModel()
  : mProject(new ProjectFake),
    mProjectModel(new ProjectModelImp(mProject))
{

}

TestProjectModel::~TestProjectModel()
{

  if (mProject){
    delete mProject;
    mProject = nullptr;
  }

  if (mProjectModel) {
    delete mProjectModel;
    mProjectModel = nullptr;
  }
}

void TestProjectModel::initTestCase()
{
  /// reading simulation
  mProjectModel->load("C:/Users/User01/Documents/inspector/Projects/prj001/prj001.xml");
}

void TestProjectModel::cleanupTestCase()
{
  mProjectModel->clear();

  QCOMPARE(QString(), mProjectModel->projectName());
  QCOMPARE(QString(), mProjectModel->projectDescription());
  QCOMPARE(QString(), mProjectModel->projectFolder());
  QCOMPARE(QString("1.0"), mProjectModel->version());
}

void TestProjectModel::testConstructor()
{
  ProjectImp *project = new ProjectImp;

  ProjectModelImp prj(project);
  QCOMPARE(QString(), prj.projectName());
  QCOMPARE(QString(), prj.projectDescription());
  QCOMPARE(QString(), prj.projectFolder());
  QCOMPARE(QString(), prj.projectPath());
  QCOMPARE(QString("1.0"), prj.version());
  QCOMPARE(false, prj.checkUnsavedChanges());

  delete project;
  project = nullptr;

  QCOMPARE(QString("prj001"), mProjectModel->projectName());
  QCOMPARE(QString("Project example"), mProjectModel->projectDescription());
  QCOMPARE(QString("C:/Users/User01/Documents/inspector/Projects/prj001"), mProjectModel->projectFolder());
  QCOMPARE(QString("1.0"), mProjectModel->version());
  QCOMPARE(QString("C:/Users/User01/Documents/inspector/Projects/prj001/prj001.xml"), mProjectModel->projectPath());
  QCOMPARE(false, mProjectModel->checkUnsavedChanges());
}

void TestProjectModel::test_name_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("Proj01") << "Proj01" << "Proj01";
  QTest::newRow("Proj02") << "Proj02" << "Proj02";
}

void TestProjectModel::test_name()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  QCOMPARE(false, mProjectModel->checkUnsavedChanges());
  mProjectModel->setProjectName(value);
  QCOMPARE(result, mProjectModel->projectName());
  QCOMPARE(true, mProjectModel->checkUnsavedChanges());
  mProjectModel->save();
  QCOMPARE(false, mProjectModel->checkUnsavedChanges());
}

void TestProjectModel::test_description_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("Description1") << "Descripción del proyecto 1" << "Descripción del proyecto 1";
  QTest::newRow("Description2") << "Descripción del proyecto 2" << "Descripción del proyecto 2";
}

void TestProjectModel::test_description()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  QCOMPARE(false, mProjectModel->checkUnsavedChanges());
  mProjectModel->setProjectDescription(value);
  QCOMPARE(true, mProjectModel->checkUnsavedChanges());
  QCOMPARE(result, mProjectModel->projectDescription());
  mProjectModel->save();
}

void TestProjectModel::test_projectFolder_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("Folder_prj_01") << "C:\\Users\\User01\\Documents\\inspector\\Projects\\prj001" << "C:\\Users\\User01\\Documents\\inspector\\Projects\\prj001";
  QTest::newRow("Folder_prj_02") << "C:/Users/User01/Documents/inspector/Projects/prj002" << "C:/Users/User01/Documents/inspector/Projects/prj002";
}

void TestProjectModel::test_projectFolder()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  QCOMPARE(false, mProjectModel->checkUnsavedChanges());
  mProjectModel->setProjectFolder(value);
  QCOMPARE(true, mProjectModel->checkUnsavedChanges());
  QCOMPARE(result, mProjectModel->projectFolder());
  mProjectModel->save();
}

void TestProjectModel::test_save()
{
  mProjectModel->saveAs("C:/Users/User01/Documents/inspector/Projects/prj001/prj001.xml");
  QCOMPARE(QString("C:/Users/User01/Documents/inspector/Projects/prj001/prj001.xml"), mProjectModel->projectPath());
}


QTEST_APPLESS_MAIN(TestProjectModel)

#include "tst_projectmodel.moc"
