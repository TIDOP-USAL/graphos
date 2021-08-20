#ifndef GRAPHOS_CMVS_PMVS_H
#define GRAPHOS_CMVS_PMVS_H

#include "config_graphos.h"

#include "graphos/core/densification/densification.h"

#include <QString>

namespace graphos
{

namespace internal
{
class Reconstruction;
}

class GRAPHOS_EXPORT CmvsPmvsProperties
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


class GRAPHOS_EXPORT CmvsPmvsDensifier
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
                    int minimunImageNumber);
  ~CmvsPmvsDensifier() override;
  
  CmvsPmvsDensifier(const CmvsPmvsDensifier &cmvsPmvsProcess) = delete;
  CmvsPmvsDensifier(CmvsPmvsDensifier &&cmvsPmvsProcess) = delete;
  CmvsPmvsDensifier &operator =(const CmvsPmvsDensifier &cmvsPmvsProcess) = delete;
  CmvsPmvsDensifier &operator =(CmvsPmvsDensifier &&cmvsPmvsProcess) = delete;

// DensificationProcess interface

  bool undistort(const QString &reconstructionPath,
                 const QString &outputPath) override;
  bool densify(const QString &undistortPath) override;
  void enableCuda(bool enable) override;

// Densification interface

public:

  void reset() override;

// Member functions

private:

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
  std::string mOutputPath;
  internal::Reconstruction *mReconstruction;
};


} // namespace graphos

#endif // GRAPHOS_CMVS_PMVS_H
