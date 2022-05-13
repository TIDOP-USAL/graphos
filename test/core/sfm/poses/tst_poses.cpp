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

#define BOOST_TEST_MODULE GRAPHOS CameraPose test

#include <boost/test/unit_test.hpp>
#include "graphos/core/sfm/poses.h"
#include <tidop/math/algebra/rotation_matrix.h>
#include <tidop/math/algebra/quaternion.h>
#include <tidop/math/algebra/rotation_convert.h>

using namespace graphos;
using namespace tl;
using namespace math;

BOOST_AUTO_TEST_SUITE(TestCameraPoseSuite)

struct TestCameraPose
{

  TestCameraPose()
    : mCameraPose(new CameraPose)
  { }

  ~TestCameraPose()
  {
    if (mCameraPose) {
      delete mCameraPose;
    }
  }

  void setup()
  {
    point = Point3D(-0.6809533178, 0.3224212264, 1.0591648145);
    rotation_matrix = {{0.9884904280, -0.0018538636, -0.1512720629,
                        0.0054840786, 0.9997068109, 0.0235842565,
                        0.1511839896, -0.0241423997, 0.9882107801}};

    RotationConverter<double>::convert(rotation_matrix, quaternion);
  }

  void teardown()
  {

  }

  CameraPose *mCameraPose;
  Point3D point;
  RotationMatrix<double> rotation_matrix;
  Quaterniond quaternion;
};


BOOST_FIXTURE_TEST_CASE(DefaultConstructor, TestCameraPose)
{
  BOOST_CHECK(Point3D() == mCameraPose->position());
  BOOST_CHECK(Quaterniond::zero() == mCameraPose->quaternion());
  BOOST_CHECK(RotationMatrix<double>::zero()  == mCameraPose->rotationMatrix());
  BOOST_CHECK_EQUAL("", mCameraPose->crs().toStdString());
  BOOST_CHECK_EQUAL("", mCameraPose->source().toStdString());
  BOOST_CHECK_EQUAL(true, mCameraPose->isEmpty());
}

BOOST_FIXTURE_TEST_CASE(ConstructorPointRotationMatrix, TestCameraPose)
{
  CameraPose camera_pose(point, rotation_matrix);
  BOOST_CHECK(point == camera_pose.position());
  BOOST_CHECK(quaternion == camera_pose.quaternion());
  auto rotmat = camera_pose.rotationMatrix();
  BOOST_CHECK_CLOSE(rotation_matrix[0][0], rotmat[0][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[0][1], rotmat[0][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[0][2], rotmat[0][2], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][0], rotmat[1][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][1], rotmat[1][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][2], rotmat[1][2], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][0], rotmat[2][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][1], rotmat[2][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][2], rotmat[2][2], 0.1);
  BOOST_CHECK_EQUAL("", camera_pose.crs().toStdString());
  BOOST_CHECK_EQUAL("", camera_pose.source().toStdString());
  BOOST_CHECK_EQUAL(false, camera_pose.isEmpty());
}

BOOST_FIXTURE_TEST_CASE(ConstructorXYZRotationMatrix, TestCameraPose)
{
  CameraPose camera_pose(point.x, point.y, point.z, rotation_matrix);
  BOOST_CHECK(point == camera_pose.position());
  BOOST_CHECK(quaternion == camera_pose.quaternion());
  auto rotmat = camera_pose.rotationMatrix();
  BOOST_CHECK_CLOSE(rotation_matrix[0][0], rotmat[0][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[0][1], rotmat[0][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[0][2], rotmat[0][2], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][0], rotmat[1][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][1], rotmat[1][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][2], rotmat[1][2], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][0], rotmat[2][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][1], rotmat[2][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][2], rotmat[2][2], 0.1);
  BOOST_CHECK_EQUAL("", camera_pose.crs().toStdString());
  BOOST_CHECK_EQUAL("", camera_pose.source().toStdString());
  BOOST_CHECK_EQUAL(false, camera_pose.isEmpty());
}

