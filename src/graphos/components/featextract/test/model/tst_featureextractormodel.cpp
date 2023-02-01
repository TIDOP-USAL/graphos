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

#define BOOST_TEST_MODULE GRAPHOS feature extractor model test

#include <boost/test/unit_test.hpp>

#include "graphos/components/featextract/impl/FeatureExtractorModel.h"
#include "graphos/core/project.h"
#include "graphos/core/features/sift.h"
#include "../test/fake/ProjectFake.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(TestFeatureExtractorModelSuite)


struct TestFeatureExtractorModel
{

  TestFeatureExtractorModel()
    : project(new ProjectFake),
      mFeatureExtractorModel(new FeatureExtractorModelImp(project))
  {}

  ~TestFeatureExtractorModel() {}

  void setup() {}
  void teardown() {}

  Project *project;
  FeatureExtractorModel *mFeatureExtractorModel;
};


BOOST_FIXTURE_TEST_CASE(DefaultConstructor, TestFeatureExtractorModel)
{
  BOOST_CHECK_EQUAL(nullptr, mFeatureExtractorModel->featureExtractor());
  BOOST_CHECK_EQUAL("", mFeatureExtractorModel->database().toString());
  auto &images = mFeatureExtractorModel->images();
  BOOST_CHECK_EQUAL(0, images.size());
  auto &cameras = mFeatureExtractorModel->cameras();
  BOOST_CHECK_EQUAL(true, cameras.begin() == cameras.end());
}

BOOST_FIXTURE_TEST_CASE(feature_extractor, TestFeatureExtractorModel)
{
  {
    std::shared_ptr<Feature> feature = std::make_shared<SiftProperties>();
    mFeatureExtractorModel->setFeatureExtractor(feature);
  }

  std::shared_ptr<Feature> feature = mFeatureExtractorModel->featureExtractor();
  BOOST_CHECK_EQUAL(5000, std::dynamic_pointer_cast<SiftProperties>(feature)->featuresNumber());
  BOOST_CHECK_EQUAL(3, std::dynamic_pointer_cast<SiftProperties>(feature)->octaveLayers());
}

BOOST_FIXTURE_TEST_CASE(database, TestFeatureExtractorModel)
{
  tl::Path database_path("C:\\Users\\User01\\Documents\\Graphos\\Projects\\proj01\\database.db");

  project->setDatabase(database_path);
  BOOST_CHECK_EQUAL(database_path, mFeatureExtractorModel->database());
}

BOOST_FIXTURE_TEST_CASE(features, TestFeatureExtractorModel)
{
  Image image("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png");
  mFeatureExtractorModel->addFeatures(image.id(), "C:/features/image001.xml");
  BOOST_CHECK_EQUAL("C:/features/image001.xml", project->features(image.id()).toStdString());
}

BOOST_FIXTURE_TEST_CASE(images, TestFeatureExtractorModel)
{
  Image image1("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png");
  Image image2("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img002.png");
  project->addImage(image1);
  project->addImage(image2);

  auto &images = mFeatureExtractorModel->images();
  BOOST_CHECK_EQUAL(2, images.size());
  BOOST_CHECK_EQUAL("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png", images.at(image1.id()).path().toStdString());
  BOOST_CHECK_EQUAL("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img002.png", images.at(image2.id()).path().toStdString());
}

BOOST_FIXTURE_TEST_CASE(cameras, TestFeatureExtractorModel)
{
  Camera camera("DJI", "FC6310");
  project->addCamera(camera);

  auto &cameras = mFeatureExtractorModel->cameras();
  auto it_camera = cameras.begin();
  BOOST_CHECK_EQUAL(1, it_camera->first);
  BOOST_CHECK_EQUAL("DJI", it_camera->second.make());
  BOOST_CHECK_EQUAL("FC6310", it_camera->second.model());
}

BOOST_AUTO_TEST_SUITE_END()