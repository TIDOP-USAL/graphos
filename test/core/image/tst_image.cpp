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

#define BOOST_TEST_MODULE GRAPHOS Image test

#include <boost/test/unit_test.hpp>
#include "graphos/core/image.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(TestImageSuite)

struct TestImage
{

  TestImage() 
    : mImageDefaultConstructor(new Image),
      mImage(new Image("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png"))
  {

  }

  ~TestImage()
  {
    if (mImageDefaultConstructor) {
      delete mImageDefaultConstructor;
    }

    if (mImage) {
      delete mImage;
    }
  }

  void setup()
  {
    mImage->setCameraId(1);
    CameraPose camera_pose;
    camera_pose.setPosition(tl::Point3D(0.5, 2.3, 10.2));
    mImage->setCameraPose(camera_pose);
  }

  void teardown()
  {

  }

  Image *mImageDefaultConstructor;
  Image *mImage;
};


BOOST_FIXTURE_TEST_CASE(DefaultConstructor, TestImage)
{
  BOOST_CHECK_EQUAL("", mImageDefaultConstructor->path().toStdString());
  BOOST_CHECK_EQUAL("", mImageDefaultConstructor->name().toStdString());
  BOOST_CHECK_EQUAL(0, mImageDefaultConstructor->cameraId());
  CameraPose camera_pose = mImageDefaultConstructor->cameraPose();
  BOOST_CHECK_EQUAL(true, camera_pose.isEmpty());
}

BOOST_FIXTURE_TEST_CASE(Constructor, TestImage)
{
  BOOST_CHECK_EQUAL("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png", mImage->path().toStdString());
  BOOST_CHECK_EQUAL("img001.png", mImage->name().toStdString());
  BOOST_CHECK_EQUAL(1, mImage->cameraId());
  CameraPose camera_pose = mImage->cameraPose();
  BOOST_CHECK_EQUAL(false, camera_pose.isEmpty());
  BOOST_CHECK(tl::Point3D(0.5, 2.3, 10.2) == camera_pose.position());
}

BOOST_FIXTURE_TEST_CASE(copy_constructor, TestImage)
{
  Image image(*mImage);
  BOOST_CHECK_EQUAL("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png", image.path().toStdString());
  BOOST_CHECK_EQUAL("img001.png", image.name().toStdString());
  BOOST_CHECK_EQUAL(1, image.cameraId());
  CameraPose camera_pose = image.cameraPose();
  BOOST_CHECK(tl::Point3D(0.5, 2.3, 10.2) == camera_pose.position());
}

BOOST_FIXTURE_TEST_CASE(move_constructor, TestImage)
{
  Image image("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img002.png");
  image.setCameraId(1);
  CameraPose camera_pose;
  camera_pose.setPosition(tl::Point3D(0.6, 2.4, 10.1));
  image.setCameraPose(camera_pose);

  Image image_move(image);
  BOOST_CHECK_EQUAL("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img002.png", image_move.path().toStdString());
  BOOST_CHECK_EQUAL("img002.png", image_move.name().toStdString());
  BOOST_CHECK_EQUAL(1, image_move.cameraId());
  camera_pose = image_move.cameraPose();
  BOOST_CHECK(tl::Point3D(0.6, 2.4, 10.1) == camera_pose.position());

}

BOOST_FIXTURE_TEST_CASE(path, TestImage)
{
  mImage->setPath("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png");
  BOOST_CHECK_EQUAL("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png", mImage->path().toStdString());
}

BOOST_FIXTURE_TEST_CASE(name, TestImage)
{
  mImage->setPath("C:\\Users\\User01\\Documents\\Graphos\\Projects\\images\\img001.png");
  BOOST_CHECK_EQUAL("img001.png", mImage->name().toStdString());
}

BOOST_FIXTURE_TEST_CASE(cameraId, TestImage)
{
  mImage->setCameraId(1);
  BOOST_CHECK_EQUAL(1, mImage->cameraId());

  mImage->setCameraId(2);
  BOOST_CHECK_EQUAL(2, mImage->cameraId());
}

BOOST_AUTO_TEST_SUITE_END()