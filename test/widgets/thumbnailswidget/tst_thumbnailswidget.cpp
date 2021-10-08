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

#include "graphos/widgets/ThumbnailsWidget.h"

#include <QAction>

using namespace graphos;

class TestThumbnailsWidget
  : public ThumbnailsWidget
{
  Q_OBJECT

public:
  TestThumbnailsWidget();
  ~TestThumbnailsWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_thumbnailChanged();
  void test_thumbnailSmallChanged();
  void test_detailsChanged();

};

TestThumbnailsWidget::TestThumbnailsWidget()
  : ThumbnailsWidget()
{
}

TestThumbnailsWidget::~TestThumbnailsWidget()
{
}

void TestThumbnailsWidget::initTestCase()
{

}

void TestThumbnailsWidget::cleanupTestCase()
{

}

void TestThumbnailsWidget::test_windowTitle()
{
  QCOMPARE("Thumbnails", this->windowTitle());
}

void TestThumbnailsWidget::test_thumbnailChanged()
{
  emit mThumbnailAction->changed();

  QCOMPARE(mThumbnailAction->isChecked(), true);
  QCOMPARE(mThumbnailSmallAction->isChecked(), false);
  QCOMPARE(mDetailsAction->isChecked(), false);
}

void TestThumbnailsWidget::test_thumbnailSmallChanged()
{
  emit mThumbnailSmallAction->changed();

  QCOMPARE(mThumbnailAction->isChecked(), false);
  QCOMPARE(mThumbnailSmallAction->isChecked(), true);
  QCOMPARE(mDetailsAction->isChecked(), false);
}

void TestThumbnailsWidget::test_detailsChanged()
{
  emit mDetailsAction->changed();

  QCOMPARE(mThumbnailAction->isChecked(), false);
  QCOMPARE(mThumbnailSmallAction->isChecked(), false);
  QCOMPARE(mDetailsAction->isChecked(), true);
}

QTEST_MAIN(TestThumbnailsWidget)

#include "tst_thumbnailswidget.moc"
