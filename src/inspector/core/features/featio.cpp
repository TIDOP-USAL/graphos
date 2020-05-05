#include "featio.h"

#include <tidop/core/messages.h>

#include <QFileInfo>

#include <stdexcept>

namespace inspector
{

FeaturesWriter::FeaturesWriter(const QString &fileName)
  : mFileName(fileName)
{

}

void FeaturesWriter::setKeyPoints(const colmap::FeatureKeypoints &keyPoints)
{
  mKeyPoints = keyPoints;
}

void FeaturesWriter::setDescriptors(const colmap::FeatureDescriptors &descriptors)
{
  mDescriptors = descriptors;
}


/* ---------------------------------------------------------------------------------- */



FeaturesReader::FeaturesReader(const QString &fileName)
  : mFileName(fileName)
{

}

colmap::FeatureKeypoints FeaturesReader::keyPoints() const
{
  return mKeyPoints;
}

colmap::FeatureDescriptors FeaturesReader::descriptors() const
{
  return mDescriptors;
}



/* ---------------------------------------------------------------------------------- */



class FeaturesReaderColmap
  : public FeaturesReader
{

public:

  explicit FeaturesReaderColmap(const QString &fileName);
  ~FeaturesReaderColmap() override = default;

// FeaturesReader interface

public:

  bool read() override;

private:

  void open();
  bool isOpen();
  void readDB();
  void close();

private:

};

FeaturesReaderColmap::FeaturesReaderColmap(const QString &fileName)
  : FeaturesReader(fileName)
{

}

bool FeaturesReaderColmap::read()
{
  try {
    open();
    if (isOpen()) {
      readDB();
      close();
    }
  } catch (std::exception &e) {
    msgError(e.what());
    return true;
  }
  return false;
}

void FeaturesReaderColmap::open()
{

}

bool FeaturesReaderColmap::isOpen()
{
  return false;
}

void FeaturesReaderColmap::readDB()
{

}

void FeaturesReaderColmap::close()
{

}



/* ---------------------------------------------------------------------------------- */



class FeaturesWriterColmap
    : public FeaturesWriter
{

public:

  explicit FeaturesWriterColmap(const QString &fileName);
  ~FeaturesWriterColmap() override = default;

// FeaturesWriter interface

public:

  bool write() override
  {
    try {
      open();
      if (isOpen()){
        writeDB();
        close();
      }
    } catch (std::exception &e) {
      msgError(e.what());
      return true;
    }
    return false;
  }

private:

  void open();
  bool isOpen();
  void writeDB();
  void close();

private:



};

FeaturesWriterColmap::FeaturesWriterColmap(const QString &fileName)
  : FeaturesWriter(fileName)
{

}

void FeaturesWriterColmap::open()
{

}

bool FeaturesWriterColmap::isOpen()
{
  return false;
}

void FeaturesWriterColmap::writeDB()
{

}

void FeaturesWriterColmap::close()
{

}



/* ---------------------------------------------------------------------------------- */

std::unique_ptr<FeaturesReader> FeaturesReaderFactory::createReader(const QString &fileName)
{
  QString ext = QFileInfo(fileName).suffix();
  std::unique_ptr<FeaturesReader> features_reader;
  if (ext.compare("db", Qt::CaseInsensitive) == 0) {
    features_reader = std::make_unique<FeaturesReaderColmap>(fileName);
  } else {
    throw std::runtime_error("Invalid Features Reader");
  }
  return features_reader;
}



/* ---------------------------------------------------------------------------------- */



std::unique_ptr<FeaturesWriter> FeaturesWriterFactory::createWriter(const QString &fileName)
{
  QString ext = QFileInfo(fileName).suffix();
  std::unique_ptr<FeaturesWriter> features_writer;
  if (ext.compare("db", Qt::CaseInsensitive) == 0) {
    features_writer = std::make_unique<FeaturesWriterColmap>(fileName);
  } else {
    throw std::runtime_error("Invalid Features Writer");
  }
  return features_writer;
}



/* ---------------------------------------------------------------------------------- */



FeaturesIOHandler::FeaturesIOHandler()
{
}

bool FeaturesIOHandler::read(const QString &file)
{
  try {
    mReader = FeaturesReaderFactory::createReader(file);
    return mReader->read();
  } catch (std::exception &e) {
    msgError(e.what());
    return true;
  }
}

bool FeaturesIOHandler::write(const QString &file)
{
  try {
    mWriter = FeaturesWriterFactory::createWriter(file);
    return mWriter->write();
  } catch (std::exception &e) {
    msgError(e.what());
    return true;
  }
}


} // namespace inspector

