#include <QtTest>
#include <QCoreApplication>

#include "inspector/widgets/NvmFormatWidget.h"

#include <QLineEdit>

using namespace inspector;

class TestNvmFormatWidget
  : public NvmFormatWidgetImp
{
  Q_OBJECT

public:

  TestNvmFormatWidget();
  ~TestNvmFormatWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_file();
  void test_file_keyboard_input();
  void test_fileChange();
};

TestNvmFormatWidget::TestNvmFormatWidget()
{
}

TestNvmFormatWidget::~TestNvmFormatWidget()
{
}

void TestNvmFormatWidget::initTestCase()
{
  QCOMPARE(QString(""), this->file());
}

void TestNvmFormatWidget::cleanupTestCase()
{
  this->clear();

  QCOMPARE(QString(""), this->file());
}

void TestNvmFormatWidget::test_windowTitle()
{
  QCOMPARE("NVM", this->windowTitle());
}

void TestNvmFormatWidget::test_file()
{
  this->setFile("C:\\prj\\file.nvm");
  QCOMPARE("C:\\prj\\file.nvm", this->file());
}

void TestNvmFormatWidget::test_file_keyboard_input()
{
  this->mLineEditFile->clear();
  QSignalSpy spy_fileChanged(this, &TestNvmFormatWidget::fileChanged);

  QTest::keyClicks(this->mLineEditFile, "C:/prj/file.nvm");

  QCOMPARE(spy_fileChanged.count(), 15);

  QList<QVariant> args = spy_fileChanged.takeLast();
  QCOMPARE(args.at(0).toString(), "C:/prj/file.nvm");
}

void TestNvmFormatWidget::test_fileChange()
{
  this->mLineEditFile->clear();

  QSignalSpy spy_fileChanged(this, &TestNvmFormatWidget::fileChanged);

  this->mLineEditFile->setText("C:\\prj\\file.nvm");

  QCOMPARE(spy_fileChanged.count(), 1);
  QList<QVariant> args = spy_fileChanged.takeFirst();
  QCOMPARE(args.at(0).toString(), "C:\\prj\\file.nvm");

  this->setFile("C:\\prj\\file2.nvm");
  QCOMPARE(spy_fileChanged.count(), 0);
}


QTEST_MAIN(TestNvmFormatWidget)

#include "tst_nvmwidget.moc"
