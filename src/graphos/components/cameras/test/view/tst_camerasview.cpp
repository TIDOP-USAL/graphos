#include <QtTest>
#include <QCoreApplication>

#include "graphos/components/cameras/CamerasView.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QComboBox>

using namespace graphos;

class TestCamerasView 
  : public CamerasViewImp
{
  Q_OBJECT

public:

  TestCamerasView();
  ~TestCamerasView();

private slots:

  void test_constructor();
  void test_activeCamera();
  void test_addCamera();
  void test_enableCameraEdition();
  void test_setMake();
  void test_setModel();
  void test_setWidth();
  void test_setHeight();
  void test_setSensorSize();
  void test_setFocal();
  void test_setType();
  void test_setImages();
  void test_setCalibCx();
  void test_setCalibCy();
  void test_setCalibF();
  void test_setCalibFx();
  void test_setCalibFy();
  void test_setCalibK1();
  void test_setCalibK2();
  void test_setCalibK3();
  void test_setCalibK4();
  void test_setCalibK5();
  void test_setCalibK6();
  void test_setCalibP1();
  void test_setCalibP2();

  void test_cameraChange();
  void test_makeChange();
  void test_modelChange();
//  void test_widthChange();
//  void test_heightChange();
  void test_sensorSizeChange();
  void test_focalChange();
  void test_typeChange();

  void test_calibCxChange();
  void test_calibCyChange();
  void test_calibFChange();
  void test_calibFxChange();
  void test_calibFyChange();
  void test_calibK1Change();
  void test_calibK2Change();
  void test_calibK3Change();
  void test_calibK4Change();
  void test_calibK5Change();
  void test_calibK6Change();
  void test_calibP1Change();
  void test_calibP2Change();
  void test_dialogButtonBox();
  
};

TestCamerasView::TestCamerasView()
  : CamerasViewImp()
{
  QApplication::setActiveWindow(this);

}

TestCamerasView::~TestCamerasView()
{
}

void TestCamerasView::test_constructor()
{
}

void TestCamerasView::test_activeCamera()
{

}

void TestCamerasView::test_addCamera()
{
  this->addCamera(2, "Make-Model");

  QListWidgetItem *item = nullptr;
  for (int i = 0; i < mListWidgetCameras->count(); i++){
    if (mListWidgetCameras->item(i)->data(Qt::UserRole).toInt() == 2){
      item = mListWidgetCameras->item(i);
    }
  }

  QCOMPARE("Make-Model", item->text());
  QCOMPARE(2, item->data(Qt::UserRole).toInt());
}

void TestCamerasView::test_enableCameraEdition()
{
  this->enableCameraEdition(true);

  QCOMPARE(true, this->mLineEditMake->isEnabled());
  QCOMPARE(true, this->mLineEditModel->isEnabled());
  QCOMPARE(true, this->mLineEditSensorSize->isEnabled());
  QCOMPARE(true, this->mLineEditFocal->isEnabled());
}

void TestCamerasView::test_setMake()
{
  this->setMake("DJI");
  QCOMPARE("DJI", this->mLineEditMake->text());
}

void TestCamerasView::test_setModel()
{
  this->setModel("FC6310");
  QCOMPARE("FC6310", this->mLineEditModel->text());
}

void TestCamerasView::test_setWidth()
{
  this->setWidth(5472);
  QCOMPARE(5472, this->mSpinBoxWidth->value());
}

void TestCamerasView::test_setHeight()
{
  this->setHeight(3648);
  QCOMPARE(3648, this->mSpinBoxHeight->value());
}

void TestCamerasView::test_setSensorSize()
{
  this->setSensorSize("12.8333");
  QCOMPARE("12.8333", this->mLineEditSensorSize->text());
}

void TestCamerasView::test_setFocal()
{
  this->setFocal("3552.23");
  QCOMPARE("3552.23", this->mLineEditFocal->text());
}

