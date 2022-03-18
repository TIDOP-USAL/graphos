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

#include <QtTest>
#include <QCoreApplication>

#include "graphos/widgets/CmvsPmvsWidget.h"

using namespace graphos;

class TestCmvsPmvsWidget
  : public QObject
{
  Q_OBJECT

public:

  TestCmvsPmvsWidget();
  ~TestCmvsPmvsWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_useVisibilityInformation_data();
  void test_useVisibilityInformation();
  void test_imagesPerCluster_data();
  void test_imagesPerCluster();
  void test_level_data();
  void test_level();
  void test_cellSize_data();
  void test_cellSize();
  void test_threshold_data();
  void test_threshold();
  void test_windowSize_data();
  void test_windowSize();
  void test_minimunImageNumber_data();
  void test_minimunImageNumber();

private:

  CmvsPmvsWidget *mCmvsPmvsWidget;

};

TestCmvsPmvsWidget::TestCmvsPmvsWidget()
{
  mCmvsPmvsWidget = new CmvsPmvsWidgetImp();
}

TestCmvsPmvsWidget::~TestCmvsPmvsWidget()
{
  if (mCmvsPmvsWidget){
    delete mCmvsPmvsWidget;
    mCmvsPmvsWidget = nullptr;
  }
}

void TestCmvsPmvsWidget::initTestCase()
{
  /// Check default values
  QCOMPARE(true, mCmvsPmvsWidget->useVisibilityInformation());
  QCOMPARE(100, mCmvsPmvsWidget->imagesPerCluster());
  QCOMPARE(1, mCmvsPmvsWidget->level());
  QCOMPARE(2, mCmvsPmvsWidget->cellSize());
  QCOMPARE(0.7, mCmvsPmvsWidget->threshold());
  QCOMPARE(7, mCmvsPmvsWidget->windowSize());
  QCOMPARE(3, mCmvsPmvsWidget->minimunImageNumber());
}

void TestCmvsPmvsWidget::cleanupTestCase()
{
  mCmvsPmvsWidget->clear();

  QCOMPARE(true, mCmvsPmvsWidget->useVisibilityInformation());
  QCOMPARE(100, mCmvsPmvsWidget->imagesPerCluster());
  QCOMPARE(1, mCmvsPmvsWidget->level());
  QCOMPARE(2, mCmvsPmvsWidget->cellSize());
  QCOMPARE(0.7, mCmvsPmvsWidget->threshold());
  QCOMPARE(7, mCmvsPmvsWidget->windowSize());
  QCOMPARE(3, mCmvsPmvsWidget->minimunImageNumber());
}

void TestCmvsPmvsWidget::test_windowTitle()
{
  QCOMPARE("CMVS/PMVS", mCmvsPmvsWidget->windowTitle());
}


void TestCmvsPmvsWidget::test_useVisibilityInformation_data()
{
  QTest::addColumn<bool>("value");
  QTest::addColumn<bool>("result");

  QTest::newRow("false") << false << false;
  QTest::newRow("true") << true << true;
}

void TestCmvsPmvsWidget::test_useVisibilityInformation()
{
  QFETCH(bool, value);
  QFETCH(bool, result);

  mCmvsPmvsWidget->setUseVisibilityInformation(value);
  QCOMPARE(result, mCmvsPmvsWidget->useVisibilityInformation());
}


void TestCmvsPmvsWidget::test_imagesPerCluster_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("1") << 1 << 1;
  QTest::newRow("50") << 50 << 50;
  QTest::newRow("100") << 100 << 100;
  QTest::newRow("1000") << 1000 << 1000;
  QTest::newRow("Out of range value") << 1001 << 1000;
}

void TestCmvsPmvsWidget::test_imagesPerCluster()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCmvsPmvsWidget->setImagesPerCluster(value);
  QCOMPARE(result, mCmvsPmvsWidget->imagesPerCluster());
}

void TestCmvsPmvsWidget::test_level_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("0") << 0 << 0;
  QTest::newRow("3") << 3 << 3;
  QTest::newRow("6") << 6 << 6;
  QTest::newRow("Out of range value") << 7 << 6;
}

void TestCmvsPmvsWidget::test_level()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCmvsPmvsWidget->setLevel(value);
  QCOMPARE(result, mCmvsPmvsWidget->level());
}

void TestCmvsPmvsWidget::test_cellSize_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("1") << 1 << 1;
  QTest::newRow("3") << 3 << 3;
  QTest::newRow("7") << 7 << 7;
  QTest::newRow("Out of range value") << 8 << 7;
}

void TestCmvsPmvsWidget::test_cellSize()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCmvsPmvsWidget->setCellSize(value);
  QCOMPARE(result, mCmvsPmvsWidget->cellSize());
}

void TestCmvsPmvsWidget::test_threshold_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.") << 0. << 0.;
  QTest::newRow("0.6") << 0.6 << 0.6;
  QTest::newRow("1.") << 1. << 1.;
  QTest::newRow("Out of range value") << 1.1 <<  1.;
}

void TestCmvsPmvsWidget::test_threshold()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mCmvsPmvsWidget->setThreshold(value);
  QCOMPARE(result, mCmvsPmvsWidget->threshold());
}

void TestCmvsPmvsWidget::test_windowSize_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("5") << 5 << 5;
  QTest::newRow("10") << 10 << 10;
  QTest::newRow("20") << 20 << 20;
  QTest::newRow("Out of range value") << 21 << 20;
}

void TestCmvsPmvsWidget::test_windowSize()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCmvsPmvsWidget->setWindowSize(value);
  QCOMPARE(result, mCmvsPmvsWidget->windowSize());
}

void TestCmvsPmvsWidget::test_minimunImageNumber_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("2") << 2 << 2;
  QTest::newRow("3") << 3 << 3;
  QTest::newRow("6") << 6 << 6;
  QTest::newRow("Out of range value") << 7 << 6;
}

void TestCmvsPmvsWidget::test_minimunImageNumber()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mCmvsPmvsWidget->setMinimunImageNumber(value);
  QCOMPARE(result, mCmvsPmvsWidget->minimunImageNumber());
}


QTEST_MAIN(TestCmvsPmvsWidget)

#include "tst_cmvs_pmvs_widget.moc"
