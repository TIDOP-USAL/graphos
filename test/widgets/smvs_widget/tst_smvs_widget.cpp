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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include <QtTest>
#include <QCoreApplication>

#include "graphos/widgets/SmvsWidget.h"

using namespace graphos;

class TestSmvsWidget
  : public QObject
{
  Q_OBJECT

public:

  TestSmvsWidget();
  ~TestSmvsWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_inputImageScale_data();
  void test_inputImageScale();
  void test_outputDepthScale_data();
  void test_outputDepthScale();
  void test_shadingBasedOptimization_data();
  void test_shadingBasedOptimization();
  void test_semiGlobalMatching_data();
  void test_semiGlobalMatching();
  void test_surfaceSmoothingFactor_data();
  void test_surfaceSmoothingFactor();

private:

  SmvsWidget *mSmvsWidget;

};

TestSmvsWidget::TestSmvsWidget()
{
  mSmvsWidget = new SmvsWidgetImp();
}

TestSmvsWidget::~TestSmvsWidget()
{
  if (mSmvsWidget){
    delete mSmvsWidget;
    mSmvsWidget = nullptr;
  }
}

void TestSmvsWidget::initTestCase()
{
  /// Check default values
  QCOMPARE(1, mSmvsWidget->inputImageScale());
  QCOMPARE(2, mSmvsWidget->outputDepthScale());
  QCOMPARE(true, mSmvsWidget->semiGlobalMatching());
  QCOMPARE(1., mSmvsWidget->surfaceSmoothingFactor());
  QCOMPARE(false, mSmvsWidget->shadingBasedOptimization());
}

void TestSmvsWidget::cleanupTestCase()
{
  mSmvsWidget->clear();

  QCOMPARE(1, mSmvsWidget->inputImageScale());
  QCOMPARE(2, mSmvsWidget->outputDepthScale());
  QCOMPARE(true, mSmvsWidget->semiGlobalMatching());
  QCOMPARE(1., mSmvsWidget->surfaceSmoothingFactor());
  QCOMPARE(false, mSmvsWidget->shadingBasedOptimization());
}

void TestSmvsWidget::test_windowTitle()
{
  QCOMPARE("Shading-Aware Multi-View Stereo", mSmvsWidget->windowTitle());
}

void TestSmvsWidget::test_inputImageScale_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("0") << 0 << 0;
  QTest::newRow("6") << 6 << 6;
  QTest::newRow("Out of range value") << 7 << 6;
}

void TestSmvsWidget::test_inputImageScale()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mSmvsWidget->setInputImageScale(value);
  QCOMPARE(result, mSmvsWidget->inputImageScale());
}

void TestSmvsWidget::test_outputDepthScale_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("0") << 0 << 0;
  QTest::newRow("1") << 1 << 1;
  QTest::newRow("2") << 2 << 2;
  QTest::newRow("4") << 4 << 4;
  QTest::newRow("6") << 6 << 6;
  QTest::newRow("Out of range value") << 7 << 6;
}

void TestSmvsWidget::test_outputDepthScale()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mSmvsWidget->setOutputDepthScale(value);
  QCOMPARE(result, mSmvsWidget->outputDepthScale());
}

void TestSmvsWidget::test_shadingBasedOptimization_data()
{
  QTest::addColumn<bool>("value");
  QTest::addColumn<bool>("result");

  QTest::newRow("true") << true << true;
  QTest::newRow("false") << false << false;
}

void TestSmvsWidget::test_shadingBasedOptimization()
{
  QFETCH(bool, value);
  QFETCH(bool, result);

  mSmvsWidget->setShadingBasedOptimization(value);
  QCOMPARE(result, mSmvsWidget->shadingBasedOptimization());
}

void TestSmvsWidget::test_semiGlobalMatching_data()
{
  QTest::addColumn<bool>("value");
  QTest::addColumn<bool>("result");

  QTest::newRow("true") << true << true;
  QTest::newRow("false") << false << false;
}

void TestSmvsWidget::test_semiGlobalMatching()
{
  QFETCH(bool, value);
  QFETCH(bool, result);

  mSmvsWidget->setSemiGlobalMatching(value);
  QCOMPARE(result, mSmvsWidget->semiGlobalMatching());
}

void TestSmvsWidget::test_surfaceSmoothingFactor_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.") << 0. << 0.;
  QTest::newRow("1.") << 1. << 1.;
  QTest::newRow("10.") << 10. << 10.;
  QTest::newRow("Out of range value") << 10.1 << 10.;
}

void TestSmvsWidget::test_surfaceSmoothingFactor()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mSmvsWidget->setSurfaceSmoothingFactor(value);
  QCOMPARE(result, mSmvsWidget->surfaceSmoothingFactor());
}


QTEST_MAIN(TestSmvsWidget)

#include "tst_smvs_widget.moc"
