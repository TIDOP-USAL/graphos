#include <QtTest>
#include <QCoreApplication>

#include "inspector/widgets/StartPageWidget.h"

#include <QCommandLinkButton>
#include <QListWidget>

using namespace inspector;

class TestStartPageWidget
  : public StartPageWidget
{
  Q_OBJECT

public:

  TestStartPageWidget();
  ~TestStartPageWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_openNew();
  void test_openProject();
  void test_openSettings();
  void test_clearHistory();
  void test_openProjectFromHistory();
  void test_recentProjects();

};

TestStartPageWidget::TestStartPageWidget()
  : StartPageWidget()
{
}

TestStartPageWidget::~TestStartPageWidget()
{
}

void TestStartPageWidget::initTestCase()
{

  QStringList history {
    QString("C:\\Users\\Tido\\Documents\\Inspector\\Projects\\230\\230.xml"),
    QString("C:\\Users\\Tido\\Documents\\Inspector\\Projects\\523\\523.xml"),
    QString("C:\\Users\\Tido\\Documents\\Inspector\\Projects\\165\\165.xml"),
    QString("C:\\Users\\Tido\\Documents\\Inspector\\Projects\\963\\963.xml")};

  this->setHistory(history);
}

void TestStartPageWidget::cleanupTestCase()
{

}

void TestStartPageWidget::test_windowTitle()
{
  QCOMPARE("Start Page", this->windowTitle());
}

void TestStartPageWidget::test_openNew()
{
//  QSignalSpy spy_openProject(this, &TestStartPageWidget::openProject);

//  QTest::mouseClick(this->mCommandLinkButtonNewProject, Qt::MouseButton::LeftButton,);

//  QCOMPARE(spy_openProject.count(), 1);
}

void TestStartPageWidget::test_openProject()
{
//  QSignalSpy spy_openSettings(this, &TestStartPageWidget::openSettings);

//  QTest::mouseClick(this->mCommandLinkButtonOpenProject, Qt::MouseButton::LeftButton);

//  QCOMPARE(spy_openSettings.count(), 1);
}

void TestStartPageWidget::test_openSettings()
{
//  QSignalSpy spy_openSettings(this, &TestStartPageWidget::openSettings);

//  QTest::mouseClick(this->mCommandLinkButtonSettings, Qt::MouseButton::LeftButton);

//  QCOMPARE(spy_openSettings.count(), 1);
}

void TestStartPageWidget::test_clearHistory()
{
//  QSignalSpy spy_clearHistory(this, &TestStartPageWidget::clearHistory);

//  QTest::mouseClick(this->mCommandLinkButtonClearHistory, Qt::MouseButton::LeftButton);

//  QCOMPARE(spy_clearHistory.count(), 1);
}

void TestStartPageWidget::test_openProjectFromHistory()
{

}

void TestStartPageWidget::test_recentProjects()
{
  QCOMPARE(4, mListWidgetRecentProjects->count());
}

QTEST_MAIN(TestStartPageWidget)

#include "tst_startpagewidget.moc"
