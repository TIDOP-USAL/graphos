#ifndef INSPECTOR_CORE_PHOTO_ORIENTATION_H
#define INSPECTOR_CORE_PHOTO_ORIENTATION_H

#include <array>

namespace inspector
{

struct PhotoOrientation
{
  double x = 0.;
  double y = 0.;
  double z = 0.;
  std::array<std::array<float, 3>, 3> rot;
};

} // namespace inspector

#endif // INSPECTOR_CORE_PHOTO_ORIENTATION_H
