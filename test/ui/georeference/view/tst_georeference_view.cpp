#include <QtTest>
#include <QCoreApplication>

#include "inspector/ui/georeference/impl/GeoreferenceView.h"

#include <QLineEdit>
//#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QStringList>
//#include <QListWidget>
//#include <QSpinBox>
#include <QComboBox>

using namespace inspector;
using namespace ui;

class TestGeoreferenceView 
  : public GeoreferenceViewImp
{
  Q_OBJECT

public:

  TestGeoreferenceView();
  ~TestGeoreferenceView();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_constructor();
  void test_orientationFile_data();
  void test_orientationFile();
  void test_imageColumn();
  void test_imageColumnChange();
  void test_xColumn();
  void test_xColumnChange();
  void test_yColumn();
  void test_yColumnChange();
  void test_zColumn();
  void test_zColumnChange();
  void test_qxColumn();
  void test_qxColumnChange();
  void test_qyColumn();
  void test_qyColumnChange();
  void test_qzColumn();
  void test_qzColumnChange();
  void test_qwColumn();
  void test_qwColumnChange();
  void test_dialogButtonBox();
};

TestGeoreferenceView::TestGeoreferenceView()
  : GeoreferenceViewImp()
{
  QApplication::setActiveWindow(this);

}

TestGeoreferenceView::~TestGeoreferenceView()
{
}

void TestGeoreferenceView::initTestCase()
{
  QStringList header;
  header.push_back("Image");
  header.push_back("Lon");
  header.push_back("Lat");
  header.push_back("Alt");
  header.push_back("Qx");
  header.push_back("Qy");
  header.push_back("Qz");
  header.push_back("Qw");

  this->setTableHeader(header);
}

void TestGeoreferenceView::cleanupTestCase()
{
  this->clear();

  QCOMPARE("", this->orientationFile());
}

void TestGeoreferenceView::test_constructor()
{
  QCOMPARE("", this->orientationFile());
}

void TestGeoreferenceView::test_orientationFile_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("C:/Projects/cameras.txt") << "C:/Projects/cameras.txt" << "C:/Projects/cameras.txt";
  QTest::newRow("C:/Projects/cameras.csv") << "C:/Projects/cameras.csv" << "C:/Projects/cameras.csv";

}

void TestGeoreferenceView::test_orientationFile()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  this->mLineEditImportCameras->clear();
  QTest::keyClicks(this->mLineEditImportCameras, value);

  QCOMPARE(result, this->orientationFile());
}

void TestGeoreferenceView::test_imageColumn()
{
  this->setImageColumn("Image");
  QCOMPARE("Image", mComboBoxImageColumn->currentText());

  this->setImageColumn("");
}

void TestGeoreferenceView::test_imageColumnChange()
{
  QSignalSpy spy_imageColumnChange(this, &GeoreferenceViewImp::imageColumnChange);

  mComboBoxImageColumn->setCurrentText("Image");

  QCOMPARE(spy_imageColumnChange.count(), 1);

  QList<QVariant> args = spy_imageColumnChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "Image");

  this->setImageColumn("");
  QCOMPARE(spy_imageColumnChange.count(), 0);
}

void TestGeoreferenceView::test_xColumn()
{
  this->setXColumn("Lon");
  QCOMPARE("Lon", mComboBoxXColumn->currentText());

  this->setXColumn("");
}

void TestGeoreferenceView::test_xColumnChange()
{
  QSignalSpy spy_xColumnChange(this, &GeoreferenceViewImp::xColumnChange);

  mComboBoxXColumn->setCurrentText("Lon");

  QCOMPARE(spy_xColumnChange.count(), 1);

  QList<QVariant> args = spy_xColumnChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "Lon");

  this->setXColumn("");
  QCOMPARE(spy_xColumnChange.count(), 0);
}

void TestGeoreferenceView::test_yColumn()
{
  this->setYColumn("Lat");
  QCOMPARE("Lat", mComboBoxYColumn->currentText());

  this->setYColumn("");
}

void TestGeoreferenceView::test_yColumnChange()
{
  QSignalSpy spy_yColumnChange(this, &GeoreferenceViewImp::yColumnChange);

  mComboBoxYColumn->setCurrentText("Lat");

  QCOMPARE(spy_yColumnChange.count(), 1);

  QList<QVariant> args = spy_yColumnChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "Lat");

  this->setYColumn("");
  QCOMPARE(spy_yColumnChange.count(), 0);
}

