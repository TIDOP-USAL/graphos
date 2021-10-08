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

#include "graphos/widgets/DtmInvDistWidget.h"

using namespace graphos;

class TestDtmInvDistWidget
  : public QObject
{
  Q_OBJECT

public:

  TestDtmInvDistWidget();
  ~TestDtmInvDistWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_power_data();
  void test_power();
  void test_smoothing_data();
  void test_smoothing();
  void test_radius1_data();
  void test_radius1();
  void test_radius2_data();
  void test_radius2();
  void test_angle_data();
  void test_angle();
  void test_maxPoints_data();
  void test_maxPoints();
  void test_minPoints_data();
  void test_minPoints();

private:

  DtmInvDistWidget *mDtmInvDistWidget;

};

TestDtmInvDistWidget::TestDtmInvDistWidget()
{
  mDtmInvDistWidget = new DtmInvDistWidgetImp();
}

TestDtmInvDistWidget::~TestDtmInvDistWidget()
{
  if (mDtmInvDistWidget){
    delete mDtmInvDistWidget;
    mDtmInvDistWidget = nullptr;
  }
}

void TestDtmInvDistWidget::initTestCase()
{
  /// Check default values
  QCOMPARE(2.0, mDtmInvDistWidget->power());
  QCOMPARE(0.0, mDtmInvDistWidget->smoothing());
  QCOMPARE(0.0, mDtmInvDistWidget->radius1());
  QCOMPARE(0.0, mDtmInvDistWidget->radius2());
  QCOMPARE(0.0, mDtmInvDistWidget->angle());
  QCOMPARE(0, mDtmInvDistWidget->maxPoints());
  QCOMPARE(0, mDtmInvDistWidget->minPoints());
}

void TestDtmInvDistWidget::cleanupTestCase()
{
  mDtmInvDistWidget->clear();

  QCOMPARE(2.0, mDtmInvDistWidget->power());
  QCOMPARE(0.0, mDtmInvDistWidget->smoothing());
  QCOMPARE(0.0, mDtmInvDistWidget->radius1());
  QCOMPARE(0.0, mDtmInvDistWidget->radius2());
  QCOMPARE(0.0, mDtmInvDistWidget->angle());
  QCOMPARE(0, mDtmInvDistWidget->maxPoints());
  QCOMPARE(0, mDtmInvDistWidget->minPoints());
}

void TestDtmInvDistWidget::test_windowTitle()
{
  QCOMPARE("Inverse distance to a power", mDtmInvDistWidget->windowTitle());
}

void TestDtmInvDistWidget::test_power_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("3") << 3. << 3.;
  QTest::newRow("4") << 4. << 4.;
}

void TestDtmInvDistWidget::test_power()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDistWidget->setPower(value);
  QCOMPARE(result, mDtmInvDistWidget->power());
}

void TestDtmInvDistWidget::test_smoothing_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("1.0") << 1.0 << 1.0;
  QTest::newRow("0.5") << 0.5 << 0.5;
  QTest::newRow("2.0") << 2.0 << 2.0;
}

void TestDtmInvDistWidget::test_smoothing()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDistWidget->setSmoothing(value);
  QCOMPARE(result, mDtmInvDistWidget->smoothing());
}

void TestDtmInvDistWidget::test_radius1_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("1.0") << 1.0 << 1.0;
  QTest::newRow("0.5") << 0.5 << 0.5;
  QTest::newRow("2.0") << 2.0 << 2.0;
}

void TestDtmInvDistWidget::test_radius1()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDistWidget->setRadius1(value);
  QCOMPARE(result, mDtmInvDistWidget->radius1());
}

void TestDtmInvDistWidget::test_radius2_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("1.0") << 1.0 << 1.0;
  QTest::newRow("0.5") << 0.5 << 0.5;
  QTest::newRow("2.0") << 2.0 << 2.0;
}

void TestDtmInvDistWidget::test_radius2()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDistWidget->setRadius2(value);
  QCOMPARE(result, mDtmInvDistWidget->radius2());
}

void TestDtmInvDistWidget::test_angle_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");

  QTest::newRow("1.0") << 1.0 << 1.0;
  QTest::newRow("0.5") << 0.5 << 0.5;
  QTest::newRow("2.0") << 2.0 << 2.0;
}

void TestDtmInvDistWidget::test_angle()
{
  QFETCH(double, value);
  QFETCH(double, result);

  mDtmInvDistWidget->setAngle(value);
  QCOMPARE(result, mDtmInvDistWidget->angle());
}

void TestDtmInvDistWidget::test_maxPoints_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("1") << 1 << 1;
  QTest::newRow("3") << 3 << 3;
  QTest::newRow("7") << 7 << 7;
}

void TestDtmInvDistWidget::test_maxPoints()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mDtmInvDistWidget->setMaxPoints(value);
  QCOMPARE(result, mDtmInvDistWidget->maxPoints());
}

void TestDtmInvDistWidget::test_minPoints_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("2") << 2 << 2;
  QTest::newRow("3") << 3 << 3;
  QTest::newRow("6") << 6 << 6;
}

void TestDtmInvDistWidget::test_minPoints()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mDtmInvDistWidget->setMinPoints(value);
  QCOMPARE(result, mDtmInvDistWidget->minPoints());
}

QTEST_MAIN(TestDtmInvDistWidget)

#include "tst_dtm_inv_dist_widget.moc"
