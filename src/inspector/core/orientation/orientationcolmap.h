#ifndef INSPECTOR_CORE_ORIENTATION_COLMAP_H
#define INSPECTOR_CORE_ORIENTATION_COLMAP_H

#include "inspector/core/orientation/orientation.h"

#include <memory>

namespace colmap
{
class ReconstructionManager;
struct IncrementalMapperOptions;
class IncrementalMapperController;
}

namespace inspector
{

class TL_EXPORT OrientationColmapProperties
  : public Orientation
{

public:

  OrientationColmapProperties();
  ~OrientationColmapProperties() override = default;

  virtual bool refineFocalLength() const;
  virtual void setRefineFocalLength(bool refineFocalLength);

  virtual bool refinePrincipalPoint() const;
  virtual void setRefinePrincipalPoint(bool refinePrincipalPoint);

  virtual bool refineExtraParams() const;
  virtual void setRefineExtraParams(bool refineExtraParams);

// Orientation interface

public:

  void reset() override;
  QString name() const override;

private:

  bool mRefineFocalLength;
  bool mRefinePrincipalPoint;
  bool mRefineExtraParams;
};


/*----------------------------------------------------------------*/


class TL_EXPORT OrientationColmapProcess
  : public OrientationColmapProperties,
    public OrientationProcess
{

public:

  OrientationColmapProcess(const QString &database,
                           const QString &imagePath,
                           const QString &outputPath);
  ~OrientationColmapProcess() override;

// OrientationProcess interface

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


class AbsoluteOrientationColmapProcess
  : public AbsoluteOrientationColmapProperties,
    public AbsoluteOrientationProcess
{

public:

  AbsoluteOrientationColmapProcess(const QString &inputPath,
                                   const QString &imagePath,
                                   const QString &outputPath);
  ~AbsoluteOrientationColmapProcess() override;

// AbsoluteOrientationProcess interface

public:

  void run() override;

private:

  QString mInputPath;
  QString mImagePath;
  QString mOutputPath;
  std::shared_ptr<colmap::ReconstructionManager> mReconstructionManager;
};



} // namespace inspector

#endif // INSPECTOR_CORE_ORIENTATION_COLMAP_H
