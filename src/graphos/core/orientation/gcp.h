#ifndef GRAPHOS_CORE_GROUND_CONTROL_POINT_H
#define GRAPHOS_CORE_GROUND_CONTROL_POINT_H


#include "graphos/graphos_global.h"

#include <tidop/geometry/entities/point.h>

#include <map>

namespace graphos
{
	
class GroundControlPoint
{

public:

  GroundControlPoint();
  ~GroundControlPoint();

  std::string name() const;
  void setName(const std::string &name);
  tl::Point3D point() const;
  void setPoint(const tl::Point3D &point);
  double x() const;
  void setX(double x);
  double y() const;
  void setY(double y);
  double z() const;
  void setZ(double z);
  void addImagePoint(const std::string &image, const tl::PointD &point);
  tl::PointD imagePoint(const std::string &image);
  bool existImagePoint(const std::string &image);

protected:

  std::string mName;
  tl::Point3D mCoordinates;
  std::map<std::string, tl::PointD> mPoints;
};

} // namespace graphos

#endif // GRAPHOS_CORE_GROUND_CONTROL_POINT_H
