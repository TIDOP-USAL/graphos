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

#ifndef GRAPHOS_CORE_DENSE_MVS_H
#define GRAPHOS_CORE_DENSE_MVS_H

#include "config_graphos.h"

#include "graphos/core/dense/dense.h"

#include <tidop/core/path.h>

#include <QString>

namespace tl
{
class Progress;
}

namespace graphos
{

class ReadCalibration;

namespace internal
{
class Reconstruction;
}

class MvsProperties
  : public Mvs
{

public:

  MvsProperties();
  MvsProperties(const MvsProperties &mvs);
  MvsProperties(MvsProperties &&mvs) noexcept;
  ~MvsProperties() override = default;

  MvsProperties &operator =(const MvsProperties &mvs);
  MvsProperties &operator =(MvsProperties &&mvs) noexcept;

// Mvs interface

public:

  int resolutionLevel() const override;
  int minResolution() const override;
  int maxResolution() const override;
  int numberViews() const override;
  int numberViewsFuse() const override;

  void setResolutionLevel(int resolutionLevel) override;
  void setMinResolution(int minResolution) override;
  void setMaxResolution(int maxResolution) override;
  void setNumberViews(int numberViews) override;
  void setNumberViewsFuse(int numberViewsFuse) override;

// Densification interface

public:

  void reset() override;
  QString name() const override;

protected:

  int mResolutionLevel;
  int mMinResolution;
  int mMaxResolution;
  int mNumberViews;
  int mNumberViewsFuse;

};


/*----------------------------------------------------------------*/


class MvsDensifier
  : public MvsProperties,
    public Densifier
{

public:

  MvsDensifier();
  MvsDensifier(int resolutionLevel,
               int minResolution,
               int maxResolution,
               int numberViews,
               int numberViewsFuse,
               bool cuda = false);
  ~MvsDensifier() override;
  
  MvsDensifier(const MvsDensifier &MvsProcess) = delete;
  MvsDensifier(MvsDensifier &&MvsProcess) = delete;
  MvsDensifier &operator =(const MvsDensifier &MvsProcess) = delete;
  MvsDensifier &operator =(MvsDensifier &&MvsProcess) = delete;

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
  void writeNVMFile();
  void undistortImages();
  void undistortImage();
  void exportToMVS();

// Data members

private:

  bool bOpenCvRead;
  bool bCuda;
  tl::Path mOutputPath;
  internal::Reconstruction *mReconstruction;
  ReadCalibration *mCalibrationReader;
  //std::map<int, std::shared_ptr<Undistort>> mUndistort;
};



class MvsDensifier2
  : public MvsProperties,
    public DensifierBase
{

public:

  MvsDensifier2(const std::unordered_map<size_t, Image> &images,
                const std::map<int, Camera> &cameras,
                const std::unordered_map<size_t, CameraPose> &poses,
                const std::vector<GroundPoint> &groundPoints,
                const QString &outputPath,
                const QString &database,
                bool cuda = false/*,
                const QString &undistortPath = QString()*/);
  ~MvsDensifier2() override;

  MvsDensifier2(const MvsDensifier2 &MvsProcess) = delete;
  MvsDensifier2(MvsDensifier2 &&MvsProcess) = delete;
  MvsDensifier2 &operator =(const MvsDensifier2 &MvsProcess) = delete;
  MvsDensifier2 &operator =(MvsDensifier2 &&MvsProcess) = delete;

private: 

  void clearPreviousModel();
  void writeNVMFile();
  void exportToMVS();
  void densify();

// TaskBase

protected:
  
  void execute(tl::Progress *progressBar = nullptr) override;

//  // DensificationProcess interface
//
//  void undistort(const QString &reconstructionPath,
//                 const QString &outputPath) override;
//  void densify(const QString &undistortPath) override;
//  void enableCuda(bool enable) override;
//
//  // Densification interface
//
//public:
//
//  void reset() override;
//
//  // Member functions
//
//private:
//
//  void clearPreviousModel();
//  void createDirectories();
//  void createDirectory(const std::string &path);
//  void writeNVMFile();
//  void undistortImages();
//  void undistortImage();
//  void exportToMVS();
//
//  // Data members

private:

  QString mDatabase;
//  bool bOpenCvRead;
//  bool bCuda;
//  tl::Path mOutputPath;
//  internal::Reconstruction *mReconstruction;
//  ReadCalibration *mCalibrationReader;
//  //std::map<int, std::shared_ptr<Undistort>> mUndistort;
};



} // namespace graphos

#endif // GRAPHOS_CORE_DENSE_MVS_H
