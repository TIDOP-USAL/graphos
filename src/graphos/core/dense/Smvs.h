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

#include <unordered_map>

namespace graphos
{

class SmvsProperties
  : public Smvs
{

public:

  SmvsProperties();
  SmvsProperties(int inputImageScale,
                 int outputDepthScale,
                 bool shadingBasedOptimization,
                 bool semiGlobalMatching,
                 double surfaceSmoothingFactor);
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
    public DensifierBase
{

public:

  SmvsDensifier(const std::unordered_map<size_t, Image> &images,
                const std::map<int, Camera> &cameras,
                const std::unordered_map<size_t, CameraPose> &poses,
                const std::vector<GroundPoint> &groundPoints,
                const QString &outputPath,
                bool cuda = false);
  ~SmvsDensifier() override;

  SmvsDensifier(const SmvsDensifier &smvs) = delete;
  SmvsDensifier(SmvsDensifier &&smvs) = delete;
  SmvsDensifier &operator =(const SmvsDensifier &smvs) = delete;
  SmvsDensifier &operator =(SmvsDensifier &&smvs) = delete;

private:

  void clearPreviousModel();
  void writeMVEFile();
  void densify();

// TaskBase

protected:

  void execute(tl::Progress *progressBar = nullptr) override;

private:

  std::unordered_map<size_t, size_t> mGraphosToMveIds;

};



} // namespace graphos

#endif // GRAPHOS_CORE_DENSE_SMVS_H
