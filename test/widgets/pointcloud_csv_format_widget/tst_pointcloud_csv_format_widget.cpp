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

#include "graphos/widgets/PointCloudCSVFormatWidget.h"

#include <QRadioButton>

using namespace graphos;

class TestPointCloudCSVFormatWidget
  : public PointCloudCSVFormatWidgetImp
{
  Q_OBJECT

public:

  TestPointCloudCSVFormatWidget();
  ~TestPointCloudCSVFormatWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_delimiter();
  void test_delimiterChanged();
};

TestPointCloudCSVFormatWidget::TestPointCloudCSVFormatWidget()
{
}

TestPointCloudCSVFormatWidget::~TestPointCloudCSVFormatWidget()
{
}

void TestPointCloudCSVFormatWidget::initTestCase()
{
  QCOMPARE(QString(";"), this->delimiter());
}

void TestPointCloudCSVFormatWidget::cleanupTestCase()
{
  this->clear();

  QCOMPARE(QString(";"), this->delimiter());
}

void TestPointCloudCSVFormatWidget::test_windowTitle()
{
  QCOMPARE("Point Cloud CSV Format", this->windowTitle());
}

void TestPointCloudCSVFormatWidget::test_delimiter()
{
  this->setDelimiter("\t");
  QCOMPARE("\t", this->delimiter());
}

void TestPointCloudCSVFormatWidget::test_delimiterChanged()
{

}


QTEST_MAIN(TestPointCloudCSVFormatWidget)

#include "tst_pointcloud_csv_format_widget.moc"
