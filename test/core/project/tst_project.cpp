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

#define BOOST_TEST_MODULE GRAPHOS Project test

#include <boost/test/unit_test.hpp>
#include "graphos/core/project.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(TestProjectSuite)

struct TestProject
{

  TestProject()
    : mProject(new ProjectImp),
      mProjectXml(new ProjectImp)
  {

  }

  ~TestProject()
  {
    if (mProject) {
      delete mProject;
      mProject = nullptr;
    }

    if (mProjectXml) {
      delete mProjectXml;
      mProjectXml = nullptr;
    }
  }

  void setup()
  {
    mProjectXml->load(QString(GRAPHOS_SOURCE_PATH).append("/test/data/project.xml"));
  }
  
  void teardown()
  {
  }

protected:

  Project *mProject;
  Project *mProjectXml;
};



BOOST_FIXTURE_TEST_CASE(DefaultConstructor, TestProject)
{
  BOOST_CHECK_EQUAL("", mProject->name().toStdString());
  BOOST_CHECK_EQUAL("", mProject->description().toStdString());
  BOOST_CHECK_EQUAL("", mProject->projectFolder().toStdString());
  BOOST_CHECK_EQUAL("", mProject->projectPath().toStdString());
  BOOST_CHECK_EQUAL("1.0", mProject->version().toStdString());
  BOOST_CHECK_EQUAL("", mProject->database().toStdString());
  BOOST_CHECK_EQUAL(0, mProject->imagesCount());
  const auto &images = mProject->images();
  BOOST_CHECK_EQUAL(0, images.size());
  const auto &cameras = mProject->cameras();
  BOOST_CHECK_EQUAL(0, cameras.size());
  BOOST_CHECK_EQUAL(0, mProject->camerasCount());
  BOOST_CHECK_EQUAL("", mProject->crs().toStdString());
  BOOST_CHECK_EQUAL("", mProject->denseModel().toStdString());
  auto densification = mProject->densification();
  BOOST_CHECK(!densification);
  auto featureExtractor = mProject->featureExtractor();
  BOOST_CHECK(!featureExtractor);
  auto featureMatching = mProject->featureMatching();
  BOOST_CHECK(!featureMatching);
  auto features = mProject->features();
  BOOST_CHECK_EQUAL(0, features.size());
  BOOST_CHECK_EQUAL("", mProject->offset().toStdString());
  BOOST_CHECK_EQUAL("", mProject->reconstructionPath().toStdString());
  BOOST_CHECK_EQUAL("", mProject->sparseModel().toStdString());

}

BOOST_FIXTURE_TEST_CASE(clear, TestProject)
{
  mProject->clear();

  BOOST_CHECK_EQUAL("", mProject->name().toStdString());
  BOOST_CHECK_EQUAL("", mProject->description().toStdString());
  BOOST_CHECK_EQUAL("", mProject->projectFolder().toStdString());
  BOOST_CHECK_EQUAL("", mProject->projectPath().toStdString());
  BOOST_CHECK_EQUAL("1.0", mProject->version().toStdString());
  BOOST_CHECK_EQUAL("", mProject->database().toStdString());
  BOOST_CHECK_EQUAL(0, mProject->imagesCount());
  const auto &images = mProject->images();
  BOOST_CHECK_EQUAL(0, images.size());
  const auto &cameras = mProject->cameras();
  BOOST_CHECK_EQUAL(0, cameras.size());
  BOOST_CHECK_EQUAL(0, mProject->camerasCount());
  BOOST_CHECK_EQUAL("", mProject->crs().toStdString());
  BOOST_CHECK_EQUAL("", mProject->denseModel().toStdString());
  auto densification = mProject->densification();
  BOOST_CHECK(!densification);
  auto featureExtractor = mProject->featureExtractor();
  BOOST_CHECK(!featureExtractor);
  auto featureMatching = mProject->featureMatching();
  BOOST_CHECK(!featureMatching);
  auto features = mProject->features();
  BOOST_CHECK_EQUAL(0, features.size());
  BOOST_CHECK_EQUAL("", mProject->offset().toStdString());
  BOOST_CHECK_EQUAL("", mProject->reconstructionPath().toStdString());
  BOOST_CHECK_EQUAL("", mProject->sparseModel().toStdString());
}

