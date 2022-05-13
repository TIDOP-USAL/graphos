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

#define BOOST_TEST_MODULE GRAPHOS Camera test

#include <boost/test/unit_test.hpp>
#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/Colmap.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(CameraTestSuite)

struct CameraTest
{
  CameraTest()
    : camera1(new Camera()),
      camera2(new Camera("DJI", "FC6310"))
  {
  }

  ~CameraTest()
  {
    if (camera1){
      delete camera1;
      camera1 = nullptr;
    }

    if (camera2) {
      delete camera2;
      camera2 = nullptr;
    }
  }

  virtual void setup()
  {
    camera2->setType("Radial 1");
    camera2->setFocal(3552.23);
    camera2->setWidth(5472);
    camera2->setHeight(3648);
    camera2->setSensorSize(12.8333);
  }

  virtual void teardown()
  {

  }

  Camera *camera1;
  Camera *camera2;
};


BOOST_FIXTURE_TEST_CASE(constructor_default, CameraTest)
{
  BOOST_CHECK_EQUAL("", camera1->make());
  BOOST_CHECK_EQUAL("", camera1->model());
  BOOST_CHECK_EQUAL("OpenCV 1", camera1->type());
  BOOST_CHECK_EQUAL(1., camera1->focal());
  BOOST_CHECK_EQUAL(0, camera1->width());
  BOOST_CHECK_EQUAL(0, camera1->height());
  BOOST_CHECK_EQUAL(1., camera1->sensorSize());
}

BOOST_FIXTURE_TEST_CASE(constructor, CameraTest)
{
  BOOST_CHECK_EQUAL("DJI", camera2->make());
  BOOST_CHECK_EQUAL("FC6310", camera2->model());
  BOOST_CHECK_EQUAL("Radial 1", camera2->type());
  BOOST_CHECK_EQUAL(3552.23, camera2->focal());
  BOOST_CHECK_EQUAL(5472, camera2->width());
  BOOST_CHECK_EQUAL(3648, camera2->height());
  BOOST_CHECK_EQUAL(12.8333, camera2->sensorSize());
}

BOOST_FIXTURE_TEST_CASE(copy_constructor, CameraTest)
{
  Camera copy_camera(*camera2);
  BOOST_CHECK_EQUAL("DJI", copy_camera.make());
  BOOST_CHECK_EQUAL("FC6310", copy_camera.model());
  BOOST_CHECK_EQUAL("Radial 1", copy_camera.type());
  BOOST_CHECK_EQUAL(3552.23, copy_camera.focal());
  BOOST_CHECK_EQUAL(5472, copy_camera.width());
  BOOST_CHECK_EQUAL(3648, copy_camera.height());
  BOOST_CHECK_EQUAL(12.8333, copy_camera.sensorSize());
}

BOOST_FIXTURE_TEST_CASE(setMake, CameraTest)
{
  Camera camera;
  camera.setMake("Canon");
  BOOST_CHECK_EQUAL("Canon", camera.make());
}

BOOST_FIXTURE_TEST_CASE(setModel, CameraTest)
{
  Camera camera;
  camera.setModel("eos 4000d");
  BOOST_CHECK_EQUAL("eos 4000d", camera.model());
}

BOOST_FIXTURE_TEST_CASE(setType, CameraTest)
{
  Camera camera;
  camera.setType("Radial 3");
  BOOST_CHECK_EQUAL("Radial 3", camera.type());
}

BOOST_FIXTURE_TEST_CASE(setFocal, CameraTest)
{
  Camera camera;
  camera.setFocal(15.);
  BOOST_CHECK_EQUAL(15., camera.focal());
}

BOOST_FIXTURE_TEST_CASE(setWidth, CameraTest)
{
  Camera camera;
  camera.setWidth(6000);
  BOOST_CHECK_EQUAL(6000, camera.width());
}

BOOST_FIXTURE_TEST_CASE(setHeight, CameraTest)
{
  Camera camera;
  camera.setHeight(4000);
  BOOST_CHECK_EQUAL(4000, camera.height());
}

BOOST_FIXTURE_TEST_CASE(setSensorSize, CameraTest)
{
  Camera camera;
  camera.setSensorSize(0.25);
  BOOST_CHECK_EQUAL(0.25, camera.sensorSize());
}

BOOST_FIXTURE_TEST_CASE(camera_to_colmap_type, CameraTest)
{
  Camera camera;

  camera.setType("Pinhole 1");
  BOOST_CHECK_EQUAL("SIMPLE_PINHOLE", cameraToColmapType(camera).toStdString());

  camera.setType("Pinhole 2");
  BOOST_CHECK_EQUAL("PINHOLE", cameraToColmapType(camera).toStdString());

  camera.setType("Radial 1");
  BOOST_CHECK_EQUAL("SIMPLE_RADIAL", cameraToColmapType(camera).toStdString());

  camera.setType("Radial 2");
  BOOST_CHECK_EQUAL("RADIAL", cameraToColmapType(camera).toStdString());

  camera.setType("OpenCV 1");
  BOOST_CHECK_EQUAL("OPENCV", cameraToColmapType(camera).toStdString());

  camera.setType("OpenCV Fisheye");
  BOOST_CHECK_EQUAL("OPENCV_FISHEYE", cameraToColmapType(camera).toStdString());

  camera.setType("OpenCV 2");
  BOOST_CHECK_EQUAL("FULL_OPENCV", cameraToColmapType(camera).toStdString());

  camera.setType("Radial Fisheye 1");
  BOOST_CHECK_EQUAL("SIMPLE_RADIAL_FISHEYE", cameraToColmapType(camera).toStdString());

  camera.setType("Radial Fisheye 2");
  BOOST_CHECK_EQUAL("RADIAL_FISHEYE", cameraToColmapType(camera).toStdString());

  camera.setType("Radial 3");
  BOOST_CHECK_EQUAL("FULL_RADIAL", cameraToColmapType(camera).toStdString());

}

BOOST_AUTO_TEST_SUITE_END()