void TestCamerasView::test_setType()
{
  this->setType("Simple Radial");
  QCOMPARE("Simple Radial", this->mComboBoxType->currentText());
}

void TestCamerasView::test_setImages()
{
  QStringList images;
  images.push_back("C:/Users/User01/Documents/inspector/Projects/prj001/images/img001.png");
  images.push_back("C:/Users/User01/Documents/inspector/Projects/prj001/images/img002.png");
  this->setImages(images);

  QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj001/images/img001.png", this->mListWidgetImages->item(0)->text());
  QCOMPARE("C:/Users/User01/Documents/inspector/Projects/prj001/images/img002.png", this->mListWidgetImages->item(1)->text());
}

void TestCamerasView::test_setCalibCx()
{
  this->setCalibCx("0.6");
  QCOMPARE("0.6", this->mLineEditCx->text());
}

void TestCamerasView::test_setCalibCy()
{
  this->setCalibCy("0.5");
  QCOMPARE("0.5", this->mLineEditCy->text());
}

void TestCamerasView::test_setCalibF()
{
  this->setCalibF("4000");
  QCOMPARE("4000", this->mLineEditF->text());
}

void TestCamerasView::test_setCalibFx()
{
  this->setCalibFx("4000");
  QCOMPARE("4000", this->mLineEditFx->text());
}

void TestCamerasView::test_setCalibFy()
{
  this->setCalibFy("4000");
  QCOMPARE("4000", this->mLineEditFy->text());
}

void TestCamerasView::test_setCalibK1()
{
  this->setCalibK1("0.005");
  QCOMPARE("0.005", this->mLineEditK1->text());
}

void TestCamerasView::test_setCalibK2()
{
  this->setCalibK2("0.001");
  QCOMPARE("0.001", this->mLineEditK2->text());
}

void TestCamerasView::test_setCalibK3()
{
  this->setCalibK3("0.0001");
  QCOMPARE("0.0001", this->mLineEditK3->text());
}

void TestCamerasView::test_setCalibK4()
{
  this->setCalibK4("0.0001");
  QCOMPARE("0.0001", this->mLineEditK4->text());
}

void TestCamerasView::test_setCalibK5()
{
  this->setCalibK5("0.0001");
  QCOMPARE("0.0001", this->mLineEditK5->text());
}

void TestCamerasView::test_setCalibK6()
{
  this->setCalibK6("0.0001");
  QCOMPARE("0.0001", this->mLineEditK6->text());
}

void TestCamerasView::test_setCalibP1()
{
  this->setCalibP1("0.01");
  QCOMPARE("0.01", this->mLineEditP1->text());
}

void TestCamerasView::test_setCalibP2()
{
  this->setCalibP2("0.001");
  QCOMPARE("0.001", this->mLineEditP2->text());
}

void TestCamerasView::test_cameraChange()
{
  QSignalSpy spy_cameraChange(this, &TestCamerasView::cameraChange);

}

void TestCamerasView::test_makeChange()
{
  QSignalSpy spy_makeChange(this, &TestCamerasView::makeChanged);

  this->mLineEditMake->clear();
  QTest::keyClicks(this->mLineEditMake, "Canon");

  QCOMPARE(spy_makeChange.count(), 5);

  QList<QVariant> args = spy_makeChange.takeLast();
  QCOMPARE(args.at(0).toString(), "Canon");

  spy_makeChange.clear();

  this->setMake("Olympus");
  QCOMPARE(spy_makeChange.count(), 0);
}

void TestCamerasView::test_modelChange()
{
  QSignalSpy spy_modelChange(this, &TestCamerasView::modelChanged);

  this->mLineEditModel->clear();
  QTest::keyClicks(this->mLineEditModel, "ILCE-6000");

  QCOMPARE(spy_modelChange.count(), 9);

  QList<QVariant> args = spy_modelChange.takeLast();
  QCOMPARE(args.at(0).toString(), "ILCE-6000");

  spy_modelChange.clear();

  this->setModel("FC6310");
  QCOMPARE(spy_modelChange.count(), 0);
}

