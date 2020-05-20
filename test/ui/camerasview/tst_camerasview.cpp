#include <QtTest>
#include <QCoreApplication>

#include "inspector/ui/cameras/CamerasView.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QComboBox>

using namespace inspector;
using namespace ui;

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
  void test_setCalibK1();
  void test_setCalibK2();
  void test_setCalibK3();
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
  void test_calibK1Change();
  void test_calibK2Change();
  void test_calibK3Change();
  void test_calibP1Change();
  void test_calibP2Change();

/*   void test_projectName();
  void test_projectPath_data();
  void test_projectPath();
  void test_projectPath_signals();
  void test_lineEditProjectFile();
  void test_projectDescription();
  void test_createProjectFolder();
  void test_pushButtonProjectPath(); */
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
  CamerasViewImp camerasView;
/*   QCOMPARE("", newProjectView.projectName());
  QCOMPARE("", newProjectView.projectPath());
  QCOMPARE("", newProjectView.projectDescription());
  QCOMPARE(true, newProjectView.createProjectFolder()); */
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
  this->setType("SIMPLE_RADIAL");
  QCOMPARE("SIMPLE_RADIAL", this->mComboBoxType->currentText());
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
QSKIP("revisar");
  QSignalSpy spy_makeChange(this, &TestCamerasView::makeChanged);

  this->mLineEditMake->setText("Canon");

  QCOMPARE(spy_makeChange.count(), 1);

  QList<QVariant> args = spy_makeChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "Canon");

  this->setMake("Olympus");
  QCOMPARE(spy_makeChange.count(), 0);
}

void TestCamerasView::test_modelChange()
{
QSKIP("revisar");
  QSignalSpy spy_modelChange(this, &TestCamerasView::modelChanged);

  this->mLineEditModel->setText("ILCE-6000");

  QCOMPARE(spy_modelChange.count(), 1);

  QList<QVariant> args = spy_modelChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "ILCE-6000");

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
QSKIP("revisar");
  QSignalSpy spy_sensorSizeChange(this, &TestCamerasView::sensorSizeChange);

  this->mLineEditSensorSize->setText("12.8333");

  QCOMPARE(spy_sensorSizeChange.count(), 1);

  QList<QVariant> args = spy_sensorSizeChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "12.8333");

  this->setSensorSize("1.");
  QCOMPARE(spy_sensorSizeChange.count(), 0);
}

void TestCamerasView::test_focalChange()
{
QSKIP("revisar");
  QSignalSpy spy_focalChange(this, &TestCamerasView::focalChange);

  this->mLineEditFocal->setText("4500");

  QCOMPARE(spy_focalChange.count(), 1);

  QList<QVariant> args = spy_focalChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "4500");

  this->setFocal("1.");
  QCOMPARE(spy_focalChange.count(), 0);
}

void TestCamerasView::test_typeChange()
{
QSKIP("revisar");
  QSignalSpy spy_typeChange(this, &TestCamerasView::typeChange);

  this->mComboBoxType->setCurrentText("FULL_RADIAL");

  QCOMPARE(spy_typeChange.count(), 1);

  QList<QVariant> args = spy_typeChange.takeFirst();
  QCOMPARE(args.at(0).toString(), "FULL_RADIAL");

  this->setType("SIMPLE_RADIAL");
  QCOMPARE(spy_typeChange.count(), 0);
}

void TestCamerasView::test_calibCxChange()
{

}

void TestCamerasView::test_calibCyChange()
{

}

void TestCamerasView::test_calibFChange()
{

}

void TestCamerasView::test_calibK1Change()
{

}

void TestCamerasView::test_calibK2Change()
{

}

void TestCamerasView::test_calibK3Change()
{

}

void TestCamerasView::test_calibP1Change()
{

}

void TestCamerasView::test_calibP2Change()
{

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
