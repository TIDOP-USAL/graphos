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

#include "graphos/widgets/MveFormatWidget.h"

#include <QLineEdit>

using namespace graphos;

class TestMveFormatWidget
  : public MveFormatWidgetImp
{
  Q_OBJECT

public:

  TestMveFormatWidget();
  ~TestMveFormatWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_file();
  void test_file_keyboard_input();
  void test_fileChange();
};

TestMveFormatWidget::TestMveFormatWidget()
{
}

TestMveFormatWidget::~TestMveFormatWidget()
{
}

void TestMveFormatWidget::initTestCase()
{
  QCOMPARE(QString(""), this->file());
}

void TestMveFormatWidget::cleanupTestCase()
{
  this->clear();

  QCOMPARE(QString(""), this->file());
}

void TestMveFormatWidget::test_windowTitle()
{
  QCOMPARE("MVE", this->windowTitle());
}

void TestMveFormatWidget::test_file()
{
  this->setFile("C:\\prj\\dir");
  QCOMPARE("C:\\prj\\dir", this->file());
}

void TestMveFormatWidget::test_file_keyboard_input()
{
  this->mLineEditFile->clear();
  QSignalSpy spy_fileChanged(this, &TestMveFormatWidget::fileChanged);

  QTest::keyClicks(this->mLineEditFile, "C:/prj/dir");

  QCOMPARE(spy_fileChanged.count(), 10);

  QList<QVariant> args = spy_fileChanged.takeLast();
  QCOMPARE(args.at(0).toString(), "C:/prj/dir");
}

void TestMveFormatWidget::test_fileChange()
{
  this->mLineEditFile->clear();

  QSignalSpy spy_fileChanged(this, &TestMveFormatWidget::fileChanged);

  this->mLineEditFile->setText("C:\\prj\\dir");

  QCOMPARE(spy_fileChanged.count(), 1);
  QList<QVariant> args = spy_fileChanged.takeFirst();
  QCOMPARE(args.at(0).toString(), "C:\\prj\\dir");

  this->setFile("C:\\prj\\dir2");
  QCOMPARE(spy_fileChanged.count(), 0);
}


QTEST_MAIN(TestMveFormatWidget)

#include "tst_mvewidget.moc"
