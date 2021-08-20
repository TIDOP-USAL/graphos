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
    smvs
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
                         const QString &outputPath) = 0;
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


} // namespace graphos

#endif // GRAPHOS_CORE_DENSIFICATION_H
