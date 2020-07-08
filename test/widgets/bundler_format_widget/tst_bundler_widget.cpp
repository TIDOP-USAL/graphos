#include <QtTest>
#include <QCoreApplication>

#include "inspector/widgets/BundlerFormatWidget.h"

#include <QLineEdit>

using namespace inspector;

class TestBundlerFormatWidget
  : public BundlerFormatWidgetImp
{
  Q_OBJECT

public:

  TestBundlerFormatWidget();
  ~TestBundlerFormatWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_file();
  void test_file_keyboard_input();
  void test_fileChange();
};

TestBundlerFormatWidget::TestBundlerFormatWidget()
{
}

TestBundlerFormatWidget::~TestBundlerFormatWidget()
{
}

void TestBundlerFormatWidget::initTestCase()
{
  QCOMPARE(QString(""), this->file());
}

void TestBundlerFormatWidget::cleanupTestCase()
{
  this->clear();

  QCOMPARE(QString(""), this->file());
}

void TestBundlerFormatWidget::test_windowTitle()
{
  QCOMPARE("Bundler", this->windowTitle());
}

void TestBundlerFormatWidget::test_file()
{
  this->setFile("C:\\prj\\file.out");
  QCOMPARE("C:\\prj\\file.out", this->file());
}

void TestBundlerFormatWidget::test_file_keyboard_input()
{
  this->mLineEditFile->clear();
  QSignalSpy spy_fileChanged(this, &TestBundlerFormatWidget::fileChanged);

  QTest::keyClicks(this->mLineEditFile, "C:/prj/file.out");

  QCOMPARE(spy_fileChanged.count(), 15);

  QList<QVariant> args = spy_fileChanged.takeLast();
  QCOMPARE(args.at(0).toString(), "C:/prj/file.out");
}

void TestBundlerFormatWidget::test_fileChange()
{
  this->mLineEditFile->clear();

  QSignalSpy spy_fileChanged(this, &TestBundlerFormatWidget::fileChanged);

  this->mLineEditFile->setText("C:\\prj\\file.out");

  QCOMPARE(spy_fileChanged.count(), 1);
  QList<QVariant> args = spy_fileChanged.takeFirst();
  QCOMPARE(args.at(0).toString(), "C:\\prj\\file.out");

  this->setFile("C:\\prj\\file2.out");
  QCOMPARE(spy_fileChanged.count(), 0);
}


QTEST_MAIN(TestBundlerFormatWidget)

#include "tst_bundler_widget.moc"
