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

#define BOOST_TEST_MODULE GRAPHOS open project view test

#include <boost/test/unit_test.hpp>
#include <QtTest>
#include <QCoreApplication>

#include "graphos/components/openproject/impl/OpenProjectView.h"

//#include <QLineEdit>
//#include <QCheckBox>
//#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>

using namespace graphos;

BOOST_AUTO_TEST_SUITE(TestOpenProjectViewSuite)

QApplication app(boost::unit_test::framework::master_test_suite().argc,
                 boost::unit_test::framework::master_test_suite().argv);


class TestOpenProjectView
  : public OpenProjectViewImp
{

public:

  TestOpenProjectView();
  ~TestOpenProjectView();

  void setup()
  {
  }

  void teardown() {}

};

TestOpenProjectView::TestOpenProjectView()
  : OpenProjectViewImp()
{
}

TestOpenProjectView::~TestOpenProjectView()
{
}

BOOST_FIXTURE_TEST_CASE(default_constructor, TestOpenProjectView)
{
  QString name_filter = qApp->applicationName().append(" project (*.xml)");

  BOOST_CHECK_EQUAL("Open project", windowTitle().toStdString());
  BOOST_CHECK_EQUAL(name_filter.toStdString(), nameFilters().at(0).toStdString());
  BOOST_CHECK_EQUAL(QFileDialog::ExistingFile, fileMode());
}

BOOST_FIXTURE_TEST_CASE(_clear, TestOpenProjectView)
{
}

//
//void TestOpenProjectView::TestOpenProjectView()
//{
//  QTest::addColumn<QString>("value");
//  QTest::addColumn<QString>("result");
//
//  QTest::newRow("C:/Users/Tido/Documents/photomatch/Projects") << "C:/Users/Tido/Documents/photomatch/Projects" << "C:/Users/Tido/Documents/photomatch/Projects";
//  QTest::newRow("C:/Users/user1/Documents/photomatch/Projects") << "C:/Users/user1/Documents/photomatch/Projects" << "C:/Users/user1/Documents/photomatch/Projects";
//}
//
//void TestOpenProjectView::test_setGraphosProjectsPath()
//{
//  QFETCH(QString, value);
//  QFETCH(QString, result);
//
//  this->setGraphosProjectsPath(value);
//  QCOMPARE(result, this->directory().absolutePath());
//}

BOOST_AUTO_TEST_SUITE_END()