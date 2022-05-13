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

#ifndef GRAPHOS_CORE_ORIENTATION_COLMAP_H
#define GRAPHOS_CORE_ORIENTATION_COLMAP_H

#include "graphos/core/sfm/orientation.h"

#include <memory>
#include <map>
#include <vector>

namespace colmap
{
class ReconstructionManager;
struct IncrementalMapperOptions;
class IncrementalMapperController;
}

namespace graphos
{

class RelativeOrientationColmapProperties
  : public RelativeOrientation
{

public:

  RelativeOrientationColmapProperties();
  ~RelativeOrientationColmapProperties() override = default;

  virtual bool refineFocalLength() const;
  virtual void setRefineFocalLength(bool refineFocalLength);

  virtual bool refinePrincipalPoint() const;
  virtual void setRefinePrincipalPoint(bool refinePrincipalPoint);

  virtual bool refineExtraParams() const;
  virtual void setRefineExtraParams(bool refineExtraParams);

// RelativeOrientation interface

public:

  void reset() override;
  QString name() const override;

private:

  bool mRefineFocalLength;
  bool mRefinePrincipalPoint;
  bool mRefineExtraParams;
};


/*----------------------------------------------------------------*/


class RelativeOrientationColmapAlgorithm
  : public RelativeOrientationColmapProperties,
    public RelativeOrientationAlgorithm
{

public:

  RelativeOrientationColmapAlgorithm(const QString &database,
                                     const QString &outputPath,
                                     bool fixCalibration);
  ~RelativeOrientationColmapAlgorithm() override;

// RelativeOrientationAlgorithm interface

public:

  void run() override;

private:

  QString mDatabase;
  QString mOutputPath;
  colmap::IncrementalMapperOptions *mIncrementalMapper;
  colmap::IncrementalMapperController *mMapper;
  std::shared_ptr<colmap::ReconstructionManager> mReconstructionManager;
};



/*----------------------------------------------------------------*/

class AbsoluteOrientationColmapProperties
  : public AbsoluteOrientation
{

public:

  AbsoluteOrientationColmapProperties();
  ~AbsoluteOrientationColmapProperties() override = default;

  virtual int minCommonImages() const;
  virtual void setMinCommonImages(int minCommonImages);

  virtual bool robustAlignment() const;
  virtual void setRobustAlignment(bool robustAlignment);

  virtual double robustAlignmentMaxError() const;
  virtual void setRobustAlignmentMaxError(double robustAlignmentMaxError);

// AbsoluteOrientation Interface

public:

  void reset() override;
  QString name() const override;

private:

  int mMinCommonImages;
  bool mRobustAlignment;
  double mRobustAlignmentMaxError;
};


/*----------------------------------------------------------------*/


class AbsoluteOrientationColmapAlgorithm
  : public AbsoluteOrientationColmapProperties,
    public AbsoluteOrientationAlgorithm
{

public:

  AbsoluteOrientationColmapAlgorithm(const QString &inputPath,
                                     const std::map<QString, std::array<double, 3>>  &cameraPositions,
                                     const QString &outputPath);
  ~AbsoluteOrientationColmapAlgorithm() override;

// AbsoluteOrientationAlgorithm interface

public:

  void run() override;

private:

  QString mInputPath;
  std::map<QString, std::array<double, 3>> mCameraPositions;
  QString mOutputPath;
  
};


/*----------------------------------------------------------------*/

class AbsoluteOrientationColmapProperties2
  : public AbsoluteOrientation
{

public:

  AbsoluteOrientationColmapProperties2();
  ~AbsoluteOrientationColmapProperties2() override = default;

// AbsoluteOrientation Interface

public:

  void reset() override;
  QString name() const override;

private:

};


/*----------------------------------------------------------------*/


class AbsoluteOrientationColmapAlgorithm2
  : public AbsoluteOrientationColmapProperties2,
    public AbsoluteOrientationAlgorithm
{

public:

  AbsoluteOrientationColmapAlgorithm2(const QString &inputPath,
                                     const std::map<QString, std::array<double, 7>>  &cameraPositions,
                                     const QString &outputPath);
  ~AbsoluteOrientationColmapAlgorithm2() override;

// AbsoluteOrientationAlgorithm interface

public:

  void run() override;

private:

  QString mInputPath;
  std::map<QString, std::array<double, 7>> mCameraPositions;
  QString mOutputPath;
  
};


/*----------------------------------------------------------------*/

void colmapRemoveOrientations(const std::vector<std::string> &images, 
                              const std::string &reconstruction);


} // namespace graphos

#endif // GRAPHOS_CORE_ORIENTATION_COLMAP_H
