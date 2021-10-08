/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include <QtTest>
#include <QCoreApplication>

#include "graphos/widgets/SiftWidget.h"

using namespace graphos;

class TestSiftWidget 
  : public QObject
{
  Q_OBJECT

public:

  TestSiftWidget();
  ~TestSiftWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_featuresNumber_data();
  void test_featuresNumber();
  void test_octaveLayers_data();
  void test_octaveLayers();
  void test_contrastThreshold_data();
  void test_contrastThreshold();
  void test_edgeThreshold_data();
  void test_edgeThreshold();
  void test_sigma_data();
  void test_sigma();

private:

  SiftWidget *mSiftWidget;

};

TestSiftWidget::TestSiftWidget()
{
  mSiftWidget = new SiftWidgetImp();
}

TestSiftWidget::~TestSiftWidget()
{
  if (mSiftWidget){
    delete mSiftWidget;
    mSiftWidget = nullptr;
  }
}

void TestSiftWidget::initTestCase()
{
  /// Check default values
  QCOMPARE(5000, mSiftWidget->featuresNumber());
  QCOMPARE(3, mSiftWidget->octaveLayers());
  QCOMPARE(0.0067, mSiftWidget->contrastThreshold());
  QCOMPARE(10., mSiftWidget->edgeThreshold());
  QCOMPARE(1.6, mSiftWidget->sigma());
}

void TestSiftWidget::cleanupTestCase()
{
  mSiftWidget->setFeaturesNumber(500);
  mSiftWidget->setOctaveLayers(4);
  mSiftWidget->setContrastThreshold(0.5);
  mSiftWidget->setEdgeThreshold(20.);
  mSiftWidget->setSigma(3.);

  mSiftWidget->clear();

  QCOMPARE(5000, mSiftWidget->featuresNumber());
  QCOMPARE(3, mSiftWidget->octaveLayers());
  QCOMPARE(0.0067, mSiftWidget->contrastThreshold());
  QCOMPARE(10., mSiftWidget->edgeThreshold());
  QCOMPARE(1.6, mSiftWidget->sigma());
}

void TestSiftWidget::test_windowTitle()
{
  QCOMPARE("SIFT", mSiftWidget->windowTitle());
}

void TestSiftWidget::test_featuresNumber_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("500") << 500 << 500;
  QTest::newRow("10000") << 10000 << 10000;
  QTest::newRow("Out of range value") << 50001 << 50000;
}

void TestSiftWidget::test_featuresNumber()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mSiftWidget->setFeaturesNumber(value);
  QCOMPARE(result, mSiftWidget->featuresNumber());
}

void TestSiftWidget::test_octaveLayers_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("0") << 0 << 0;
  QTest::newRow("1") << 1 << 1;
  QTest::newRow("2") << 2 << 2;
  QTest::newRow("4") << 4 << 4;
  QTest::newRow("7") << 7 << 7;
  QTest::newRow("Out of range value") << 11 << 10;
}

void TestSiftWidget::test_octaveLayers()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mSiftWidget->setOctaveLayers(value);
  QCOMPARE(result, mSiftWidget->octaveLayers());
}

void TestSiftWidget::test_contrastThreshold_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.04") << 0.04 << 0.04;
  QTest::newRow("0.1") << 0.1 << 0.1;
  QTest::newRow("0.2") << 0.2 << 0.2;
  QTest::newRow("Out of range value") << 11.1 << 10.;
}

void TestSiftWidget::test_contrastThreshold()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mSiftWidget->setContrastThresholdAuto(false);
  mSiftWidget->setContrastThreshold(value);
  QCOMPARE(result, mSiftWidget->contrastThreshold());
}

void TestSiftWidget::test_edgeThreshold_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("10.") << 10. << 10.;
  QTest::newRow("1.") << 1. << 1.;
  QTest::newRow("20.") << 20. << 20.;
  QTest::newRow("Out of range value") << 100.1 << 100.;
}

void TestSiftWidget::test_edgeThreshold()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mSiftWidget->setEdgeThreshold(value);
  QCOMPARE(result, mSiftWidget->edgeThreshold());
}

void TestSiftWidget::test_sigma_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("10.") << 10. << 10.;
  QTest::newRow("1.") << 1. << 1.;
  QTest::newRow("20.") << 20. << 20.;
  QTest::newRow("Out of range value") << 100.1 << 100.;
}

void TestSiftWidget::test_sigma()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mSiftWidget->setSigma(value);
  QCOMPARE(result, mSiftWidget->sigma());
}


QTEST_MAIN(TestSiftWidget)

#include "tst_siftwidget.moc"
