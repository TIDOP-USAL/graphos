/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_CORE_DENSIFICATION_H
#define GRAPHOS_CORE_DENSIFICATION_H

#include "graphos/graphos_global.h"

#include <tidop/core/flags.h>

#include <QString>

namespace graphos
{

class GRAPHOS_EXPORT Densification
{

public:

  enum class Method
  {
    cmvs_pmvs,
    smvs,
    mvs
  };

public:

  Densification(Method method) : mDensificationMethod(method) {}
  virtual ~Densification() = default;


  virtual void reset() = 0;
  virtual QString name() const = 0;
  Method method() const { return mDensificationMethod.flags(); }

protected:

  tl::EnumFlags<Method> mDensificationMethod;

};
ALLOW_BITWISE_FLAG_OPERATIONS(Densification::Method)



/*----------------------------------------------------------------*/



class GRAPHOS_EXPORT Densifier
{

public:

  Densifier() {}
  virtual ~Densifier() = default;

  virtual bool undistort(const QString &reconstructionPath,
                         const QString &outputPath);
  virtual bool densify(const QString &undistortPath) = 0;

  virtual void enableCuda(bool enable) = 0;
};



/*----------------------------------------------------------------*/



class GRAPHOS_EXPORT CmvsPmvs
  : public Densification
{

public:

  CmvsPmvs() : Densification(Densification::Method::cmvs_pmvs) {}
  ~CmvsPmvs() override = default;
  
  virtual bool useVisibilityInformation() const = 0;
  virtual int imagesPerCluster() const  = 0;
  virtual int level() const = 0;
  virtual int cellSize() const = 0;
  virtual double threshold() const = 0;
  virtual int windowSize() const = 0;
  virtual int minimunImageNumber() const = 0;

  virtual void setUseVisibilityInformation(bool useVisibilityInformation) = 0;
  virtual void setImagesPerCluster(int imagesPerCluster)  = 0;
  virtual void setLevel(int level) = 0;
  virtual void setCellSize(int cellSize) = 0;
  virtual void setThreshold(double threshold) = 0;
  virtual void setWindowSize(int windowSize) = 0;
  virtual void setMinimunImageNumber(int minimunImageNumber) = 0;
};


/*----------------------------------------------------------------*/



class GRAPHOS_EXPORT Smvs
  : public Densification
{

public:

  Smvs() : Densification(Densification::Method::smvs) {}
  ~Smvs() override = default;
    
  virtual int inputImageScale() const = 0;
  virtual int outputDepthScale() const  = 0;
  virtual bool shadingBasedOptimization() const = 0;
  virtual bool semiGlobalMatching() const = 0;
  virtual double surfaceSmoothingFactor() const = 0;

  virtual void setInputImageScale(int inputImageScale) = 0;
  virtual void setOutputDepthScale(int outputDepthScale)  = 0;
  virtual void setShadingBasedOptimization(bool shadingBasedOptimization) = 0;
  virtual void setSemiGlobalMatching(bool semiGlobalMatching) = 0;
  virtual void setSurfaceSmoothingFactor(double surfaceSmoothingFactor) = 0;

};

/*----------------------------------------------------------------*/



class GRAPHOS_EXPORT Mvs
  : public Densification
{

public:

  Mvs() : Densification(Densification::Method::mvs)
  {
  }
  ~Mvs() override = default;

};

} // namespace graphos

#endif // GRAPHOS_CORE_DENSIFICATION_H
