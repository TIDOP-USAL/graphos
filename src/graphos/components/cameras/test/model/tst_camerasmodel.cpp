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

#define BOOST_TEST_MODULE GRAPHOS cameras model test

#include <boost/test/unit_test.hpp>

#include "graphos/core/project.h"
#include "graphos/components/cameras/impl/CamerasModel.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(TestCamerasModelSuite)

struct TestCamerasModel
{
    TestCamerasModel()
        : project(new ProjectImp)
    {

        project->load(tl::Path(GRAPHOS_SOURCE_PATH).append("/test/data/project.xml"));

        camerasModel = new CamerasModelImp(project);
    }

    ~TestCamerasModel()
    {
        if (project) {
            delete project;
            project = nullptr;
        }

        if (camerasModel) {
            delete camerasModel;
            camerasModel = nullptr;
        }
    }

    void setup() {}
    void teardown() {}

protected:

    Project *project;
    CamerasModel *camerasModel;
};


BOOST_FIXTURE_TEST_CASE(addCamera, TestCamerasModel)
{
  Camera camera("DJI", "FC6310");
  camera.setType("Simple radial");
  camera.setFocal(3552.23);
  camera.setWidth(5472);
  camera.setHeight(3648);
  camera.setSensorSize(12.8333);
  int id = camerasModel->addCamera(camera);
  BOOST_CHECK_EQUAL(2, id);
}

BOOST_FIXTURE_TEST_CASE(clear, TestCamerasModel)
{
  camerasModel->clear();
  
  BOOST_CHECK_EQUAL(false, camerasModel->modified());
}

// Hay que tener la base de datos de un proyecto...
//BOOST_FIXTURE_TEST_CASE(updateCamera, TestCamerasModel)
//{
//  Camera camera = camerasModel->camera(1);
//  camera.setType("Full radial");
//  bool bUpdate = camerasModel->updateCamera(1, camera);
//
//  BOOST_CHECK_EQUAL(true, bUpdate);
//
//  camera = camerasModel->camera(1);
//
//  BOOST_CHECK_EQUAL("Full radial", camera.type());
//
//  /// Se recupera el estado anterior
//  camera.setType("Radial");
//  camerasModel->updateCamera(1, camera);
//}

BOOST_FIXTURE_TEST_CASE(deleteCamera, TestCamerasModel)
{
  Camera camera("SONY", "ILCE-6000");
  int id = camerasModel->addCamera(camera);

  bool remove = camerasModel->removeCamera(4);
  BOOST_CHECK_EQUAL(false, remove);

  remove = camerasModel->removeCamera(id);
  BOOST_CHECK_EQUAL(true, remove);
}

BOOST_FIXTURE_TEST_CASE(cameraId, TestCamerasModel)
{
  int id = camerasModel->cameraID("Unknown camera", "0");
  BOOST_CHECK_EQUAL(1, id);

  id = camerasModel->cameraID("DJI", "FC6310");
  BOOST_CHECK_EQUAL(2, id);

  id = camerasModel->cameraID("Sony", "ILCE-6000");
  BOOST_CHECK_EQUAL(0, id);
}

//BOOST_FIXTURE_TEST_CASE(findCamera, TestCamerasModel)
//{
//  Camera camera = camerasModel->camera(1);
//
//  BOOST_CHECK_EQUAL("Unknown camera", camera.make());
//  BOOST_CHECK_EQUAL("0", camera.model());
//  BOOST_CHECK_EQUAL("Radial", camera.type());
//  BOOST_CHECK_EQUAL(5835.6, camera.focal());
//  BOOST_CHECK_EQUAL(4863, camera.width());
//  BOOST_CHECK_EQUAL(3221, camera.height());
//  BOOST_CHECK_EQUAL(1., camera.sensorSize());
//
//  camera = camerasModel->camera("Unknown camera", "0");
//
//  BOOST_CHECK_EQUAL("0", camera.model());
//  BOOST_CHECK_EQUAL("Radial", camera.type());
//  BOOST_CHECK_EQUAL(5835.6, camera.focal());
//  BOOST_CHECK_EQUAL(4863, camera.width());
//  BOOST_CHECK_EQUAL(3221, camera.height());
//  BOOST_CHECK_EQUAL(1., camera.sensorSize());
//}
//
//BOOST_FIXTURE_TEST_CASE(findCamera_exception, TestCamerasModel)
//{
//  try {
//    Camera camera = camerasModel->camera(5);
//  } catch (std::exception &e) {
//    BOOST_CHECK_EQUAL("Camera not exist", e.what());
//  }
//}
//
//BOOST_FIXTURE_TEST_CASE(iterator, TestCamerasModel)
//{
//    auto &cameras = camerasModel->cameras();
//  auto it = cameras.begin();
//  int id = it->first;
//  Camera camera = it->second;
//
//  BOOST_CHECK_EQUAL(1, id);
//  BOOST_CHECK_EQUAL("Unknown camera", camera.make());
//  BOOST_CHECK_EQUAL("0", camera.model());
//  BOOST_CHECK_EQUAL("Radial", camera.type());
//  BOOST_CHECK_EQUAL(5835.6, camera.focal());
//  BOOST_CHECK_EQUAL(4863, camera.width());
//  BOOST_CHECK_EQUAL(3221, camera.height());
//  BOOST_CHECK_EQUAL(1., camera.sensorSize());
//
//  it++;
//
//  id = it->first;
//  camera = it->second;
//
//  BOOST_CHECK_EQUAL(2, id);
//  BOOST_CHECK_EQUAL("DJI", camera.make());
//  BOOST_CHECK_EQUAL("FC6310", camera.model());
//  BOOST_CHECK_EQUAL("Simple radial", camera.type());
//  BOOST_CHECK_EQUAL(3552.23, camera.focal());
//  BOOST_CHECK_EQUAL(5472, camera.width());
//  BOOST_CHECK_EQUAL(3648, camera.height());
//  BOOST_CHECK_EQUAL(12.8333, camera.sensorSize());
//
//}

BOOST_AUTO_TEST_SUITE_END()