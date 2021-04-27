#ifndef INSPECTOR_CORE_PHOTO_ORIENTATION_H
#define INSPECTOR_CORE_PHOTO_ORIENTATION_H

#include <array>

#include <QString>

#include <tidop/geometry/entities/point.h>
#include <tidop/math/algebra/rotation_matrix.h>

#include "inspector/inspector_global.h"

namespace colmap
{
class Reconstruction;
}

namespace inspector
{

struct INSPECTOR_EXPORT CameraPose
{
  tl::Point3D position{};
  tl::math::RotationMatrix<double> rotation{};
};


class INSPECTOR_EXPORT ReadPhotoOrientations
{

public:

  ReadPhotoOrientations();
  ~ReadPhotoOrientations();

  void open(const QString &path);
  CameraPose orientation(const QString &image) const;

protected:

  colmap::Reconstruction *mReconstruction;

};

} // namespace inspector

#endif // INSPECTOR_CORE_PHOTO_ORIENTATION_H
