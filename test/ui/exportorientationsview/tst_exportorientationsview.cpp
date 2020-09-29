#include <QtTest>
#include <QCoreApplication>

#include "inspector/ui/export/orientations/ExportOrientationsView.h"

#include <QPushButton>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>

using namespace inspector::ui;

class TestExportOrientationsView
  : public ExportOrientationsViewImp
{
  Q_OBJECT

public:

  TestExportOrientationsView();
  ~TestExportOrientationsView();

private slots:

  void initTestCase();
  void cleanupTestCase();

  void test_constructor();
  void test_windowTitle();
  void test_formats();
  void test_currentFormat_data();
  void test_currentFormat();
  void test_formatChange();
  void test_dialogButtonBox();
  void test_clear();

private:

  QStringList sessions;
  QStringList images;
  QStringList formats;
};

TestExportOrientationsView::TestExportOrientationsView()
  : ExportOrientationsViewImp()
{
  QApplication::setActiveWindow(this);

//  formats.push_back("NVM");
//  formats.push_back("PLY");
//  formats.push_back("VRML");
//  formats.push_back("Bundler");
//  formats.push_back("Pix4D");
//  //formats.push_back("MVE");
//  this->setFormats(formats);

}

TestExportOrientationsView::~TestExportOrientationsView()
{
}

void TestExportOrientationsView::initTestCase()
{

}

void TestExportOrientationsView::cleanupTestCase()
{
  
}

void TestExportOrientationsView::test_windowTitle()
{
  QCOMPARE("Export Cameras", this->windowTitle());
}

void TestExportOrientationsView::test_constructor()
{
  ExportOrientationsViewImp exportOrientationsView;
  QCOMPARE(QString(), exportOrientationsView.format());

  QCOMPARE(QString("NVM"), this->format());
  QCOMPARE(QString("NVM"), this->mComboBoxFormat->currentText());
}

void TestExportOrientationsView::test_formats()
{
  QCOMPARE(3, this->mComboBoxFormat->count());
  QCOMPARE("NVM", this->mComboBoxFormat->itemText(0));
  QCOMPARE("PLY", this->mComboBoxFormat->itemText(1));
  QCOMPARE("VRML", this->mComboBoxFormat->itemText(2));
  QCOMPARE("Bundler", this->mComboBoxFormat->itemText(2));
  QCOMPARE("Pix4D", this->mComboBoxFormat->itemText(2));
}

void TestExportOrientationsView::test_currentFormat_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("PLY") << "PLY" << "PLY";
  QTest::newRow("VRML") << "VRML" << "VRML";
  QTest::newRow("Bundler") << "Bundler" << "Bundler";
  QTest::newRow("Pix4D") << "Pix4D" << "Pix4D";
  QTest::newRow("NVM") << "NVM" << "NVM";
}

void TestExportOrientationsView::test_currentFormat()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  this->setCurrentFormat(value);
  QCOMPARE(result, this->format());
  QCOMPARE(result, this->mComboBoxFormat->currentText());
}

void TestExportOrientationsView::test_formatChange()
{
  QSignalSpy spy_formatChange(this, &ExportOrientationsViewImp::formatChange);

  this->mComboBoxFormat->setCurrentText("Bundler");

  QCOMPARE(spy_formatChange.count(), 1);

  QList<QVariant> args = spy_formatChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "Bundler");

  this->setCurrentFormat("Pix4D");
  QCOMPARE(spy_formatChange.count(), 0);
}

void TestExportOrientationsView::test_dialogButtonBox()
{

  QSignalSpy spy_rejected(this, &ExportOrientationsViewImp::rejected);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Cancel), Qt::LeftButton);
  QCOMPARE(spy_rejected.count(), 1);

  mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
  QSignalSpy spy_accepted(this, &ExportOrientationsViewImp::accepted);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Ok), Qt::LeftButton);
  QCOMPARE(spy_accepted.count(), 1);

  QSignalSpy spy_help(this, &ExportOrientationsViewImp::help);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Help), Qt::LeftButton);
  QCOMPARE(spy_help.count(), 1);
}

void TestExportOrientationsView::test_clear()
{
  this->clear();

  QCOMPARE(QString(), this->format());
  QCOMPARE(QString(), this->mComboBoxFormat->currentText());
}

QTEST_MAIN(TestExportOrientationsView)

#include "tst_exportorientationsview.moc"
