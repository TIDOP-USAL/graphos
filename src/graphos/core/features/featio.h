#ifndef GRAPHOS_FEATURES_IO_H
#define GRAPHOS_FEATURES_IO_H

#include "graphos/graphos_global.h"

#include <memory>

#include <QString>

//#include <opencv2/features2d.hpp>
#include <colmap/feature/sift.h>

namespace graphos
{

/*!
 * \brief FeaturesWriter class allows the writing of the detected features in different formats
 */
class GRAPHOS_EXPORT FeaturesWriter
{

public:

  FeaturesWriter(const QString &fileName);
  virtual ~FeaturesWriter() = default;

  virtual bool write() = 0;

  void setKeyPoints(const colmap::FeatureKeypoints &keyPoints);
  void setDescriptors(const colmap::FeatureDescriptors &descriptors);

protected:

  QString mFileName;
  colmap::FeatureKeypoints mKeyPoints;
  colmap::FeatureDescriptors mDescriptors;
};


/*----------------------------------------------------------------*/


/*!
 * \brief The FeaturesWriter class allows the reading of the different formats of features files
 */
class GRAPHOS_EXPORT FeaturesReader
{

public:

  FeaturesReader(const QString &fileName);
  virtual ~FeaturesReader() = default;

  virtual bool read() = 0;

  colmap::FeatureKeypoints keyPoints() const;
  colmap::FeatureDescriptors descriptors() const;

protected:

  QString mFileName;
  colmap::FeatureKeypoints mKeyPoints;
  colmap::FeatureDescriptors mDescriptors;
};



/*----------------------------------------------------------------*/



/*!
 * \brief Factory class to create different reading formats
 */
class GRAPHOS_EXPORT FeaturesReaderFactory
{

private:

  FeaturesReaderFactory() {}

public:

  static std::unique_ptr<FeaturesReader> createReader(const QString &fileName);
};

/*!
 * \brief Factory class to create different writing formats
 */
class GRAPHOS_EXPORT FeaturesWriterFactory
{
public:

private:

  FeaturesWriterFactory() {}

public:

  static std::unique_ptr<FeaturesWriter> createWriter(const QString &fileName);
};



/*----------------------------------------------------------------*/



class GRAPHOS_EXPORT FeaturesIOHandler
{

public:

  FeaturesIOHandler();
  virtual ~FeaturesIOHandler() = default;

  bool read(const QString &file);
  bool write(const QString &file);

protected:

  std::unique_ptr<FeaturesReader> mReader;
  std::unique_ptr<FeaturesWriter> mWriter;
};


} // namespace graphos


#endif // GRAPHOS_FEATIO_H
