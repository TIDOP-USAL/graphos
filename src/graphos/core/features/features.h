#ifndef GRAPHOS_FEATURES_H
#define GRAPHOS_FEATURES_H

#include "graphos/graphos_global.h"

#include <memory>

#include <QSize>
#include <QString>

#include <opencv2/features2d.hpp>

#include <colmap/feature/sift.h>

#include <tidop/core/flags.h>


namespace graphos
{


class GRAPHOS_EXPORT Feature
{

public:

  enum class Type
  {
    akaze,
    sift
  };

public:

  Feature(Type type) : mFeatType(type) {}
  virtual ~Feature() = default;

  /*!
   * \brief Recover the default values
   */
  virtual void reset() = 0;

  Type type() const { return mFeatType.flags(); }
  virtual QString name() const = 0;

protected:

  tl::EnumFlags<Type> mFeatType;

};
ALLOW_BITWISE_FLAG_OPERATIONS(Feature::Type)



/*----------------------------------------------------------------*/



class GRAPHOS_EXPORT KeypointDetector
{

public:

  KeypointDetector() {}
  virtual ~KeypointDetector() = default;

  /*!
   * \brief Detects keypoints in an image
   * \param[in] img Image
   * \param[out] keyPoints The detected keypoints
   * \param[in] mask Optional mask
   * \return true if error
   */
  virtual bool detect(const cv::Mat &img,
                      std::vector<cv::KeyPoint> &keyPoints,
                      cv::InputArray &mask = cv::noArray()) = 0;

};



/*----------------------------------------------------------------*/



class GRAPHOS_EXPORT DescriptorExtractor
{

public:

  DescriptorExtractor() {}
  virtual ~DescriptorExtractor() = default;

  /*!
   * \brief Extract descriptors
   * \param[in] img Image
   * \param[in] keyPoints KeyPoints
   * \param[out] descriptors Computed descriptors
   * \return true if error
   */
  virtual bool extract(const cv::Mat &img,
                       std::vector<cv::KeyPoint> &keyPoints,
                       cv::Mat &descriptors) = 0;

};



/*----------------------------------------------------------------*/


class GRAPHOS_EXPORT FeatureExtractor
{

public:

  FeatureExtractor() {}
  virtual ~FeatureExtractor() = default;

  /*!
   * \brief Detect keypoints and extract descriptors
   * \param[in] bitmap Image
   * \param[out] keyPoints Detected keypoints
   * \param[out] descriptors Computed descriptors
   */
  virtual void run(const colmap::Bitmap &bitmap,
                   colmap::FeatureKeypoints &keyPoints,
                   colmap::FeatureDescriptors &descriptors) = 0;

