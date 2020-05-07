#include <QtTest>
#include <QCoreApplication>

#include "inspector/widgets/ThumbnailsWidget.h"

#include <QAction>

using namespace inspector;

class TestThumbnailsWidget
  : public ThumbnailsWidget
{
  Q_OBJECT

public:
  TestThumbnailsWidget();
  ~TestThumbnailsWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_thumbnailChanged();
  void test_thumbnailSmallChanged();
  void test_detailsChanged();

};

TestThumbnailsWidget::TestThumbnailsWidget()
  : ThumbnailsWidget()
{
}

TestThumbnailsWidget::~TestThumbnailsWidget()
{
}

void TestThumbnailsWidget::initTestCase()
{

}

void TestThumbnailsWidget::cleanupTestCase()
{

}

void TestThumbnailsWidget::test_windowTitle()
{
  QCOMPARE("Thumbnails", this->windowTitle());
}

void TestThumbnailsWidget::test_thumbnailChanged()
{
  emit mThumbnailAction->changed();

  QCOMPARE(mThumbnailAction->isChecked(), true);
  QCOMPARE(mThumbnailSmallAction->isChecked(), false);
  QCOMPARE(mDetailsAction->isChecked(), false);
}

void TestThumbnailsWidget::test_thumbnailSmallChanged()
{
  emit mThumbnailSmallAction->changed();

  QCOMPARE(mThumbnailAction->isChecked(), false);
  QCOMPARE(mThumbnailSmallAction->isChecked(), true);
  QCOMPARE(mDetailsAction->isChecked(), false);
}

void TestThumbnailsWidget::test_detailsChanged()
{
  emit mDetailsAction->changed();

  QCOMPARE(mThumbnailAction->isChecked(), false);
  QCOMPARE(mThumbnailSmallAction->isChecked(), false);
  QCOMPARE(mDetailsAction->isChecked(), true);
}

QTEST_MAIN(TestThumbnailsWidget)

#include "tst_thumbnailswidget.moc"
