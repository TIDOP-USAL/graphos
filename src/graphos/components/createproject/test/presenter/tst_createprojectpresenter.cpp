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

#define BOOST_TEST_MODULE GRAPHOS create project presenter test

#include <boost/test/unit_test.hpp>

#include "graphos/components/createproject/impl/CreateProjectPresenter.h"
#include "graphos/components/createproject/impl/CreateProjectModel.h"
#include "graphos/components/createproject/impl/CreateProjectView.h"
#include "graphos/core/Application.h"
#include "graphos/core/project.h"
#include "../test/fake/ProjectFake.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(TestCreateProjectViewSuite)



class TestCreateProjectPresenter
  //: public QObject
{
  //Q_OBJECT

public:

  TestCreateProjectPresenter()
    : project(new ProjectFake),
      createProjectModel(new CreateProjectModelImp(project)),
      createProjectViewImp(new CreateProjectViewImp()),
      createProjectPresenter(nullptr)
  {

  }

  ~TestCreateProjectPresenter(){}

  void setup() 
  {
    AppStatus *status = Application::instance().status();
    createProjectPresenter = new CreateProjectPresenterImp(createProjectViewImp, 
                                                           createProjectModel,
                                                           status);
  }

  void teardown() {}

private:

  Project *project;
  CreateProjectModel *createProjectModel;
  CreateProjectView *createProjectViewImp;
  CreateProjectPresenter *createProjectPresenter;
};



BOOST_AUTO_TEST_SUITE_END()