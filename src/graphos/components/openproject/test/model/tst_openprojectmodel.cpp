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

#define BOOST_TEST_MODULE GRAPHOS open project model test

#include <boost/test/unit_test.hpp>

#include "graphos/components/openproject/impl/OpenProjectModel.h"
#include "graphos/core/project.h"
#include "../test/fake/ProjectFake.h"

#include <QStandardPaths>
#include <QApplication>

using namespace graphos;

BOOST_AUTO_TEST_SUITE(TestOpenProjectModelSuite)



struct TestOpenProjectModel
{

  TestOpenProjectModel()
    : project(new ProjectFake),
      mProjectModel(new OpenProjectModelImp(project))
  {}

  ~TestOpenProjectModel(){}

  void setup() 
  {
    qApp->setApplicationName("GRAPHOS");
  }

  void teardown() {}

  Project *project;
  OpenProjectModel *mProjectModel;
};


BOOST_FIXTURE_TEST_CASE(init, TestOpenProjectModel)
{
  QString graphos_directory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  graphos_directory.append("/GRAPHOS/Projects");

  BOOST_CHECK(mProjectModel->graphosProjectsDirectory().equivalent(graphos_directory.toStdString()));
  //BOOST_CHECK_EQUAL(graphos_directory.toStdString(), mProjectModel->graphosProjectsDirectory().toString());
}


BOOST_AUTO_TEST_SUITE_END()
