/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "graphos/core/project.h"

#include "graphos/core/features/sift.h"
#include "graphos/core/features/matching.h"
#include "graphos/core/dense/Smvs.h"
#include "graphos/core/dense/CmvsPmvs.h"
#include "graphos/core/dense/mvs.h"
#include "graphos/core/dtm/invdist.h"
#include "graphos/core/dtm/invdistnn.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/core/mesh/PoissonRecon.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/exception.h>

#include <colmap/base/database.h>

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamWriter>

#include <fstream>

#define GRAPHOS_PROJECT_FILE_VERSION "1.0"

namespace graphos
{

std::mutex ProjectImp::sMutex;

ProjectImp::ProjectImp()
  : Project(),
    mName(""),
    mDescription(""),
    mProjectFolder(""),
    mProjectPath(""),
    mVersion(GRAPHOS_PROJECT_FILE_VERSION),
    mDatabase(""),
    mCrs(""),
    mReconstructionPath(""),
    mCameraCount(0),
    mTransform(tl::Matrix<double, 4, 4>::identity())
{
    mImages.reserve(1000);
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

tl::Path ProjectImp::projectFolder() const
{
    return mProjectFolder;
}

void ProjectImp::setProjectFolder(const tl::Path &folder)
{
    mProjectFolder = folder;
}

tl::Path ProjectImp::projectPath() const
{
    return mProjectPath;
}


QString ProjectImp::version() const
{
    return mVersion;
}

tl::Path ProjectImp::database() const
{
    return mDatabase;
}

void ProjectImp::setDatabase(const tl::Path &database)
{
    mDatabase = database;
}

QString ProjectImp::crs() const
{
    return mCrs;
}

void ProjectImp::setCrs(const QString &crs)
{
    mCrs = crs;
}

void ProjectImp::addImage(const Image &img)
{
    TL_TODO("Comprobar el id por si se modifica a mano el xml")
    //tl::Path img_path(img.path().toStdWString());
    //size_t id = tl::Path::hash(img_path);
    auto it = mImages.find(img.id());
    if (it != mImages.end()) {
        tl::Message::warning("Image {} already in the project", img.path().toStdString());
    } else {
        mImages[img.id()] = img;
    }
}

bool ProjectImp::updateImage(size_t imageId, const Image &image)
{
    auto it = mImages.find(imageId);
    if (it != mImages.end()) {
        mImages[imageId] = image;
        return true;
    } else {
        return false;
    }
}

void ProjectImp::removeImage(size_t imageId)
{
    auto it = mImages.find(imageId);
    if (it != mImages.end()) {
        mImages.erase(it);
    }

    TL_TODO("Borrar las features, matches, etc")
}

Image ProjectImp::findImageById(size_t id) const
{
    try {
        return mImages.at(id);
    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Image ID not found");
    }
}

bool ProjectImp::existImage(size_t imageId) const
{
    auto it = mImages.find(imageId);
    return it != mImages.end();
}

const std::unordered_map<size_t, Image> &ProjectImp::images() const
{
    return mImages;
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

const std::map<int, Camera> &ProjectImp::cameras() const
{
    return mCameras;
}

Camera ProjectImp::findCamera(const QString &make, const QString &model) const
{
    Camera camera;
    for (auto it = mCameras.begin(); it != mCameras.end(); it++) {
        camera = it->second;
        if (camera.make().compare(make.toStdString()) == 0 &&
            camera.model().compare(model.toStdString()) == 0) {
            return camera;
        }
    }

    TL_THROW_EXCEPTION("Camera not found: {} {}", make.toStdString(), model.toStdString());
}

Camera ProjectImp::findCamera(int idCamera) const
{
    auto it = mCameras.find(idCamera);
    if (it != mCameras.end()) {
        return mCameras.at(idCamera);
    } else {
        throw std::runtime_error("Camera not exist");
    }
}

bool ProjectImp::existCamera(const QString &make, const QString &model) const
{
    for (auto it = mCameras.begin(); it != mCameras.end(); it++) {
        Camera camera = it->second;
        if (camera.make().compare(make.toStdString()) == 0 &&
            camera.model().compare(model.toStdString()) == 0) {
            return true;
        }
    }
    return false;
}

bool ProjectImp::updateCamera(int idCamera, const Camera &camera)
{
    auto it = mCameras.find(idCamera);
    if (it != mCameras.end()) {
        it->second = camera;

        colmap::camera_t camera_id = static_cast<colmap::camera_t>(idCamera);
        colmap::Database database(this->database().toString());
        colmap::Camera camera_colmap = database.ReadCamera(camera_id);
        QString colmap_camera_type = cameraToColmapType(camera);
        camera_colmap.SetModelIdFromName(colmap_camera_type.toStdString());
        database.UpdateCamera(camera_colmap);
        database.Close();

        return true;
    } else {
        return false;
    }
}

bool ProjectImp::removeCamera(int idCamera)
{
    auto it = mCameras.find(idCamera);
    if (it != mCameras.end()) {
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
        if (camera.make().compare(make.toStdString()) == 0 &&
            camera.model().compare(model.toStdString()) == 0) {
            return it->first;
        }
    }
    return 0;
}

size_t ProjectImp::camerasCount() const
{
    return mCameras.size();
}

std::shared_ptr<Feature> ProjectImp::featureExtractor() const
{
    return mFeatureExtractor;
}

void ProjectImp::setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor)
{
    mFeatureExtractor = featureExtractor;
}

QString ProjectImp::features(size_t imageId) const
{
    return mFeatures.at(imageId);
}

void ProjectImp::addFeatures(size_t imageId, const QString &featureFile)
{
    mFeatures[imageId] = featureFile;
}

void ProjectImp::removeFeatures()
{
    mFeatures.clear();
    this->removeMatchesPair();
}

void ProjectImp::removeFeatures(size_t imageId)
{
    auto it = mFeatures.find(imageId);
    if (it != mFeatures.end()) {
        mFeatures.erase(it);
    }
}

const std::unordered_map<size_t, QString> &ProjectImp::features() const
{
    return mFeatures;
}

std::shared_ptr<FeatureMatching> ProjectImp::featureMatching() const
{
    return mFeatureMatching;
}

void ProjectImp::setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching)
{
    mFeatureMatching = featureMatching;
}

void ProjectImp::addMatchesPair(size_t imageLeftId,
                                size_t imageRightId)
{
    auto it = mImagesPairs.find(imageLeftId);
    if (it != mImagesPairs.end()) {
        for (auto &pair : it->second) {
            if (pair == imageRightId) {
                return;
            }
        }
    }

    mImagesPairs[imageLeftId].push_back(imageRightId);
    mImagesPairs[imageRightId].push_back(imageLeftId);
}

const std::vector<size_t> ProjectImp::matchesPairs(size_t imageId) const
{
    std::vector<size_t> pairs;

    for (auto &matches : mImagesPairs) {
        if (imageId == matches.first) {
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

void ProjectImp::removeMatchesPair(size_t imageLeftId)
{
    auto it = mImagesPairs.find(imageLeftId);
    if (it != mImagesPairs.end()) {
        mImagesPairs.erase(it);
    }
}

tl::Path ProjectImp::sparseModel() const
{
    return mSparseModel;
}

void ProjectImp::setSparseModel(const tl::Path &sparseModel)
{
    mSparseModel = sparseModel;
}

tl::Path ProjectImp::offset() const
{
    return mOffset;
}

void ProjectImp::setOffset(const tl::Path &offset)
{
    mOffset = offset;
}

tl::Path ProjectImp::groundPoints() const
{
    return mGroundPoints;
}

void ProjectImp::setGroundPoints(const tl::Path &groundPoints)
{
    mGroundPoints = groundPoints;
}

tl::Path ProjectImp::reconstructionPath() const
{
    return mReconstructionPath;
}

void ProjectImp::setReconstructionPath(const tl::Path &reconstructionPath)
{
    mReconstructionPath = reconstructionPath;
}

bool ProjectImp::isPhotoOriented(size_t imageId) const
{
    return mPhotoOrientation.find(imageId) != mPhotoOrientation.end();
}

CameraPose ProjectImp::photoOrientation(size_t imageId) const
{
    return mPhotoOrientation.at(imageId);
}

const std::unordered_map<size_t, CameraPose> &ProjectImp::poses() const
{
    return mPhotoOrientation;
}

void ProjectImp::addPhotoOrientation(size_t imageId, 
                                     const CameraPose &photoOrientation)
{
    mPhotoOrientation[imageId] = photoOrientation;
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

void ProjectImp::setDenseModel(const tl::Path &denseModel)
{
    mDenseModel = denseModel;
}

tl::Path ProjectImp::denseModel() const
{
    return mDenseModel;
}

void ProjectImp::clearDensification()
{
    mDenseModel.clear();
}

std::shared_ptr<PoissonReconParameters> ProjectImp::meshParameters() const
{
    return mMeshParameters;
}

void ProjectImp::setMeshParameters(const std::shared_ptr<PoissonReconParameters> &meshParameters)
{
    mMeshParameters = meshParameters;
}

tl::Path ProjectImp::meshPath() const
{
    return mMeshModel;
}

void ProjectImp::setMeshPath(const tl::Path &meshPath)
{
    mMeshModel = meshPath;
}

void ProjectImp::clearMesh()
{
    mMeshModel.clear();
}

const DTMData &ProjectImp::dtm() const
{
    return mDTM;
}

DTMData &ProjectImp::dtm()
{
    return mDTM;
}

void ProjectImp::setDtm(const DTMData &dtm)
{
    mDTM = dtm;
}

void ProjectImp::clearDTM()
{
    mDTM.dsmPath.clear();
    mDTM.dtmPath.clear();
    mDTM.gsd = 0.1;
}

tl::Path ProjectImp::orthophotoPath() const
{
    return mOrthophoto;
}

void ProjectImp::setOrthophotoPath(const tl::Path &orthophotoPath)
{
    mOrthophoto = orthophotoPath;
}

void ProjectImp::clearOrthophotoDTM()
{
    mOrthophoto.clear();
}

void ProjectImp::clear()
{
    mName = "";
    mDescription = "";
    mProjectFolder.clear();
    mProjectPath.clear();
    mVersion = GRAPHOS_PROJECT_FILE_VERSION;
    mDatabase.clear();
    mCrs = "";
    mImages.clear();
    mCameras.clear();
    mFeatureExtractor.reset();
    mFeatures.clear();
    mFeatureMatching.reset();
    mImagesPairs.clear();
    mPhotoOrientation.clear();
    mSparseModel.clear();
    mOffset.clear();
    mGroundPoints.clear();
    mReconstructionPath.clear();
    mDensification.reset();
    mMeshModel.clear();
    mDenseModel.clear();
    clearDTM();
    mOrthophoto.clear();
    mCameraCount = 0;
    mTransform = tl::Matrix<double, 4, 4>::identity();
}

bool ProjectImp::load(const tl::Path &file)
{
    std::lock_guard<std::mutex> lck(ProjectImp::sMutex);

    bool err = false;
    QFile input(QString::fromStdWString(file.toWString()));
    mProjectPath = file;
    mProjectPath.normalize();

    if (input.open(QIODevice::ReadOnly)) {
        QXmlStreamReader stream;
        stream.setDevice(&input);

        err = this->read(stream);
        input.close();
    } else err = true;

    return err;
}

bool ProjectImp::save(const tl::Path &file)
{
    bool err = false;
    std::lock_guard<std::mutex> lck(ProjectImp::sMutex);

    mProjectPath = file;

    tl::Path tmp_file = file;
    tmp_file.replaceExtension(".bak");
    std::ifstream  src(file.toString(), std::ios::binary);
    std::ofstream  dst(tmp_file.toString(), std::ios::binary);
    dst << src.rdbuf();
    src.close();
    dst.close();

    try {

        QFile output(QString::fromStdWString(file.toWString()));
        if (output.open(QFile::WriteOnly)) {
            QXmlStreamWriter stream(&output);
            stream.setAutoFormatting(true);
            stream.writeStartDocument();

            stream.writeStartElement("Graphos");
            {
                writeVersion(stream);
                writeGeneral(stream);
                writeCameras(stream);
                writeImages(stream);
                writeDatabase(stream);
                writeCrs(stream);
                writeFeatures(stream);
                writeMatches(stream);
                writeOrientations(stream);
                writeDensification(stream);
                writeMesh(stream);
                writeDtm(stream);
                writeOrthophoto(stream);
            }

            stream.writeEndElement(); // Graphos

            output.close();

        } else {
            err = true;
        }

    } catch (std::exception &e) {
        tl::printException(e);

        std::ifstream  src(tmp_file.toString(), std::ios::binary);
        std::ofstream  dst(file.toString(), std::ios::binary);
        dst << src.rdbuf();
        src.close();
        dst.close();

        err = true;
    }

    tl::Path::removeFile(tmp_file);

    return err;
}

bool ProjectImp::checkOldVersion(const tl::Path &file) const
{
    std::lock_guard<std::mutex> lck(ProjectImp::sMutex);
    bool bUpdateVersion = false;

    QFile input(QString::fromStdWString(file.toWString()));

    if (input.open(QIODevice::ReadOnly)) {
        QXmlStreamReader stream;
        stream.setDevice(&input);

        if (stream.readNextStartElement()) {
            if (stream.name() == "Graphos") {
                QString version = "0";
                for (auto &attr : stream.attributes()) {
                    if (attr.name().compare(QString("version")) == 0) {
                        version = attr.value().toString();
                        break;
                    }
                }
                if (version.compare(GRAPHOS_PROJECT_FILE_VERSION) < 0) {
                    // Es una versión mas antigua
                    bUpdateVersion = true;
                }
            } else
                stream.raiseError(QObject::tr("Incorrect file"));
        }
    }

    return bUpdateVersion;
}

void ProjectImp::oldVersionBak(const tl::Path &file) const
{
    // Versión antigua
    QString version = "0";
    QFile input(QString::fromStdWString(file.toWString()));

    if (input.open(QIODevice::ReadOnly)) {
        QXmlStreamReader stream;
        stream.setDevice(&input);

        if (stream.readNextStartElement()) {
            if (stream.name() == "Graphos") {
                for (auto &attr : stream.attributes()) {
                    if (attr.name().compare(QString("version")) == 0) {
                        version = attr.value().toString();
                        break;
                    }
                }
            }
        }
    }

    tl::Path tmp_file = file;
    std::string bak_file = file.baseName().toString();
    bak_file.append("_v").append(version.toStdString()).append(".bak");
    tmp_file.replaceFileName(bak_file);
    std::ifstream  src(file.toString(), std::ios::binary);
    std::ofstream  dst(tmp_file.toString(), std::ios::binary);
    dst << src.rdbuf();
    src.close();
    dst.close();
}

tl::Matrix<double, 4, 4> ProjectImp::transform() const
{
    return mTransform;
}

void ProjectImp::setTransform(const tl::Matrix<double, 4, 4> &transform)
{
    mTransform = transform;
}

bool ProjectImp::read(QXmlStreamReader &stream)
{
    if (stream.readNextStartElement()) {
        if (stream.name() == "Graphos") {
            while (stream.readNextStartElement()) {
                if (stream.name() == "General") {
                    this->readGeneral(stream);
                } else if (stream.name() == "Database") {
                    this->readDatabase(stream);
                } else if (stream.name() == "ProjectCRS") {
                    this->readCrs(stream);
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
                } else if (stream.name() == "Mesh") {
                    readMesh(stream);
                } else if (stream.name() == "Dtm") {
                    readDtm(stream);
                } else if (stream.name() == "Orthophoto") {
                    readOrthophoto(stream);
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
            this->setProjectFolder(stream.readElementText().toStdWString());
        } else if (stream.name() == "Description") {
            this->setDescription(stream.readElementText());
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readDatabase(QXmlStreamReader &stream)
{
    this->setDatabase(stream.readElementText().toStdWString());
}

void ProjectImp::readCrs(QXmlStreamReader &stream)
{
    this->setCrs(stream.readElementText());
}

void ProjectImp::readImages(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "Image") {
            this->addImage(readImage(stream));
        } else
            stream.skipCurrentElement();
    }
}

Image ProjectImp::readImage(QXmlStreamReader &stream)
{
    Image photo;

    size_t id = 0;
    for (auto &attr : stream.attributes()) {
        if (attr.name().compare(QString("id")) == 0) {
            id = attr.value().toULongLong();
            break;
        }
    }

    while (stream.readNextStartElement()) {
        if (stream.name() == "File") {
            photo.setPath(stream.readElementText());
        } else if (stream.name() == "CameraId") {
            photo.setCameraId(readInt(stream));
        } else if (stream.name() == "CameraPosition") {
            photo.setCameraPose(readCameraPosition(stream));
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

CameraPose ProjectImp::readCameraPosition(QXmlStreamReader &stream)
{
    CameraPose cameraPose;
    tl::Point3<double> p;
    tl::Quaterniond q;

    while (stream.readNextStartElement()) {
        if (stream.name() == "CRS") {
            cameraPose.setCrs(stream.readElementText());
        } else if (stream.name() == "X") {
            p.x = readDouble(stream);
        } else if (stream.name() == "Y") {
            p.y = readDouble(stream);
        } else if (stream.name() == "Z") {
            p.z = readDouble(stream);
        } else if (stream.name() == "Source") {
            cameraPose.setSource(stream.readElementText());
        } else if (stream.name() == "QX") {
            q.x = readDouble(stream);
        } else if (stream.name() == "QY") {
            q.y = readDouble(stream);
        } else if (stream.name() == "QZ") {
            q.z = readDouble(stream);
        } else if (stream.name() == "QW") {
            q.w = readDouble(stream);
        }
    }
    cameraPose.setPosition(p);
    cameraPose.setQuaternion(q);
    return cameraPose;
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
            camera.setMake(stream.readElementText().toStdString());
        } else if (stream.name() == "Model") {
            camera.setModel(stream.readElementText().toStdString());
        } else if (stream.name() == "Type") {
            camera.setType(stream.readElementText().toStdString());
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
    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void ProjectImp::readFeatures(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement()) {
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
    while (stream.readNextStartElement()) {
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
        } /*else if (stream.name() == "Sigma") {
          sift->setSigma(readDouble(stream));
        }*/ else
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
    size_t image_id = 0;
    for (auto &attr : stream.attributes()) {
        if (attr.name().compare(QString("image_id")) == 0) {
            image_id = attr.value().toULongLong();
            break;
        }
    }

    QString file = stream.readElementText();
    this->addFeatures(image_id, file);
}

void ProjectImp::readMatches(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "FeatureMatchingMethod") {
            this->readMatchingMethod(stream);
        } else if (stream.name() == "Image") {
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
    size_t id_left_image = 0;
    for (auto &attr : stream.attributes()) {
        if (attr.name().compare(QString("image_id")) == 0) {
            id_left_image = attr.value().toULongLong();
            break;
        }
    }

    while (stream.readNextStartElement()) {
        if (stream.name() == "Pair") {
            this->addMatchesPair(id_left_image, stream.readElementText().toULongLong());
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readOrientations(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "ReconstructionPath") {
            this->readReconstructionPath(stream);
        } else if (stream.name() == "SparseModel") {
            this->readOrientationSparseModel(stream);
        } else if (stream.name() == "Offset") {
            this->readOffset(stream);
        } else if (stream.name() == "GroundPoints") {
            this->readGroundPoints(stream);
        } else if (stream.name() == "Image") {
            this->readPhotoOrientations(stream);
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readReconstructionPath(QXmlStreamReader &stream)
{
    this->setReconstructionPath(stream.readElementText().toStdWString());
}

void ProjectImp::readOrientationSparseModel(QXmlStreamReader &stream)
{
    this->setSparseModel(stream.readElementText().toStdWString());
}

void ProjectImp::readOffset(QXmlStreamReader &stream)
{
    this->setOffset(stream.readElementText().toStdWString());
}

void ProjectImp::readGroundPoints(QXmlStreamReader &stream)
{
    this->setGroundPoints(stream.readElementText().toStdWString());
}

void ProjectImp::readPhotoOrientations(QXmlStreamReader &stream)
{
    size_t id_image;
    for (auto &attr : stream.attributes()) {
        if (attr.name().compare(QString("image_id")) == 0) {
            id_image = attr.value().toULongLong();
            break;
        }
    }

    CameraPose camera_pose;
    tl::Point3<double> p;
    tl::RotationMatrix<double> rot;

    while (stream.readNextStartElement()) {
        if (stream.name() == "X") {
            p.x = readDouble(stream);
        } else if (stream.name() == "Y") {
            p.y = readDouble(stream);
        } else if (stream.name() == "Z") {
            p.z = readDouble(stream);
        } else if (stream.name() == "Rot") {
            QStringList rot_string = stream.readElementText().split(" ");
            rot.at(0, 0) = rot_string.at(0).toDouble();
            rot.at(0, 1) = rot_string.at(1).toDouble();
            rot.at(0, 2) = rot_string.at(2).toDouble();
            rot.at(1, 0) = rot_string.at(3).toDouble();
            rot.at(1, 1) = rot_string.at(4).toDouble();
            rot.at(1, 2) = rot_string.at(5).toDouble();
            rot.at(2, 0) = rot_string.at(6).toDouble();
            rot.at(2, 1) = rot_string.at(7).toDouble();
            rot.at(2, 2) = rot_string.at(8).toDouble();
        } else
            stream.skipCurrentElement();
    }

    camera_pose.setPosition(p);
    camera_pose.setRotationMatrix(rot);

    this->addPhotoOrientation(id_image, camera_pose);
}

void ProjectImp::readDensification(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "DenseModel") {
            this->readDenseModel(stream);
        } else if (stream.name() == "DensificationMethod") {
            this->readDensificationMethod(stream);
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readDenseModel(QXmlStreamReader &stream)
{
    this->setDenseModel(stream.readElementText().toStdWString());
}

void ProjectImp::readDensificationMethod(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "Smvs") {
            this->readSmvs(stream);
        } else if (stream.name() == "CmvsPmvs") {
            this->readCmvsPmvs(stream);
        } else if (stream.name() == "MVS") {
            this->readMVS(stream);
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

void ProjectImp::readMVS(QXmlStreamReader &stream)
{
    std::shared_ptr<Mvs> mvs = std::make_shared<MvsProperties>();

    while (stream.readNextStartElement()) {
        if (stream.name() == "ResolutionLevel") {
            mvs->setResolutionLevel(readInt(stream));
        } else if (stream.name() == "MinResolution") {
            mvs->setMinResolution(readInt(stream));
        } else if (stream.name() == "MaxResolution") {
            mvs->setMaxResolution(readInt(stream));
        } else if (stream.name() == "NumberViews") {
            mvs->setNumberViews(readInt(stream));
        } else if (stream.name() == "NumberViewsFuse") {
            mvs->setNumberViewsFuse(readInt(stream));
        } else
            stream.skipCurrentElement();
    }

    this->setDensification(mvs);
}

void ProjectImp::readMesh(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "MeshModel") {
            this->readMeshModel(stream);
        } else if (stream.name() == "PoissonParameters") {
            this->readMeshParameters(stream);
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readMeshModel(QXmlStreamReader &stream)
{
    this->setMeshPath(stream.readElementText().toStdWString());
}

void ProjectImp::readMeshParameters(QXmlStreamReader &stream)
{
    auto mesh = std::make_shared<PoissonReconParameters>();

    while (stream.readNextStartElement()) {
        if (stream.name() == "Depth") {
            mesh->setDepth(readInt(stream));
        } else if (stream.name() == "SolveDepth") {
            mesh->setSolveDepth(readInt(stream));
        } else if (stream.name() == "BoundaryType") {
            mesh->setBoundaryType(stream.text().toString());
        } else if (stream.name() == "Width") {
            mesh->setWidth(readInt(stream));
        } else if (stream.name() == "FullDepth") {
            mesh->setFullDepth(readInt(stream));
        } else
            stream.skipCurrentElement();
    }

    setMeshParameters(mesh);
}

void ProjectImp::readDtm(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "DTMPath") {
            this->mDTM.dtmPath = stream.readElementText().toStdWString();
        } else if (stream.name() == "DSMPath") {
            this->mDTM.dsmPath = stream.readElementText().toStdWString();
        }else if (stream.name() == "GSD") {
            this->mDTM.gsd = stream.readElementText().toDouble();
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readOrthophoto(QXmlStreamReader &stream)
{
    this->setOrthophotoPath(stream.readElementText().toStdWString());
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
        stream.writeTextElement("Path", QString::fromStdWString(this->projectFolder().toWString()));
        stream.writeTextElement("Description", this->description());
    }
    stream.writeEndElement();
}

void ProjectImp::writeDatabase(QXmlStreamWriter &stream) const
{
    stream.writeTextElement("Database", QString::fromStdWString(this->database().toWString()));
}

void ProjectImp::writeCrs(QXmlStreamWriter &stream) const
{
    stream.writeTextElement("ProjectCRS", this->crs());
}

void ProjectImp::writeCameras(QXmlStreamWriter &stream) const
{
    stream.writeStartElement("Cameras");
    {
        const auto &cameras = this->cameras();
        for (auto it = cameras.begin(); it != cameras.end(); it++) {
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

        stream.writeTextElement("Make", camera.make().c_str());
        stream.writeTextElement("Model", camera.model().c_str());
        stream.writeTextElement("Type", camera.type().c_str());
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
    if (calibration) {
        stream.writeStartElement("Calibration");
        {
            for (auto param = calibration->parametersBegin(); param != calibration->parametersEnd(); param++) {
                stream.writeTextElement(calibration->parameterName(param->first).c_str(), QString::number(param->second, 'f', 10));
            }
        }
        stream.writeEndElement(); // Calibration
    }
}

void ProjectImp::writeImages(QXmlStreamWriter &stream) const
{
    stream.writeStartElement("Images");
    {
        auto &images = this->images();
        for (auto it = images.begin(); it != images.end(); it++) {
            writeImage(stream, (*it));
        }
    }
    stream.writeEndElement();
}

void ProjectImp::writeImage(QXmlStreamWriter &stream, const std::pair<size_t, Image> &image) const
{
    stream.writeStartElement("Image");
    {
        stream.writeAttribute("id", QString::number(image.first));
        stream.writeTextElement("File", image.second.path());
        stream.writeTextElement("CameraId", QString::number(image.second.cameraId()));
        writeCameraPosition(stream, image.second.cameraPose());
        //    stream.writeTextElement("LongitudeExif", QString::number(image.longitudeExif()));
        //    stream.writeTextElement("LatitudeExif", QString::number(image.latitudeExif()));
        //    stream.writeTextElement("AltitudeExif", QString::number(image.altitudeExif()));
    }
    stream.writeEndElement();
}

void ProjectImp::writeCameraPosition(QXmlStreamWriter &stream,
                                     const CameraPose &cameraPosition) const
{
    if (!cameraPosition.isEmpty()) {
        stream.writeStartElement("CameraPosition");
        {
            stream.writeTextElement("CRS", cameraPosition.crs());
            stream.writeTextElement("X", QString::number(cameraPosition.position().x, 'f', 10));
            stream.writeTextElement("Y", QString::number(cameraPosition.position().y, 'f', 10));
            stream.writeTextElement("Z", QString::number(cameraPosition.position().z, 'f', 10));
            stream.writeTextElement("QX", QString::number(cameraPosition.quaternion().x, 'f', 10));
            stream.writeTextElement("QY", QString::number(cameraPosition.quaternion().y, 'f', 10));
            stream.writeTextElement("QZ", QString::number(cameraPosition.quaternion().z, 'f', 10));
            stream.writeTextElement("QW", QString::number(cameraPosition.quaternion().w, 'f', 10));
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
    if (auto feature = this->featureExtractor()) {
        stream.writeStartElement("FeatureExtractor");
        if (feature->type() == Feature::Type::sift) {
            this->writeSIFT(stream, dynamic_cast<Sift *>(feature.get()));
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
        stream.writeTextElement("ContrastThresholdAuto", sift->constrastThresholdAuto() ? "true" : "false");
        stream.writeTextElement("ContrastThreshold", QString::number(sift->contrastThreshold()));
        stream.writeTextElement("EdgeThreshold", QString::number(sift->edgeThreshold()));
        //stream.writeTextElement("Sigma", QString::number(sift->sigma()));
    }
    stream.writeEndElement(); // SIFT
}

void ProjectImp::writeFeatureFiles(QXmlStreamWriter &stream) const
{
    stream.writeStartElement("Files");
    {

        for (const auto &features : mFeatures) {
            stream.writeStartElement("FeatFile");
            {
                stream.writeAttribute("image_id", QString::number(features.first));
                stream.writeCharacters(features.second);
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
    if (auto matchingMethod = this->featureMatching()) {
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
    if (!mImagesPairs.empty()) {

        for (auto it = this->images().begin(); it != this->images().end(); it++) {
            stream.writeStartElement("Image");
            {
                stream.writeAttribute("image_id", QString::number(it->first));
                std::vector<size_t> pairs = this->matchesPairs(it->first);
                for (size_t i = 0; i < pairs.size(); i++) {
                    stream.writeTextElement("Pair", QString::number(pairs.at(i)));
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
        this->writeGroundPoints(stream);
        this->writePhotoOrientations(stream);
    }
    stream.writeEndElement(); // Orientations
}

void ProjectImp::writeReconstructionPath(QXmlStreamWriter &stream) const
{
    tl::Path reconstruction_path = reconstructionPath();
    if (!reconstruction_path.empty())
        stream.writeTextElement("ReconstructionPath", QString::fromStdWString(reconstruction_path.toWString()));
}

void ProjectImp::writeOrientationSparseModel(QXmlStreamWriter &stream) const
{
    QString sparse_model = QString::fromStdWString(sparseModel().toWString());
    if (!sparse_model.isEmpty())
        stream.writeTextElement("SparseModel", sparse_model);
}

void ProjectImp::writeOffset(QXmlStreamWriter &stream) const
{
    QString offset = QString::fromStdWString(this->offset().toWString());
    if (!offset.isEmpty())
        stream.writeTextElement("Offset", offset);
}

void ProjectImp::writeGroundPoints(QXmlStreamWriter &stream) const
{
    QString ground_points = QString::fromStdWString(this->groundPoints().toWString());
    if (!ground_points.isEmpty())
        stream.writeTextElement("GroundPoints", ground_points);
}

void ProjectImp::writePhotoOrientations(QXmlStreamWriter &stream) const
{
    if (!mPhotoOrientation.empty()) {
        for (const auto &image : this->images()) {
            if (this->isPhotoOriented(image.first)) {
                CameraPose photoOrientation = this->photoOrientation(image.first);
                stream.writeStartElement("Image");
                {
                    stream.writeAttribute("image_id", QString::number(image.first));
                    if (photoOrientation.position().x != 0. &&
                        photoOrientation.position().y != 0. &&
                        photoOrientation.position().z != 0.) {

                        stream.writeTextElement("X", QString::number(photoOrientation.position().x, 'f', 10));
                        stream.writeTextElement("Y", QString::number(photoOrientation.position().y, 'f', 10));
                        stream.writeTextElement("Z", QString::number(photoOrientation.position().z, 'f', 10));
                        QString rot_mat = QString::number(photoOrientation.rotationMatrix().at(0, 0), 'f', 10).append(" ");
                        rot_mat.append(QString::number(photoOrientation.rotationMatrix().at(0, 1), 'f', 10)).append(" ");
                        rot_mat.append(QString::number(photoOrientation.rotationMatrix().at(0, 2), 'f', 10)).append(" ");
                        rot_mat.append(QString::number(photoOrientation.rotationMatrix().at(1, 0), 'f', 10)).append(" ");
                        rot_mat.append(QString::number(photoOrientation.rotationMatrix().at(1, 1), 'f', 10)).append(" ");
                        rot_mat.append(QString::number(photoOrientation.rotationMatrix().at(1, 2), 'f', 10)).append(" ");
                        rot_mat.append(QString::number(photoOrientation.rotationMatrix().at(2, 0), 'f', 10)).append(" ");
                        rot_mat.append(QString::number(photoOrientation.rotationMatrix().at(2, 1), 'f', 10)).append(" ");
                        rot_mat.append(QString::number(photoOrientation.rotationMatrix().at(2, 2), 'f', 10));
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
    tl::Path dense_model = denseModel();
    if (!dense_model.empty())
        stream.writeTextElement("DenseModel", QString::fromStdWString(dense_model.toWString()));
}

void ProjectImp::writeDensificationMethod(QXmlStreamWriter &stream) const
{
    if (auto densificationMethod = this->densification()) {

        stream.writeStartElement("DensificationMethod");

        if (densificationMethod->method() == Densification::Method::smvs) {

            stream.writeStartElement("Smvs");

            auto smvs = std::dynamic_pointer_cast<Smvs>(densificationMethod);
            stream.writeTextElement("InputImageScale", QString::number(smvs->inputImageScale()));
            stream.writeTextElement("OutputDepthScale", QString::number(smvs->outputDepthScale()));
            stream.writeTextElement("SemiGlobalMatching", smvs->semiGlobalMatching() ? "true" : "false");
            stream.writeTextElement("SurfaceSmoothingFactor", QString::number(smvs->surfaceSmoothingFactor()));
            stream.writeTextElement("ShadingBasedOptimization", smvs->shadingBasedOptimization() ? "true" : "false");

            stream.writeEndElement();

        } else if (densificationMethod->method() == Densification::Method::cmvs_pmvs) {

            stream.writeStartElement("CmvsPmvs");

            auto cmvsPmvs = std::dynamic_pointer_cast<CmvsPmvs>(densificationMethod);
            stream.writeTextElement("Level", QString::number(cmvsPmvs->level()));
            stream.writeTextElement("CellSize", QString::number(cmvsPmvs->cellSize()));
            stream.writeTextElement("Threshold", QString::number(cmvsPmvs->threshold()));
            stream.writeTextElement("WindowSize", QString::number(cmvsPmvs->windowSize()));
            stream.writeTextElement("ImagesPerCluster", QString::number(cmvsPmvs->imagesPerCluster()));
            stream.writeTextElement("MinimunImageNumber", QString::number(cmvsPmvs->minimunImageNumber()));
            stream.writeTextElement("UseVisibilityInformation", cmvsPmvs->useVisibilityInformation() ? "true" : "false");

            stream.writeEndElement();
        } else if (densificationMethod->method() == Densification::Method::mvs) {

            stream.writeStartElement("MVS");

            auto mvs = std::dynamic_pointer_cast<Mvs>(densificationMethod);
            stream.writeTextElement("ResolutionLevel", QString::number(mvs->resolutionLevel()));
            stream.writeTextElement("MinResolution", QString::number(mvs->minResolution()));
            stream.writeTextElement("MaxResolution", QString::number(mvs->maxResolution()));
            stream.writeTextElement("NumberViews", QString::number(mvs->numberViews()));
            stream.writeTextElement("NumberViewsFuse", QString::number(mvs->numberViewsFuse()));

            stream.writeEndElement();
        }

        stream.writeEndElement();
    }
}

void ProjectImp::writeMesh(QXmlStreamWriter &stream) const
{
    stream.writeStartElement("Mesh");
    {
        this->writeMeshModel(stream);
        this->writeMeshParameters(stream);
    }
    stream.writeEndElement(); // Densification
}

void ProjectImp::writeMeshModel(QXmlStreamWriter &stream) const
{
    QString mesh_model = QString::fromStdWString(meshPath().toWString());
    if (!mesh_model.isEmpty())
        stream.writeTextElement("MeshModel", mesh_model);
}

void ProjectImp::writeMeshParameters(QXmlStreamWriter &stream) const
{
    if (auto mesh = std::dynamic_pointer_cast<PoissonReconParameters>(meshParameters())) {

        stream.writeStartElement("PoissonParameters");

        stream.writeTextElement("Depth", QString::number(mesh->depth()));
        stream.writeTextElement("SolveDepth", QString::number(mesh->solveDepth()));
        stream.writeTextElement("BoundaryType", mesh->boundaryType());
        stream.writeTextElement("Width", QString::number(mesh->width()));
        stream.writeTextElement("FullDepth", QString::number(mesh->fullDepth()));

        stream.writeEndElement();

    }
}

void ProjectImp::writeDtm(QXmlStreamWriter &stream) const
{
    if (mDTM.dtmPath.empty() || mDTM.dsmPath.empty()) return;

    stream.writeStartElement("Dtm");
    {
        stream.writeTextElement("DTMPath", QString::fromStdWString(mDTM.dtmPath.toWString()));
        stream.writeTextElement("DTMPath", QString::fromStdWString(mDTM.dsmPath.toWString()));
        stream.writeTextElement("GSD", QString::number(mDTM.gsd));
    }
    stream.writeEndElement();
}

void ProjectImp::writeOrthophoto(QXmlStreamWriter &stream) const
{
    QString ortho_path = QString::fromStdWString(orthophotoPath().toWString());
    if (!ortho_path.isEmpty())
        stream.writeTextElement("Orthophoto", ortho_path);
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

} // end namespace graphos
