#include "dtm.h"

namespace inspector
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

	
} // namespace inspector
