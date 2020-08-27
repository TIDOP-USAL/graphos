#ifndef INSPECTOR_CORE_ORIENTATION_H
#define INSPECTOR_CORE_ORIENTATION_H

#include "inspector/inspector_global.h"

#include <tidop/core/flags.h>

#include <QString>

namespace inspector
{

class TL_EXPORT Orientation
{

public:

  enum class Method
  {
    colmap
  };

public:

  Orientation(){}
  virtual ~Orientation() = default;

  virtual void reset() = 0;
  virtual QString name() const = 0;
  Method method() const { return mOrientationMethod.flags(); }

protected:

  tl::EnumFlags<Method> mOrientationMethod;

};
ALLOW_BITWISE_FLAG_OPERATIONS(Orientation::Method)



/*----------------------------------------------------------------*/



class TL_EXPORT OrientationProcess
{

public:

  OrientationProcess() {}
  virtual ~OrientationProcess() = default;

  virtual void run() = 0;
};


/*----------------------------------------------------------------*/

class TL_EXPORT AbsoluteOrientation
{

public:

  enum class Method
  {
    colmap
  };

public:

  AbsoluteOrientation(){}
  virtual ~AbsoluteOrientation() = default;

  virtual void reset() = 0;
  virtual QString name() const = 0;
  Method method() const { return mOrientationMethod.flags(); }

protected:

  tl::EnumFlags<Method> mOrientationMethod;
};
ALLOW_BITWISE_FLAG_OPERATIONS(AbsoluteOrientation::Method)


/*----------------------------------------------------------------*/


class TL_EXPORT AbsoluteOrientationProcess
{

public:

  AbsoluteOrientationProcess() {}
  virtual ~AbsoluteOrientationProcess() = default;

  virtual void run() = 0;
};

} // namespace inspector

#endif // INSPECTOR_CORE_ORIENTATION_H
