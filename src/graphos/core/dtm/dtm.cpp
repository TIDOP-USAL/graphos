#include "dtm.h"

namespace graphos
{

DtmBase::DtmBase(Interpolation interpolation)
  : Dtm(),
    mInterpolation(interpolation)
{

}

DtmBase::~DtmBase()
{

}

Dtm::Interpolation DtmBase::interpolation() const
{
  return mInterpolation;
}

	
} // namespace graphos
