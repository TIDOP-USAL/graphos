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

#define BOOST_TEST_MODULE GRAPHOS create project model test

#include <boost/test/unit_test.hpp>

#include "graphos/components/createproject/impl/CreateProjectModel.h"
#include "graphos/core/project.h"
#include "../test/fake/ProjectFake.h"


using namespace graphos;

BOOST_AUTO_TEST_SUITE(TestCreateProjectModelSuite)


struct TestCreateProjectModel
{

    TestCreateProjectModel()
        : project(new ProjectFake),
          projectModel(new CreateProjectModelImp(project))
    {

    }

    ~TestCreateProjectModel()
    {
        if (project) {
            delete project;
            project = nullptr;
        }

        if (projectModel) {
            delete projectModel;
            projectModel = nullptr;
        }
    }

    void setup() {}
    void teardown() {}

    Project *project;
    CreateProjectModel *projectModel;
};

BOOST_FIXTURE_TEST_CASE(DefaultConstructor, TestCreateProjectModel)
{
    BOOST_CHECK_EQUAL("", project->name().toStdString());
    BOOST_CHECK_EQUAL("", project->projectPath().toString());
    BOOST_CHECK_EQUAL("", project->description().toStdString());
    BOOST_CHECK_EQUAL("", project->projectFolder().toString());
}

BOOST_FIXTURE_TEST_CASE(clear, TestCreateProjectModel)
{
    projectModel->setProjectName("test");
    projectModel->setProjectFolder(tl::Path("C:/Users/Tido/Documents/graphos/Projects"));
    projectModel->setProjectDescription("project description");

    projectModel->clear();

    BOOST_CHECK_EQUAL("", project->name().toStdString());
    BOOST_CHECK_EQUAL("", project->projectPath().toString());
    BOOST_CHECK_EQUAL("", project->description().toStdString());
    BOOST_CHECK_EQUAL("", project->projectFolder().toString());
}


BOOST_FIXTURE_TEST_CASE(project_name, TestCreateProjectModel)
{
    projectModel->setProjectName("Project 1");
    BOOST_CHECK_EQUAL("Project 1", project->name().toStdString());

    projectModel->setProjectName("Project 2");
    BOOST_CHECK_EQUAL("Project 2", project->name().toStdString());
}

BOOST_FIXTURE_TEST_CASE(project_description, TestCreateProjectModel)
{
    projectModel->setProjectDescription("Project 1 description");
    BOOST_CHECK_EQUAL("Project 1 description", project->description().toStdString());

    projectModel->setProjectDescription("Project 2 description");
    BOOST_CHECK_EQUAL("Project 2 description", project->description().toStdString());
}

BOOST_FIXTURE_TEST_CASE(project_folder, TestCreateProjectModel)
{
    projectModel->setProjectFolder(tl::Path("C:/Users/Tido/Documents/graphos/Projects"));
    BOOST_CHECK_EQUAL(tl::Path("C:/Users/Tido/Documents/graphos/Projects"), project->projectFolder());

    projectModel->setProjectFolder(tl::Path("C:/Users/user1/Documents/graphos/Projects"));
    BOOST_CHECK_EQUAL(tl::Path("C:/Users/user1/Documents/graphos/Projects"), project->projectFolder());
}

BOOST_FIXTURE_TEST_CASE(project_path, TestCreateProjectModel)
{
    projectModel->setProjectName("Project 1");
    projectModel->setProjectFolder(tl::Path("C:/Users/Tido/Documents/graphos/Projects/Project 1"));
    projectModel->save();
    tl::Path path1("C:/Users/Tido/Documents/graphos/Projects/Project 1/Project 1.xml");
    path1.normalize();
    BOOST_CHECK_EQUAL(path1, project->projectPath());

    projectModel->setProjectName("Project 2");
    projectModel->setProjectFolder(tl::Path("C:/Users/user1/Documents/graphos/Projects/Project 2"));
    projectModel->save();
    tl::Path path2("C:/Users/user1/Documents/graphos/Projects/Project 2/Project 2.xml");
    path2.normalize();
    BOOST_CHECK_EQUAL(path2, project->projectPath());
}

BOOST_AUTO_TEST_SUITE_END()