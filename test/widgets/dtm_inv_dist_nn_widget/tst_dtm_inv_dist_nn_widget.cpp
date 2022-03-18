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

#include "graphos/widgets/DtmInvDistNNWidget.h"

using namespace graphos;

class TestDtmInvDistNNWidget
  : public QObject
{
  Q_OBJECT

public:

  TestDtmInvDistNNWidget();
  ~TestDtmInvDistNNWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_power_data();
  void test_power();
  void test_smoothing_data();
  void test_smoothing();
  void test_radius_data();
  void test_radius();
  void test_maxPoints_data();
  void test_maxPoints();
  void test_minPoints_data();
  void test_minPoints();

private:

  DtmInvDistNNWidget *mDtmInvDistNNWidget;

};

TestDtmInvDistNNWidget::TestDtmInvDistNNWidget()
{
  mDtmInvDistNNWidget = new DtmInvDistNNWidgetImp();
}

TestDtmInvDistNNWidget::~TestDtmInvDistNNWidget()
{
  if (mDtmInvDistNNWidget){
    delete mDtmInvDistNNWidget;
    mDtmInvDistNNWidget = nullptr;
  }
}

void TestDtmInvDistNNWidget::initTestCase()
{
  /// Check default values
  QCOMPARE(2.0, mDtmInvDistNNWidget->power());
  QCOMPARE(0.0, mDtmInvDistNNWidget->smoothing());
  QCOMPARE(1.0, mDtmInvDistNNWidget->radius());
  QCOMPARE(0, mDtmInvDistNNWidget->maxPoints());
  QCOMPARE(0, mDtmInvDistNNWidget->minPoints());
}

void TestDtmInvDistNNWidget::cleanupTestCase()
{
  mDtmInvDistNNWidget->clear();

  QCOMPARE(2.0, mDtmInvDistNNWidget->power());
  QCOMPARE(0.0, mDtmInvDistNNWidget->smoothing());
  QCOMPARE(1.0, mDtmInvDistNNWidget->radius());
  QCOMPARE(0, mDtmInvDistNNWidget->maxPoints());
  QCOMPARE(0, mDtmInvDistNNWidget->minPoints());
}

void TestDtmInvDistNNWidget::test_windowTitle()
{
  QCOMPARE("Inverse distance to a power with nearest neighbor searching", mDtmInvDistNNWidget->windowTitle());
}

void TestDtmInvDistNNWidget::test_power_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("3") << 3. << 3.;
  QTest::newRow("4") << 4. << 4.;
}

void TestDtmInvDistNNWidget::test_power()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDistNNWidget->setPower(value);
  QCOMPARE(result, mDtmInvDistNNWidget->power());
}

void TestDtmInvDistNNWidget::test_smoothing_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("1.0") << 1.0 << 1.0;
  QTest::newRow("0.5") << 0.5 << 0.5;
  QTest::newRow("2.0") << 2.0 << 2.0;
}

void TestDtmInvDistNNWidget::test_smoothing()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDistNNWidget->setSmoothing(value);
  QCOMPARE(result, mDtmInvDistNNWidget->smoothing());
}

void TestDtmInvDistNNWidget::test_radius_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("1.0") << 1.0 << 1.0;
  QTest::newRow("0.5") << 0.5 << 0.5;
  QTest::newRow("2.0") << 2.0 << 2.0;
}

void TestDtmInvDistNNWidget::test_radius()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDistNNWidget->setRadius(value);
  QCOMPARE(result, mDtmInvDistNNWidget->radius());
}

void TestDtmInvDistNNWidget::test_maxPoints_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("1") << 1 << 1;
  QTest::newRow("3") << 3 << 3;
  QTest::newRow("7") << 7 << 7;
}

void TestDtmInvDistNNWidget::test_maxPoints()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mDtmInvDistNNWidget->setMaxPoints(value);
  QCOMPARE(result, mDtmInvDistNNWidget->maxPoints());
}

void TestDtmInvDistNNWidget::test_minPoints_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("2") << 2 << 2;
  QTest::newRow("3") << 3 << 3;
  QTest::newRow("6") << 6 << 6;
}

void TestDtmInvDistNNWidget::test_minPoints()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mDtmInvDistNNWidget->setMinPoints(value);
  QCOMPARE(result, mDtmInvDistNNWidget->minPoints());
}

QTEST_MAIN(TestDtmInvDistNNWidget)

#include "tst_dtm_inv_dist_nn_widget.moc"
