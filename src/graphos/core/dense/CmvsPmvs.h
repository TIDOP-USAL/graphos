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

#ifndef GRAPHOS_CORE_DENSE_CMVS_PMVS_H
#define GRAPHOS_CORE_DENSE_CMVS_PMVS_H

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

class CmvsPmvsProperties
  : public CmvsPmvs
{

public:

  CmvsPmvsProperties();
  CmvsPmvsProperties(const CmvsPmvsProperties &cmvsPmvs);
  CmvsPmvsProperties(CmvsPmvsProperties &&cmvsPmvs) noexcept;
  ~CmvsPmvsProperties() override = default;

  CmvsPmvsProperties &operator =(const CmvsPmvsProperties &cmvsPmvs);
  CmvsPmvsProperties &operator =(CmvsPmvsProperties &&cmvsPmvs) noexcept;

// CmvsPmvs interface

public:

  bool useVisibilityInformation() const override;
  int imagesPerCluster() const override;
  int level() const override;
  int cellSize() const override;
  double threshold() const override;
  int windowSize() const override;
  int minimunImageNumber() const override;

  void setUseVisibilityInformation(bool useVisibilityInformation) override;
  void setImagesPerCluster(int imagesPerCluster) override;
  void setLevel(int level) override;
  void setCellSize(int cellSize) override;
  void setThreshold(double threshold) override;
  void setWindowSize(int windowSize) override;
  void setMinimunImageNumber(int minimunImageNumber) override;

// Densification interface

public:

  void reset() override;
  QString name() const override;

protected:

  bool mUseVisibilityInformation;
  int mImagesPerCluster;
  int mLevel;
  int mCellSize;
  double mThreshold;
  int mWindowSize;
  int mMinimunImageNumber;
  bool mImageOriginalDepth{false};
};


/*----------------------------------------------------------------*/


class CmvsPmvsDensifier
  : public CmvsPmvsProperties,
    public Densifier
{

public:

  CmvsPmvsDensifier();
  CmvsPmvsDensifier(bool useVisibilityInformation,
                    int imagesPerCluster,
                    int level,
                    int cellSize,
                    double threshold,
                    int windowSize,
                    int minimunImageNumber,
                    bool cuda);
  ~CmvsPmvsDensifier() override;
  
  CmvsPmvsDensifier(const CmvsPmvsDensifier &cmvsPmvsProcess) = delete;
  CmvsPmvsDensifier(CmvsPmvsDensifier &&cmvsPmvsProcess) = delete;
  CmvsPmvsDensifier &operator =(const CmvsPmvsDensifier &cmvsPmvsProcess) = delete;
  CmvsPmvsDensifier &operator =(CmvsPmvsDensifier &&cmvsPmvsProcess) = delete;

// DensificationProcess interface


  void undistort(const QString &reconstructionPath,
                 const QString &outputPath) override;
  void densify(const QString &undistortPath) override;
  void enableCuda(bool enable) override;

// Densification interface

public:

  void reset() override;

// Member functions

private:

  void clearPreviousModel();
  void createDirectories();
  void createDirectory(const std::string &path);
  void writeBundleFile();
  void undistortImages();
  void undistortImage();
  void writeVisibility();
  void writeOptions();

// Data members

private:

  bool bOpenCvRead;
  bool bCuda;
  tl::Path mOutputPath;
  internal::Reconstruction *mReconstruction;
  ReadCalibration *mCalibrationReader;
};


} // namespace graphos

#endif // GRAPHOS_CORE_DENSE_CMVS_PMVS_H
