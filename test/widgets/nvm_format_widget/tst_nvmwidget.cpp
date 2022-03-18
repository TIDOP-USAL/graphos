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

#include "graphos/widgets/NvmFormatWidget.h"

#include <QLineEdit>

using namespace graphos;

class TestNvmFormatWidget
  : public NvmFormatWidgetImp
{
  Q_OBJECT

public:

  TestNvmFormatWidget();
  ~TestNvmFormatWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_file();
  void test_file_keyboard_input();
  void test_fileChange();
};

TestNvmFormatWidget::TestNvmFormatWidget()
{
}

TestNvmFormatWidget::~TestNvmFormatWidget()
{
}

void TestNvmFormatWidget::initTestCase()
{
  QCOMPARE(QString(""), this->file());
}

void TestNvmFormatWidget::cleanupTestCase()
{
  this->clear();

  QCOMPARE(QString(""), this->file());
}

void TestNvmFormatWidget::test_windowTitle()
{
  QCOMPARE("NVM", this->windowTitle());
}

void TestNvmFormatWidget::test_file()
{
  this->setFile("C:\\prj\\file.nvm");
  QCOMPARE("C:\\prj\\file.nvm", this->file());
}

void TestNvmFormatWidget::test_file_keyboard_input()
{
  this->mLineEditFile->clear();
  QSignalSpy spy_fileChanged(this, &TestNvmFormatWidget::fileChanged);

  QTest::keyClicks(this->mLineEditFile, "C:/prj/file.nvm");

  QCOMPARE(spy_fileChanged.count(), 15);

  QList<QVariant> args = spy_fileChanged.takeLast();
  QCOMPARE(args.at(0).toString(), "C:/prj/file.nvm");
}

void TestNvmFormatWidget::test_fileChange()
{
  this->mLineEditFile->clear();

  QSignalSpy spy_fileChanged(this, &TestNvmFormatWidget::fileChanged);

  this->mLineEditFile->setText("C:\\prj\\file.nvm");

  QCOMPARE(spy_fileChanged.count(), 1);
  QList<QVariant> args = spy_fileChanged.takeFirst();
  QCOMPARE(args.at(0).toString(), "C:\\prj\\file.nvm");

  this->setFile("C:\\prj\\file2.nvm");
  QCOMPARE(spy_fileChanged.count(), 0);
}


QTEST_MAIN(TestNvmFormatWidget)

#include "tst_nvmwidget.moc"
