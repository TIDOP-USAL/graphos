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

#include "graphos/widgets/StartPageWidget.h"

#include <QCommandLinkButton>
#include <QListWidget>

using namespace graphos;

class TestStartPageWidget
  : public StartPageWidget
{
  Q_OBJECT

public:

  TestStartPageWidget();
  ~TestStartPageWidget();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_windowTitle();
  void test_openNew();
  void test_openProject();
  void test_openSettings();
  void test_clearHistory();
  void test_openProjectFromHistory();
  void test_recentProjects();

};

TestStartPageWidget::TestStartPageWidget()
  : StartPageWidget()
{
}

TestStartPageWidget::~TestStartPageWidget()
{
}

void TestStartPageWidget::initTestCase()
{

  QStringList history {
    QString("C:\\Users\\Tido\\Documents\\Inspector\\Projects\\230\\230.xml"),
    QString("C:\\Users\\Tido\\Documents\\Inspector\\Projects\\523\\523.xml"),
    QString("C:\\Users\\Tido\\Documents\\Inspector\\Projects\\165\\165.xml"),
    QString("C:\\Users\\Tido\\Documents\\Inspector\\Projects\\963\\963.xml")};

  this->setHistory(history);
}

void TestStartPageWidget::cleanupTestCase()
{

}

void TestStartPageWidget::test_windowTitle()
{
  QCOMPARE("Start Page", this->windowTitle());
}

void TestStartPageWidget::test_openNew()
{
//  QSignalSpy spy_openProject(this, &TestStartPageWidget::openProject);

//  QTest::mouseClick(this->mCommandLinkButtonNewProject, Qt::MouseButton::LeftButton,);

//  QCOMPARE(spy_openProject.count(), 1);
}

void TestStartPageWidget::test_openProject()
{
//  QSignalSpy spy_openSettings(this, &TestStartPageWidget::openSettings);

//  QTest::mouseClick(this->mCommandLinkButtonOpenProject, Qt::MouseButton::LeftButton);

//  QCOMPARE(spy_openSettings.count(), 1);
}

void TestStartPageWidget::test_openSettings()
{
//  QSignalSpy spy_openSettings(this, &TestStartPageWidget::openSettings);

//  QTest::mouseClick(this->mCommandLinkButtonSettings, Qt::MouseButton::LeftButton);

//  QCOMPARE(spy_openSettings.count(), 1);
}

void TestStartPageWidget::test_clearHistory()
{
//  QSignalSpy spy_clearHistory(this, &TestStartPageWidget::clearHistory);

//  QTest::mouseClick(this->mCommandLinkButtonClearHistory, Qt::MouseButton::LeftButton);

//  QCOMPARE(spy_clearHistory.count(), 1);
}

void TestStartPageWidget::test_openProjectFromHistory()
{

}

void TestStartPageWidget::test_recentProjects()
{
  QCOMPARE(4, mListWidgetRecentProjects->count());
}

QTEST_MAIN(TestStartPageWidget)

#include "tst_startpagewidget.moc"
