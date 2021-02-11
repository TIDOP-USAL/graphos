#include "gcp.h"

namespace inspector
{

GroundControlPoint::GroundControlPoint()
{

}

GroundControlPoint::~GroundControlPoint()
{
}

std::string GroundControlPoint::name() const
{
  return mName;
}

void GroundControlPoint::setName(const std::string &name)
{
  mName = name;
}

tl::Point3D GroundControlPoint::point() const
{
  return mCoordinates;
}

void GroundControlPoint::setPoint(const tl::Point3D &point)
{
  mCoordinates = point;
}

double GroundControlPoint::x() const
{
  return mCoordinates.x;
}

void GroundControlPoint::setX(double x)
{
  mCoordinates.x = x;
}

double GroundControlPoint::y() const
{
  return mCoordinates.y;
}

void GroundControlPoint::setY(double y)
{
  mCoordinates.y = y;
}

double GroundControlPoint::z() const
{
  return mCoordinates.z;
}

void GroundControlPoint::setZ(double z)
{
  mCoordinates.z = z;
}

void GroundControlPoint::addImagePoint(const std::string &image, const tl::PointD &point)
{
  mPoints[image] = point;
}

tl::PointD GroundControlPoint::imagePoint(const std::string &image)
{
  return mPoints[image];
}

bool GroundControlPoint::existImagePoint(const std::string &image)
{
  auto it = mPoints.find(image);
  
  return (it != mPoints.end());
}

}
