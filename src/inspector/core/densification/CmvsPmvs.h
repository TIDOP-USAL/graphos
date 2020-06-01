#ifndef INSPECTOR_CMVS_PMVS_H
#define INSPECTOR_CMVS_PMVS_H

#include "config_inspector.h"

#include "inspector/core/densification/densification.h"

#include <QString>

namespace inspector
{


class TL_EXPORT CmvsPmvsProperties
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
};


/*----------------------------------------------------------------*/


class TL_EXPORT CmvsPmvsProcess
  : public CmvsPmvsProperties,
    public DensificationProcess
{

public:

  CmvsPmvsProcess();
  CmvsPmvsProcess(const CmvsPmvsProcess &cmvsPmvsProcess);
  CmvsPmvsProcess(CmvsPmvsProcess &&cmvsPmvsProcess) noexcept;
  CmvsPmvsProcess(bool useVisibilityInformation,
                  int imagesPerCluster,
                  int level,
                  int cellSize,
                  double threshold,
                  int windowSize,
                  int minimunImageNumber);
  ~CmvsPmvsProcess() override = default;
  CmvsPmvsProcess &operator =(const CmvsPmvsProcess &cmvsPmvsProcess);
  CmvsPmvsProcess &operator =(CmvsPmvsProcess &&cmvsPmvsProcess) noexcept;


// Heredado vía DensificationProcess

  bool undistort(const std::string &reconstructionPath, 
                 const std::string &imagesPath, 
                 const std::string &outputPath) override;
  bool densify(const std::string &undistortPath) override;

// Densification interface

public:

  void reset() override;


protected:

  //cv::Ptr<cv::AgastFeatureDetector> mAgast;


};


} // namespace inspector

#endif // INSPECTOR_CMVS_PMVS_H
