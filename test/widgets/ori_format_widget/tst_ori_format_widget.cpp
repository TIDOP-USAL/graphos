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

#include "graphos/widgets/OriTxtFormatWidget.h"

#include <QLineEdit>
#include <QComboBox>

using namespace graphos;

class TestOriFormatWidget
  : public OriTxtFormatWidgetImp
{
  Q_OBJECT

public:

  TestOriFormatWidget();
  ~TestOriFormatWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_file();
  void test_file_keyboard_input();
  void test_fileChange();
  void test_rotation();
  void test_rotation_keyboard_input();
  void test_rotationChanged();
};

TestOriFormatWidget::TestOriFormatWidget()
{
}

TestOriFormatWidget::~TestOriFormatWidget()
{
}

void TestOriFormatWidget::initTestCase()
{
  QCOMPARE(QString(""), this->file());
  QCOMPARE(QString("Omega Phi Kappa"), this->rotation());
}

void TestOriFormatWidget::cleanupTestCase()
{
  this->clear();

  QCOMPARE(QString(""), this->file());
  QCOMPARE(QString("Omega Phi Kappa"), this->rotation());
}

void TestOriFormatWidget::test_windowTitle()
{
  QCOMPARE("Ori Format", this->windowTitle());
}

void TestOriFormatWidget::test_file()
{
  this->setFile("C:\\prj\\file.txt");
  QCOMPARE("C:\\prj\\file.txt", this->file());
}

void TestOriFormatWidget::test_file_keyboard_input()
{
  this->mLineEditFile->clear();
  QSignalSpy spy_fileChanged(this, &TestOriFormatWidget::fileChanged);

  QTest::keyClicks(this->mLineEditFile, "C:/prj/file.txt");

  QCOMPARE(spy_fileChanged.count(), 15);

  QList<QVariant> args = spy_fileChanged.takeLast();
  QCOMPARE(args.at(0).toString(), "C:/prj/file.txt");
}

void TestOriFormatWidget::test_fileChange()
{
  this->mLineEditFile->clear();

  QSignalSpy spy_fileChanged(this, &TestOriFormatWidget::fileChanged);

  this->mLineEditFile->setText("C:\\prj\\file.txt");

  QCOMPARE(spy_fileChanged.count(), 1);
  QList<QVariant> args = spy_fileChanged.takeFirst();
  QCOMPARE(args.at(0).toString(), "C:\\prj\\file.txt");

  this->setFile("C:\\prj\\file2.txt");
  QCOMPARE(spy_fileChanged.count(), 0);
}

void TestOriFormatWidget::test_rotation()
{
  this->setRotation("Quaternions");
  QCOMPARE("Quaternions", this->rotation());
}

void TestOriFormatWidget::test_rotation_keyboard_input()
{
  this->mLineEditFile->clear();
  QSignalSpy spy_fileChanged(this, &TestOriFormatWidget::fileChanged);

  QTest::keyClicks(this->mLineEditFile, "Quaternions");

  QCOMPARE(spy_fileChanged.count(), 11);

  QList<QVariant> args = spy_fileChanged.takeLast();
  QCOMPARE(args.at(0).toString(), "Quaternions");
}

void TestOriFormatWidget::test_rotationChanged()
{
  this->clear();

  QSignalSpy spy_fileChanged(this, &TestOriFormatWidget::fileChanged);

  this->mComboBoxRotation->setCurrentText("Quaternions");

  QCOMPARE(spy_fileChanged.count(), 1);
  QList<QVariant> args = spy_fileChanged.takeFirst();
  QCOMPARE(args.at(0).toString(), "Quaternions");

  this->setRotation("Omega Phi Kappa");
  QCOMPARE(spy_fileChanged.count(), 0);
}

QTEST_MAIN(TestOriFormatWidget)

#include "tst_ori_format_widget.moc"
