#include <QtTest>

#include "graphos/ui/createproject/impl/CreateProjectModel.h"
#include "graphos/core/project.h"

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
/*   void cleanupTestCase();
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
  void test_setExistingProject(); */

};

TestCreateProjectModel::TestCreateProjectModel()
{
}

TestCreateProjectModel::~TestCreateProjectModel()
{
}

void TestCreateProjectModel::initTestCase()
{

}

/* void TestCreateProjectModel::cleanupTestCase()
{
  this->clear();

  QCOMPARE(this->projectName(), QString(""));
  QCOMPARE(this->projectPath(), QString(""));
  QCOMPARE(this->mLineEditProjectFile->text(), QString(""));
  QCOMPARE(this->projectDescription(), QString(""));
  QCOMPARE(true, this->createProjectFolderEnable());
}

void TestCreateProjectModel::test_constructor()
{
  CreateProjectModelImp newProjectModel;
  QCOMPARE("", newProjectModel.projectName());
  QCOMPARE("", newProjectModel.projectPath());
  QCOMPARE("", newProjectModel.projectDescription());
  QCOMPARE(true, newProjectModel.createProjectFolderEnable());
}

void TestCreateProjectModel::test_projectName()
{

  /// Simulación de entrada por teclado

  this->mLineEditProjectName->clear();
  QTest::keyClicks(this->mLineEditProjectName, "project");

  QCOMPARE(this->projectName(), QString("project"));
}

void TestCreateProjectModel::test_projectPath_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("C:/Users/Tido/Documents/photomatch/Projects") << "C:/Users/Tido/Documents/photomatch/Projects" << "C:/Users/Tido/Documents/photomatch/Projects";
  QTest::newRow("C:/Users/user1/Documents/photomatch/Projects") << "C:/Users/user1/Documents/photomatch/Projects" << "C:/Users/user1/Documents/photomatch/Projects";
}

void TestCreateProjectModel::test_projectPath()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  this->setProjectPath(value);
  QCOMPARE(result, this->projectPath());
}

void TestCreateProjectModel::test_projectPath_signals()
{
  /// Simulación de entrada por teclado

  QString project_path = this->projectPath();

  QTest::keyClick(this->mLineEditProjectPath, '_');
  QCOMPARE(this->projectPath(), project_path.append("_"));

  this->mLineEditProjectPath->clear();
  QTest::keyClicks(this->mLineEditProjectPath, "C:/Users/user1/Documents/photomatch/Projects");
  QCOMPARE(this->projectPath(), QString("C:/Users/user1/Documents/photomatch/Projects"));
}

void TestCreateProjectModel::test_lineEditProjectFile()
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

void TestCreateProjectModel::test_projectDescription()
{
  /// Simulación de entrada por teclado

  QString project_description = this->projectDescription();

  QTest::keyClick(this->mTextEditDescription, '_');
  QCOMPARE(project_description.append("_"), this->projectDescription());

  this->mTextEditDescription->clear();

  QTest::keyClicks(this->mTextEditDescription, "description");
  QCOMPARE(QString("description"), this->projectDescription());
}

void TestCreateProjectModel::test_createProjectFolder()
{
  mCheckBoxProjectFolder->setChecked(true);
  QCOMPARE(true, this->createProjectFolderEnable());

  mCheckBoxProjectFolder->setChecked(false);
  QCOMPARE(false, this->createProjectFolderEnable());
}

void TestCreateProjectModel::test_pushButtonProjectPath()
{

}

void TestCreateProjectModel::test_dialogButtonBox()
{
  QSignalSpy spy_rejected(this, &CreateProjectModelImp::rejected);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Cancel), Qt::LeftButton);
  QCOMPARE(spy_rejected.count(), 1);

  mButtonBox->button(QDialogButtonBox::Save)->setEnabled(true);
  QSignalSpy spy_accepted(this, &CreateProjectModelImp::accepted);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Save), Qt::LeftButton);
  QCOMPARE(spy_accepted.count(), 1);

  QSignalSpy spy_help(this, &CreateProjectModelImp::help);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Help), Qt::LeftButton);
  QCOMPARE(spy_help.count(), 1);
}

void TestCreateProjectModel::test_setExistingProject()
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
} */


QTEST_APPLESS_MAIN(TestCreateProjectModel)

#include "tst_createprojectmodel.moc"
