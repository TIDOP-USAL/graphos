#ifndef INSPECTOR_FEATURE_EXTRACTOR_PROCESS_H
#define INSPECTOR_FEATURE_EXTRACTOR_PROCESS_H

#include "inspector/process/ProcessConcurrent.h"
#include "inspector/core/features/features.h"

namespace inspector
{

class INSPECTOR_EXPORT FeatureExtractorProcess
  : public ProcessConcurrent
{

  Q_OBJECT

public:

  /*!
   * \brief Constructor FeatureExtractorProcess
   * \param[in] image Image file
   * \param[in] features Features output file
   * \param[in] imageScale Image scale
   * \param[in] featureExtractor FeatureExtractor object
   */
  FeatureExtractorProcess(const QString &image,
                          const QString &features,
                          double imageScale,
                          const std::shared_ptr<FeatureExtractor> &featureExtractor);

  QString image() const;
  void setImage(const QString &image);
  QString features() const;
  void setFeatures(const QString &features);
  void setScale(double scale);

  std::shared_ptr<FeatureExtractor> featureExtractor() const;

signals:

  void featuresExtracted(QString);

// ProcessConcurrent interface

protected:

  void run() override;

protected:

  QString mImage;
  QString mFeatures;
  double mImageScale;
  std::shared_ptr<FeatureExtractor> mFeatureExtractor;
};

} // inspector

#endif // INSPECTOR_FEATURE_EXTRACTOR_PROCESS_H
