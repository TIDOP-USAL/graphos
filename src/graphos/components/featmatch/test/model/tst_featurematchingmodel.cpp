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

#define BOOST_TEST_MODULE GRAPHOS feature matching model test

#include <boost/test/unit_test.hpp>

#include "graphos/components/featmatch/impl/FeatureMatchingModel.h"
#include "graphos/core/project.h"
#include "graphos/core/features/matching.h"
#include "../test/fake/ProjectFake.h"

using namespace graphos;
using namespace graphos;

BOOST_AUTO_TEST_SUITE(TestFeatureMatchingModelSuite)


struct TestFeatureMatchingModel
{

  TestFeatureMatchingModel()
    : project(new ProjectFake),
      mFeatureMatchingModel(new FeatureMatchingModelImp(project))
  {
  }

  ~TestFeatureMatchingModel(){}

  void setup() {}
  void teardown() {}

  Project *project;
  FeatureMatchingModel *mFeatureMatchingModel;
};

BOOST_FIXTURE_TEST_CASE(DefaultConstructor, TestFeatureMatchingModel)
{
  BOOST_CHECK_EQUAL(nullptr, mFeatureMatchingModel->featureMatching());
  BOOST_CHECK_EQUAL("", mFeatureMatchingModel->database().toString());
  BOOST_CHECK_EQUAL(false, mFeatureMatchingModel->spatialMatching());
}

BOOST_FIXTURE_TEST_CASE(feature_matching, TestFeatureMatchingModel)

{
  {
    std::shared_ptr<FeatureMatching> match_properties = std::make_shared<FeatureMatchingProperties>();
    mFeatureMatchingModel->setFeatureMatching(match_properties);
  }

  std::shared_ptr<FeatureMatching> match_properties = mFeatureMatchingModel->featureMatching();
  BOOST_CHECK_EQUAL(true, std::dynamic_pointer_cast<FeatureMatchingProperties>(match_properties)->crossCheck());
  BOOST_CHECK_EQUAL(0.8, std::dynamic_pointer_cast<FeatureMatchingProperties>(match_properties)->ratio());
}

BOOST_FIXTURE_TEST_CASE(database, TestFeatureMatchingModel)
{
  tl::Path database_path("C:\\Users\\User01\\Documents\\Graphos\\Projects\\proj01\\database.db");

  project->setDatabase(database_path);
  BOOST_CHECK_EQUAL(database_path, mFeatureMatchingModel->database());
}

BOOST_FIXTURE_TEST_CASE(spatial_matching, TestFeatureMatchingModel)
{

  Image image1("C:/Users/User01/Documents/Graphos/Projects/images/img001.png");
  CameraPose camera_pose1(5.,5.,5., tl::math::Quaternion<double>::identity());
  image1.setCameraPose(camera_pose1);
  Image image2("C:/Users/User01/Documents/Graphos/Projects/images/img002.png");
  CameraPose camera_pose2(10.,12.,5., tl::math::Quaternion<double>::identity());
  image2.setCameraPose(camera_pose2);

  project->addImage(image1);
  project->addImage(image2);

  BOOST_CHECK_EQUAL(true, mFeatureMatchingModel->spatialMatching());

}


BOOST_AUTO_TEST_SUITE_END()