void TestGeoreferenceView::test_zColumn()
{
  this->setZColumn("Alt");
  QCOMPARE("Alt", mComboBoxZColumn->currentText());
  this->setZColumn("");
}

void TestGeoreferenceView::test_zColumnChange()
{
  QSignalSpy spy_zColumnChange(this, &GeoreferenceViewImp::zColumnChange);

  mComboBoxZColumn->setCurrentText("Alt");

  QCOMPARE(spy_zColumnChange.count(), 1);

  QList<QVariant> args = spy_zColumnChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "Alt");

  this->setZColumn("");
  QCOMPARE(spy_zColumnChange.count(), 0);
}

void TestGeoreferenceView::test_qxColumn()
{
  this->setQxColumn("Qx");
  QCOMPARE("Qx", mComboBoxQxColumn->currentText());
  this->setQxColumn("");
}

void TestGeoreferenceView::test_qxColumnChange()
{
  QSignalSpy spy_qxColumnChange(this, &GeoreferenceViewImp::qxColumnChange);

  mComboBoxQxColumn->setCurrentText("Qx");

  QCOMPARE(spy_qxColumnChange.count(), 1);

  QList<QVariant> args = spy_qxColumnChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "Qx");

  this->setQxColumn("");
  QCOMPARE(spy_qxColumnChange.count(), 0);
}

void TestGeoreferenceView::test_qyColumn()
{
  this->setQyColumn("Qy");
  QCOMPARE("Qy", mComboBoxQyColumn->currentText());
  this->setQyColumn("");
}

void TestGeoreferenceView::test_qyColumnChange()
{
  QSignalSpy spy_qyColumnChange(this, &GeoreferenceViewImp::qyColumnChange);

  mComboBoxQyColumn->setCurrentText("Qy");

  QCOMPARE(spy_qyColumnChange.count(), 1);

  QList<QVariant> args = spy_qyColumnChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "Qy");

  this->setQyColumn("");
  QCOMPARE(spy_qyColumnChange.count(), 0);
}

void TestGeoreferenceView::test_qzColumn()
{
  this->setQzColumn("Qz");
  QCOMPARE("Qz", mComboBoxQzColumn->currentText());
  this->setQzColumn("");
}

void TestGeoreferenceView::test_qzColumnChange()
{
  QSignalSpy spy_qzColumnChange(this, &GeoreferenceViewImp::qzColumnChange);

  mComboBoxQzColumn->setCurrentText("Qz");

  QCOMPARE(spy_qzColumnChange.count(), 1);

  QList<QVariant> args = spy_qzColumnChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "Qz");

  this->setQzColumn("");
  QCOMPARE(spy_qzColumnChange.count(), 0);
}

void TestGeoreferenceView::test_qwColumn()
{
  this->setQwColumn("Qw");
  QCOMPARE("Qw", mComboBoxQwColumn->currentText());
  this->setQwColumn("");
}

void TestGeoreferenceView::test_qwColumnChange()
{
  QSignalSpy spy_qwColumnChange(this, &GeoreferenceViewImp::qwColumnChange);

  mComboBoxQwColumn->setCurrentText("Qw");

  QCOMPARE(spy_qwColumnChange.count(), 1);

  QList<QVariant> args = spy_qwColumnChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "Qw");

  this->setQwColumn("");
  QCOMPARE(spy_qwColumnChange.count(), 0);
}

void TestGeoreferenceView::test_dialogButtonBox()
{
  QSignalSpy spy_rejected(this, &GeoreferenceViewImp::rejected);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Cancel), Qt::LeftButton);
  QCOMPARE(spy_rejected.count(), 1);

  QSignalSpy spy_accepted(this, &GeoreferenceViewImp::accepted);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Ok), Qt::LeftButton);
  QCOMPARE(spy_accepted.count(), 1);

  QSignalSpy spy_help(this, &GeoreferenceViewImp::help);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Help), Qt::LeftButton);
  QCOMPARE(spy_help.count(), 1);
}


QTEST_MAIN(TestGeoreferenceView)

#include "tst_georeference_view.moc"
