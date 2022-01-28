#include <QtTest>
#include <QCoreApplication>

#include "graphos/components/openproject/impl/OpenProjectView.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>

using namespace graphos;

class TestOpenProjectView
  : public OpenProjectViewImp
{
  Q_OBJECT

public:

  TestOpenProjectView();
  ~TestOpenProjectView();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_setGraphosProjectsPath_data();
  void test_setGraphosProjectsPath();

};

TestOpenProjectView::TestOpenProjectView()
  : OpenProjectViewImp()
{
  QApplication::setActiveWindow(this);
}

TestOpenProjectView::~TestOpenProjectView()
{
}

void TestOpenProjectView::initTestCase()
{
  QCOMPARE("Open Graphos Project" , this->windowTitle());
  QCOMPARE("Graphos Project (*.xml)" , this->nameFilters().at(0));
  QCOMPARE(QFileDialog::ExistingFile , this->fileMode());
  //QCOMPARE("" , this->directory());
}

void TestOpenProjectView::cleanupTestCase()
{
}


void TestOpenProjectView::test_setGraphosProjectsPath_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("C:/Users/Tido/Documents/photomatch/Projects") << "C:/Users/Tido/Documents/photomatch/Projects" << "C:/Users/Tido/Documents/photomatch/Projects";
  QTest::newRow("C:/Users/user1/Documents/photomatch/Projects") << "C:/Users/user1/Documents/photomatch/Projects" << "C:/Users/user1/Documents/photomatch/Projects";
}

void TestOpenProjectView::test_setGraphosProjectsPath()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  this->setGraphosProjectsPath(value);
  QCOMPARE(result, this->directory().absolutePath());
}

QTEST_MAIN(TestOpenProjectView)

#include "tst_openprojectview.moc"