  virtual void run(const cv::Mat &bitmap,
                   colmap::FeatureKeypoints &keyPoints,
                   colmap::FeatureDescriptors &descriptors) = 0;

};


/*----------------------------------------------------------------*/



/*!
 * \brief The IAkaze class
 */
class GRAPHOS_EXPORT Akaze
  : public Feature
{

public:

  Akaze() : Feature(Feature::Type::akaze) {}
  ~Akaze() override = default;

  /*!
   * \brief Type of the extracted descriptor
   * \return KAZE, KAZE_UPRIGHT, MLDB or MLDB_UPRIGHT
   */
  virtual QString descriptorType() const = 0;

  /*!
   * \brief Size of the descriptor in bits. 0 -\> Full size
   * \return Descriptor size
   */
  virtual int descriptorSize() const = 0;

  /*!
   * \brief Number of channels in the descriptor [1, 2, 3 (default)]
   * \return Number of channels
   */
  virtual int descriptorChannels() const = 0;

  /*!
   * \brief Detector response threshold to accept point
   * \return threshold
   */
  virtual double threshold() const = 0;

  /*!
   * \brief Maximum octave evolution of the image
   * \return
   */
  virtual int octaves() const = 0;

  /*!
   * \brief Default number of sublevels per scale level
   * \return
   */
  virtual int octaveLayers() const = 0;

  /*!
   * \brief Diffusivity type
   * \return DIFF_PM_G1, DIFF_PM_G2 (default), DIFF_WEICKERT or DIFF_CHARBONNIER
   */
  virtual QString diffusivity() const = 0;

  /*!
   * \brief Set the type of the extracted descriptor
   * \param[in] descriptorType Descriptor type: KAZE, KAZE_UPRIGHT, MLDB or MLDB_UPRIGHT
   */
  virtual void setDescriptorType(const QString &descriptorType) = 0;

  /*!
   * \brief Set the size of the descriptor in bits.
   * \param[in] descriptorSize
   */
  virtual void setDescriptorSize(int descriptorSize) = 0;

  /*!
   * \brief Set the number of channels in the descriptor
   * \param[in] channels 1, 2 or 3 (default)
   */
  virtual void setDescriptorChannels(int channels) = 0;

  /*!
   * \brief Set the detector response threshold to accept point
   * \param[in] threshold
   */
  virtual void setThreshold(double threshold) = 0;

  /*!
   * \brief Set the maximum octave evolution of the image
   * \param[in] octaves
   */
  virtual void setOctaves(int octaves) = 0;

  /*!
   * \brief Sets the default number of sublevels per scale level
   * \param[in] octaveLayers
   */
  virtual void setOctaveLayers(int octaveLayers) = 0;

  /*!
   * \brief Set diffusivity type
   * \param[in] diffusivity DIFF_PM_G1, DIFF_PM_G2 (default), DIFF_WEICKERT or DIFF_CHARBONNIER
   */
  virtual void setDiffusivity(const QString &diffusivity) = 0;

};



/*----------------------------------------------------------------*/



class GRAPHOS_EXPORT Sift
  : public Feature
{
public:
  Sift() : Feature(Feature::Type::sift)  {}
  ~Sift() override = default;

  /*!
   * \brief The number of best features to retain
   * The features are ranked by their scores (measured in
   * SIFT algorithm as the local contrast)
   * \return
   */
  virtual int featuresNumber() const = 0;

  /*!
   * \brief The number of layers in each octave.
   * 3 is the value used in D. Lowe paper. The number of octaves
   * is computed automatically from the image resolution.
   * \return
   */
  virtual int octaveLayers() const = 0;

  virtual bool constrastThresholdAuto() const = 0;

  /*!
   * \brief The contrast threshold used to filter out weak features in semi-uniform (low-contrast) regions.
   * The larger the threshold, the less features are produced by the detector.
   * \return
   */
  virtual double contrastThreshold() const = 0;

  /*!
   * \brief The threshold used to filter out edge-like features
   * Note that the its meaning is different from the contrastThreshold, i.e. the larger
   * the edgeThreshold, the less features are filtered out (more features are retained).
   * \return
   */
  virtual double edgeThreshold() const = 0;

  /*!
   * \brief The sigma of the Gaussian applied to the input image at the octave 0.
   * If your image is captured with a weak camera with soft lenses, you might want to reduce the number.
   * \return
   */
  virtual double sigma() const = 0;

  /*!
   * \brief Set the number of best features to retain
   * \param[in] featuresNumber Number of features
   */
  virtual void setFeaturesNumber(int featuresNumber) = 0;

  /*!
   * \brief Set the number of layers in each octave
   * \param[in] octaveLayers The number of layers in each octave (3 by default)
   */
  virtual void setOctaveLayers(int octaveLayers) = 0;

  virtual void setContrastThresholdAuto(bool active) = 0;

  /*!
   * \brief Set the contrast threshold
   * \param[in] contrastThreshold Contrast threshold
   */
  virtual void setContrastThreshold(double contrastThreshold) = 0;

  /*!
   * \brief Set the threshold used to filter out edge-like features
   * \param[in] edgeThreshold Edge threshold
   */
  virtual void setEdgeThreshold(double edgeThreshold) = 0;

  /*!
   * \brief Set sigma of the Gaussian
   * \param[in] sigma Sigma of the Gaussian
   */
  virtual void setSigma(double sigma) = 0;

};




class GRAPHOS_EXPORT FeatureMatching
{
public:

  FeatureMatching() {}
  virtual ~FeatureMatching() {}

//  virtual int blockSize() const = 0;
//  virtual void setBlockSize(int blockSize) = 0;
  virtual bool crossCheck() const = 0;
  virtual void enableCrossCheck(bool enable = true) = 0;
  virtual double ratio() const = 0;
  virtual void setRatio(double ratio) = 0;
  virtual double distance() const = 0;
  virtual void setDistance(double distance) = 0;
//  virtual int maxMatches() const = 0;
//  virtual void setMaxMatches(int maxMatches) = 0;
  virtual double maxError() const = 0;
  virtual void setMaxError(double error) = 0;
  virtual double confidence() const = 0;
  virtual void setConfidence(double error) = 0;

};


} // namespace graphos

#endif // GRAPHOS_FEATURES_H
