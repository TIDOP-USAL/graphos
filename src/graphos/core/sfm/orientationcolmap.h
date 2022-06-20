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
#include <unordered_map>
#include <vector>

#include <tidop/core/task.h>

namespace tl
{
class Progress;
}

namespace colmap
{
class ReconstructionManager;
struct IncrementalMapperOptions;
class IncrementalMapperController;
class BundleAdjustmentController;
}

namespace graphos
{

class Image;
class Camera;

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


class RelativeOrientationColmapTask
  : public RelativeOrientationColmapProperties,
    public tl::TaskBase
{

public:

  RelativeOrientationColmapTask(const QString &database,
                                const QString &outputPath,
                                const std::vector<Image> &images,
                                const std::map<int, Camera> &cameras,
                                bool fixCalibration);
  ~RelativeOrientationColmapTask() override;

  std::map<int, Camera> cameras() const;

// tl::TaskBase

public:

  void stop() override;

protected:

  void execute(tl::Progress *progressBar = nullptr) override;

private:

  QString mDatabase;
  QString mOutputPath;
  std::vector<Image> mImages;
  std::map<int, Camera> mCameras;
  colmap::IncrementalMapperOptions *mIncrementalMapper;
  colmap::IncrementalMapperController *mMapper;
  colmap::BundleAdjustmentController *mBundleAdjustmentController;
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


class AbsoluteOrientationColmapTask
  : public AbsoluteOrientationColmapProperties,
    public tl::TaskBase
{

public:

  AbsoluteOrientationColmapTask(const QString &path,
                                const std::vector<Image> &images);
  ~AbsoluteOrientationColmapTask() override;

  std::unordered_map<size_t, double> cameraPosesErrors() const;

// tl::TaskBase interface

public:

  void stop() override;

protected:

  void execute(tl::Progress *progressBar) override;

private:

  QString mInputPath;
  std::vector<Image> mImages;
  std::unordered_map<size_t, double> mCameraPosesErrors;
};


/*----------------------------------------------------------------*/

void colmapRemoveOrientations(const std::vector<std::string> &images, 
                              const std::string &reconstruction);




} // namespace graphos

#endif // GRAPHOS_CORE_ORIENTATION_COLMAP_H