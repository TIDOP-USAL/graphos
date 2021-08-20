#ifndef GRAPHOS_SMVS_H
#define GRAPHOS_SMVS_H

#include "config_graphos.h"

#include "graphos/core/densification/densification.h"

#include <QString>


namespace graphos
{

namespace internal
{
class Reconstruction;
}

class GRAPHOS_EXPORT SmvsProperties
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

  int inputImageScale() const override;
  int outputDepthScale() const override;
  bool shadingBasedOptimization() const override;
  bool semiGlobalMatching() const override;
  double surfaceSmoothingFactor() const override;
  void setInputImageScale(int inputImageScale) override;
  void setOutputDepthScale(int outputDepthScale) override;
  void setShadingBasedOptimization(bool shadingBasedOptimization) override;
  void setSemiGlobalMatching(bool semiGlobalMatching) override;
  void setSurfaceSmoothingFactor(double surfaceSmoothingFactor) override;

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


class GRAPHOS_EXPORT SmvsDensifier
  : public SmvsProperties,
    public Densifier
{

public:

  SmvsDensifier();
  SmvsDensifier(int inputImageScale,
                int outputDepthScale,
                bool shadingBasedOptimization,
                bool semiGlobalMatching,
                double surfaceSmoothingFactor);
  ~SmvsDensifier() override;

  SmvsDensifier(const SmvsDensifier &smvs) = delete;
  SmvsDensifier(SmvsDensifier &&smvs) = delete;
  SmvsDensifier &operator =(const SmvsDensifier &smvs) = delete;
  SmvsDensifier &operator =(SmvsDensifier &&smvs) = delete;

// DensificationProcess interface
 
public:

  bool undistort(const QString &reconstructionPath,
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
  internal::Reconstruction *mReconstruction;
};


} // namespace graphos

#endif // GRAPHOS_SMVS_H
