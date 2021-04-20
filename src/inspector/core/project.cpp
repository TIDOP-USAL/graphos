#include "project.h"

#include <tidop/core/messages.h>
#include <tidop/core/exception.h>

#include "inspector/core/features/sift.h"
#include "inspector/core/features/matching.h"
#include "inspector/core/densification/Smvs.h"
#include "inspector/core/densification/CmvsPmvs.h"
#include "inspector/core/utils.h"

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamWriter>


#define INSPECTOR_PROJECT_FILE_VERSION "1.0"

namespace inspector
{

std::mutex ProjectImp::sMutex;

ProjectImp::ProjectImp()
  : Project(),
    mName(""),
    mDescription(""),
    mProjectFolder(""),
    mProjectPath(""),
    mVersion(INSPECTOR_PROJECT_FILE_VERSION),
    mDatabase(""),
    bRefinePrincipalPoint(true),
    mReconstructionPath(""),
    mCameraCount(0)
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

QString ProjectImp::projectPath() const
{
  return mProjectPath;
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

QString ProjectImp::imageDirectory() const
{
  return mImagesDirectory;
}

void ProjectImp::setImageDirectory(const QString &imageDirectory)
{
  mImagesDirectory = imageDirectory;
}

void ProjectImp::addImage(const Image &img)
{
  if (existImage(img.path())){
    QByteArray ba = img.path().toLocal8Bit();
    msgWarning("Image %s already in the project", ba.data());
  } else {
    mImages.push_back(img);
  }
}

bool ProjectImp::updateImage(size_t imageId, const Image &image)
{
  if (imageId <= mImages.size()){
    mImages[imageId] = image;
    return true;
  } else {
    return false;
  }
}

bool ProjectImp::removeImage(const QString &imgPath)
{
  if (existImage(imgPath)){
    removeImage(imageId(imgPath));
    return true;
  } else {
    QByteArray ba = imgPath.toLocal8Bit();
    msgWarning("Image to be deleted does not exist in the project: %s", ba.data());
    return false;
  }
}

bool ProjectImp::removeImage(size_t imgId)
{
  if (imgId != std::numeric_limits<size_t>::max() &&
      imgId < mImages.size()){
    mImages.erase(mImages.begin() + static_cast<long long>(imgId));
    return true;
  } else {
    return false;
  }
}

Image ProjectImp::findImage(const QString &imgName) const
{
  for (auto &image : mImages) {
    if (image.name().compare(imgName) == 0) {
      return image;
    }
  }
  QByteArray ba = imgName.toLocal8Bit();
  throw TL_ERROR("Image not found: %s", ba.data());
}

Image ProjectImp::findImageById(size_t id) const
{
  return mImages[id];
}

Image ProjectImp::findImageByName(const QString &imgName) const
{
  for (auto &image : mImages) {
    if (image.name().compare(imgName) == 0) {
      return image;
    }
  }
  QByteArray ba = imgName.toLocal8Bit();
  throw TL_ERROR("Image not found: %s", ba.data());
}

bool ProjectImp::existImage(const QString &imgName) const
{
  for (auto &image : mImages) {
    if (image.name().compare(imgName) == 0) {
      return true;
    }
  }
  return false;
}

size_t ProjectImp::imageId(const QString &imageName) const
{
  for (size_t i = 0; i < mImages.size(); i++){
    if (mImages[i].name().compare(imageName) == 0) {
      return i;
    }
  }
  std::string msg = std::string("Image not found: ").append(imageName.toStdString());
  throw std::runtime_error(msg.c_str());
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

int ProjectImp::addCamera(const Camera &camera)
{
  mCameras[++mCameraCount] = camera;
  return mCameraCount;
}

Camera ProjectImp::findCamera(const QString &make, const QString &model) const
{
  Camera camera;
  for (auto it = mCameras.begin(); it != mCameras.end(); it++) {
    camera = it->second;
    if (camera.make().compare(make) == 0 &&
        camera.model().compare(model) == 0) {
      return camera;
    }
  }
  std::string msg = std::string("Camera not found: ").append(make.toStdString()).append(" ").append(model.toStdString());
  throw std::runtime_error(msg.c_str());
}

Camera ProjectImp::findCamera(int idCamera) const
{
  camera_const_iterator it = mCameras.find(idCamera);
  if (it != mCameras.end()){
    return mCameras.at(idCamera);
  } else {
    throw std::runtime_error("Camera not exist");
  }
}

bool ProjectImp::existCamera(const QString &make, const QString &model) const
{
  for (auto it = mCameras.begin(); it != mCameras.end(); it++) {
    Camera camera = it->second;
    if (camera.make().compare(make) == 0 &&
        camera.model().compare(model) == 0) {
      return true;
    }
  }
  return false;
}

bool ProjectImp::updateCamera(int idCamera, const Camera &camera)
{
  auto it = mCameras.find(idCamera);
  if (it != mCameras.end()){
    it->second = camera;
    return true;
  } else {
    return false;
  }
}

bool ProjectImp::removeCamera(int idCamera)
{
  auto it = mCameras.find(idCamera);
  if (it != mCameras.end()){
    mCameras.erase(it);
    return true;
  } else {
    return false;
  }
}

int ProjectImp::cameraId(const QString &make, const QString &model) const
{
  Camera camera;
  for (auto it = mCameras.begin(); it != mCameras.end(); it++) {
    camera = it->second;
    if (camera.make().compare(make) == 0 &&
        camera.model().compare(model) == 0) {
      return it->first;
    }
  }
  return 0;
}

Project::camera_iterator ProjectImp::cameraBegin()
{
  return mCameras.begin();
}

Project::camera_const_iterator ProjectImp::cameraBegin() const
{
  return mCameras.cbegin();
}

Project::camera_iterator ProjectImp::cameraEnd()
{
  return mCameras.end();
}

Project::camera_const_iterator ProjectImp::cameraEnd() const
{
  return mCameras.cend();
}

std::shared_ptr<Feature> ProjectImp::featureExtractor() const
{
  return mFeatureExtractor;
}

void ProjectImp::setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor)
{
  mFeatureExtractor = featureExtractor;
}

QString ProjectImp::features(const QString &imgName) const
{
  return mFeatures.at(imgName);
}

void ProjectImp::addFeatures(const QString &imgName, const QString &featureFile)
{
  mFeatures[imgName] = featureFile;
}

void ProjectImp::removeFeatures()
{
  mFeatures.clear();
  this->removeMatchesPair();
}

bool ProjectImp::removeFeatures(const QString &imgName)
{
  auto it = mFeatures.find(imgName);
  if (it != mFeatures.end()){
    mFeatures.erase(it);
    return true;
  } else {
    return false;
  }
}

Project::features_iterator ProjectImp::featuresBegin()
{
  return mFeatures.begin();
}

Project::features_const_iterator ProjectImp::featuresBegin() const
{
  return mFeatures.begin();
}

Project::features_iterator ProjectImp::featuresEnd()
{
  return mFeatures.end();
}

Project::features_const_iterator ProjectImp::featuresEnd() const
{
  return mFeatures.end();
}

std::shared_ptr<FeatureMatching> ProjectImp::featureMatching() const
{
  return mFeatureMatching;
}

void ProjectImp::setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching)
{
  mFeatureMatching = featureMatching;
}

void ProjectImp::addMatchesPair(const QString &imageLeft, const QString &imageRight)
{
  auto it = mImagesPairs.find(imageLeft);
  if (it != mImagesPairs.end()){
    std::vector<QString> pairs = it->second;
    for (auto &pair : pairs) {
      if (pair.compare(imageRight) == 0){
        return;
      }
    }
  }

  mImagesPairs[imageLeft].push_back(imageRight);
  mImagesPairs[imageRight].push_back(imageLeft);
}

const std::vector<QString> ProjectImp::matchesPairs(const QString &imageLeft) const
{
  std::vector<QString> pairs;
  for (auto &matches : mImagesPairs){
    if (imageLeft.compare(matches.first) == 0) {
      pairs = matches.second;
      break;
    }
  }
  return pairs;
}

void ProjectImp::removeMatchesPair()
{
  mImagesPairs.clear();
  this->clearReconstruction();
}

void ProjectImp::removeMatchesPair(const QString &imageLeft)
{
  auto it = mImagesPairs.find(imageLeft);
  if (it != mImagesPairs.end()){
    mImagesPairs.erase(it);
  }
}

bool ProjectImp::refinePrincipalPoint() const
{
  return bRefinePrincipalPoint;
}

void ProjectImp::setRefinePrincipalPoint(bool refine)
{
  bRefinePrincipalPoint = refine;
}

QString ProjectImp::sparseModel() const
{
  return mSparseModel;
}

void ProjectImp::setSparseModel(const QString &sparseModel)
{
  mSparseModel = sparseModel;
}

QString ProjectImp::offset() const
{
  return mOffset;
}

void ProjectImp::setOffset(const QString &offset)
{
  mOffset = offset;
}

QString ProjectImp::reconstructionPath() const
{
  return mReconstructionPath;
}

void ProjectImp::setReconstructionPath(const QString &reconstructionPath)
{
  mReconstructionPath = reconstructionPath;
}

bool ProjectImp::isPhotoOriented(const QString &imgName) const
{
  return mPhotoOrientation.find(imgName) != mPhotoOrientation.end();
}

PhotoOrientation ProjectImp::photoOrientation(const QString &imgName) const
{
  return mPhotoOrientation.at(imgName);
}

void ProjectImp::addPhotoOrientation(const QString &imgName, const PhotoOrientation &photoOrientation)
{
  mPhotoOrientation[imgName] = photoOrientation;
}

void ProjectImp::clearReconstruction()
{
  mPhotoOrientation.clear();
  mSparseModel.clear();
  mOffset.clear();
  mReconstructionPath.clear();
  this->clearDensification();
}

std::shared_ptr<Densification> ProjectImp::densification() const
{
  return mDensification;
}

void ProjectImp::setDensification(const std::shared_ptr<Densification> &densification)
{
  mDensification = densification;
}

void ProjectImp::setDenseModel(const QString &denseModel)
{
  mDenseModel = denseModel;
}

QString ProjectImp::denseModel() const
{
  return mDenseModel;
}

void ProjectImp::clearDensification()
{
  mDenseModel.clear();
}

void ProjectImp::clear()
{
  mName = "";
  mDescription = "";
  mProjectFolder = "";
  mProjectPath = "";
  mVersion = INSPECTOR_PROJECT_FILE_VERSION;
  mDatabase = "";
  mImagesDirectory = "";
  mImages.resize(0);
  mCameras.clear();
  mFeatureExtractor.reset();
  mFeatures.clear();
  mFeatureMatching.reset();
  mImagesPairs.clear();
  mPhotoOrientation.clear();
  bRefinePrincipalPoint = true;
  mSparseModel = "";
  mOffset = "";
  mReconstructionPath = "";
  mDensification.reset();
  mDenseModel = "";
  mCameraCount = 0;
}

bool ProjectImp::load(const QString &file)
{
  std::lock_guard<std::mutex> lck(ProjectImp::sMutex);

  bool err = false;
  QFile input(file);
  mProjectPath = file;

  if (input.open(QIODevice::ReadOnly)) {
    QXmlStreamReader stream;
    stream.setDevice(&input);

    err = this->read(stream);
    input.close();
  }
  return err;
}

bool ProjectImp::save(const QString &file)
{
  bool err = false;
  std::lock_guard<std::mutex> lck(ProjectImp::sMutex);

  mProjectPath = file;

  QFileInfo file_info(file);
  QString tmpfile = file_info.path().append(file_info.baseName()).append(".bak");
  std::ifstream  src(file.toStdString().c_str(), std::ios::binary);
  std::ofstream  dst(tmpfile.toStdString().c_str(), std::ios::binary);
  dst << src.rdbuf();
  src.close();
  dst.close();

  try {

    QFile output(file);
    if (output.open(QFile::WriteOnly)){
      QXmlStreamWriter stream(&output);
      stream.setAutoFormatting(true);
      stream.writeStartDocument();

      stream.writeStartElement("Inspector");
      {
        writeVersion(stream);
        writeGeneral(stream);
        writeCameras(stream);
        writeImages(stream);
        writeDatabase(stream);
        writeFeatures(stream);
        writeMatches(stream);
        writeOrientations(stream);
        writeDensification(stream);
      }

      stream.writeEndElement(); // Inspector

      output.close();

    } else {
      err = true;
    }
  } catch (std::exception &e) {
    msgError(e.what());

    std::ifstream  src(tmpfile.toStdString().c_str(), std::ios::binary);
    std::ofstream  dst(file.toStdString().c_str(), std::ios::binary);
    dst << src.rdbuf();
    src.close();
    dst.close();

    err = true;
  }

  std::remove(tmpfile.toStdString().c_str());

  return err;
}

bool ProjectImp::checkOldVersion(const QString &file) const
{
  std::lock_guard<std::mutex> lck(ProjectImp::sMutex);
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

void ProjectImp::oldVersionBak(const QString &file) const
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

bool ProjectImp::read(QXmlStreamReader &stream)
{
  if (stream.readNextStartElement()) {
    if (stream.name() == "Inspector") {
      while (stream.readNextStartElement()) {
        if (stream.name() == "General") {
          this->readGeneral(stream);
        } else if (stream.name() == "Database") {
          this->readDatabase(stream);
        } else if (stream.name() == "Cameras") {
          this->readCameras(stream);
        } else if (stream.name() == "Images") {
          this->readImages(stream);
        } else if (stream.name() == "Features") {
          this->readFeatures(stream);
        } else if (stream.name() == "Matches") {
          this->readMatches(stream);
        } else if (stream.name() == "Orientations") {
          readOrientations(stream);
        } else if (stream.name() == "Densification") {
          readDensification(stream);
        } else
          stream.skipCurrentElement();
        }
    } else {
      stream.raiseError(QObject::tr("Incorrect project file"));
      return true;
    }
  } else return true;

  return false;
}

void ProjectImp::readGeneral(QXmlStreamReader &stream)
{
  while (stream.readNextStartElement()) {
    if (stream.name() == "Name") {
      this->setName(stream.readElementText());
    } else if (stream.name() == "Path") {
      this->setProjectFolder(stream.readElementText());
    } else if (stream.name() == "Description") {
      this->setDescription(stream.readElementText());
    } else
      stream.skipCurrentElement();
  }
}

void ProjectImp::readDatabase(QXmlStreamReader &stream)
{
  this->setDatabase(stream.readElementText());
}

void ProjectImp::readImages(QXmlStreamReader &stream)
{
  while (stream.readNextStartElement()) {
    if (stream.name() == "ImagesDirectory") {
      this->setImageDirectory(stream.readElementText());
    } else if (stream.name() == "Image") {
      this->addImage(readImage(stream));
    } else
      stream.skipCurrentElement();
  }
}

Image ProjectImp::readImage(QXmlStreamReader &stream)
{
  Image photo;
  while (stream.readNextStartElement()) {
    if (stream.name() == "File") {
      photo.setPath(stream.readElementText());
    } else if (stream.name() == "CameraId") {
      photo.setCameraId(readInt(stream));
    } else if (stream.name() == "CameraPosition") {
      photo.setCameraPosition(readCameraPosition(stream));
    } /*else if (stream.name() == "LongitudeExif") {
      photo.setLongitudeExif(readDouble(stream));
    } else if (stream.name() == "LongitudeExif") {
      photo.setLongitudeExif(readDouble(stream));
    } else if (stream.name() == "LatitudeExif") {
      photo.setLatitudeExif(readDouble(stream));
    } else if (stream.name() == "AltitudeExif") {
      photo.setAltitudeExif(readDouble(stream));
    }*/ else
      stream.skipCurrentElement();
  }
  return photo;
}

CameraPosition ProjectImp::readCameraPosition(QXmlStreamReader &stream)
{
  CameraPosition cameraPosition;

  while (stream.readNextStartElement()) {
    if (stream.name() == "CRS") {
      cameraPosition.setCrs(stream.readElementText());
    } else if (stream.name() == "X") {
      cameraPosition.setX(readDouble(stream));
    } else if (stream.name() == "Y") {
      cameraPosition.setY(readDouble(stream));
    } else if (stream.name() == "Z") {
      cameraPosition.setZ(readDouble(stream));
    } else if (stream.name() == "Source") {
      cameraPosition.setSource(stream.readElementText());
    } else if (stream.name() == "QX") {
      tl::math::Quaterniond q = cameraPosition.quaternion();
      q.x = readDouble(stream);
      cameraPosition.setQuaternion(q);
    } else if (stream.name() == "QY") {
      tl::math::Quaterniond q = cameraPosition.quaternion();
      q.y = readDouble(stream);
      cameraPosition.setQuaternion(q);
    } else if (stream.name() == "QZ") {
      tl::math::Quaterniond q = cameraPosition.quaternion();
      q.z = readDouble(stream);
      cameraPosition.setQuaternion(q);
    } else if (stream.name() == "QW") {
      tl::math::Quaterniond q = cameraPosition.quaternion();
      q.w = readDouble(stream);
      cameraPosition.setQuaternion(q);
    }
  }

  return cameraPosition;
}

void ProjectImp::readCameras(QXmlStreamReader &stream)
{
  while (stream.readNextStartElement()) {
    if (stream.name() == "Camera") {
      this->addCamera(readCamera(stream));
    } else
      stream.skipCurrentElement();
  }
}

Camera ProjectImp::readCamera(QXmlStreamReader &stream)
{
  Camera camera;

  int id = 0;
  for (auto &attr : stream.attributes()) {
    if (attr.name().compare(QString("id")) == 0) {
      id = attr.value().toInt();
      break;
    }
  }

  while (stream.readNextStartElement()) {
    if (stream.name() == "Make") {
      camera.setMake(stream.readElementText());
    } else if (stream.name() == "Model") {
      camera.setModel(stream.readElementText());
    } else if (stream.name() == "Type") {
      camera.setType(stream.readElementText());
    } else if (stream.name() == "Focal") {
      camera.setFocal(readDouble(stream));
    } else if (stream.name() == "Width") {
      camera.setWidth(readInt(stream));
    } else if (stream.name() == "Height") {
      camera.setHeight(readInt(stream));
    } else if (stream.name() == "SensorSize") {
      camera.setSensorSize(readDouble(stream));
    } else if (stream.name() == "Calibration") {
      this->readCalibration(stream, camera);
    } else
      stream.skipCurrentElement();
  }
  return camera;
}

void ProjectImp::readCalibration(QXmlStreamReader &stream, Camera &camera)
{
  try {
    std::shared_ptr<Calibration> calibration;
    calibration = CalibrationFactory::create(camera.type());
    while (stream.readNextStartElement()) {
      if (stream.name().compare(QString("f")) == 0) {
        calibration->setParameter(Calibration::Parameters::focal, readDouble(stream));
      } else if (stream.name() == "fx") {
        calibration->setParameter(Calibration::Parameters::focalx, readDouble(stream));
      } else if (stream.name() == "fy") {
        calibration->setParameter(Calibration::Parameters::focaly, readDouble(stream));
      } else if (stream.name() == "cx") {
        calibration->setParameter(Calibration::Parameters::cx, readDouble(stream));
      } else if (stream.name() == "cy") {
        calibration->setParameter(Calibration::Parameters::cy, readDouble(stream));
      } else if (stream.name() == "k1") {
        calibration->setParameter(Calibration::Parameters::k1, readDouble(stream));
      } else if (stream.name() == "k2") {
        calibration->setParameter(Calibration::Parameters::k2, readDouble(stream));
      } else if (stream.name() == "k3") {
        calibration->setParameter(Calibration::Parameters::k3, readDouble(stream));
      } else if (stream.name() == "k4") {
        calibration->setParameter(Calibration::Parameters::k4, readDouble(stream));
      } else if (stream.name() == "k5") {
        calibration->setParameter(Calibration::Parameters::k5, readDouble(stream));
      } else if (stream.name() == "k6") {
        calibration->setParameter(Calibration::Parameters::k6, readDouble(stream));
      } else if (stream.name() == "p1") {
        calibration->setParameter(Calibration::Parameters::p1, readDouble(stream));
      } else if (stream.name() == "p2") {
        calibration->setParameter(Calibration::Parameters::p2, readDouble(stream));
      } else
        stream.skipCurrentElement();
    }
    camera.setCalibration(calibration);
  } catch (std::exception &e){
    msgError(e.what());
  }
}

void ProjectImp::readFeatures(QXmlStreamReader &stream)
{
  while (stream.readNextStartElement()){
    if (stream.name() == "FeatureExtractor") {
      this->readFeatureExtractor(stream);
    } else if (stream.name() == "Files") {
      this->readFeatureFiles(stream);
    } else
      stream.skipCurrentElement();
  }
}

void ProjectImp::readFeatureExtractor(QXmlStreamReader &stream)
{
  while (stream.readNextStartElement()){
    if (stream.name() == "SIFT") {
      readSIFT(stream);
    } else
      stream.skipCurrentElement();
  }
}

void ProjectImp::readSIFT(QXmlStreamReader &stream)
{
  std::shared_ptr<Sift> sift = std::make_shared<SiftProperties>();
  while (stream.readNextStartElement()) {
    if (stream.name() == "FeaturesNumber") {
      sift->setFeaturesNumber(readInt(stream));
    } else if (stream.name() == "OctaveLayers") {
      sift->setOctaveLayers(readInt(stream));
    } else if (stream.name() == "ContrastThresholdAuto") {
      sift->setContrastThresholdAuto(readBoolean(stream));
    } else if (stream.name() == "ContrastThreshold") {
      sift->setContrastThreshold(readDouble(stream));
    } else if (stream.name() == "EdgeThreshold") {
      sift->setEdgeThreshold(readDouble(stream));
    } else if (stream.name() == "Sigma") {
      sift->setSigma(readDouble(stream));
    } else
      stream.skipCurrentElement();
  }
  this->setFeatureExtractor(sift);
}

void ProjectImp::readFeatureFiles(QXmlStreamReader &stream)
{
  while (stream.readNextStartElement()) {
    if (stream.name() == "FeatFile") {
      this->readFeatureFile(stream);
    } else
      stream.skipCurrentElement();
  }
}

void ProjectImp::readFeatureFile(QXmlStreamReader &stream)
{
  QString id;
  for (auto &attr : stream.attributes()) {
    if (attr.name().compare(QString("id")) == 0) {
      id = attr.value().toString();
      break;
    }
  }

  QString file = stream.readElementText();
  this->addFeatures(id, file);
}

void ProjectImp::readMatches(QXmlStreamReader &stream)
{
  while (stream.readNextStartElement()){
    if (stream.name() == "FeatureMatchingMethod") {
      this->readMatchingMethod(stream);
    } else if (stream.name() == "Image"){
      this->readPairs(stream);
    } else
      stream.skipCurrentElement();
  }
}

void ProjectImp::readMatchingMethod(QXmlStreamReader &stream)
{
  std::shared_ptr<FeatureMatching> matchingMethod = std::make_shared<FeatureMatchingProperties>();
  while (stream.readNextStartElement()) {
    if (stream.name() == "Distance") {
      matchingMethod->setDistance(readDouble(stream));
    } else if (stream.name() == "Ratio") {
      matchingMethod->setRatio(readDouble(stream));
    } else if (stream.name() == "MaxError") {
      matchingMethod->setMaxError(readDouble(stream));
    } else if (stream.name() == "Confidence") {
      matchingMethod->setConfidence(readDouble(stream));
    } else if (stream.name() == "CrossCheck") {
      matchingMethod->enableCrossCheck(readBoolean(stream));
    } else
      stream.skipCurrentElement();
  }
  this->setFeatureMatching(matchingMethod);
}

void ProjectImp::readPairs(QXmlStreamReader &stream)
{
  QString id_left_image;
  for (auto &attr : stream.attributes()) {
    if (attr.name().compare(QString("id")) == 0) {
      id_left_image = attr.value().toString();
      break;
    }
  }

  while (stream.readNextStartElement()) {
    if (stream.name() == "Pair") {
      this->addMatchesPair(id_left_image, stream.readElementText());
    } else
      stream.skipCurrentElement();
  }
}

void ProjectImp::readOrientations(QXmlStreamReader &stream)
{
  while (stream.readNextStartElement()){
    if (stream.name() == "ReconstructionPath") {
      this->readReconstructionPath(stream);
    } else if (stream.name() == "SparseModel") {
      this->readOrientationSparseModel(stream);
    } else if (stream.name() == "Offset") {
      this->readOffset(stream);
    } else if (stream.name() == "Image"){
      this->readPhotoOrientations(stream);
    } else
      stream.skipCurrentElement();
  }
}

void ProjectImp::readReconstructionPath(QXmlStreamReader &stream)
{
  this->setReconstructionPath(stream.readElementText());
}

void ProjectImp::readOrientationSparseModel(QXmlStreamReader &stream)
{
  this->setSparseModel(stream.readElementText());
}

void ProjectImp::readOffset(QXmlStreamReader &stream)
{
  this->setOffset(stream.readElementText());
}

void ProjectImp::readPhotoOrientations(QXmlStreamReader &stream)
{
  QString id_image;
  for (auto &attr : stream.attributes()) {
    if (attr.name().compare(QString("id")) == 0) {
      id_image = attr.value().toString();
      break;
    }
  }

  PhotoOrientation image_ori;
  while (stream.readNextStartElement()) {
    if (stream.name() == "X") {
      image_ori.x = readDouble(stream);
    } else if (stream.name() == "Y") {
      image_ori.y = readDouble(stream);
    } else if (stream.name() == "Z") {
      image_ori.z = readDouble(stream);
    } else if (stream.name() == "Rot") {
      QStringList rot_string = stream.readElementText().split(" ");
      image_ori.rot[0][0] = rot_string.at(0).toFloat();
      image_ori.rot[0][1] = rot_string.at(1).toFloat();
      image_ori.rot[0][2] = rot_string.at(2).toFloat();
      image_ori.rot[1][0] = rot_string.at(3).toFloat();
      image_ori.rot[1][1] = rot_string.at(4).toFloat();
      image_ori.rot[1][2] = rot_string.at(5).toFloat();
      image_ori.rot[2][0] = rot_string.at(6).toFloat();
      image_ori.rot[2][1] = rot_string.at(7).toFloat();
      image_ori.rot[2][2] = rot_string.at(8).toFloat();
    } else
      stream.skipCurrentElement();
  }

  this->addPhotoOrientation(id_image, image_ori);
}

void ProjectImp::readDensification(QXmlStreamReader &stream)
{
  while (stream.readNextStartElement()){
    if (stream.name() == "DenseModel") {
      this->readDenseModel(stream);
    } else if (stream.name() == "DensificationMethod"){
      this->readDensificationMethod(stream);
    } else
      stream.skipCurrentElement();
  }
}

void ProjectImp::readDenseModel(QXmlStreamReader &stream)
{
  this->setDenseModel(stream.readElementText());
}

void ProjectImp::readDensificationMethod(QXmlStreamReader &stream)
{
  while (stream.readNextStartElement()) {
    if (stream.name() == "Smvs") {
      this->readSmvs(stream);
    } else if (stream.name() == "CmvsPmvs"){
      this->readCmvsPmvs(stream);
    } else
      stream.skipCurrentElement();
  }
}

void ProjectImp::readSmvs(QXmlStreamReader &stream)
{
  std::shared_ptr<Smvs> smvs = std::make_shared<SmvsProperties>();
  while (stream.readNextStartElement()) {
    if (stream.name() == "InputImageScale") {
      smvs->setInputImageScale(readInt(stream));
    } else if (stream.name() == "OutputDepthScale") {
      smvs->setOutputDepthScale(readInt(stream));
    } else if (stream.name() == "SemiGlobalMatching") {
      smvs->setSemiGlobalMatching(readBoolean(stream));
    } else if (stream.name() == "SurfaceSmoothingFactor") {
      smvs->setSurfaceSmoothingFactor(readDouble(stream));
    } else if (stream.name() == "ShadingBasedOptimization") {
      smvs->setShadingBasedOptimization(readBoolean(stream));
    } else
      stream.skipCurrentElement();
  }
  this->setDensification(smvs);
}

void ProjectImp::readCmvsPmvs(QXmlStreamReader &stream)
{
  std::shared_ptr<CmvsPmvs> cmvsPmvs = std::make_shared<CmvsPmvsProperties>();
  while (stream.readNextStartElement()) {
    if (stream.name() == "Level") {
      cmvsPmvs->setLevel(readInt(stream));
    } else if (stream.name() == "CellSize") {
      cmvsPmvs->setCellSize(readInt(stream));
    } else if (stream.name() == "Threshold") {
      cmvsPmvs->setThreshold(readDouble(stream));
    } else if (stream.name() == "Confidence") {
      cmvsPmvs->setWindowSize(readInt(stream));
    } else if (stream.name() == "ImagesPerCluster") {
      cmvsPmvs->setImagesPerCluster(readInt(stream));
    } else if (stream.name() == "MinimunImageNumber") {
      cmvsPmvs->setMinimunImageNumber(readInt(stream));
    } else if (stream.name() == "UseVisibilityInformation") {
      cmvsPmvs->setUseVisibilityInformation(readBoolean(stream));
    } else
      stream.skipCurrentElement();
  }
  this->setDensification(cmvsPmvs);
}

void ProjectImp::writeVersion(QXmlStreamWriter &stream) const
{
  stream.writeAttribute("version", this->version());
}

void ProjectImp::writeGeneral(QXmlStreamWriter &stream) const
{
  stream.writeStartElement("General");
  {
    stream.writeTextElement("Name", this->name());
    stream.writeTextElement("Path", this->projectFolder());
    stream.writeTextElement("Description", this->description());
  }
  stream.writeEndElement();
}

void ProjectImp::writeDatabase(QXmlStreamWriter &stream) const
{
  stream.writeTextElement("Database", this->database());
}

void ProjectImp::writeCameras(QXmlStreamWriter &stream) const
{
  stream.writeStartElement("Cameras");
  {
    for (auto it = this->cameraBegin(); it != this->cameraEnd(); it++) {
      this->writeCamera(stream, (*it).first, (*it).second);
    }
  }
  stream.writeEndElement();
}

void ProjectImp::writeCamera(QXmlStreamWriter &stream, int id, const Camera &camera) const
{
  stream.writeStartElement("Camera");
  {
    stream.writeAttribute("id", QString::number(id));

    stream.writeTextElement("Make", camera.make());
    stream.writeTextElement("Model", camera.model());
    stream.writeTextElement("Type", camera.type());
    stream.writeTextElement("Focal", QString::number(camera.focal()));
    stream.writeTextElement("Width", QString::number(camera.width()));
    stream.writeTextElement("Height", QString::number(camera.height()));
    stream.writeTextElement("SensorSize", QString::number(camera.sensorSize()));
    writeCalibration(stream, camera.calibration());

  }
  stream.writeEndElement(); // Camera
}

void ProjectImp::writeCalibration(QXmlStreamWriter &stream, std::shared_ptr<Calibration> calibration) const
{
  if (calibration){
    stream.writeStartElement("Calibration");
    {
      for (auto param = calibration->parametersBegin(); param != calibration->parametersEnd(); param++){
        stream.writeTextElement(calibration->parameterName(param->first), QString::number(param->second, 'f', 8));
      }
    }
    stream.writeEndElement(); // Calibration
  }
}

void ProjectImp::writeImages(QXmlStreamWriter &stream) const
{
  stream.writeStartElement("Images");
  {
    stream.writeTextElement("ImagesDirectory", mImagesDirectory);
    for (auto it = this->imageBegin(); it != this->imageEnd(); it++){
      writeImage(stream, (*it));
    }
  }
  stream.writeEndElement();
}

void ProjectImp::writeImage(QXmlStreamWriter &stream, const Image &image) const
{
  stream.writeStartElement("Image");
  {
    stream.writeTextElement("Name", image.name());
    stream.writeTextElement("File", image.path());
    stream.writeTextElement("CameraId", QString::number(image.cameraId()));
    writeCameraPosition(stream, image.cameraPosition());
//    stream.writeTextElement("LongitudeExif", QString::number(image.longitudeExif()));
//    stream.writeTextElement("LatitudeExif", QString::number(image.latitudeExif()));
//    stream.writeTextElement("AltitudeExif", QString::number(image.altitudeExif()));
  }
  stream.writeEndElement();
}

void ProjectImp::writeCameraPosition(QXmlStreamWriter &stream,
                                     const CameraPosition &cameraPosition) const
{
  if (!cameraPosition.isEmpty()) {
    stream.writeStartElement("CameraPosition");
    {
      stream.writeTextElement("CRS", cameraPosition.crs());
      stream.writeTextElement("X", QString::number(cameraPosition.x(), 'f', 3));
      stream.writeTextElement("Y", QString::number(cameraPosition.y(), 'f', 3));
      stream.writeTextElement("Z", QString::number(cameraPosition.z(), 'f', 3));
      stream.writeTextElement("QX", QString::number(cameraPosition.quaternion().x, 'f', 6));
      stream.writeTextElement("QY", QString::number(cameraPosition.quaternion().y, 'f', 6));
      stream.writeTextElement("QZ", QString::number(cameraPosition.quaternion().z, 'f', 6));
      stream.writeTextElement("QW", QString::number(cameraPosition.quaternion().w, 'f', 6));
      stream.writeTextElement("Source", cameraPosition.source());
    }
    stream.writeEndElement();
  }
}

void ProjectImp::writeFeatures(QXmlStreamWriter &stream) const
{
  stream.writeStartElement("Features");
  {
    this->writeFeatureExtractor(stream);
    this->writeFeatureFiles(stream);
  }
  stream.writeEndElement();
}

void ProjectImp::writeFeatureExtractor(QXmlStreamWriter &stream) const
{
  if (Feature *feature = this->featureExtractor().get()){
    stream.writeStartElement("FeatureExtractor");
    if (feature->type() == Feature::Type::sift){
      this->writeSIFT(stream, dynamic_cast<Sift *>(feature));
    }
    stream.writeEndElement();
  }
}

void ProjectImp::writeSIFT(QXmlStreamWriter &stream, Sift *sift) const
{
  stream.writeStartElement("SIFT");
  {
    stream.writeTextElement("FeaturesNumber", QString::number(sift->featuresNumber()));
    stream.writeTextElement("OctaveLayers", QString::number(sift->octaveLayers()));
    stream.writeTextElement("ContrastThresholdAuto", sift->constrastThresholdAuto()  ? "true" : "false");
    stream.writeTextElement("ContrastThreshold", QString::number(sift->contrastThreshold()));
    stream.writeTextElement("EdgeThreshold", QString::number(sift->edgeThreshold()));
    stream.writeTextElement("Sigma", QString::number(sift->sigma()));
  }
  stream.writeEndElement(); // SIFT
}

void ProjectImp::writeFeatureFiles(QXmlStreamWriter &stream) const
{
  stream.writeStartElement("Files");
  {
    for (auto it = this->featuresBegin(); it != this->featuresEnd(); it++){
      stream.writeStartElement("FeatFile");
      {
        stream.writeAttribute("id", it->first);
        stream.writeCharacters(it->second);
      }
      stream.writeEndElement(); // FeatFile
    }
  }
  stream.writeEndElement(); // Files
}

void ProjectImp::writeMatches(QXmlStreamWriter &stream) const
{
  stream.writeStartElement("Matches");
  {
    this->writeFeatureMatchingMethod(stream);
    this->writePairs(stream);
  }
  stream.writeEndElement();
}

void ProjectImp::writeFeatureMatchingMethod(QXmlStreamWriter &stream) const
{
  if (FeatureMatching *matchingMethod = this->featureMatching().get()){
    stream.writeStartElement("FeatureMatchingMethod");
    {
      stream.writeTextElement("Distance", QString::number(matchingMethod->distance()));
      stream.writeTextElement("Ratio", QString::number(matchingMethod->ratio()));
      stream.writeTextElement("MaxError", QString::number(matchingMethod->maxError()));
      stream.writeTextElement("Confidence", QString::number(matchingMethod->confidence()));
      stream.writeTextElement("CrossCheck", matchingMethod->crossCheck() ? "true" : "false");
    }
    stream.writeEndElement();
  }
}

void ProjectImp::writePairs(QXmlStreamWriter &stream) const
{
  if (!mImagesPairs.empty()){
    for (auto it = this->imageBegin(); it != this->imageEnd(); it++){
      stream.writeStartElement("Image");
      {
        stream.writeAttribute("id", it->name());
        std::vector<QString> pairs = this->matchesPairs(it->name());
        for (size_t i = 0; i < pairs.size(); i++){
          stream.writeTextElement("Pair", pairs[i]);
        }
      }
      stream.writeEndElement(); // Image
    }
  }
}

void ProjectImp::writeOrientations(QXmlStreamWriter &stream) const
{
  stream.writeStartElement("Orientations");
  {
    this->writeReconstructionPath(stream);
    this->writeOrientationSparseModel(stream);
    this->writeOffset(stream);
    this->writePhotoOrientations(stream);
  }
  stream.writeEndElement(); // Orientations
}

void ProjectImp::writeReconstructionPath(QXmlStreamWriter &stream) const
{
  QString reconstruction_path = this->reconstructionPath();
  if (!reconstruction_path.isEmpty())
    stream.writeTextElement("ReconstructionPath", reconstruction_path);
}

void ProjectImp::writeOrientationSparseModel(QXmlStreamWriter &stream) const
{
  QString sparse_model = this->sparseModel();
  if (!sparse_model.isEmpty())
    stream.writeTextElement("SparseModel", sparse_model);
}

void ProjectImp::writeOffset(QXmlStreamWriter &stream) const
{
  QString offset = this->offset();
  if (!offset.isEmpty())
    stream.writeTextElement("Offset", offset);
}

void ProjectImp::writePhotoOrientations(QXmlStreamWriter &stream) const
{
  if (!mPhotoOrientation.empty()){
    for (auto it = this->imageBegin(); it != this->imageEnd(); it++){
      if (this->isPhotoOriented(it->name()))
      {
        PhotoOrientation photoOrientation = this->photoOrientation(it->name());
        stream.writeStartElement("Image");
        {
          stream.writeAttribute("id", it->name());
          if (photoOrientation.x != 0. && photoOrientation.y != 0. && photoOrientation.z != 0.) {
              stream.writeTextElement("X", QString::number(photoOrientation.x, 'f', 6));
              stream.writeTextElement("Y", QString::number(photoOrientation.y, 'f', 6));
              stream.writeTextElement("Z", QString::number(photoOrientation.z, 'f', 6));
              QString rot_mat = QString::number(photoOrientation.rot[0][0]).append(" ");
              rot_mat.append(QString::number(photoOrientation.rot[0][1])).append(" ");
              rot_mat.append(QString::number(photoOrientation.rot[0][2])).append(" ");
              rot_mat.append(QString::number(photoOrientation.rot[1][0])).append(" ");
              rot_mat.append(QString::number(photoOrientation.rot[1][1])).append(" ");
              rot_mat.append(QString::number(photoOrientation.rot[1][2])).append(" ");
              rot_mat.append(QString::number(photoOrientation.rot[2][0])).append(" ");
              rot_mat.append(QString::number(photoOrientation.rot[2][1])).append(" ");
              rot_mat.append(QString::number(photoOrientation.rot[2][2]));
              stream.writeTextElement("Rot", rot_mat);
          }
        }
        stream.writeEndElement(); // Image
      }
    }
  }
}

void ProjectImp::writeDensification(QXmlStreamWriter &stream) const
{
  stream.writeStartElement("Densification");
  {
    this->writeDenseModel(stream);
    this->writeDensificationMethod(stream);
  }
  stream.writeEndElement(); // Densification
}

void ProjectImp::writeDenseModel(QXmlStreamWriter &stream) const
{
  QString dense_model = this->denseModel();
  if (!dense_model.isEmpty())
    stream.writeTextElement("DenseModel", this->denseModel());
}

void ProjectImp::writeDensificationMethod(QXmlStreamWriter &stream) const
{
  if (Densification *densificationMethod = this->densification().get()){

    stream.writeStartElement("DensificationMethod");

    if (densificationMethod->method() == Densification::Method::smvs) {

      stream.writeStartElement("Smvs");

      Smvs *smvs = dynamic_cast<Smvs *>(densificationMethod);
      stream.writeTextElement("InputImageScale", QString::number(smvs->inputImageScale()));
      stream.writeTextElement("OutputDepthScale", QString::number(smvs->outputDepthScale()));
      stream.writeTextElement("SemiGlobalMatching", smvs->semiGlobalMatching() ? "true" : "false");
      stream.writeTextElement("SurfaceSmoothingFactor", QString::number(smvs->surfaceSmoothingFactor()));
      stream.writeTextElement("ShadingBasedOptimization", smvs->shadingBasedOptimization() ? "true" : "false");

      stream.writeEndElement();

    } else if (densificationMethod->method() == Densification::Method::cmvs_pmvs) {

      stream.writeStartElement("CmvsPmvs");

      CmvsPmvs *cmvsPmvs = dynamic_cast<CmvsPmvs *>(densificationMethod);
      stream.writeTextElement("Level", QString::number(cmvsPmvs->level()));
      stream.writeTextElement("CellSize", QString::number(cmvsPmvs->cellSize()));
      stream.writeTextElement("Threshold", QString::number(cmvsPmvs->threshold()));
      stream.writeTextElement("WindowSize", QString::number(cmvsPmvs->windowSize()));
      stream.writeTextElement("ImagesPerCluster", QString::number(cmvsPmvs->imagesPerCluster()));
      stream.writeTextElement("MinimunImageNumber", QString::number(cmvsPmvs->minimunImageNumber()));
      stream.writeTextElement("UseVisibilityInformation", cmvsPmvs->useVisibilityInformation() ? "true" : "false");

      stream.writeEndElement();
    }

    stream.writeEndElement();
  }
}

QSize ProjectImp::readSize(QXmlStreamReader &stream) const
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

int ProjectImp::readInt(QXmlStreamReader &stream) const
{
  QString string = stream.readElementText();
  if (!string.isEmpty()) return string.toInt();
  else return 0;
}

double ProjectImp::readDouble(QXmlStreamReader &stream) const
{
  QString string = stream.readElementText();
  if (!string.isEmpty()) return string.toDouble();
  else return 0.;
}

bool ProjectImp::readBoolean(QXmlStreamReader &stream) const
{
  return stream.readElementText().compare("true") == 0 ? true : false;
}

//std::mutex ProjectControllerImp::sMutex;

//ProjectControllerImp::ProjectControllerImp()
// : ProjectController()
//{

//}

//bool ProjectControllerImp::read(const QString &file, Project &prj)
//{
//  std::lock_guard<std::mutex> lck(ProjectControllerImp::sMutex);

//  QFile input(file);
//  if (input.open(QIODevice::ReadOnly)) {
//    QXmlStreamReader stream;
//    stream.setDevice(&input);

//    if (stream.readNextStartElement()) {
//      if (stream.name() == "Inspector") {
//        while (stream.readNextStartElement()) {
//          if (stream.name() == "General") {
//            readGeneral(stream, prj);
//          } else if (stream.name() == "Database") {
//            readDatabase(stream, prj);
//          } else if (stream.name() == "Cameras") {
//            readCameras(stream, prj);
//          } else if (stream.name() == "Images") {
//            readImages(stream, prj);
//          } /*else if (stream.name() == "GroundTruth") {
//            readGroundTruth(stream, prj);
//          } else if (stream.name() == "Sessions") {
//            readSessions(stream, prj);
//          }*/
//        }
//      } else {
//        stream.raiseError(QObject::tr("Incorrect project file"));
//        return true;
//      }
//    }
//  }
//  return false;
//}

//bool ProjectControllerImp::write(const QString &file, const Project &prj) const
//{
//  QFileInfo file_info(file);
//  QString tmpfile = file_info.path().append(file_info.baseName()).append(".bak");
//  std::ifstream  src(file.toStdString().c_str(), std::ios::binary);
//  std::ofstream  dst(tmpfile.toStdString().c_str(), std::ios::binary);
//  dst << src.rdbuf();
//  src.close();
//  dst.close();

//  std::lock_guard<std::mutex> lck(ProjectControllerImp::sMutex);

//  QFile output(file);
//  output.open(QFile::WriteOnly);
//  QXmlStreamWriter stream(&output);
//  stream.setAutoFormatting(true);
//  stream.writeStartDocument();

//  stream.writeStartElement("Inspector");
//  {
//    writeVersion(stream, prj.version());
//    writeGeneral(stream, prj);
//    writeCameras(stream, prj);
//    writeImages(stream, prj);
//    writeDatabase(stream, prj);
////    writeGroundTruth(stream, prj.groundTruth());
////    writeSessions(stream, prj);
//  }

//  stream.writeEndElement(); // Inspector

//  output.close();

//  std::remove(tmpfile.toStdString().c_str());

//  return false;
//}

//bool ProjectControllerImp::checkOldVersion(const QString &file) const
//{
//  std::lock_guard<std::mutex> lck(ProjectControllerImp::sMutex);
//  bool bUpdateVersion = false;
//  QFile input(file);
//  if (input.open(QIODevice::ReadOnly)) {
//    QXmlStreamReader stream;
//    stream.setDevice(&input);

//    if (stream.readNextStartElement()) {
//      if (stream.name() == "Inspector") {
//        QString version = "0";
//        for (auto &attr : stream.attributes()) {
//          if (attr.name().compare(QString("version")) == 0) {
//            version = attr.value().toString();
//            break;
//          }
//        }
//        if (version.compare(INSPECTOR_PROJECT_FILE_VERSION) < 0) {
//          // Es una versión mas antigua
//          bUpdateVersion = true;
//        }
//      } else
//        stream.raiseError(QObject::tr("Incorrect file"));
//    }
//  }
//  return bUpdateVersion;
//}

//void ProjectControllerImp::oldVersionBak(const QString &file) const
//{
//  // Versión antigua
//  QString version = "0";
//  QFile input(file);
//  if (input.open(QIODevice::ReadOnly)) {
//    QXmlStreamReader stream;
//    stream.setDevice(&input);

//    if (stream.readNextStartElement()) {
//      if (stream.name() == "Inspector") {
//        for (auto &attr : stream.attributes()) {
//          if (attr.name().compare(QString("version")) == 0) {
//            version = attr.value().toString();
//            break;
//          }
//        }
//      }
//    }
//  }

//  QFileInfo file_info(file);
//  QString tmpfile = file_info.path().append(file_info.baseName()).append("_v").append(version).append(".bak");
//  std::ifstream  src(file.toStdString().c_str(), std::ios::binary);
//  std::ofstream  dst(tmpfile.toStdString().c_str(), std::ios::binary);
//  dst << src.rdbuf();
//  src.close();
//  dst.close();
//}

//void ProjectControllerImp::readGeneral(QXmlStreamReader &stream, Project &prj)
//{
//  while (stream.readNextStartElement()) {
//    if (stream.name() == "Name") {
//      prj.setName(stream.readElementText());
//    } else if (stream.name() == "Path") {
//      prj.setProjectFolder(stream.readElementText());
//    } else if (stream.name() == "Description") {
//      prj.setDescription(stream.readElementText());
//    } else
//      stream.skipCurrentElement();
//  }
//}

//void ProjectControllerImp::readDatabase(QXmlStreamReader &stream, Project &prj)
//{
//  prj.setDatabase(stream.readElementText());
//}

//void ProjectControllerImp::readImages(QXmlStreamReader &stream, Project &prj)
//{
//  while (stream.readNextStartElement()) {
//    if (stream.name() == "Image") {
//      prj.addImage(readImage(stream));
//    } else
//      stream.skipCurrentElement();
//  }
//}

//Image ProjectControllerImp::readImage(QXmlStreamReader &stream)
//{
//  Image photo(new Image);
//  while (stream.readNextStartElement()) {
//    if (stream.name() == "File") {
//      photo->setPath(stream.readElementText());
//    } else if (stream.name() == "CameraId") {
//      photo->setCameraId(readInt(stream));
//    } else if (stream.name() == "LongitudeExif") {
//      photo->setLongitudeExif(readDouble(stream));
//    } else if (stream.name() == "LongitudeExif") {
//      photo->setLongitudeExif(readDouble(stream));
//    } else if (stream.name() == "LatitudeExif") {
//      photo->setLatitudeExif(readDouble(stream));
//    } else if (stream.name() == "AltitudeExif") {
//      photo->setAltitudeExif(readDouble(stream));
//    } else
//      stream.skipCurrentElement();
//  }
//  return photo;
//}

//void ProjectControllerImp::readCameras(QXmlStreamReader &stream, Project &prj)
//{
//  while (stream.readNextStartElement()) {
//    if (stream.name() == "Camera") {
//      prj.addCamera(readCamera(stream));
//    } else
//      stream.skipCurrentElement();
//  }
//}

//Camera ProjectControllerImp::readCamera(QXmlStreamReader &stream)
//{
//  Camera camera(new Camera);

//  int id = 0;
//  for (auto &attr : stream.attributes()) {
//    if (attr.name().compare(QString("id")) == 0) {
//      id = attr.value().toInt();
//      break;
//    }
//  }

//  while (stream.readNextStartElement()) {
//    if (stream.name() == "Make") {
//      camera->setMake(stream.readElementText());
//    } else if (stream.name() == "Model") {
//      camera->setModel(stream.readElementText());
//    } else if (stream.name() == "Type") {
//      camera->setType(stream.readElementText());
//    } else if (stream.name() == "Focal") {
//      camera->setFocal(readDouble(stream));
//    } else if (stream.name() == "Width") {
//      camera->setWidth(readInt(stream));
//    } else if (stream.name() == "Height") {
//      camera->setHeight(readInt(stream));
//    } else if (stream.name() == "SensorSize") {
//      camera->setSensorSize(readDouble(stream));
//    } else if (stream.name() == "Calibration") {

//      QString sensor_type("RADIAL");
//      for (auto &attr : stream.attributes()) {
//        if (attr.name().compare(QString("type")) == 0) {
//          sensor_type = attr.value().toString();
//          break;
//        }
//      }
//      camera->setType(sensor_type);

////                    while (stream.readNextStartElement()) {
////                      std::shared_ptr<Calibration> calibration = camera->calibration();
////                      if (stream.name() == "f") {
////                        Calibration::parameter_iterator it_param = calibration->find(Calibration::Parameters::focal);
////                        if (it_param != calibration->parametersEnd()) {
////                          it_param->second = stream.readElementText().toDouble();
////                        }
////                      } else if (stream.name() == "cx") {
////                        Calibration::parameter_iterator it_param = calibration->find(Calibration::Parameters::cx);
////                        if (it_param != calibration->parametersEnd()) {
////                          it_param->second = stream.readElementText().toDouble();
////                        }
////                      } else if (stream.name() == "cy") {
////                        Calibration::parameter_iterator it_param = calibration->find(Calibration::Parameters::cy);
////                        if (it_param != calibration->parametersEnd()) {
////                          it_param->second = stream.readElementText().toDouble();
////                        }
////                      } else if (stream.name() == "fx") {
////                        Calibration::parameter_iterator it_param = calibration->find(Calibration::Parameters::focalx);
////                        if (it_param != calibration->parametersEnd()) {
////                          it_param->second = stream.readElementText().toDouble();
////                        }
////                      } else if (stream.name() == "fy") {
////                        Calibration::parameter_iterator it_param = calibration->find(Calibration::Parameters::focaly);
////                        if (it_param != calibration->parametersEnd()) {
////                          it_param->second = stream.readElementText().toDouble();
////                        }
////                      } else if (stream.name() == "k1") {
////                        Calibration::parameter_iterator it_param = calibration->find(Calibration::Parameters::k1);
////                        if (it_param != calibration->parametersEnd()) {
////                          it_param->second = stream.readElementText().toDouble();
////                        }
////                      } else if (stream.name() == "k2") {
////                        Calibration::parameter_iterator it_param = calibration->find(Calibration::Parameters::k2);
////                        if (it_param != calibration->parametersEnd()) {
////                          it_param->second = stream.readElementText().toDouble();
////                        }
////                      } else if (stream.name() == "k3") {
////                        Calibration::parameter_iterator it_param = calibration->find(Calibration::Parameters::k3);
////                        if (it_param != calibration->parametersEnd()) {
////                          it_param->second = stream.readElementText().toDouble();
////                        }
////                      } else if (stream.name() == "k4") {
////                        Calibration::parameter_iterator it_param = calibration->find(Calibration::Parameters::k4);
////                        if (it_param != calibration->parametersEnd()) {
////                          it_param->second = stream.readElementText().toDouble();
////                        }
////                      } else if (stream.name() == "k5") {
////                        Calibration::parameter_iterator it_param = calibration->find(Calibration::Parameters::k5);
////                        if (it_param != calibration->parametersEnd()) {
////                          it_param->second = stream.readElementText().toDouble();
////                        }
////                      } else if (stream.name() == "k6") {
////                        Calibration::parameter_iterator it_param = calibration->find(Calibration::Parameters::k6);
////                        if (it_param != calibration->parametersEnd()) {
////                          it_param->second = stream.readElementText().toDouble();
////                        }
////                      } else if (stream.name() == "p1") {
////                        Calibration::parameter_iterator it_param = calibration->find(Calibration::Parameters::p1);
////                        if (it_param != calibration->parametersEnd()) {
////                          it_param->second = stream.readElementText().toDouble();
////                        }
////                      } else if (stream.name() == "p2") {
////                        Calibration::parameter_iterator it_param = calibration->find(Calibration::Parameters::p2);
////                        if (it_param != calibration->parametersEnd()) {
////                          it_param->second = stream.readElementText().toDouble();
////                        }
////                      } else
////                        stream.skipCurrentElement();
////                    }

//    } else
//      stream.skipCurrentElement();

////    if (id == 0) {
////      QByteArray camera_make = camera->make().toLocal8Bit();
////      QByteArray camera_model = camera->model().toLocal8Bit();
////      msgWarning("The id of camera %s %s is not valid", camera_make.data(), camera_model.data());
////    } else {
////      prj.addCamera(camera);
////    }
//  }
//  return camera;
//}

//void ProjectControllerImp::writeVersion(QXmlStreamWriter &stream, const QString &version) const
//{
//  stream.writeAttribute("version", version);
//}

//void ProjectControllerImp::writeGeneral(QXmlStreamWriter &stream, const Project &prj) const
//{
//  stream.writeStartElement("General");
//  {
//    stream.writeTextElement("Name", prj.name());
//    stream.writeTextElement("Path", prj.projectFolder());
//    stream.writeTextElement("Description", prj.description());
//  }
//  stream.writeEndElement();
//}

//void ProjectControllerImp::writeDatabase(QXmlStreamWriter &stream, const Project &prj) const
//{
//  stream.writeTextElement("Database", prj.database());
//}

//void ProjectControllerImp::writeCameras(QXmlStreamWriter &stream, const Project &prj) const
//{
//  stream.writeStartElement("Cameras");
//  {
//    for (auto it = prj.cameraBegin(); it != prj.cameraEnd(); it++) {
//      writeCamera(stream, (*it).first, (*it).second.get());
//    }
//  }
//  stream.writeEndElement();
//}

//void ProjectControllerImp::writeCamera(QXmlStreamWriter &stream, int id, const Camera *camera) const
//{
//  stream.writeStartElement("Camera");
//  {
//    stream.writeAttribute("id", QString::number(id));

//    stream.writeTextElement("Make", camera->make());
//    stream.writeTextElement("Model", camera->model());
//    stream.writeTextElement("Type", camera->type());
//    stream.writeTextElement("Focal", QString::number(camera->focal()));
//    stream.writeTextElement("Width", QString::number(camera->width()));
//    stream.writeTextElement("Height", QString::number(camera->height()));
//    stream.writeTextElement("SensorSize", QString::number(camera->sensorSize()));

////    stream.writeStartElement("Calibration");
////    {
////      std::shared_ptr<Calibration> calibration = it->second->calibration();

////      stream.writeAttribute("type", it->second->type());

////      for (Calibration::parameter_iterator it = calibration->parametersBegin();
////           it != calibration->parametersEnd(); it++){
////        QString param_name(calibration->parameterName(it->first).c_str());
////        stream.writeTextElement(param_name, QString::number(it->second));
////      }
////    }
////    stream.writeEndElement(); // Calibration
//  }
//  stream.writeEndElement(); // Camera
//}

//void ProjectControllerImp::writeImages(QXmlStreamWriter &stream, const Project &prj) const
//{
//  stream.writeStartElement("Images");
//  {
//    for (auto it = prj.imageBegin(); it != prj.imageEnd(); it++){
//      writeImage(stream, (*it).get());
//    }
//  }
//  stream.writeEndElement();
//}

//void ProjectControllerImp::writeImage(QXmlStreamWriter &stream, const Image *image) const
//{
//  stream.writeStartElement("Image");
//  {
//    stream.writeTextElement("Name", image->name());
//    stream.writeTextElement("File", image->path());
//    stream.writeTextElement("CameraId", QString::number(image->cameraId()));
//    stream.writeTextElement("LongitudeExif", QString::number(image->longitudeExif()));
//    stream.writeTextElement("LatitudeExif", QString::number(image->latitudeExif()));
//    stream.writeTextElement("AltitudeExif", QString::number(image->altitudeExif()));
//  }
//  stream.writeEndElement();
//}

//QSize ProjectControllerImp::readSize(QXmlStreamReader &stream) const
//{
//  QSize blockSize;
//  while (stream.readNextStartElement()) {
//    if (stream.name() == "Width") {
//      blockSize.setWidth(readInt(stream));
//    } else if (stream.name() == "Height") {
//      blockSize.setHeight(readInt(stream));
//    } else
//      stream.skipCurrentElement();
//  }
//  return blockSize;
//}

//int ProjectControllerImp::readInt(QXmlStreamReader &stream) const
//{
//  QString string = stream.readElementText();
//  if (!string.isEmpty()) return string.toInt();
//  else return 0;
//}

//double ProjectControllerImp::readDouble(QXmlStreamReader &stream) const
//{
//  QString string = stream.readElementText();
//  if (!string.isEmpty()) return string.toDouble();
//  else return 0.;
//}

//bool ProjectControllerImp::readBoolean(QXmlStreamReader &stream) const
//{
//  return stream.readElementText().compare("true") == 0 ? true : false;
//}

} // end namespace inspector
