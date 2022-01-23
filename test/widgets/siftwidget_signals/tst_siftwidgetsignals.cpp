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

#include "graphos/widgets/SiftWidget.h"

#include <QSpinBox>

using namespace graphos;

class TestSiftWidgetSignals
  : public SiftWidgetImp
{
  Q_OBJECT

public:

  TestSiftWidgetSignals();
  ~TestSiftWidgetSignals();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_featuresNumberChange();
  void test_octaveLayersChange();
  void test_contrastThresholdChange();
  void test_edgeThresholdChange();

};

TestSiftWidgetSignals::TestSiftWidgetSignals()
  : SiftWidgetImp()
{

}

TestSiftWidgetSignals::~TestSiftWidgetSignals()
{

}

void TestSiftWidgetSignals::initTestCase()
{

}

void TestSiftWidgetSignals::cleanupTestCase()
{
  QSignalSpy spyFeaturesNumber(this, &SiftWidgetImp::featuresNumberChange);
  QSignalSpy spyOctaveLayers(this, &SiftWidgetImp::octaveLayersChange);
  QSignalSpy spyContrastThreshold(this, &SiftWidgetImp::contrastThresholdChange);
  QSignalSpy spyEdgeThreshold(this, &SiftWidgetImp::edgeThresholdChange);

  this->setFeaturesNumber(5000);
  this->setOctaveLayers(6);
  this->setContrastThreshold(0.6);
  this->setEdgeThreshold(15.);

  this->clear();

  QCOMPARE(spyFeaturesNumber.count(), 0);
  QCOMPARE(spyOctaveLayers.count(), 0);
  QCOMPARE(spyContrastThreshold.count(), 0);
  QCOMPARE(spyEdgeThreshold.count(), 0);
}

void TestSiftWidgetSignals::test_featuresNumberChange()
{
  QSignalSpy spyFeaturesNumber(this, &SiftWidgetImp::featuresNumberChange);

  this->mFeaturesNumber->setValue(1500);

  QCOMPARE(spyFeaturesNumber.count(), 1);

  QList<QVariant> args = spyFeaturesNumber.takeFirst();
  QCOMPARE(args.at(0).toInt(), 1500);

  this->setFeaturesNumber(5000);
  QCOMPARE(spyFeaturesNumber.count(), 0);
}

void TestSiftWidgetSignals::test_octaveLayersChange()
{
  QSignalSpy spyOctaveLayers(this, &SiftWidgetImp::octaveLayersChange);

  this->mOctaveLayers->setValue(5);

  QCOMPARE(spyOctaveLayers.count(), 1);

  QList<QVariant> args = spyOctaveLayers.takeFirst();
  QCOMPARE(args.at(0).toInt(), 5);

  this->setOctaveLayers(6);
  QCOMPARE(spyOctaveLayers.count(), 0);
}

void TestSiftWidgetSignals::test_contrastThresholdChange()
{
  QSignalSpy spyContrastThreshold(this, &SiftWidgetImp::contrastThresholdChange);

  this->mContrastThreshold->setValue(0.5);

  QCOMPARE(spyContrastThreshold.count(), 1);

  QList<QVariant> args = spyContrastThreshold.takeFirst();
  QCOMPARE(args.at(0).toDouble(), 0.5);

  this->setContrastThreshold(0.6);
  QCOMPARE(spyContrastThreshold.count(), 0);
}

void TestSiftWidgetSignals::test_edgeThresholdChange()
{
  QSignalSpy spyEdgeThreshold(this, &SiftWidgetImp::edgeThresholdChange);

  this->mEdgeThreshold->setValue(20.);

  QCOMPARE(spyEdgeThreshold.count(), 1);

  QList<QVariant> args = spyEdgeThreshold.takeFirst();
  QCOMPARE(args.at(0).toDouble(), 20.);

  this->setEdgeThreshold(15.);
  QCOMPARE(spyEdgeThreshold.count(), 0);
}


QTEST_MAIN(TestSiftWidgetSignals)

#include "tst_siftwidgetsignals.moc"
