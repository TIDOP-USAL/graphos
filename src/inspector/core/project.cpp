#include "project.h"

#include <tidop/core/messages.h>

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamWriter>

#define INSPECTOR_PROJECT_FILE_VERSION "1.0"

namespace inspector
{

ProjectImp::ProjectImp()
  : Project(),
    mName(""),
    mDescription(""),
    mProjectFolder(""),
    mVersion(INSPECTOR_PROJECT_FILE_VERSION),
    mDatabase("")
{
}

QString ProjectImp::name() const
{
  return mName;
}

void ProjectImp::setName(const QString &name)
{
  mName = name;
}

QString ProjectImp::description() const
{
  return mDescription;
}

void ProjectImp::setDescription(const QString &description)
{
  mDescription = description;
}

QString ProjectImp::projectFolder() const
{
  return mProjectFolder;
}

void ProjectImp::setProjectFolder(const QString &dir)
{
  mProjectFolder = dir;
}


QString ProjectImp::version() const
{
  return mVersion;
}

QString ProjectImp::database() const
{
  return mDatabase;
}

void ProjectImp::setDatabase(const QString &database)
{
  mDatabase = database;
}

void ProjectImp::addImage(const std::shared_ptr<Image> &img)
{
  std::shared_ptr<Image> image = findImage(img->path());
  if (image != nullptr) {
    QByteArray ba = img->path().toLocal8Bit();
    msgWarning("Image %s already in the project", ba.data());
  } else {
    mImages.push_back(img);
  }
}

void ProjectImp::deleteImage(const QString &img)
{
  deleteImage(findImageId(img));
}

void ProjectImp::deleteImage(size_t imgId)
{
  if (imgId != std::numeric_limits<size_t>::max()){
    mImages.erase(mImages.begin()+ static_cast<long long>(imgId));
  }
}

std::shared_ptr<Image> ProjectImp::findImage(const QString &path)
{
  for (auto &image : mImages) {
    if (image->path().compare(path) == 0) {
      return image;
    }
  }
  return nullptr;
}

const std::shared_ptr<Image> ProjectImp::findImage(const QString &path) const
{
  for (auto &image : mImages) {
    if (image->path().compare(path) == 0) {
      return image;
    }
  }
  return nullptr;
}

std::shared_ptr<Image> ProjectImp::findImageById(size_t id)
{
  return mImages[id];
}

const std::shared_ptr<Image> ProjectImp::findImageById(size_t id) const
{
  return mImages[id];
}

std::shared_ptr<Image> ProjectImp::findImageByName(const QString &imgName)
{
  for (auto &image : mImages) {
    if (image->name().compare(imgName) == 0) {
      return image;
    }
  }
  return nullptr;
}

const std::shared_ptr<Image> ProjectImp::findImageByName(const QString &imgName) const
{
  for (auto &image : mImages) {
    if (image->name().compare(imgName) == 0) {
      return image;
    }
  }
  return nullptr;
}

size_t ProjectImp::findImageId(const QString &path) const
{
  for (size_t i = 0; i < mImages.size(); i++){
    if (mImages[i]->path().compare(path) == 0) {
      return i;
    }
  }
  return std::numeric_limits<size_t>().max();
}

Project::image_iterator ProjectImp::imageBegin()
{
  return mImages.begin();
}

Project::image_const_iterator ProjectImp::imageBegin() const
{
  return mImages.cbegin();
}

Project::image_iterator ProjectImp::imageEnd()
{
  return mImages.end();
}

Project::image_const_iterator ProjectImp::imageEnd() const
{
  return mImages.cend();
}

size_t ProjectImp::imagesCount() const
{
  return mImages.size();
}

void ProjectImp::clear()
{
  mName = "";
  mDescription = "";
  mProjectFolder = "";
  mVersion = INSPECTOR_PROJECT_FILE_VERSION;
  mImages.resize(0);
  mDatabase = "";
}



std::mutex ProjectControllerImp::sMutex;

ProjectControllerImp::ProjectControllerImp()
 : ProjectController()
{

}


bool ProjectControllerImp::read(const QString &file, Project &prj)
{
  std::lock_guard<std::mutex> lck(ProjectControllerImp::sMutex);

  QFile input(file);
  if (input.open(QIODevice::ReadOnly)) {
    QXmlStreamReader stream;
    stream.setDevice(&input);

    if (stream.readNextStartElement()) {
      if (stream.name() == "Inspector") {
        while (stream.readNextStartElement()) {
          if (stream.name() == "General") {
            readGeneral(stream, prj);
          } else if (stream.name() == "Database") {
            readDatabase(stream, prj);
          } else if (stream.name() == "Images") {
            readImages(stream, prj);
          } /*else if (stream.name() == "GroundTruth") {
            readGroundTruth(stream, prj);
          } else if (stream.name() == "Sessions") {
            readSessions(stream, prj);
          }*/
        }
      } else {
        stream.raiseError(QObject::tr("Incorrect project file"));
        return true;
      }
    }
  }
  return false;
}

bool ProjectControllerImp::write(const QString &file, const Project &prj) const
{
  QFileInfo file_info(file);
  QString tmpfile = file_info.path().append(file_info.baseName()).append(".bak");
  std::ifstream  src(file.toStdString().c_str(), std::ios::binary);
  std::ofstream  dst(tmpfile.toStdString().c_str(), std::ios::binary);
  dst << src.rdbuf();
  src.close();
  dst.close();

  std::lock_guard<std::mutex> lck(ProjectControllerImp::sMutex);

  QFile output(file);
  output.open(QFile::WriteOnly);
  QXmlStreamWriter stream(&output);
  stream.setAutoFormatting(true);
  stream.writeStartDocument();

  stream.writeStartElement("Inspector");
  {
    writeVersion(stream, prj.version());
    writeGeneral(stream, prj);
    writeImages(stream, prj);
    writeDatabase(stream, prj);
//    writeGroundTruth(stream, prj.groundTruth());
//    writeSessions(stream, prj);
  }

  stream.writeEndElement(); // Inspector

  output.close();

  std::remove(tmpfile.toStdString().c_str());

  return false;
}

bool ProjectControllerImp::checkOldVersion(const QString &file) const
{
  std::lock_guard<std::mutex> lck(ProjectControllerImp::sMutex);
  bool bUpdateVersion = false;
  QFile input(file);
  if (input.open(QIODevice::ReadOnly)) {
    QXmlStreamReader stream;
    stream.setDevice(&input);

    if (stream.readNextStartElement()) {
      if (stream.name() == "Inspector") {
        QString version = "0";
        for (auto &attr : stream.attributes()) {
          if (attr.name().compare(QString("version")) == 0) {
            version = attr.value().toString();
            break;
          }
        }
        if (version.compare(INSPECTOR_PROJECT_FILE_VERSION) < 0) {
          // Es una versión mas antigua
          bUpdateVersion = true;
        }
      } else
        stream.raiseError(QObject::tr("Incorrect file"));
    }
  }
  return bUpdateVersion;
}

void ProjectControllerImp::oldVersionBak(const QString &file) const
{
  // Versión antigua
  QString version = "0";
  QFile input(file);
  if (input.open(QIODevice::ReadOnly)) {
    QXmlStreamReader stream;
    stream.setDevice(&input);

    if (stream.readNextStartElement()) {
      if (stream.name() == "Inspector") {
        for (auto &attr : stream.attributes()) {
          if (attr.name().compare(QString("version")) == 0) {
            version = attr.value().toString();
            break;
          }
        }
      }
    }
  }

  QFileInfo file_info(file);
  QString tmpfile = file_info.path().append(file_info.baseName()).append("_v").append(version).append(".bak");
  std::ifstream  src(file.toStdString().c_str(), std::ios::binary);
  std::ofstream  dst(tmpfile.toStdString().c_str(), std::ios::binary);
  dst << src.rdbuf();
  src.close();
  dst.close();
}

void ProjectControllerImp::readGeneral(QXmlStreamReader &stream, Project &prj)
{
  while (stream.readNextStartElement()) {
    if (stream.name() == "Name") {
      prj.setName(stream.readElementText());
    } else if (stream.name() == "Path") {
      prj.setProjectFolder(stream.readElementText());
    } else if (stream.name() == "Description") {
      prj.setDescription(stream.readElementText());
    } else
      stream.skipCurrentElement();
  }
}

void ProjectControllerImp::readDatabase(QXmlStreamReader &stream, Project &prj)
{
  prj.setDatabase(stream.readElementText());
}

void ProjectControllerImp::readImages(QXmlStreamReader &stream, Project &prj)
{
  while (stream.readNextStartElement()) {
    if (stream.name() == "Image") {
      prj.addImage(readImage(stream));
    } else
      stream.skipCurrentElement();
  }
}

std::shared_ptr<Image> ProjectControllerImp::readImage(QXmlStreamReader &stream)
{
  std::shared_ptr<Image> photo(new Image);
  while (stream.readNextStartElement()) {
    if (stream.name() == "File") {
      photo->setPath(stream.readElementText());
    } else if (stream.name() == "LongitudeExif") {
      photo->setLongitudeExif(readDouble(stream));
    } else if (stream.name() == "LatitudeExif") {
      photo->setLatitudeExif(readDouble(stream));
    } else if (stream.name() == "AltitudeExif") {
      photo->setAltitudeExif(readDouble(stream));
    } else
      stream.skipCurrentElement();
  }
  return photo;
}

void ProjectControllerImp::writeVersion(QXmlStreamWriter &stream, const QString &version) const
{
  stream.writeAttribute("version", version);
}

void ProjectControllerImp::writeGeneral(QXmlStreamWriter &stream, const Project &prj) const
{
  stream.writeStartElement("General");
  {
    stream.writeTextElement("Name", prj.name());
    stream.writeTextElement("Path", prj.projectFolder());
    stream.writeTextElement("Description", prj.description());
  }
  stream.writeEndElement();
}

void ProjectControllerImp::writeDatabase(QXmlStreamWriter &stream, const Project &prj) const
{
  stream.writeTextElement("Database", prj.database());
}

void ProjectControllerImp::writeImages(QXmlStreamWriter &stream, const Project &prj) const
{
  stream.writeStartElement("Images");
  {
    for (auto it = prj.imageBegin(); it != prj.imageEnd(); it++){
      writeImage(stream, (*it).get());
    }
  }
  stream.writeEndElement();
}

void ProjectControllerImp::writeImage(QXmlStreamWriter &stream, const Image *image) const
{
  stream.writeStartElement("Image");
  {
    stream.writeTextElement("File", image->path());
    stream.writeTextElement("LongitudeExif", QString::number(image->longitudeExif()));
    stream.writeTextElement("LatitudeExif", QString::number(image->latitudeExif()));
    stream.writeTextElement("AltitudeExif", QString::number(image->altitudeExif()));
  }
  stream.writeEndElement();
}

QSize ProjectControllerImp::readSize(QXmlStreamReader &stream) const
{
  QSize blockSize;
  while (stream.readNextStartElement()) {
    if (stream.name() == "Width") {
      blockSize.setWidth(readInt(stream));
    } else if (stream.name() == "Height") {
      blockSize.setHeight(readInt(stream));
    } else
      stream.skipCurrentElement();
  }
  return blockSize;
}

int ProjectControllerImp::readInt(QXmlStreamReader &stream) const
{
  return stream.readElementText().toInt();
}

double ProjectControllerImp::readDouble(QXmlStreamReader &stream) const
{
  return stream.readElementText().toDouble();
}

bool ProjectControllerImp::readBoolean(QXmlStreamReader &stream) const
{
  return stream.readElementText().compare("true") == 0 ? true : false;
}

} // end namespace inspector
