#ifndef INSPECTOR_SMVS_H
#define INSPECTOR_SMVS_H

#include "config_inspector.h"

#include "inspector/core/densification/densification.h"

#include <QString>

namespace inspector
{


class TL_EXPORT SmvsProperties
  : public Smvs
{

public:

  SmvsProperties();
  SmvsProperties(const SmvsProperties &smvs);
  SmvsProperties(SmvsProperties &&smvs) noexcept;
  ~SmvsProperties() override = default;

  SmvsProperties &operator =(const SmvsProperties &smvs);
  SmvsProperties &operator =(SmvsProperties &&smvs) noexcept;

// Smvs interface

public:

  virtual int inputImageScale() const override;
  virtual int outputDepthScale() const override;
  virtual bool shadingBasedOptimization() const override;
  virtual bool semiGlobalMatching() const override;
  virtual double surfaceSmoothingFactor() const override;
  virtual void setInputImageScale(int inputImageScale) override;
  virtual void setOutputDepthScale(int outputDepthScale) override;
  virtual void setShadingBasedOptimization(bool shadingBasedOptimization) override;
  virtual void setSemiGlobalMatching(bool semiGlobalMatching) override;
  virtual void setSurfaceSmoothingFactor(double surfaceSmoothingFactor) override;

// Densification interface

public:

  void reset() override;
  QString name() const final;

private:

  int mInputImageScale;
  int mOutputDepthScale;
  bool mShadingBasedOptimization;
  bool mSemiGlobalMatching;
  double mSurfaceSmoothingFactor;
};


/*----------------------------------------------------------------*/


class TL_EXPORT SmvsProcess
  : public SmvsProperties,
    public DensificationProcess
{

public:

  SmvsProcess();
  SmvsProcess(const SmvsProcess &smvs);
  SmvsProcess(SmvsProcess &&smvs) noexcept;
  SmvsProcess(int inputImageScale,
              int outputDepthScale,
              bool shadingBasedOptimization,
              bool semiGlobalMatching,
              double surfaceSmoothingFactor);
  ~SmvsProcess() override = default;
  SmvsProcess &operator =(const SmvsProcess &smvs);
  SmvsProcess &operator =(SmvsProcess &&smvs) noexcept;


// DensificationProcess interface
 
public:

  bool undistort(const std::string &reconstructionPath, 
                 const std::string &imagesPath, 
                 const std::string &outputPath) override;
  bool densify(const std::string &undistortPath) override;

// Densification interface

public:

  void reset() override;

protected:

  //cv::Ptr<cv::AgastFeatureDetector> mAgast;




};


} // namespace inspector

#endif // INSPECTOR_SMVS_H
