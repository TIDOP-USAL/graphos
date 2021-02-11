#ifndef INSPECTOR_SMVS_H
#define INSPECTOR_SMVS_H

#include "config_inspector.h"

#include "inspector/core/densification/densification.h"

#include <QString>

namespace colmap
{
class Reconstruction;
}


namespace inspector
{


class INSPECTOR_EXPORT SmvsProperties
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


class INSPECTOR_EXPORT SmvsDensifier
  : public SmvsProperties,
    public Densifier
{

public:

  SmvsDensifier();
  //SmvsDensifier(const SmvsDensifier &smvs);
  //SmvsDensifier(SmvsDensifier &&smvs) noexcept;
  SmvsDensifier(int inputImageScale,
                int outputDepthScale,
                bool shadingBasedOptimization,
                bool semiGlobalMatching,
                double surfaceSmoothingFactor);
  ~SmvsDensifier() override;
  //SmvsDensifier &operator =(const SmvsDensifier &smvs);
  //SmvsDensifier &operator =(SmvsDensifier &&smvs) noexcept;

  SmvsDensifier(const SmvsDensifier &smvs) = delete;
  SmvsDensifier(SmvsDensifier &&smvs) = delete;
  SmvsDensifier &operator =(const SmvsDensifier &smvs) = delete;
  SmvsDensifier &operator =(SmvsDensifier &&smvs) = delete;

// DensificationProcess interface
 
public:

  bool undistort(const QString &reconstructionPath,
                 const QString &imagesPath,
                 const QString &outputPath) override;
  bool densify(const QString &undistortPath) override;
  void enableCuda(bool enable) override;

// Densification interface

public:

  void reset() override;

// Member functions

private:

  void createDirectories();
  void createDirectory(const std::string &path);
  void writeMVEFile();
  void undistortImages();

// Data members

private:

  bool bOpenCvRead;
  bool bCuda;
  std::string mOutputPath;
  std::string mImagesPath;
  colmap::Reconstruction *mReconstruction;
};


} // namespace inspector

#endif // INSPECTOR_SMVS_H
