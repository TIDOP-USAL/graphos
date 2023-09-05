/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#define BOOST_TEST_MODULE GRAPHOS cameras view test

#include <boost/test/unit_test.hpp>

#include "graphos/components/cameras/impl/CamerasView.h"

#include <QtTest>
#include <QApplication>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QComboBox>

using namespace graphos;

BOOST_AUTO_TEST_SUITE(TestCamerasViewSuite)

QApplication app(boost::unit_test::framework::master_test_suite().argc,
                 boost::unit_test::framework::master_test_suite().argv);

class TestCamerasView
    : public CamerasViewImp
{

public:

    TestCamerasView() : CamerasViewImp() {}
    ~TestCamerasView() {}

    void setup() {}
    void teardown() {}

};



BOOST_FIXTURE_TEST_CASE(add_camera, TestCamerasView)
{

    this->addCamera(2, "Make-Model");

    QListWidgetItem *item = nullptr;
    for (int i = 0; i < mListWidgetCameras->count(); i++) {
        if (mListWidgetCameras->item(i)->data(Qt::UserRole).toInt() == 2) {
            item = mListWidgetCameras->item(i);
        }
    }

    BOOST_CHECK_EQUAL("Make-Model", item->text().toStdString());
    BOOST_CHECK_EQUAL(2, item->data(Qt::UserRole).toInt());
}

BOOST_FIXTURE_TEST_CASE(enable_camera_edition, TestCamerasView)
{
    this->enableCameraEdition(true);

    BOOST_CHECK_EQUAL(true, this->mLineEditMake->isEnabled());
    BOOST_CHECK_EQUAL(true, this->mLineEditModel->isEnabled());
    BOOST_CHECK_EQUAL(true, this->mLineEditSensorSize->isEnabled());
    BOOST_CHECK_EQUAL(true, this->mLineEditFocal->isEnabled());
}

BOOST_FIXTURE_TEST_CASE(set_make, TestCamerasView)
{
  this->setMake("DJI");
  BOOST_CHECK_EQUAL("DJI", this->mLineEditMake->text().toStdString());
}

BOOST_FIXTURE_TEST_CASE(test_setModel, TestCamerasView)
{
  this->setModel("FC6310");
  BOOST_CHECK_EQUAL("FC6310", this->mLineEditModel->text().toStdString());
}

BOOST_FIXTURE_TEST_CASE(test_setWidth, TestCamerasView)
{
  this->setWidth(5472);
  BOOST_CHECK_EQUAL(5472, this->mSpinBoxWidth->value());
}

BOOST_FIXTURE_TEST_CASE(test_setHeight, TestCamerasView)
{
  this->setHeight(3648);
  BOOST_CHECK_EQUAL(3648, this->mSpinBoxHeight->value());
}

BOOST_FIXTURE_TEST_CASE(test_setSensorSize, TestCamerasView)
{
  this->setSensorSize("12.8333");
  BOOST_CHECK_EQUAL("12.8333", this->mLineEditSensorSize->text().toStdString());
}

BOOST_FIXTURE_TEST_CASE(test_setFocal, TestCamerasView)
{
  this->setFocal("3552.23");
  BOOST_CHECK_EQUAL("3552.23", this->mLineEditFocal->text().toStdString());
}

BOOST_FIXTURE_TEST_CASE(test_setType, TestCamerasView)
{
  this->setType("Radial 2");
  BOOST_CHECK_EQUAL("Radial 2", this->mComboBoxType->currentText().toStdString());
}

BOOST_FIXTURE_TEST_CASE(test_setImages, TestCamerasView)
{
  QStringList images;
  images.push_back("C:/Users/User01/Documents/inspector/Projects/prj001/images/img001.png");
  images.push_back("C:/Users/User01/Documents/inspector/Projects/prj001/images/img002.png");
  this->setImages(images);

  BOOST_CHECK_EQUAL("C:/Users/User01/Documents/inspector/Projects/prj001/images/img001.png", this->mListWidgetImages->item(0)->text().toStdString());
  BOOST_CHECK_EQUAL("C:/Users/User01/Documents/inspector/Projects/prj001/images/img002.png", this->mListWidgetImages->item(1)->text().toStdString());
}