BOOST_FIXTURE_TEST_CASE(name, TestProject)
{
  mProject->setName("Project1");
  BOOST_CHECK_EQUAL("Project1", mProject->name().toStdString());
  BOOST_CHECK_EQUAL("San Segundo", mProjectXml->name().toStdString());
}

BOOST_FIXTURE_TEST_CASE(description, TestProject)
{
  mProject->setDescription("****");
  BOOST_CHECK_EQUAL("****", mProject->description().toStdString());
  BOOST_CHECK_EQUAL("Portada de San Segundo", mProjectXml->description().toStdString());
}

BOOST_FIXTURE_TEST_CASE(projectFolder, TestProject)
{
  mProject->setProjectFolder("C:/Users/temp");
  BOOST_CHECK_EQUAL("C:/Users/temp", mProject->projectFolder().toStdString());
  BOOST_CHECK_EQUAL("C:/Users/qwerty/Documents/graphos/Projects/San Segundo", mProjectXml->projectFolder().toStdString());
}

BOOST_FIXTURE_TEST_CASE(database, TestProject)
{
  BOOST_CHECK_EQUAL("C:/Users/qwerty/Documents/graphos/Projects/San Segundo/San Segundo.db", mProjectXml->database().toStdString());
}

BOOST_FIXTURE_TEST_CASE(crs, TestProject)
{
  BOOST_CHECK_EQUAL("", mProjectXml->crs().toStdString());
}

BOOST_FIXTURE_TEST_CASE(addImage, TestProject)
{
  Image image("C:/Users/temp/images/image01.jpg");
  mProject->addImage(image);
  BOOST_CHECK_EQUAL(1, mProject->images().size());
}

BOOST_FIXTURE_TEST_CASE(removeImage, TestProject)
{
  Image image("C:/Users/temp/images/image01.jpg");
  mProject->addImage(image);
  BOOST_CHECK_EQUAL(1, mProject->images().size());
  mProject->removeImage(image.id());
  BOOST_CHECK_EQUAL(0, mProject->images().size());
}

BOOST_FIXTURE_TEST_CASE(existImage, TestProject)
{
  BOOST_CHECK(mProjectXml->existImage(1904085996873346300));
  BOOST_CHECK(!mProjectXml->existImage(1));
}

BOOST_FIXTURE_TEST_CASE(findImageById, TestProject)
{
  Image image = mProjectXml->findImageById(1904085996873346300);
  BOOST_CHECK_EQUAL("IMG_7212.jpg", image.name().toStdString());
}

BOOST_FIXTURE_TEST_CASE(images, TestProject)
{
  BOOST_CHECK_EQUAL(4, mProjectXml->images().size());
}

BOOST_FIXTURE_TEST_CASE(imagesCount, TestProject)
{
  BOOST_CHECK_EQUAL(4, mProjectXml->imagesCount());
}

BOOST_FIXTURE_TEST_CASE(addCamera, TestProject)
{
  BOOST_CHECK_EQUAL(0, mProject->camerasCount());
  Camera camera("SONY", "ILCE-6000");
  camera.setFocal(3552.23);
  camera.setWidth(5472);
  camera.setHeight(3648);
  mProject->addCamera(camera);
  BOOST_CHECK_EQUAL(1, mProject->camerasCount());
}

BOOST_FIXTURE_TEST_CASE(removeCamera, TestProject)
{
  int id = mProject->cameraId("SONY", "ILCE-6000");
  mProject->removeCamera(id);
  BOOST_CHECK_EQUAL(0, mProject->camerasCount());
}

