#include <QtTest>
#include <QCoreApplication>

#include "inspector/ui/SettingsView.h"

#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QListWidget>
#include <QCheckBox>

using namespace inspector;
using namespace ui;

class TestSettingsView
  : public SettingsViewImp
{
  Q_OBJECT

public:

  TestSettingsView();
  ~TestSettingsView();

private slots:

  void initTestCase();
  void cleanupTestCase();

  void test_setLanguages();
  void test_setActiveLanguage();
  void test_languageChange();

  void test_historyMaxSize_data();
  void test_historyMaxSize();
  void test_historyMaxSizeChange();

  void test_imageViewerBGColor_data();
  void test_imageViewerBGColor();
  void test_imageViewerBGColorChange();

  void test_useCuda();
  void test_useCudaChange();

  void test_dialogButtonBox();
  void test_setUnsavedChanges();

private:

  QStringList list;

};

TestSettingsView::TestSettingsView()
  : SettingsViewImp()
{
  QApplication::setActiveWindow(this);
}

TestSettingsView::~TestSettingsView()
{

}

void TestSettingsView::initTestCase()
{
  list.push_back("English");
  list.push_back("Spanish");
  this->setLanguages(list);

}

void TestSettingsView::cleanupTestCase()
{

}

void TestSettingsView::test_setLanguages()
{
  QSignalSpy spy_languageChange(this, &SettingsViewImp::languageChange);
  this->setLanguages(list);
  QCOMPARE(spy_languageChange.count(), 0);
  QCOMPARE(2, mLanguages->count());
  QCOMPARE("English", mLanguages->itemText(0));
  QCOMPARE("Spanish", mLanguages->itemText(1));
}

void TestSettingsView::test_setActiveLanguage()
{
  this->setActiveLanguage("English");
  QCOMPARE("English", mLanguages->currentText());
}

void TestSettingsView::test_languageChange()
{
  QSignalSpy spy_languageChange(this, &SettingsViewImp::languageChange);

  this->mLanguages->setCurrentText("Spanish");
  QCOMPARE(spy_languageChange.count(), 1);
  QList<QVariant> args = spy_languageChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "Spanish");

  this->mLanguages->setCurrentText("Spanish");
  QCOMPARE(spy_languageChange.count(), 0);

  this->setActiveLanguage("English");
  QCOMPARE(spy_languageChange.count(), 0);
}

void TestSettingsView::test_historyMaxSize_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("1") << 1 << 1;
  QTest::newRow("5") << 5 << 5;
  QTest::newRow("10") << 10 << 10;
  QTest::newRow("Out of range value") << 0 << 1;
  QTest::newRow("Out of range value") << 51 << 50;
}

void TestSettingsView::test_historyMaxSize()
{
  QFETCH(int, value);
  QFETCH(int, result);

  this->setHistoryMaxSize(value);
  QCOMPARE(result, this->historyMaxSize());
}

void TestSettingsView::test_historyMaxSizeChange()
{
  QSignalSpy spy_historyMaxSizeChange(this, &SettingsViewImp::historyMaxSizeChange);

  this->mHistoryMaxSize->setValue(5);

  QCOMPARE(spy_historyMaxSizeChange.count(), 1);

  QList<QVariant> args = spy_historyMaxSizeChange.takeFirst();
  QCOMPARE(args.at(0).toInt(), 5);

  this->setHistoryMaxSize(20);
  QCOMPARE(spy_historyMaxSizeChange.count(), 0);
}

void TestSettingsView::test_imageViewerBGColor_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("#FF00FF") << "#FF00FF" << "#FF00FF";
  QTest::newRow("#253612") << "#253612" << "#253612";
  QTest::newRow("#205060") << "#205060" << "#205060";
}

void TestSettingsView::test_imageViewerBGColor()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  this->setImageViewerBGcolor(value);
  QCOMPARE(result, this->imageViewerBGColor());
}

void TestSettingsView::test_imageViewerBGColorChange()
{
  QSignalSpy spy_imageViewerBGColorChange(this, &SettingsViewImp::imageViewerBGColorChange);

  this->mLineEditImageViewerBGcolor->setText("#5625ff");

  QCOMPARE(spy_imageViewerBGColorChange.count(), 1);

  QList<QVariant> args = spy_imageViewerBGColorChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "#5625ff");

  this->setImageViewerBGcolor("FF00FF");
  QCOMPARE(spy_imageViewerBGColorChange.count(), 0);
}

void TestSettingsView::test_useCuda()
{
  this->setUseCuda(false);
  QCOMPARE(false, this->useCuda());

  this->setUseCuda(true);
  QCOMPARE(true, this->useCuda());

  this->setCudaEnabled(false);
  QCOMPARE(false, this->useCuda());

  this->setCudaEnabled(true);
  QCOMPARE(true, this->useCuda());
}

void TestSettingsView::test_useCudaChange()
{
QSKIP("");
  QSignalSpy spy_useCudaChange(this, &SettingsViewImp::useCudaChange);
  QTest::mouseClick(this->mCheckBoxUseCuda, Qt::MouseButton::LeftButton);

  QCOMPARE(spy_useCudaChange.count(), 1);

  QList<QVariant> args = spy_useCudaChange.takeFirst();
  QCOMPARE(args.at(0).toBool(), true);

  this->setUseCuda(true);
  QCOMPARE(spy_useCudaChange.count(), 0);
}

void TestSettingsView::test_dialogButtonBox()
{
  QSignalSpy spy_rejected(this, &SettingsViewImp::rejected);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Cancel), Qt::LeftButton);
  QCOMPARE(spy_rejected.count(), 1);

  QSignalSpy spy_accepted(this, &SettingsViewImp::accepted);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Ok), Qt::LeftButton);
  QCOMPARE(spy_accepted.count(), 1);

  QSignalSpy spy_applyChanges(this, &SettingsViewImp::applyChanges);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Apply), Qt::LeftButton);
  QCOMPARE(spy_applyChanges.count(), 0);
  this->setUnsavedChanges(true); //Para activarlo
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Apply), Qt::LeftButton);
  QCOMPARE(spy_applyChanges.count(), 1);

  QSignalSpy spy_help(this, &SettingsViewImp::help);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Help), Qt::LeftButton);
  QCOMPARE(spy_help.count(), 1);
}

void TestSettingsView::test_setUnsavedChanges()
{
  this->setUnsavedChanges(true);
  QCOMPARE(true, mButtonBox->button(QDialogButtonBox::Apply)->isEnabled());

  this->setUnsavedChanges(false);
  QCOMPARE(false, mButtonBox->button(QDialogButtonBox::Apply)->isEnabled());
}

QTEST_MAIN(TestSettingsView)

#include "tst_settingsview.moc"
