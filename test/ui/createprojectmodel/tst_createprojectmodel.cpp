#include <QtTest>

#include "graphos/ui/createproject/impl/CreateProjectModel.h"
#include "graphos/core/project.h"
#include "../test/fake/ProjectFake.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>

using namespace graphos;


class TestCreateProjectModel
  : public QObject
{
  Q_OBJECT

public:

  TestCreateProjectModel();
  ~TestCreateProjectModel();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_projectName_data();
  void test_projectName();
  void test_projectDescription_data();
  void test_projectDescription();
  void test_projectFolder_data();
  void test_projectFolder();
  void test_projectPath_data();
  void test_projectPath();

private:
  Project *project;
  CreateProjectModel *mProjectModel;
};

TestCreateProjectModel::TestCreateProjectModel()
  : project(new ProjectFake),
    mProjectModel(new CreateProjectModelImp(project))
{
}

TestCreateProjectModel::~TestCreateProjectModel()
{
}

void TestCreateProjectModel::initTestCase()
{
  QCOMPARE(QString(""), mProjectModel->projectName());
  QCOMPARE(QString(""), mProjectModel->projectPath());
  QCOMPARE(QString(""), mProjectModel->projectDescription());
  QCOMPARE(QString(""), mProjectModel->projectFolder());
}

void TestCreateProjectModel::cleanupTestCase()
{
  mProjectModel->clear();

  QCOMPARE(QString(""), mProjectModel->projectName());
  QCOMPARE(QString(""), mProjectModel->projectPath());
  QCOMPARE(QString(""), mProjectModel->projectDescription());
  QCOMPARE(QString(""), mProjectModel->projectFolder());
}

void TestCreateProjectModel::test_projectName_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("Project 1") << "Project 1" << "Project 1";
  QTest::newRow("Project 2") << "Project 2" << "Project 2";
}

void TestCreateProjectModel::test_projectName()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mProjectModel->setProjectName(value);
  QCOMPARE(result, mProjectModel->projectName());
}

void TestCreateProjectModel::test_projectDescription_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("Project 1") << "Project 1" << "Project 1";
  QTest::newRow("Project 2") << "Project 2" << "Project 2";
}

void TestCreateProjectModel::test_projectDescription()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mProjectModel->setProjectDescription(value);
  QCOMPARE(result, mProjectModel->projectDescription());
}

void TestCreateProjectModel::test_projectFolder_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("C:/Users/Tido/Documents/graphos/Projects")
                << "C:/Users/Tido/Documents/graphos/Projects"
                << "C:/Users/Tido/Documents/graphos/Projects";
  QTest::newRow("C:/Users/user1/Documents/graphos/Projects")
                << "C:/Users/user1/Documents/graphos/Projects"
                << "C:/Users/user1/Documents/graphos/Projects";
}

void TestCreateProjectModel::test_projectFolder()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mProjectModel->setProjectFolder(value);
  QCOMPARE(result, mProjectModel->projectFolder());
}

void TestCreateProjectModel::test_projectPath_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("C:/Users/Tido/Documents/graphos/Projects/Project 1/Project 1.xml")
                << "C:/Users/Tido/Documents/graphos/Projects/Project 1/Project 1.xml"
                << "C:/Users/Tido/Documents/graphos/Projects/Project 1/Project 1.xml";
  QTest::newRow("C:/Users/user1/Documents/graphos/Projects/Project 2/Project 2.xml")
                << "C:/Users/user1/Documents/graphos/Projects/Project 2/Project 2.xml"
                << "C:/Users/user1/Documents/graphos/Projects/Project 2/Project 2.xml";
}

void TestCreateProjectModel::test_projectPath()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mProjectModel->saveAs(value);
  QCOMPARE(result, mProjectModel->projectPath());
}

QTEST_APPLESS_MAIN(TestCreateProjectModel)

#include "tst_createprojectmodel.moc"
