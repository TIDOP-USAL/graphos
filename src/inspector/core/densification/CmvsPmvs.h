#ifndef INSPECTOR_CMVS_PMVS_H
#define INSPECTOR_CMVS_PMVS_H

#include "config_inspector.h"

#include "inspector/core/densification/densification.h"

#include <QString>

namespace inspector
{


class INSPECTOR_EXPORT CmvsPmvsProperties
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


class INSPECTOR_EXPORT CmvsPmvsDensifier
  : public CmvsPmvsProperties,
    public Densifier
{

public:

  CmvsPmvsDensifier();
  CmvsPmvsDensifier(const CmvsPmvsDensifier &cmvsPmvsProcess);
  CmvsPmvsDensifier(CmvsPmvsDensifier &&cmvsPmvsProcess) noexcept;
  CmvsPmvsDensifier(bool useVisibilityInformation,
                    int imagesPerCluster,
                    int level,
                    int cellSize,
                    double threshold,
                    int windowSize,
                    int minimunImageNumber);
  ~CmvsPmvsDensifier() override = default;
  CmvsPmvsDensifier &operator =(const CmvsPmvsDensifier &cmvsPmvsProcess);
  CmvsPmvsDensifier &operator =(CmvsPmvsDensifier &&cmvsPmvsProcess) noexcept;


// Heredado vía DensificationProcess

  bool undistort(const QString &reconstructionPath,
                 const QString &imagesPath,
                 const QString &outputPath) override;
  bool densify(const QString &undistortPath) override;
  void enableCuda(bool enable) override;

// Densification interface

public:

  void reset() override;

private:

  bool bOpenCvRead;
  bool bCuda;
};


} // namespace inspector

#endif // INSPECTOR_CMVS_PMVS_H
