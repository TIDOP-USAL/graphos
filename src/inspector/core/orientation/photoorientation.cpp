#include "photoorientation.h"

#include <tidop/core/defs.h>

#include <colmap/base/reconstruction.h>

namespace inspector
{

ReadPhotoOrientations::ReadPhotoOrientations()
  : mReconstruction(new colmap::Reconstruction)
{

}

ReadPhotoOrientations::~ReadPhotoOrientations()
{
  if (mReconstruction){
    delete mReconstruction;
    mReconstruction = nullptr;
  }
}

void ReadPhotoOrientations::open(const QString &path)
{
//#ifdef _DEBUG
//  TL_TODO("Excepción al leer el binario en debug")
//  mReconstruction->ReadText(path.toStdString());
//#else
  mReconstruction->ReadBinary(path.toStdString());
//#endif
}

CameraPose ReadPhotoOrientations::orientation(const QString &imageName) const
{
  CameraPose photoOrientation;

  for (auto &image : mReconstruction->Images()) {
    std::string image_name = image.second.Name();
    if (imageName.compare(image_name.c_str()) == 0) {

      colmap::image_t image_id = image.second.ImageId();

      colmap::Image &colmap_image = mReconstruction->Image(image_id);
      const Eigen::Matrix<double, 3, 4> inv_proj_matrix = colmap_image.InverseProjectionMatrix();
      const Eigen::Vector3d pc = inv_proj_matrix.rightCols<1>();
      photoOrientation.position.x = pc(0);
      photoOrientation.position.y = pc(1);
      photoOrientation.position.z = pc(2);

      Eigen::Matrix3d rot = colmap_image.RotationMatrix();
      photoOrientation.rotation.at(0, 0) = rot(0, 0);
      photoOrientation.rotation.at(0, 1) = rot(0, 1);
      photoOrientation.rotation.at(0, 2) = rot(0, 2);
      photoOrientation.rotation.at(1, 0) = rot(1, 0);
      photoOrientation.rotation.at(1, 1) = rot(1, 1);
      photoOrientation.rotation.at(1, 2) = rot(1, 2);
      photoOrientation.rotation.at(2, 0) = rot(2, 0);
      photoOrientation.rotation.at(2, 1) = rot(2, 1);
      photoOrientation.rotation.at(2, 2) = rot(2, 2);
    }
  }

  return photoOrientation;
}



} // namespace inspector
