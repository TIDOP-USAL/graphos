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

#ifndef GRAPHOS_CORE_DENSE_SMVS_H
#define GRAPHOS_CORE_DENSE_SMVS_H

#include "config_graphos.h"

#include "graphos/core/dense/dense.h"

#include <tidop/core/path.h>

#include <QString>


namespace graphos
{

class ReadCalibration;

namespace internal
{
class Reconstruction;
}

class SmvsProperties
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


class SmvsDensifier
  : public SmvsProperties,
    public Densifier
{

public:

  SmvsDensifier();
  SmvsDensifier(int inputImageScale,
                int outputDepthScale,
                bool shadingBasedOptimization,
                bool semiGlobalMatching,
                double surfaceSmoothingFactor,
                bool cuda = false);
  ~SmvsDensifier() override;

  SmvsDensifier(const SmvsDensifier &smvs) = delete;
  SmvsDensifier(SmvsDensifier &&smvs) = delete;
  SmvsDensifier &operator =(const SmvsDensifier &smvs) = delete;
  SmvsDensifier &operator =(SmvsDensifier &&smvs) = delete;

// DensificationProcess interface
 
public:

  void undistort(const QString &reconstructionPath,
                 const QString &outputPath) override;
  void densify(const QString &undistortPath) override;
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
  tl::Path mOutputPath;
  internal::Reconstruction *mReconstruction;
  ReadCalibration *mCalibrationReader;
};


} // namespace graphos

#endif // GRAPHOS_CORE_DENSE_SMVS_H