BOOST_FIXTURE_TEST_CASE(findCamera, TestProject)
{
  Camera camera = mProjectXml->findCamera("Unknown camera", "0");
  BOOST_CHECK_EQUAL(5835.6, camera.focal());
  BOOST_CHECK_EQUAL(4863, camera.width());
  BOOST_CHECK_EQUAL(3221, camera.height());
  BOOST_CHECK_EQUAL(1, camera.sensorSize());

  Camera camera2 = mProjectXml->findCamera(1);
  BOOST_CHECK_EQUAL(5835.6, camera2.focal());
  BOOST_CHECK_EQUAL(4863, camera2.width());
  BOOST_CHECK_EQUAL(3221, camera2.height());
  BOOST_CHECK_EQUAL(1, camera2.sensorSize());
}

BOOST_FIXTURE_TEST_CASE(cameraId, TestProject)
{
  int id = mProjectXml->cameraId("Unknown camera", "0");
  BOOST_CHECK_EQUAL(1, id);
}

BOOST_FIXTURE_TEST_CASE(existCamera, TestProject)
{
  BOOST_CHECK(!mProjectXml->existCamera("DJI", "FC6310"));
  BOOST_CHECK(mProjectXml->existCamera("Unknown camera", "0"));
}

BOOST_FIXTURE_TEST_CASE(featureExtractor, TestProject)
{
  std::shared_ptr<Feature> feature_extractor = mProjectXml->featureExtractor();
  BOOST_CHECK(Feature::Type::sift == feature_extractor->type());
}

BOOST_FIXTURE_TEST_CASE(features, TestProject)
{
  QString features = mProjectXml->features(16355064570535277777);
  BOOST_CHECK_EQUAL("IMG_7209@C:/Users/qwerty/Documents/graphos/Projects/San Segundo/San Segundo.db", features.toStdString());

  features = mProjectXml->features(1904085996873346300);
  BOOST_CHECK_EQUAL("IMG_7210@C:/Users/qwerty/Documents/graphos/Projects/San Segundo/San Segundo.db", features.toStdString());
}

BOOST_FIXTURE_TEST_CASE(featureMatching, TestProject)
{
  std::shared_ptr<FeatureMatching> feature_matching = mProjectXml->featureMatching();
  BOOST_CHECK(feature_matching);
}

BOOST_FIXTURE_TEST_CASE(photoOrientation, TestProject)
{
  CameraPose ori = mProjectXml->photoOrientation(16355064570535277777);
  BOOST_CHECK_CLOSE(5.3242274858, ori.position().x, 0.01);
  BOOST_CHECK_CLOSE(-0.1640853199, ori.position().y, 0.01);
  BOOST_CHECK_CLOSE(-0.4738843519, ori.position().z, 0.01);
}

BOOST_FIXTURE_TEST_CASE(sparseModel, TestProject)
{
  mProject->setSparseModel("C:/Users/esteban/Documents/inspector/Projects/SanSegundo/sparse/0/sparse.ply");
  BOOST_CHECK_EQUAL("C:/Users/esteban/Documents/inspector/Projects/SanSegundo/sparse/0/sparse.ply", mProject->sparseModel().toStdString());
}

BOOST_FIXTURE_TEST_CASE(densification, TestProject)
{
  std::shared_ptr<Densification> dense = mProjectXml->densification();
  BOOST_CHECK(Densification::Method::cmvs_pmvs == dense->method());
}

BOOST_FIXTURE_TEST_CASE(denseModel, TestProject)
{
  mProject->setDenseModel("C:/Users/esteban/Documents/inspector/Projects/SanSegundo/dense/pmvs/models/option-all.ply");
  BOOST_CHECK_EQUAL("C:/Users/esteban/Documents/inspector/Projects/SanSegundo/dense/pmvs/models/option-all.ply", mProject->denseModel().toStdString());
}

BOOST_AUTO_TEST_SUITE_END()