#ifndef INSPECTOR_DTM_H
#define INSPECTOR_DTM_H

#include "inspector/inspector_global.h"

#include <map>
#include <memory>

#include <tidop/core/defs.h>
#include <tidop/core/flags.h>
#include <tidop/geometry/entities/bbox.h>

namespace tl
{
template<typename T> class Size;
}


namespace inspector
{


class INSPECTOR_EXPORT Dtm
{

public:

  enum class Interpolation : uint8_t
  {
    inv_dist,
    inv_distnn
  };

public:

  Dtm() {}
  virtual ~Dtm() = default;

  virtual Interpolation interpolation() const = 0;
  virtual std::string name() const = 0;
  virtual void reset() = 0;
};

class INSPECTOR_EXPORT DtmBase
  : public Dtm
{

public:

  DtmBase(Interpolation interpolation);
  virtual ~DtmBase();

  Interpolation interpolation() const;

protected:

  Interpolation mInterpolation;

};

class INSPECTOR_EXPORT DtmAlgorithm
{

public:

  DtmAlgorithm() {}
  virtual ~DtmAlgorithm() = default;

  virtual bool run(const std::string &pointCloud,
                   const std::string &dtmFile,
                   const tl::Size<int> &size) = 0;

  virtual bool run(const std::string &pointCloud,
                   const std::string &dtmFile,
                   const tl::BoundingBox<tl::Point3<double>> &bbox,
                   double gsd) = 0;

};



class INSPECTOR_EXPORT DtmInvDist
  : public DtmBase
{

public:

  DtmInvDist() : DtmBase(Interpolation::inv_dist){}
  ~DtmInvDist() override = default;

  virtual double power() const = 0;
  virtual double smoothing() const = 0;
  virtual double radius1() const = 0;
  virtual double radius2() const = 0;
  virtual double angle() const = 0;
  virtual int maxPoints() const = 0;
  virtual int minPoints() const = 0;

public:

  virtual void setPower(double power) = 0;
  virtual void setSmoothing(double smoothing) = 0;
  virtual void setRadius1(double radius1) = 0;
  virtual void setRadius2(double radius2) = 0;
  virtual void setAngle(double angle) = 0;
  virtual void setMaxPoints(int maxPoints) = 0;
  virtual void setMinPoints(int minPoints) = 0;
};


class INSPECTOR_EXPORT DtmInvDistNN
  : public DtmBase
{

public:

  DtmInvDistNN() : DtmBase(Interpolation::inv_distnn){}
  ~DtmInvDistNN() override = default;

  virtual double power() const = 0;
  virtual double smoothing() const = 0;
  virtual double radius() const = 0;
  virtual int maxPoints() const = 0;
  virtual int minPoints() const = 0;

public:

  virtual void setPower(double power) = 0;
  virtual void setSmoothing(double smoothing) = 0;
  virtual void setRadius(double radius) = 0;
  virtual void setMaxPoints(int maxPoints) = 0;
  virtual void setMinPoints(int minPoints) = 0;
};


} // namespace inspector

#endif // INSPECTOR_DTM_H