//void TestCamerasView::test_widthChange()
//{
//  QSKIP("revisar");
//  QSignalSpy spy_widthChange(this, &TestCamerasView::widthChanged);

//  this->mSpinBoxWidth->setValue(4000);

//  QCOMPARE(spy_widthChange.count(), 1);

//  QList<QVariant> args = spy_widthChange.takeFirst();
//  QCOMPARE(args.at(0).toInt(), 4000);

//  this->setWidth(4500);
//  QCOMPARE(spy_widthChange.count(), 0);
//}

//void TestCamerasView::test_heightChange()
//{
//QSKIP("revisar");
//  QSignalSpy spy_heightChange(this, &CamerasView::heightChange);

//  this->mSpinBoxHeight->setValue(3500);

//  QCOMPARE(spy_heightChange.count(), 1);

//  QList<QVariant> args = spy_heightChange.takeFirst();
//  QCOMPARE(args.at(0).toInt(), 3500);

//  this->setHeight(4000);
//  QCOMPARE(spy_heightChange.count(), 0);
//}

void TestCamerasView::test_sensorSizeChange()
{
  QSignalSpy spy_sensorSizeChange(this, &TestCamerasView::sensorSizeChange);

  this->mLineEditSensorSize->clear();
  QTest::keyClicks(this->mLineEditSensorSize, "12.8333");

  QCOMPARE(spy_sensorSizeChange.count(), 7);

  QList<QVariant> args = spy_sensorSizeChange.takeLast();
  QCOMPARE(args.at(0).toString(), "12.8333");

  spy_sensorSizeChange.clear();

  this->setSensorSize("1.");
  QCOMPARE(spy_sensorSizeChange.count(), 0);
}

void TestCamerasView::test_focalChange()
{
  QSignalSpy spy_focalChange(this, &TestCamerasView::focalChange);

  this->mLineEditFocal->clear();
  QTest::keyClicks(this->mLineEditFocal, "4500");

  QCOMPARE(spy_focalChange.count(), 4);

  QList<QVariant> args = spy_focalChange.takeLast();
  QCOMPARE(args.at(0).toString(), "4500");

  spy_focalChange.clear();

  this->setFocal("1.");
  QCOMPARE(spy_focalChange.count(), 0);
}

void TestCamerasView::test_typeChange()
{
  QSignalSpy spy_typeChange(this, &TestCamerasView::typeChange);

  this->mComboBoxType->setCurrentText("Full Radial");

  QCOMPARE(spy_typeChange.count(), 1);

  QList<QVariant> args = spy_typeChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "Full Radial");

  this->setType("Simple Radial");
  QCOMPARE(spy_typeChange.count(), 0);
}

void TestCamerasView::test_calibCxChange()
{
  QSignalSpy spy_calibCxChange(this, &TestCamerasView::calibCxChange);

  this->mLineEditCx->clear();
  QTest::keyClicks(this->mLineEditCx, "2250");

  QCOMPARE(spy_calibCxChange.count(), 4);

  QList<QVariant> args = spy_calibCxChange.takeLast();
  QCOMPARE(args.at(0).toString(), "2250");

  spy_calibCxChange.clear();

  this->setCalibCx("1.");
  QCOMPARE(spy_calibCxChange.count(), 0);
}

void TestCamerasView::test_calibCyChange()
{
  QSignalSpy spy_calibCyChange(this, &TestCamerasView::calibCyChange);

  this->mLineEditCy->clear();
  QTest::keyClicks(this->mLineEditCy, "1800");

  QCOMPARE(spy_calibCyChange.count(), 4);

  QList<QVariant> args = spy_calibCyChange.takeLast();
  QCOMPARE(args.at(0).toString(), "1800");

  spy_calibCyChange.clear();

  this->setCalibCy("1.");
  QCOMPARE(spy_calibCyChange.count(), 0);
}

