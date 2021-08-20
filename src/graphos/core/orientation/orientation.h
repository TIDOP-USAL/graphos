#ifndef GRAPHOS_CORE_ORIENTATION_H
#define GRAPHOS_CORE_ORIENTATION_H

#include "graphos/graphos_global.h"

#include <tidop/core/flags.h>

#include <QString>

namespace graphos
{

class TL_EXPORT RelativeOrientation
{

public:

  enum class Method
  {
    colmap
  };

public:

  RelativeOrientation(){}
  virtual ~RelativeOrientation() = default;

  virtual void reset() = 0;
  virtual QString name() const = 0;
  Method method() const { return mOrientationMethod.flags(); }

protected:

  tl::EnumFlags<Method> mOrientationMethod;

};
ALLOW_BITWISE_FLAG_OPERATIONS(RelativeOrientation::Method)



/*----------------------------------------------------------------*/



class TL_EXPORT RelativeOrientationAlgorithm
{

public:

  RelativeOrientationAlgorithm() {}
  virtual ~RelativeOrientationAlgorithm() = default;

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


class TL_EXPORT AbsoluteOrientationAlgorithm
{

public:

  AbsoluteOrientationAlgorithm() {}
  virtual ~AbsoluteOrientationAlgorithm() = default;

  virtual void run() = 0;
};

} // namespace graphos

#endif // GRAPHOS_CORE_ORIENTATION_H
