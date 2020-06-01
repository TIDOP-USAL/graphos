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

  enum class Type
  {
    colmap
  };

public:

  Orientation(){}
  virtual ~Orientation() = default;

  virtual void reset() = 0;
  virtual QString name() const = 0;
  Type type() const { return mOrientationType.flags(); }

protected:

  tl::EnumFlags<Type> mOrientationType;

};
ALLOW_BITWISE_FLAG_OPERATIONS(Orientation::Type)


/*----------------------------------------------------------------*/



class TL_EXPORT OrientationProcess
{

public:

  OrientationProcess() {}
  virtual ~OrientationProcess() = default;

  virtual void run() = 0;
};


} // namespace inspector

#endif // INSPECTOR_CORE_ORIENTATION_H
