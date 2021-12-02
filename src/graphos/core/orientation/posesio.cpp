#include "posesio.h"

#include <colmap/base/reconstruction.h>

#include <tidop/core/path.h>

namespace graphos
{

ReadCameraPoses::ReadCameraPoses()
  : mReconstruction(new colmap::Reconstruction)
{

}

ReadCameraPoses::~ReadCameraPoses()
{
  if (mReconstruction){
    delete mReconstruction;
    mReconstruction = nullptr;
  }
}

void ReadCameraPoses::open(const QString &path)
{
  mReconstruction->ReadBinary(path.toStdString());
}

CameraPose ReadCameraPoses::orientation(const QString &imageName) const
{
  CameraPose photoOrientation;

  for (auto &image : mReconstruction->Images()) {
    std::string image_name = tl::Path(image.second.Name()).fileName().toString();
    if (imageName.compare(image_name.c_str()) == 0) {

      colmap::image_t image_id = image.second.ImageId();
      colmap::Image &colmap_image = mReconstruction->Image(image_id);

      const Eigen::Matrix<double, 3, 4> inv_proj_matrix = colmap_image.InverseProjectionMatrix();
      const Eigen::Vector3d pc = inv_proj_matrix.rightCols<1>();
      photoOrientation.setPosition(tl::Point3D(pc(0), pc(1), pc(2)));

      Eigen::Matrix3d rot = colmap_image.RotationMatrix();
      tl::math::RotationMatrix<double> rotation_matrix;
      rotation_matrix.at(0, 0) = rot(0, 0);
      rotation_matrix.at(0, 1) = rot(0, 1);
      rotation_matrix.at(0, 2) = rot(0, 2);
      rotation_matrix.at(1, 0) = rot(1, 0);
      rotation_matrix.at(1, 1) = rot(1, 1);
      rotation_matrix.at(1, 2) = rot(1, 2);
      rotation_matrix.at(2, 0) = rot(2, 0);
      rotation_matrix.at(2, 1) = rot(2, 1);
      rotation_matrix.at(2, 2) = rot(2, 2);
      photoOrientation.setRotationMatrix(rotation_matrix);
    }
  }

  return photoOrientation;
}



} // namespace graphos
