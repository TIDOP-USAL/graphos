#include <QtTest>
#include <QCoreApplication>

#include "graphos/components/createproject/impl/CreateProjectView.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>

using namespace graphos;

class TestCreateProjectView
  : public CreateProjectViewImp
{
  Q_OBJECT

public:

  TestCreateProjectView();
  ~TestCreateProjectView();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_constructor();
  void test_projectName();
  void test_projectPath_data();
  void test_projectPath();
  void test_projectPath_signals();
  void test_lineEditProjectFile();
  void test_projectDescription();
  void test_createProjectFolder();
  void test_pushButtonProjectPath();
  void test_dialogButtonBox();
  void test_setExistingProject();

};

TestCreateProjectView::TestCreateProjectView()
  : CreateProjectViewImp()
{
  QApplication::setActiveWindow(this);
}

TestCreateProjectView::~TestCreateProjectView()
{
}

void TestCreateProjectView::initTestCase()
{

}

void TestCreateProjectView::cleanupTestCase()
{
  this->clear();

  QCOMPARE(this->projectName(), QString(""));
  QCOMPARE(this->projectPath(), QString(""));
  QCOMPARE(this->mLineEditProjectFile->text(), QString(""));
  QCOMPARE(this->projectDescription(), QString(""));
  QCOMPARE(true, this->createProjectFolderEnable());
}

void TestCreateProjectView::test_constructor()
{
  CreateProjectViewImp newProjectView;
  QCOMPARE("", newProjectView.projectName());
  QCOMPARE("", newProjectView.projectPath());
  QCOMPARE("", newProjectView.projectDescription());
  QCOMPARE(true, newProjectView.createProjectFolderEnable());
}

void TestCreateProjectView::test_projectName()
{

  /// Simulación de entrada por teclado

  this->mLineEditProjectName->clear();
  QTest::keyClicks(this->mLineEditProjectName, "project");

  QCOMPARE(this->projectName(), QString("project"));
}

void TestCreateProjectView::test_projectPath_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("C:/Users/Tido/Documents/photomatch/Projects") << "C:/Users/Tido/Documents/photomatch/Projects" << "C:/Users/Tido/Documents/photomatch/Projects";
  QTest::newRow("C:/Users/user1/Documents/photomatch/Projects") << "C:/Users/user1/Documents/photomatch/Projects" << "C:/Users/user1/Documents/photomatch/Projects";
}

void TestCreateProjectView::test_projectPath()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  this->setProjectPath(value);
  QCOMPARE(result, this->projectPath());
}

void TestCreateProjectView::test_projectPath_signals()
{
  /// Simulación de entrada por teclado

  QString project_path = this->projectPath();

  QTest::keyClick(this->mLineEditProjectPath, '_');
  QCOMPARE(this->projectPath(), project_path.append("_"));

  this->mLineEditProjectPath->clear();
  QTest::keyClicks(this->mLineEditProjectPath, "C:/Users/user1/Documents/photomatch/Projects");
  QCOMPARE(this->projectPath(), QString("C:/Users/user1/Documents/photomatch/Projects"));
}

void TestCreateProjectView::test_lineEditProjectFile()
{
  /// Desactivado
  QCOMPARE(this->mLineEditProjectFile->isEnabled(), false);


  this->mLineEditProjectPath->clear();
  QTest::keyClicks(this->mLineEditProjectPath, "C:/Users/user1/Documents/photomatch/Projects");

  this->mLineEditProjectName->clear();
  QTest::keyClicks(this->mLineEditProjectName, "project_name");

  QTest::mouseClick(this->mCheckBoxProjectFolder, Qt::MouseButton::LeftButton);
  QString file(this->projectPath());
  if (createProjectFolderEnable())
    file.append(QDir::separator()).append(this->projectName());
  file.append(QDir::separator()).append(this->projectName()).append(".xml");
  QCOMPARE(this->mLineEditProjectFile->text(), QDir::cleanPath(file));


  QTest::mouseClick(this->mCheckBoxProjectFolder, Qt::MouseButton::LeftButton);
  file = this->projectPath();
  if (createProjectFolderEnable())
    file.append(QDir::separator()).append(this->projectName());
  file.append(QDir::separator()).append(this->projectName()).append(".xml");
  QCOMPARE(this->mLineEditProjectFile->text(), QDir::cleanPath(file));

  this->mLineEditProjectName->clear();
  QCOMPARE(this->mLineEditProjectFile->text(), "");
}

void TestCreateProjectView::test_projectDescription()
{
  /// Simulación de entrada por teclado

  QString project_description = this->projectDescription();

  QTest::keyClick(this->mTextEditDescription, '_');
  QCOMPARE(project_description.append("_"), this->projectDescription());

  this->mTextEditDescription->clear();

  QTest::keyClicks(this->mTextEditDescription, "description");
  QCOMPARE(QString("description"), this->projectDescription());
}

void TestCreateProjectView::test_createProjectFolder()
{
  mCheckBoxProjectFolder->setChecked(true);
  QCOMPARE(true, this->createProjectFolderEnable());

  mCheckBoxProjectFolder->setChecked(false);
  QCOMPARE(false, this->createProjectFolderEnable());
}

void TestCreateProjectView::test_pushButtonProjectPath()
{

}

void TestCreateProjectView::test_dialogButtonBox()
{
  QSignalSpy spy_rejected(this, &CreateProjectViewImp::rejected);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Cancel), Qt::LeftButton);
  QCOMPARE(spy_rejected.count(), 1);

  mButtonBox->button(QDialogButtonBox::Save)->setEnabled(true);
  QSignalSpy spy_accepted(this, &CreateProjectViewImp::accepted);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Save), Qt::LeftButton);
  QCOMPARE(spy_accepted.count(), 1);

  QSignalSpy spy_help(this, &CreateProjectViewImp::help);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Help), Qt::LeftButton);
  QCOMPARE(spy_help.count(), 1);
}

void TestCreateProjectView::test_setExistingProject()
{
  this->mLineEditProjectName->setText("Prueba");
  this->setExistingProject(true);
  QPalette palette = mLineEditProjectName->palette();
  QColor color = palette.color(QPalette::Text);
  QCOMPARE(QColor(Qt::red), color);
  QCOMPARE(false, this->mButtonBox->button(QDialogButtonBox::Save)->isEnabled());

  this->setExistingProject(false);
  palette = mLineEditProjectName->palette();
  color = palette.color(QPalette::Text);
  QCOMPARE(QColor(Qt::black), color);
  QCOMPARE(true, this->mButtonBox->button(QDialogButtonBox::Save)->isEnabled());
}


QTEST_MAIN(TestCreateProjectView)

#include "tst_createprojectview.moc"