BOOST_FIXTURE_TEST_CASE(test_setCalibCx, TestCamerasView)
{
  this->setCalibCx(0.6);
  BOOST_CHECK_EQUAL(0.6, this->mDoubleSpinBoxCx->value());
}

BOOST_FIXTURE_TEST_CASE(test_setCalibCy, TestCamerasView)
{
  this->setCalibCy(0.5);
  BOOST_CHECK_EQUAL(0.5, this->mDoubleSpinBoxCy->value());
}

BOOST_FIXTURE_TEST_CASE(test_setCalibF, TestCamerasView)
{
  this->setCalibF(4000);
  BOOST_CHECK_EQUAL(4000, this->mDoubleSpinBoxF->value());
}

BOOST_FIXTURE_TEST_CASE(test_setCalibFx, TestCamerasView)
{
  this->setCalibFx(4000);
  BOOST_CHECK_EQUAL(4000, this->mDoubleSpinBoxFx->value());
}

BOOST_FIXTURE_TEST_CASE(test_setCalibFy, TestCamerasView)
{
  this->setCalibFy(4000);
  BOOST_CHECK_EQUAL(4000, this->mDoubleSpinBoxFy->value());
}

BOOST_FIXTURE_TEST_CASE(test_setCalibK1, TestCamerasView)
{
  this->setCalibK1(0.005);
  BOOST_CHECK_EQUAL(0.005, this->mDoubleSpinBoxK1->value());
}

BOOST_FIXTURE_TEST_CASE(test_setCalibK2, TestCamerasView)
{
  this->setCalibK2(0.001);
  BOOST_CHECK_EQUAL(0.001, this->mDoubleSpinBoxK2->value());
}

BOOST_FIXTURE_TEST_CASE(test_setCalibK3, TestCamerasView)
{
  this->setCalibK3(0.0001);
  BOOST_CHECK_EQUAL(0.0001, this->mDoubleSpinBoxK3->value());
}

BOOST_FIXTURE_TEST_CASE(test_setCalibK4, TestCamerasView)
{
  this->setCalibK4(0.0001);
  BOOST_CHECK_EQUAL(0.0001, this->mDoubleSpinBoxK4->value());
}

BOOST_FIXTURE_TEST_CASE(test_setCalibK5, TestCamerasView)
{
  this->setCalibK5(0.0001);
  BOOST_CHECK_EQUAL(0.0001, this->mDoubleSpinBoxK5->value());
}

BOOST_FIXTURE_TEST_CASE(test_setCalibK6, TestCamerasView)
{
  this->setCalibK6(0.0001);
  BOOST_CHECK_EQUAL(0.0001, this->mDoubleSpinBoxK6->value());
}

BOOST_FIXTURE_TEST_CASE(test_setCalibP1, TestCamerasView)
{
  this->setCalibP1(0.01);
  BOOST_CHECK_EQUAL(0.01, this->mDoubleSpinBoxP1->value());
}

BOOST_FIXTURE_TEST_CASE(test_setCalibP2, TestCamerasView)
{
  this->setCalibP2(0.001);
  BOOST_CHECK_EQUAL(0.001, this->mDoubleSpinBoxP2->value());
}

BOOST_FIXTURE_TEST_CASE(test_cameraChange, TestCamerasView)
{
  QSignalSpy spy_cameraChange(this, &TestCamerasView::cameraChange);

}

//BOOST_FIXTURE_TEST_CASE(test_makeChange, TestCamerasView)
//{
//  QSignalSpy spy_makeChange(this, &TestCamerasView::makeChanged);
//
//  this->mLineEditMake->clear();
//  QTest::keyClicks(this->mLineEditMake, "Canon");
//
//  BOOST_CHECK_EQUAL(spy_makeChange.count(), 5);
//
//  QList<QVariant> args = spy_makeChange.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "Canon");
//
//  spy_makeChange.clear();
//
//  this->setMake("Olympus");
//  BOOST_CHECK_EQUAL(spy_makeChange.count(), 0);
//}