void TestCamerasView::test_calibFChange()
{
  QSignalSpy spy_calibFChange(this, &TestCamerasView::calibFChange);

  this->mLineEditF->clear();
  QTest::keyClicks(this->mLineEditF, "2000");

  QCOMPARE(spy_calibFChange.count(), 4);

  QList<QVariant> args = spy_calibFChange.takeLast();
  QCOMPARE(args.at(0).toString(), "2000");

  spy_calibFChange.clear();

  this->setCalibF("1.");
  QCOMPARE(spy_calibFChange.count(), 0);
}

void TestCamerasView::test_calibFxChange()
{
  QSignalSpy spy_calibFxChange(this, &TestCamerasView::calibFxChange);

  this->mLineEditFx->clear();
  QTest::keyClicks(this->mLineEditFx, "2000");

  QCOMPARE(spy_calibFxChange.count(), 4);

  QList<QVariant> args = spy_calibFxChange.takeLast();
  QCOMPARE(args.at(0).toString(), "2000");

  spy_calibFxChange.clear();

  this->setCalibFx("1.");
  QCOMPARE(spy_calibFxChange.count(), 0);
}

void TestCamerasView::test_calibFyChange()
{
  QSignalSpy spy_calibFyChange(this, &TestCamerasView::calibFyChange);

  this->mLineEditFy->clear();
  QTest::keyClicks(this->mLineEditFy, "2000");

  QCOMPARE(spy_calibFyChange.count(), 4);

  QList<QVariant> args = spy_calibFyChange.takeLast();
  QCOMPARE(args.at(0).toString(), "2000");

  spy_calibFyChange.clear();

  this->setCalibFy("1.");
  QCOMPARE(spy_calibFyChange.count(), 0);
}


void TestCamerasView::test_calibK1Change()
{
  QSignalSpy spy_calibK1Change(this, &TestCamerasView::calibK1Change);

  this->mLineEditK1->clear();
  QTest::keyClicks(this->mLineEditK1, "0.1");

  QCOMPARE(spy_calibK1Change.count(), 3);

  QList<QVariant> args = spy_calibK1Change.takeLast();
  QCOMPARE(args.at(0).toString(), "0.1");

  spy_calibK1Change.clear();

  this->setCalibK1("0.5");
  QCOMPARE(spy_calibK1Change.count(), 0);
}

void TestCamerasView::test_calibK2Change()
{
  QSignalSpy spy_calibK2Change(this, &TestCamerasView::calibK2Change);

  this->mLineEditK2->setEnabled(true);
  this->mLineEditK2->clear();
  QTest::keyClicks(this->mLineEditK2, "0.01");

  QCOMPARE(spy_calibK2Change.count(), 4);

  QList<QVariant> args = spy_calibK2Change.takeLast();
  QCOMPARE(args.at(0).toString(), "0.01");

  spy_calibK2Change.clear();

  this->setCalibK2("0.05");
  QCOMPARE(spy_calibK2Change.count(), 0);
}

void TestCamerasView::test_calibK3Change()
{
  QSignalSpy spy_calibK3Change(this, &TestCamerasView::calibK3Change);

  this->mLineEditK3->setEnabled(true);
  this->mLineEditK3->clear();
  QTest::keyClicks(this->mLineEditK3, "0.001");

  QCOMPARE(spy_calibK3Change.count(), 5);

  QList<QVariant> args = spy_calibK3Change.takeLast();
  QCOMPARE(args.at(0).toString(), "0.001");

  spy_calibK3Change.clear();

  this->setCalibK3("0.05");
  QCOMPARE(spy_calibK3Change.count(), 0);
}

