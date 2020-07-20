#include "CmvsPmvs.h"

// TIDOP LIB
#include <tidop/core/messages.h>
#include <tidop/core/process.h>

// COLMAP
#include <colmap/base/reconstruction.h>
#include <colmap/base/undistortion.h>

#include <QDir>

// BOOST
#include <boost/algorithm/string.hpp>
using namespace inspector;
using namespace tl;
#if (__cplusplus >= 201703L)
namespace fs = std::filesystem;
#else
namespace fs = boost::filesystem;
#endif

namespace inspector
{


CmvsPmvsProperties::CmvsPmvsProperties()
  : mUseVisibilityInformation(true),
    mImagesPerCluster(100),
    mLevel(1),
    mCellSize(2),
    mThreshold(0.7),
    mWindowSize(7),
    mMinimunImageNumber(3)
{
}

CmvsPmvsProperties::CmvsPmvsProperties(const CmvsPmvsProperties &cmvsPmvs)
  : CmvsPmvs(cmvsPmvs),
    mUseVisibilityInformation(cmvsPmvs.mUseVisibilityInformation),
    mImagesPerCluster(cmvsPmvs.mImagesPerCluster),
    mLevel(cmvsPmvs.mLevel),
    mCellSize(cmvsPmvs.mCellSize),
    mThreshold(cmvsPmvs.mThreshold),
    mWindowSize(cmvsPmvs.mWindowSize),
    mMinimunImageNumber(cmvsPmvs.mMinimunImageNumber)
{
}

CmvsPmvsProperties::CmvsPmvsProperties(CmvsPmvsProperties &&cmvsPmvs) noexcept
  : CmvsPmvs(std::forward<CmvsPmvs>(cmvsPmvs)),
    mUseVisibilityInformation(cmvsPmvs.mUseVisibilityInformation),
    mImagesPerCluster(cmvsPmvs.mImagesPerCluster),
    mLevel(cmvsPmvs.mLevel),
    mCellSize(cmvsPmvs.mCellSize),
    mThreshold(cmvsPmvs.mThreshold),
    mWindowSize(cmvsPmvs.mWindowSize),
    mMinimunImageNumber(cmvsPmvs.mMinimunImageNumber)
{
}

CmvsPmvsProperties &CmvsPmvsProperties::operator =(const CmvsPmvsProperties &cmvsPmvs)
{
  if (this != &cmvsPmvs) {
    this->mUseVisibilityInformation = cmvsPmvs.mUseVisibilityInformation;
    this->mImagesPerCluster = cmvsPmvs.mImagesPerCluster;
    this->mLevel = cmvsPmvs.mLevel;
    this->mCellSize = cmvsPmvs.mCellSize;
    this->mThreshold = cmvsPmvs.mThreshold;
    this->mWindowSize = cmvsPmvs.mWindowSize;
    this->mMinimunImageNumber = cmvsPmvs.mMinimunImageNumber;
  }
  return *this;
}

CmvsPmvsProperties &CmvsPmvsProperties::operator =(CmvsPmvsProperties &&cmvsPmvs) noexcept
{
  if (this != &cmvsPmvs) {
    this->mUseVisibilityInformation = cmvsPmvs.mUseVisibilityInformation;
    this->mImagesPerCluster = cmvsPmvs.mImagesPerCluster;
    this->mLevel = cmvsPmvs.mLevel;
    this->mCellSize = cmvsPmvs.mCellSize;
    this->mThreshold = cmvsPmvs.mThreshold;
    this->mWindowSize = cmvsPmvs.mWindowSize;
    this->mMinimunImageNumber = cmvsPmvs.mMinimunImageNumber;
  }
  return *this;
}

bool CmvsPmvsProperties::useVisibilityInformation() const
{
  return mUseVisibilityInformation;
}

int CmvsPmvsProperties::imagesPerCluster() const
{
  return mImagesPerCluster;
}

int CmvsPmvsProperties::level() const
{
  return mLevel;
}

int CmvsPmvsProperties::cellSize() const
{
  return mCellSize;
}

double CmvsPmvsProperties::threshold() const
{
  return mThreshold;
}

int CmvsPmvsProperties::windowSize() const
{
  return mWindowSize;
}

int CmvsPmvsProperties::minimunImageNumber() const
{
  return mMinimunImageNumber;
}

void CmvsPmvsProperties::setUseVisibilityInformation(bool useVisibilityInformation)
{
  mUseVisibilityInformation = useVisibilityInformation;
}

void CmvsPmvsProperties::setImagesPerCluster(int imagesPerCluster)
{
  mImagesPerCluster = imagesPerCluster;
}

void CmvsPmvsProperties::setLevel(int level)
{
  mLevel = level;
}

void CmvsPmvsProperties::setCellSize(int cellSize)
{
  mCellSize = cellSize;
}

void CmvsPmvsProperties::setThreshold(double threshold)
{
  mThreshold = threshold;
}

void CmvsPmvsProperties::setWindowSize(int windowSize)
{
  mWindowSize = windowSize;
}

void CmvsPmvsProperties::setMinimunImageNumber(int minimunImageNumber)
{
  mMinimunImageNumber = minimunImageNumber;
}

void CmvsPmvsProperties::reset()
{
  mUseVisibilityInformation = true;
  mImagesPerCluster = 100;
  mLevel = 1;
  mCellSize = 2;
  mThreshold = 0.7;
  mWindowSize = 7;
  mMinimunImageNumber = 3;
}


QString CmvsPmvsProperties::name() const
{
  return QString("CMVS/PMVS");
}




/*----------------------------------------------------------------*/


CmvsPmvsDensifier::CmvsPmvsDensifier()
{

}

CmvsPmvsDensifier::CmvsPmvsDensifier(const CmvsPmvsDensifier &cmvsPmvsProcess)
  : CmvsPmvsProperties(cmvsPmvsProcess)
{
}

CmvsPmvsDensifier::CmvsPmvsDensifier(CmvsPmvsDensifier &&cmvsPmvsProcess) noexcept
  : CmvsPmvsProperties(std::forward<CmvsPmvsProperties>(cmvsPmvsProcess))
{
}

CmvsPmvsDensifier::CmvsPmvsDensifier(bool useVisibilityInformation,
                                     int imagesPerCluster,
                                     int level,
                                     int cellSize,
                                     double threshold,
                                     int windowSize,
                                     int minimunImageNumber)
{
  CmvsPmvsProperties::setUseVisibilityInformation(useVisibilityInformation);
  CmvsPmvsProperties::setImagesPerCluster(imagesPerCluster);
  CmvsPmvsProperties::setLevel(level);
  CmvsPmvsProperties::setCellSize(cellSize);
  CmvsPmvsProperties::setThreshold(threshold);
  CmvsPmvsProperties::setWindowSize(windowSize);
  CmvsPmvsProperties::setMinimunImageNumber(minimunImageNumber);
}

CmvsPmvsDensifier &CmvsPmvsDensifier::operator =(const CmvsPmvsDensifier &cmvsPmvsProcess)
{
  if (this != &cmvsPmvsProcess){
    CmvsPmvsProperties::operator=(cmvsPmvsProcess);
  }
  return *this;
}

CmvsPmvsDensifier &CmvsPmvsDensifier::operator =(CmvsPmvsDensifier &&cmvsPmvsProcess) noexcept
{
  if (this != &cmvsPmvsProcess){
    CmvsPmvsProperties::operator=(std::forward<CmvsPmvsProperties>(cmvsPmvsProcess));
  }
  return *this;
}

bool CmvsPmvsDensifier::undistort(const QString &reconstructionPath,
                                  const QString &imagesPath,
                                  const QString &outputPath)
{

  colmap::Reconstruction reconstruction;
#ifdef _DEBUG
  //Lectura como texto
  reconstruction.ReadText(reconstructionPath.toStdString());
#else
  //Lectura como binario
  reconstruction.ReadBinary(reconstructionPath.toStdString());
#endif

  QDir dir(outputPath);
  if (!dir.exists()) {
    if (dir.mkpath(".") == false) {
      msgError("The output directory cannot be created: %s", outputPath.toStdString().c_str());
      return true;
    }
  }

  /// Exportar
  colmap::UndistortCameraOptions undistortion_options;
  colmap::PMVSUndistorter *undistorter = new colmap::PMVSUndistorter(undistortion_options, 
                                                                     reconstruction,
                                                                     imagesPath.toStdString(),
                                                                     outputPath.toStdString());
  undistorter->Start();
  undistorter->Wait();

  return false;
}

bool CmvsPmvsDensifier::densify(const QString &undistortPath)
{

  fs::path app_path(tl::getRunfile());
  std::string cmd_cmvs("/c \"");
  cmd_cmvs.append(app_path.parent_path().string());
  cmd_cmvs.append("\\pmvs2\" ");
  cmd_cmvs.append(undistortPath.toStdString());
  cmd_cmvs.append("/pmvs/ option-all");
  CmdProcess process(cmd_cmvs);

  if (process.run() == Process::Status::error) {
    return true;
  }

  return false;
}

void CmvsPmvsDensifier::reset()
{
  CmvsPmvsProperties::reset();
}

} // namespace inspector
