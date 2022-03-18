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

#include "graphos/widgets/FeatureMatchingWidget.h"

using namespace graphos;

class TestFeatureMatchingWidget
  : public QObject
{
  Q_OBJECT

public:

  TestFeatureMatchingWidget();
  ~TestFeatureMatchingWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_crossCheck();
  void test_ratio_data();
  void test_ratio();
  void test_distance_data();
  void test_distance();
  void test_maxError_data();
  void test_maxError();
  void test_confidence_data();
  void test_confidence();

private:

  FeatureMatchingWidget *mFeatureMatchingWidget;

};

TestFeatureMatchingWidget::TestFeatureMatchingWidget()
{
  mFeatureMatchingWidget = new FeatureMatchingWidgetImp();
}

TestFeatureMatchingWidget::~TestFeatureMatchingWidget()
{
  if (mFeatureMatchingWidget){
    delete mFeatureMatchingWidget;
    mFeatureMatchingWidget = nullptr;
  }
}

void TestFeatureMatchingWidget::initTestCase()
{
  /// Check default values
  QCOMPARE(true, mFeatureMatchingWidget->crossCheck());
  QCOMPARE(0.8, mFeatureMatchingWidget->ratio());
  QCOMPARE(0.7, mFeatureMatchingWidget->distance());
  QCOMPARE(4.0, mFeatureMatchingWidget->maxError());
  QCOMPARE(0.999, mFeatureMatchingWidget->confidence());
}

void TestFeatureMatchingWidget::cleanupTestCase()
{
  mFeatureMatchingWidget->clear();

  QCOMPARE(true, mFeatureMatchingWidget->crossCheck());
  QCOMPARE(0.8, mFeatureMatchingWidget->ratio());
  QCOMPARE(0.7, mFeatureMatchingWidget->distance());
  QCOMPARE(4.0, mFeatureMatchingWidget->maxError());
  QCOMPARE(0.999, mFeatureMatchingWidget->confidence());
}

void TestFeatureMatchingWidget::test_windowTitle()
{
  QCOMPARE("Feature Matching Colmap", mFeatureMatchingWidget->windowTitle());
}

void TestFeatureMatchingWidget::test_crossCheck()
{
  mFeatureMatchingWidget->enableCrossCheck(false);
  QCOMPARE(false, mFeatureMatchingWidget->crossCheck());
  mFeatureMatchingWidget->enableCrossCheck(true);
  QCOMPARE(true, mFeatureMatchingWidget->crossCheck());
}

void TestFeatureMatchingWidget::test_ratio_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.7") << 0.7 << 0.7;
  QTest::newRow("0.8") << 0.8 << 0.8;
  QTest::newRow("0.9") << 0.9 << 0.9;
}

void TestFeatureMatchingWidget::test_ratio()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mFeatureMatchingWidget->setRatio(value);
  QCOMPARE(result, mFeatureMatchingWidget->ratio());
}

void TestFeatureMatchingWidget::test_distance_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.9") << 0.9 << 0.9;
  QTest::newRow("1.5") << 1.5 << 1.5;
  QTest::newRow("0.7") << 0.7 << 0.7;
}

void TestFeatureMatchingWidget::test_distance()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mFeatureMatchingWidget->setDistance(value);
  QCOMPARE(result, mFeatureMatchingWidget->distance());
}

void TestFeatureMatchingWidget::test_maxError_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("2.0") << 2.0 << 2.0;
  QTest::newRow("3.0") << 3.0 << 3.0;
  QTest::newRow("4.0") << 4.0 << 4.0;
}

void TestFeatureMatchingWidget::test_maxError()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mFeatureMatchingWidget->setMaxError(value);
  QCOMPARE(result, mFeatureMatchingWidget->maxError());
}

void TestFeatureMatchingWidget::test_confidence_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("0.9") << 0.9 << 0.9;
  QTest::newRow("0.995") << 0.995 << 0.995;
  QTest::newRow("0.999") << 0.999 << 0.999;
}

void TestFeatureMatchingWidget::test_confidence()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mFeatureMatchingWidget->setConfidence(value);
  QCOMPARE(result, mFeatureMatchingWidget->confidence());
}

QTEST_MAIN(TestFeatureMatchingWidget)

#include "tst_featurematchingwidget.moc"