//BOOST_FIXTURE_TEST_CASE(test_modelChange, TestCamerasView)
//{
//  QSignalSpy spy_modelChange(this, &TestCamerasView::modelChanged);
//
//  this->mLineEditModel->clear();
//  QTest::keyClicks(this->mLineEditModel, "ILCE-6000");
//
//  BOOST_CHECK_EQUAL(spy_modelChange.count(), 9);
//
//  QList<QVariant> args = spy_modelChange.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "ILCE-6000");
//
//  spy_modelChange.clear();
//
//  this->setModel("FC6310");
//  BOOST_CHECK_EQUAL(spy_modelChange.count(), 0);
//}

//BOOST_FIXTURE_TEST_CASE(test_sensorSizeChange, TestCamerasView)
//{
//  QSignalSpy spy_sensorSizeChange(this, &TestCamerasView::sensorSizeChange);
//
//  this->mLineEditSensorSize->clear();
//  QTest::keyClicks(this->mLineEditSensorSize, "12.8333");
//
//  BOOST_CHECK_EQUAL(spy_sensorSizeChange.count(), 7);
//
//  QList<QVariant> args = spy_sensorSizeChange.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "12.8333");
//
//  spy_sensorSizeChange.clear();
//
//  this->setSensorSize("1.");
//  BOOST_CHECK_EQUAL(spy_sensorSizeChange.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_focalChange, TestCamerasView)
//{
//  QSignalSpy spy_focalChange(this, &TestCamerasView::focalChange);
//
//  this->mLineEditFocal->clear();
//  QTest::keyClicks(this->mLineEditFocal, "4500");
//
//  BOOST_CHECK_EQUAL(spy_focalChange.count(), 4);
//
//  QList<QVariant> args = spy_focalChange.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "4500");
//
//  spy_focalChange.clear();
//
//  this->setFocal("1.");
//  BOOST_CHECK_EQUAL(spy_focalChange.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_typeChange, TestCamerasView)
//{
//  QSignalSpy spy_typeChange(this, &TestCamerasView::typeChange);
//
//  this->mComboBoxType->setCurrentText("Full Radial");
//
//  BOOST_CHECK_EQUAL(spy_typeChange.count(), 1);
//
//  QList<QVariant> args = spy_typeChange.takeFirst();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "Full Radial");
//
//  this->setType("Simple Radial");
//  BOOST_CHECK_EQUAL(spy_typeChange.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_calibCxChange, TestCamerasView)
//{
//  QSignalSpy spy_calibCxChange(this, &TestCamerasView::calibCxChange);
//
//  this->mDoubleSpinBoxCx->clear();
//  QTest::keyClicks(this->mDoubleSpinBoxCx, "2250");
//
//  BOOST_CHECK_EQUAL(spy_calibCxChange.count(), 4);
//
//  QList<QVariant> args = spy_calibCxChange.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "2250");
//
//  spy_calibCxChange.clear();
//
//  this->setCalibCx(1.);
//  BOOST_CHECK_EQUAL(spy_calibCxChange.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_calibCyChange, TestCamerasView)
//{
//  QSignalSpy spy_calibCyChange(this, &TestCamerasView::calibCyChange);
//
//  this->mDoubleSpinBoxCy->clear();
//  QTest::keyClicks(this->mDoubleSpinBoxCy, "1800");
//
//  BOOST_CHECK_EQUAL(spy_calibCyChange.count(), 4);
//
//  QList<QVariant> args = spy_calibCyChange.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "1800");
//
//  spy_calibCyChange.clear();
//
//  this->setCalibCy(1.);
//  BOOST_CHECK_EQUAL(spy_calibCyChange.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_calibFChange, TestCamerasView)
//{
//  QSignalSpy spy_calibFChange(this, &TestCamerasView::calibFChange);
//
//  this->mDoubleSpinBoxF->clear();
//  QTest::keyClicks(this->mDoubleSpinBoxF, "2000");
//
//  BOOST_CHECK_EQUAL(spy_calibFChange.count(), 4);
//
//  QList<QVariant> args = spy_calibFChange.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "2000");
//
//  spy_calibFChange.clear();
//
//  this->setCalibF(1.);
//  BOOST_CHECK_EQUAL(spy_calibFChange.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_calibFxChange, TestCamerasView)
//{
//  QSignalSpy spy_calibFxChange(this, &TestCamerasView::calibFxChange);
//
//  this->mDoubleSpinBoxFx->clear();
//  QTest::keyClicks(this->mDoubleSpinBoxFx, "2000");
//
//  BOOST_CHECK_EQUAL(spy_calibFxChange.count(), 4);
//
//  QList<QVariant> args = spy_calibFxChange.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "2000");
//
//  spy_calibFxChange.clear();
//
//  this->setCalibFx(1.);
//  BOOST_CHECK_EQUAL(spy_calibFxChange.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_calibFyChange, TestCamerasView)
//{
//  QSignalSpy spy_calibFyChange(this, &TestCamerasView::calibFyChange);
//
//  this->mDoubleSpinBoxFy->clear();
//  QTest::keyClicks(this->mDoubleSpinBoxFy, "2000");
//
//  BOOST_CHECK_EQUAL(spy_calibFyChange.count(), 4);
//
//  QList<QVariant> args = spy_calibFyChange.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "2000");
//
//  spy_calibFyChange.clear();
//
//  this->setCalibFy(1.);
//  BOOST_CHECK_EQUAL(spy_calibFyChange.count(), 0);
//}
//
//
//BOOST_FIXTURE_TEST_CASE(test_calibK1Change, TestCamerasView)
//{
//  QSignalSpy spy_calibK1Change(this, &TestCamerasView::calibK1Change);
//
//  this->mDoubleSpinBoxK1->clear();
//  QTest::keyClicks(this->mDoubleSpinBoxK1, "0.1");
//
//  BOOST_CHECK_EQUAL(spy_calibK1Change.count(), 3);
//
//  QList<QVariant> args = spy_calibK1Change.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "0.1");
//
//  spy_calibK1Change.clear();
//
//  this->setCalibK1(0.5);
//  BOOST_CHECK_EQUAL(spy_calibK1Change.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_calibK2Change, TestCamerasView)
//{
//  QSignalSpy spy_calibK2Change(this, &TestCamerasView::calibK2Change);
//
//  this->mDoubleSpinBoxK2->setEnabled(true);
//  this->mDoubleSpinBoxK2->clear();
//  QTest::keyClicks(this->mDoubleSpinBoxK2, "0.01");
//
//  BOOST_CHECK_EQUAL(spy_calibK2Change.count(), 4);
//
//  QList<QVariant> args = spy_calibK2Change.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "0.01");
//
//  spy_calibK2Change.clear();
//
//  this->setCalibK2(0.05);
//  BOOST_CHECK_EQUAL(spy_calibK2Change.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_calibK3Change, TestCamerasView)
//{
//  QSignalSpy spy_calibK3Change(this, &TestCamerasView::calibK3Change);
//
//  this->mDoubleSpinBoxK3->setEnabled(true);
//  this->mDoubleSpinBoxK3->clear();
//  QTest::keyClicks(this->mDoubleSpinBoxK3, "0.001");
//
//  BOOST_CHECK_EQUAL(spy_calibK3Change.count(), 5);
//
//  QList<QVariant> args = spy_calibK3Change.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "0.001");
//
//  spy_calibK3Change.clear();
//
//  this->setCalibK3(0.05);
//  BOOST_CHECK_EQUAL(spy_calibK3Change.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_calibK4Change, TestCamerasView)
//{
//  QSignalSpy spy_calibK4Change(this, &TestCamerasView::calibK4Change);
//
//  this->mDoubleSpinBoxK4->setEnabled(true);
//  this->mDoubleSpinBoxK4->clear();
//  QTest::keyClicks(this->mDoubleSpinBoxK4, "0.001");
//
//  BOOST_CHECK_EQUAL(spy_calibK4Change.count(), 5);
//
//  QList<QVariant> args = spy_calibK4Change.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "0.001");
//
//  spy_calibK4Change.clear();
//
//  this->setCalibK4(0.005);
//  BOOST_CHECK_EQUAL(spy_calibK4Change.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_calibK5Change, TestCamerasView)
//{
//  QSignalSpy spy_calibK5Change(this, &TestCamerasView::calibK5Change);
//
//  this->mDoubleSpinBoxK5->setEnabled(true);
//  this->mDoubleSpinBoxK5->clear();
//  QTest::keyClicks(this->mDoubleSpinBoxK5, "0.0001");
//
//  BOOST_CHECK_EQUAL(spy_calibK5Change.count(), 6);
//
//  QList<QVariant> args = spy_calibK5Change.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "0.0001");
//
//  spy_calibK5Change.clear();
//
//  this->setCalibK5(0.005);
//  BOOST_CHECK_EQUAL(spy_calibK5Change.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_calibK6Change, TestCamerasView)
//{
//  QSignalSpy spy_calibK6Change(this, &TestCamerasView::calibK6Change);
//
//  this->mDoubleSpinBoxK6->setEnabled(true);
//  this->mDoubleSpinBoxK6->clear();
//  QTest::keyClicks(this->mDoubleSpinBoxK6, "0.0001");
//
//  BOOST_CHECK_EQUAL(spy_calibK6Change.count(), 6);
//
//  QList<QVariant> args = spy_calibK6Change.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "0.0001");
//
//  spy_calibK6Change.clear();
//
//  this->setCalibK6(0.0005);
//  BOOST_CHECK_EQUAL(spy_calibK6Change.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_calibP1Change, TestCamerasView)
//{
//  QSignalSpy spy_calibP1Change(this, &TestCamerasView::calibP1Change);
//
//  this->mDoubleSpinBoxP1->setEnabled(true);
//  this->mDoubleSpinBoxP1->clear();
//  QTest::keyClicks(this->mDoubleSpinBoxP1, "0.01");
//
//  BOOST_CHECK_EQUAL(spy_calibP1Change.count(), 4);
//
//  QList<QVariant> args = spy_calibP1Change.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "0.01");
//
//  spy_calibP1Change.clear();
//
//  this->setCalibP1(0.5);
//  BOOST_CHECK_EQUAL(spy_calibP1Change.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_calibP2Change, TestCamerasView)
//{
//  QSignalSpy spy_calibP2Change(this, &TestCamerasView::calibP2Change);
//
//  this->mDoubleSpinBoxP2->setEnabled(true);
//  this->mDoubleSpinBoxP2->clear();
//  QTest::keyClicks(this->mDoubleSpinBoxP2, "0.001");
//
//  BOOST_CHECK_EQUAL(spy_calibP2Change.count(), 5);
//
//  QList<QVariant> args = spy_calibP2Change.takeLast();
//  BOOST_CHECK_EQUAL(args.at(0).toString().toStdString(), "0.001");
//
//  spy_calibP2Change.clear();
//
//  this->setCalibP2(0.05);
//  BOOST_CHECK_EQUAL(spy_calibP2Change.count(), 0);
//}
//
//BOOST_FIXTURE_TEST_CASE(test_dialogButtonBox, TestCamerasView)
//{
//
////  QSignalSpy spy_rejected(this, &TestCamerasView::rejected);
////  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Cancel), Qt::LeftButton);
////  BOOST_CHECK_EQUAL(spy_rejected.count(), 1);
//
////  mButtonBox->button(QDialogButtonBox::Save)->setEnabled(true);
////  QSignalSpy spy_accepted(this, &TestCamerasView::accepted);
////  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Save), Qt::LeftButton);
////  BOOST_CHECK_EQUAL(spy_accepted.count(), 1);
//
////  QSignalSpy spy_help(this, &TestCamerasView::help);
////  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Help), Qt::LeftButton);
////  BOOST_CHECK_EQUAL(spy_help.count(), 1);
//}



BOOST_AUTO_TEST_SUITE_END()