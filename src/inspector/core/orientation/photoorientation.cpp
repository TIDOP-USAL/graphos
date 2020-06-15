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
#ifdef _DEBUG
  TL_TODO("Excepción al leer el binario en debug")
  mReconstruction->ReadText(path.toStdString());
#else
  mReconstruction->ReadBinary(path.toStdString());
#endif
}

PhotoOrientation ReadPhotoOrientations::orientation(const QString &imageName) const
{
  PhotoOrientation photoOrientation;

  for (auto &image : mReconstruction->Images()) {
    std::string image_name = image.second.Name();
    if (imageName.compare(image_name.c_str()) == 0) {

      colmap::image_t image_id = image.second.ImageId();

      colmap::Image &colmap_image = mReconstruction->Image(image_id);
      const Eigen::Matrix<float, 3, 4> inv_proj_matrix = colmap_image.InverseProjectionMatrix().cast<float>();
      const Eigen::Vector3f pc = inv_proj_matrix.rightCols<1>();
      photoOrientation.x = static_cast<double>(pc(0));
      photoOrientation.y = static_cast<double>(pc(1));
      photoOrientation.z = static_cast<double>(pc(2));

      Eigen::Matrix3d rot = colmap_image.RotationMatrix();
      photoOrientation.rot[0][0] = static_cast<float>(rot(0, 0));
      photoOrientation.rot[0][1] = static_cast<float>(rot(0, 1));
      photoOrientation.rot[0][2] = static_cast<float>(rot(0, 2));
      photoOrientation.rot[1][0] = static_cast<float>(rot(1, 0));
      photoOrientation.rot[1][1] = static_cast<float>(rot(1, 1));
      photoOrientation.rot[1][2] = static_cast<float>(rot(1, 2));
      photoOrientation.rot[2][0] = static_cast<float>(rot(2, 0));
      photoOrientation.rot[2][1] = static_cast<float>(rot(2, 1));
      photoOrientation.rot[2][2] = static_cast<float>(rot(2, 2));
    }
  }

  return photoOrientation;
}



} // namespace inspector