BOOST_FIXTURE_TEST_CASE(ConstructorPointQuaternion, TestCameraPose)
{
  CameraPose camera_pose(point, quaternion);
  BOOST_CHECK(point == camera_pose.position());
  BOOST_CHECK(quaternion == camera_pose.quaternion());
  auto rotmat = camera_pose.rotationMatrix();
  BOOST_CHECK_CLOSE(rotation_matrix[0][0], rotmat[0][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[0][1], rotmat[0][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[0][2], rotmat[0][2], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][0], rotmat[1][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][1], rotmat[1][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][2], rotmat[1][2], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][0], rotmat[2][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][1], rotmat[2][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][2], rotmat[2][2], 0.1);
  BOOST_CHECK_EQUAL("", camera_pose.crs().toStdString());
  BOOST_CHECK_EQUAL("", camera_pose.source().toStdString());
  BOOST_CHECK_EQUAL(false, camera_pose.isEmpty());
}

BOOST_FIXTURE_TEST_CASE(ConstructorXYZQuaternion, TestCameraPose)
{
  CameraPose camera_pose(point.x, point.y, point.z, quaternion);
  BOOST_CHECK(point == camera_pose.position());
  BOOST_CHECK(quaternion == camera_pose.quaternion());
  auto rotmat = camera_pose.rotationMatrix();
  BOOST_CHECK_CLOSE(rotation_matrix[0][0], rotmat[0][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[0][1], rotmat[0][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[0][2], rotmat[0][2], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][0], rotmat[1][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][1], rotmat[1][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][2], rotmat[1][2], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][0], rotmat[2][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][1], rotmat[2][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][2], rotmat[2][2], 0.1);
  BOOST_CHECK_EQUAL("", camera_pose.crs().toStdString());
  BOOST_CHECK_EQUAL("", camera_pose.source().toStdString());
  BOOST_CHECK_EQUAL(false, camera_pose.isEmpty());
}

BOOST_FIXTURE_TEST_CASE(crs, TestCameraPose)
{
  mCameraPose->setCrs("EPSG:25830");
  BOOST_CHECK_EQUAL("EPSG:25830", mCameraPose->crs().toStdString());

  mCameraPose->setCrs("EPSG:4258");
  BOOST_CHECK_EQUAL("EPSG:4258", mCameraPose->crs().toStdString());
}

BOOST_FIXTURE_TEST_CASE(source, TestCameraPose)
{
  mCameraPose->setSource("C:\\Users\\User01\\Documents\\Graphos\\Projects\\cameras.txt");
  BOOST_CHECK_EQUAL("C:\\Users\\User01\\Documents\\Graphos\\Projects\\cameras.txt", mCameraPose->source().toStdString());

  mCameraPose->setSource("EXIF");
  BOOST_CHECK_EQUAL("EXIF", mCameraPose->source().toStdString());
}

BOOST_FIXTURE_TEST_CASE(setRotationMatrix, TestCameraPose)
{
  mCameraPose->setRotationMatrix(rotation_matrix);
  auto rotmat = mCameraPose->rotationMatrix();
  BOOST_CHECK_CLOSE(rotation_matrix[0][0], rotmat[0][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[0][1], rotmat[0][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[0][2], rotmat[0][2], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][0], rotmat[1][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][1], rotmat[1][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[1][2], rotmat[1][2], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][0], rotmat[2][0], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][1], rotmat[2][1], 0.1);
  BOOST_CHECK_CLOSE(rotation_matrix[2][2], rotmat[2][2], 0.1);
}

BOOST_FIXTURE_TEST_CASE(setQuaternion, TestCameraPose)
{
  Quaternion<double> quat(0.1, 0.5, 0.3, 0.2);

  mCameraPose->setQuaternion(quat);
  auto q = mCameraPose->quaternion();
  BOOST_CHECK_CLOSE(q.x, quat.x, 0.1);
  BOOST_CHECK_CLOSE(q.y, quat.y, 0.1);
  BOOST_CHECK_CLOSE(q.z, quat.z, 0.1);
  BOOST_CHECK_CLOSE(q.w, quat.w, 0.1);
}

BOOST_AUTO_TEST_SUITE_END()