void TestCamerasView::test_calibK4Change()
{
  QSignalSpy spy_calibK4Change(this, &TestCamerasView::calibK4Change);

  this->mLineEditK4->setEnabled(true);
  this->mLineEditK4->clear();
  QTest::keyClicks(this->mLineEditK4, "0.001");

  QCOMPARE(spy_calibK4Change.count(), 5);

  QList<QVariant> args = spy_calibK4Change.takeLast();
  QCOMPARE(args.at(0).toString(), "0.001");

  spy_calibK4Change.clear();

  this->setCalibK4("0.005");
  QCOMPARE(spy_calibK4Change.count(), 0);
}

void TestCamerasView::test_calibK5Change()
{
  QSignalSpy spy_calibK5Change(this, &TestCamerasView::calibK5Change);

  this->mLineEditK5->setEnabled(true);
  this->mLineEditK5->clear();
  QTest::keyClicks(this->mLineEditK5, "0.0001");

  QCOMPARE(spy_calibK5Change.count(), 6);

  QList<QVariant> args = spy_calibK5Change.takeLast();
  QCOMPARE(args.at(0).toString(), "0.0001");

  spy_calibK5Change.clear();

  this->setCalibK5("0.005");
  QCOMPARE(spy_calibK5Change.count(), 0);
}

void TestCamerasView::test_calibK6Change()
{
  QSignalSpy spy_calibK6Change(this, &TestCamerasView::calibK6Change);

  this->mLineEditK6->setEnabled(true);
  this->mLineEditK6->clear();
  QTest::keyClicks(this->mLineEditK6, "0.0001");

  QCOMPARE(spy_calibK6Change.count(), 6);

  QList<QVariant> args = spy_calibK6Change.takeLast();
  QCOMPARE(args.at(0).toString(), "0.0001");

  spy_calibK6Change.clear();

  this->setCalibK6("0.0005");
  QCOMPARE(spy_calibK6Change.count(), 0);
}

void TestCamerasView::test_calibP1Change()
{
  QSignalSpy spy_calibP1Change(this, &TestCamerasView::calibP1Change);

  this->mLineEditP1->setEnabled(true);
  this->mLineEditP1->clear();
  QTest::keyClicks(this->mLineEditP1, "0.01");

  QCOMPARE(spy_calibP1Change.count(), 4);

  QList<QVariant> args = spy_calibP1Change.takeLast();
  QCOMPARE(args.at(0).toString(), "0.01");

  spy_calibP1Change.clear();

  this->setCalibP1("0.5");
  QCOMPARE(spy_calibP1Change.count(), 0);
}

void TestCamerasView::test_calibP2Change()
{
  QSignalSpy spy_calibP2Change(this, &TestCamerasView::calibP2Change);

  this->mLineEditP2->setEnabled(true);
  this->mLineEditP2->clear();
  QTest::keyClicks(this->mLineEditP2, "0.001");

  QCOMPARE(spy_calibP2Change.count(), 5);

  QList<QVariant> args = spy_calibP2Change.takeLast();
  QCOMPARE(args.at(0).toString(), "0.001");

  spy_calibP2Change.clear();

  this->setCalibP2("0.05");
  QCOMPARE(spy_calibP2Change.count(), 0);
}

void TestCamerasView::test_dialogButtonBox()
{

//  QSignalSpy spy_rejected(this, &TestCamerasView::rejected);
//  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Cancel), Qt::LeftButton);
//  QCOMPARE(spy_rejected.count(), 1);

//  mButtonBox->button(QDialogButtonBox::Save)->setEnabled(true);
//  QSignalSpy spy_accepted(this, &TestCamerasView::accepted);
//  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Save), Qt::LeftButton);
//  QCOMPARE(spy_accepted.count(), 1);

//  QSignalSpy spy_help(this, &TestCamerasView::help);
//  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Help), Qt::LeftButton);
//  QCOMPARE(spy_help.count(), 1);
}



QTEST_MAIN(TestCamerasView)

#include "tst_camerasview.moc"
