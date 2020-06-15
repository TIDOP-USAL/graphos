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


} // namespace inspector

#endif // INSPECTOR_CORE_ORIENTATION_COLMAP_H
