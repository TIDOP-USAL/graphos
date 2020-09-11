#ifndef INSPECTOR_CORE_ORIENTATION_COLMAP_H
#define INSPECTOR_CORE_ORIENTATION_COLMAP_H

#include "inspector/core/orientation/orientation.h"

#include <memory>
#include <map>

namespace colmap
{
class ReconstructionManager;
struct IncrementalMapperOptions;
class IncrementalMapperController;
}

namespace inspector
{

class TL_EXPORT RelativeOrientationColmapProperties
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


class TL_EXPORT RelativeOrientationColmapAlgorithm
  : public RelativeOrientationColmapProperties,
    public RelativeOrientationAlgorithm
{

public:

  RelativeOrientationColmapAlgorithm(const QString &database,
                           const QString &imagePath,
                           const QString &outputPath);
  ~RelativeOrientationColmapAlgorithm() override;

// RelativeOrientationAlgorithm interface

public:

  void run() override;

private:

  QString mDatabase;
  QString mImagePath;
  QString mOutputPath;
  colmap::IncrementalMapperOptions *mIncrementalMapper;
  colmap::IncrementalMapperController *mMapper;
  std::shared_ptr<colmap::ReconstructionManager> mReconstructionManager;
};



/*----------------------------------------------------------------*/

class TL_EXPORT AbsoluteOrientationColmapProperties
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



} // namespace inspector

#endif // INSPECTOR_CORE_ORIENTATION_COLMAP_H
