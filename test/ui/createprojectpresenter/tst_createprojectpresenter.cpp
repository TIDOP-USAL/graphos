#include <QtTest>

#include "graphos/ui/createproject/impl/CreateProjectPresenter.h"
#include "graphos/ui/createproject/CreateProjectModel.h"
#include "graphos/ui/createproject/CreateProjectView.h"
#include "graphos/core/project.h"
#include "../test/fake/ProjectFake.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>

using namespace graphos;

//class TestCreateProjectModel
//  : CreateProjectModel
//{
//  Q_OBJECT
//public:

//  TestCreateProjectModel() {}

//// Model interface

//private:

//  void init() override { }

//public slots:

//  void clear() override { }

//// CreateProjectModel interface

//public:

//  QString projectName() const override { return QString(); }
//  QString projectDescription() const override { return QString(); }
//  QString projectFolder() const override { return QString(); }
//  QString projectPath() const override { return QString(); }

//public slots:

//  void setProjectName(const QString &) override {}
//  void setProjectDescription(const QString &projectDescription) override {}
//  void setProjectFolder(const QString &dir) override { }
//  void setDatabase(const QString &database) override { }
//  void saveAs(const QString &file) override { }
//};

//class TestCreateProjectView
//  : public CreateProjectView
//{
//Q_OBJECT
//public:

//  TestCreateProjectView(QWidget *parent) : CreateProjectView(parent) {}

//  // DialogView interface
//private:
//  void initUI() override
//  {
//  }
//  void initSignalAndSlots() override
//  {
//  }

//public slots:
//  void clear() override
//  {
//  }

//private slots:
//  void update() override
//  {
//  }
//  void retranslate() override
//  {
//  }

//  // CreateProjectView interface
//public:
//  QString projectName() const override { return QString(); }
//  QString projectPath() const override { return QString(); }
//  void setProjectPath(const QString &path) override { }
//  QString projectDescription() const override { return QString(); }
//  bool createProjectFolderEnable() const override { }
//  void setExistingProject(bool prjExist) override { }

//protected slots:
//  void onClickButtonSelectPath() override { }
//};


class TestCreateProjectPresenter
  : public QObject
{
  Q_OBJECT

public:

  TestCreateProjectPresenter();
  ~TestCreateProjectPresenter();

private slots:

  void initTestCase();
  void cleanupTestCase();
/*   void test_projectName_data();
  void test_projectName();
  void test_projectDescription_data();
  void test_projectDescription();
  void test_projectFolder_data();
  void test_projectFolder();
  void test_projectPath_data();
  void test_projectPath(); */

private:
  Project *project;
  CreateProjectModel *createProjectModel;
  CreateProjectView *createProjectViewImp;
  CreateProjectPresenter *createProjectPresenter;
};

TestCreateProjectPresenter::TestCreateProjectPresenter()
  : project(new ProjectFake),
    createProjectModel(new CreateProjectModelImp(project)),
    createProjectViewImp(new CreateProjectViewImp()),
    createProjectPresenter(nullptr)
{

}

TestCreateProjectPresenter::~TestCreateProjectPresenter()
{
}

void TestCreateProjectPresenter::initTestCase()
{

/*   QCOMPARE(QString(""), mProjectModel->projectName());
  QCOMPARE(QString(""), mProjectModel->projectPath());
  QCOMPARE(QString(""), mProjectModel->projectDescription());
  QCOMPARE(QString(""), mProjectModel->projectFolder()); */
}

void TestCreateProjectPresenter::cleanupTestCase()
{
/*   mProjectModel->clear();

  QCOMPARE(QString(""), mProjectModel->projectName());
  QCOMPARE(QString(""), mProjectModel->projectPath());
  QCOMPARE(QString(""), mProjectModel->projectDescription());
  QCOMPARE(QString(""), mProjectModel->projectFolder()); */
}

/* void TestCreateProjectPresenter::test_projectName_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("Project 1") << "Project 1" << "Project 1";
  QTest::newRow("Project 2") << "Project 2" << "Project 2";
}

void TestCreateProjectPresenter::test_projectName()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mProjectModel->setProjectName(value);
  QCOMPARE(result, mProjectModel->projectName());
}

void TestCreateProjectPresenter::test_projectDescription_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("Project 1") << "Project 1" << "Project 1";
  QTest::newRow("Project 2") << "Project 2" << "Project 2";
}

void TestCreateProjectPresenter::test_projectDescription()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mProjectModel->setProjectDescription(value);
  QCOMPARE(result, mProjectModel->projectDescription());
}

void TestCreateProjectPresenter::test_projectFolder_data()
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

void TestCreateProjectPresenter::test_projectFolder()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mProjectModel->setProjectFolder(value);
  QCOMPARE(result, mProjectModel->projectFolder());
}

void TestCreateProjectPresenter::test_projectPath_data()
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

void TestCreateProjectPresenter::test_projectPath()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mProjectModel->saveAs(value);
  QCOMPARE(result, mProjectModel->projectPath());
}
 */
QTEST_APPLESS_MAIN(TestCreateProjectPresenter)

#include "tst_createprojectpresenter.moc"
