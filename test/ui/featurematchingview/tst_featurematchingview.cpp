#include <QtTest>
#include <QCoreApplication>

#include "graphos/ui/featmatch/impl/FeatureMatchingView.h"
#include "graphos/widgets/FeatureMatchingWidget.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>

using namespace graphos;

class TestFeatureMatchingView
  : public FeatureMatchingViewImp
{
  Q_OBJECT

public:

  TestFeatureMatchingView();
  ~TestFeatureMatchingView();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_currentMatchingMethod();

private:

  FeatureMatchingWidget *mFeatureMatchingWidget;
};

TestFeatureMatchingView::TestFeatureMatchingView()
  : FeatureMatchingViewImp(),
    mFeatureMatchingWidget(new FeatureMatchingWidgetImp)
{
  QApplication::setActiveWindow(this);
}

TestFeatureMatchingView::~TestFeatureMatchingView()
{
  if (mFeatureMatchingWidget){
    delete mFeatureMatchingWidget;
    mFeatureMatchingWidget = nullptr;
  }
}

void TestFeatureMatchingView::initTestCase()
{
  QCOMPARE("", this->currentMatchMethod());

  this->addMatchMethod(mFeatureMatchingWidget);
  this->setCurrentMatchMethod(mFeatureMatchingWidget->windowTitle());
}

void TestFeatureMatchingView::cleanupTestCase()
{
}

void TestFeatureMatchingView::test_windowTitle()
{
  QCOMPARE("Feature Matching", this->windowTitle());
}

 void TestFeatureMatchingView::test_currentMatchingMethod()
{
  QCOMPARE("Feature Matching Colmap", this->currentMatchMethod());
}


QTEST_MAIN(TestFeatureMatchingView)

#include "tst_featurematchingview.moc"
