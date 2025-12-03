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
#include "graphos/core/camera/Colmap.h"
#include "graphos/core/mesh/PoissonRecon.h"
#include "graphos/core/multispectral/Vignetting.h"

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
    //mReconstructionPath(""),
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
    mProjectFolder.normalize();
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
    mDatabase.normalize();
}

//QString ProjectImp::crs() const
//{
//    return mCrs;
//}
//
//void ProjectImp::setCrs(const QString &crs)
//{
//    mCrs = crs;
//}

void ProjectImp::addImage(const Image &img)
{
    TL_TODO("Comprobar el id por si se modifica a mano el xml")
    size_t image_id = Image::id(img);
    auto it = mImages.find(image_id);
    if (it != mImages.end()) {
        tl::Message::warning("Image {} already in the project", img.path().toStdString());
    } else {
        mImages[image_id] = img;
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

auto ProjectImp::findImageById(size_t id) const -> Image
{
    try {
        return mImages.at(id);
    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Image ID not found");
    }
}

auto ProjectImp::existImage(size_t imageId) const -> bool
{
    auto it = mImages.find(imageId);
    return it != mImages.end();
}

auto ProjectImp::images() const -> const std::unordered_map<size_t, Image> &
{
    return mImages;
}

size_t ProjectImp::imagesCount() const
{
    return mImages.size();
}

auto ProjectImp::addCamera(const Camera &camera) -> int
{
    mCameras.try_emplace(++mCameraCount, camera);
    return mCameraCount;
}

auto ProjectImp::cameras() const -> const std::map<int, Camera> &
{
    return mCameras;
}

auto ProjectImp::findCamera(const QString &make,
                              const QString &model, 
                              const QString &serialNumber, 
                              const QString &bandName) const -> Camera
{
    for (const auto &camera : mCameras) {
        Camera _camera = camera.second;
        if (_camera.make() == make.toStdString() &&
            _camera.model() == model.toStdString() &&
            _camera.serialNumber() == serialNumber.toStdString() &&
            _camera.bandName() == bandName.toStdString()) {
            return _camera;
        }
    }

    TL_THROW_EXCEPTION("Camera not found: {} {}", make.toStdString(), model.toStdString());
}

auto ProjectImp::findCamera(int idCamera) const -> Camera
{
    auto it = mCameras.find(idCamera);
    if (it != mCameras.end()) {
        return mCameras.at(idCamera);
    } else {
        throw std::runtime_error("Camera not exist");
    }
}

auto ProjectImp::existCamera(const QString &make, 
                             const QString &model, 
                             const QString &serialNumber, 
                             const QString &bandName) const -> bool
{
    for (const auto &camera : mCameras) {
        Camera _camera = camera.second;
        if (_camera.make() == make.toStdString() &&
            _camera.model() == model.toStdString() &&
            _camera.serialNumber() == serialNumber.toStdString() &&
            _camera.bandName() == bandName.toStdString()) {
            return true;
        }
    }
    return false;
}

auto ProjectImp::updateCamera(int idCamera, const Camera &camera) -> bool
{
    auto it = mCameras.find(idCamera);
    if (it != mCameras.end()) {
        it->second = camera;

        colmap::camera_t camera_id = static_cast<colmap::camera_t>(idCamera);
        colmap::Database database(this->database().toUtf8());
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

auto ProjectImp::removeCamera(int idCamera) -> bool
{
    auto it = mCameras.find(idCamera);
    if (it != mCameras.end()) {
        mCameras.erase(it);
        return true;
    } else {
        return false;
    }
}

auto ProjectImp::cameraId(const QString &make,
                          const QString &model,
                          const QString &serialNumber,
                          const QString &bandName) const -> int
{
    for (const auto &camera : mCameras) {
        if (camera.second.make() == make.toStdString() &&
            camera.second.model() == model.toStdString() &&
            camera.second.serialNumber() == serialNumber.toStdString() &&
            camera.second.bandName() == bandName.toStdString()) {
            return camera.first;
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

FeatureExtractorReport ProjectImp::featureExtractorReport() const
{
    return mFeatureExtractorReport;
}

void ProjectImp::setFeatureExtractorReport(const FeatureExtractorReport &report)
{
    mFeatureExtractorReport = report;
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

FeatureMatchingReport ProjectImp::featureMatchingReport() const
{
    return mFeatureMatchingReport;
}

void ProjectImp::setFeatureMatchingReport(const FeatureMatchingReport &report)
{
    mFeatureMatchingReport = report;
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

QString ProjectImp::enuCrs() const
{
    return mEnuCrs;
}

void ProjectImp::setEnuCrs(const QString &enuCrs)
{
    mEnuCrs = enuCrs;
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
    return tl::Path(mProjectFolder).append("sfm");
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
    mEnuCrs.clear();
    mOrientationReport = OrientationReport();
    this->clearDensification();
}

OrientationReport ProjectImp::orientationReport() const
{
    return mOrientationReport;
}

void ProjectImp::setOrientationReport(const OrientationReport &orientationReport)
{
    mOrientationReport = orientationReport;
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

DenseReport ProjectImp::denseReport() const
{
    return mDenseReport;
}

void ProjectImp::setDenseReport(const DenseReport &denseReport)
{
    mDenseReport = denseReport;
}

tl::Path ProjectImp::denseModel() const
{
    return mDenseModel;
}

void ProjectImp::clearDensification()
{
    mDenseModel.clear();
    mDenseReport = DenseReport();
    clearMesh();
    clearDem();
}

std::shared_ptr<PoissonReconProperties> ProjectImp::meshProperties() const
{
    return mMeshProperties;
}

void ProjectImp::setMeshProperties(const std::shared_ptr<PoissonReconProperties> &meshProperties)
{
    mMeshProperties = meshProperties;
}

tl::Path ProjectImp::meshPath() const
{
    return mMeshModel;
}

void ProjectImp::setMeshPath(const tl::Path &meshPath)
{
    mMeshModel = meshPath;
}

MeshReport ProjectImp::meshReport() const
{
    return mMeshReport;
}

void ProjectImp::setMeshReport(const MeshReport &report)
{
    mMeshReport = report;
}

void ProjectImp::clearMesh()
{
    mMeshModel.clear();
    mMeshReport = MeshReport();
}

const DemData &ProjectImp::dem() const
{
    return mDem;
}

DemData &ProjectImp::dem()
{
    return mDem;
}

void ProjectImp::setDem(const DemData &dem)
{
    mDem = dem;
}

DemReport ProjectImp::demReport() const
{
    return mDemReport;
}

void ProjectImp::setDemReport(const DemReport& report)
{
    mDemReport = report;
}

void ProjectImp::clearDem()
{
    mDem.dsmPath.clear();
    mDem.dtmPath.clear();
    mDem.epsgCode.clear();
    mDem.gsd = 0.1;
    mDemReport = DemReport();
    clearOrthophoto();
}

auto ProjectImp::orthophotos() const -> const std::map<size_t, OrthophotoData>&
{
    return mOrthophotos;
}

auto ProjectImp::orthophotos() -> std::map<size_t, OrthophotoData>&
{
    return mOrthophotos;
}

void ProjectImp::setOrthophoto(const OrthophotoData &orthophoto)
{
    auto id = tl::Path::hash(orthophoto.path);
    mOrthophotos[id] = orthophoto;
}

void ProjectImp::clearOrthophoto()
{
    mOrthophotos.clear();
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
    mFeatureExtractorReport = FeatureExtractorReport();
    mFeatures.clear();
    mFeatureMatching.reset();
    mImagesPairs.clear();
    clearReconstruction();
    mGroundPoints.clear();
    mOrientationReport = OrientationReport();
    mDensification.reset();
    clearDensification();
    clearMesh();
    clearDem();
    clearOrthophoto();
    mCameraCount = 0;
    mTransform = tl::Matrix<double, 4, 4>::identity();
}

void ProjectImp::load(const tl::Path &file)
{
    std::lock_guard<std::mutex> lck(ProjectImp::sMutex);

    try {

        QFile input(QString::fromStdWString(file.toWString()));
        mProjectPath = file;
        mProjectPath.normalize();

        if (input.open(QIODevice::ReadOnly)) {

            QXmlStreamReader stream;
            stream.setDevice(&input);

            this->read(stream);

            input.close();

        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Exception detected when reading the project");
    }

}

void ProjectImp::save(const tl::Path &file)
{
    std::lock_guard<std::mutex> lck(ProjectImp::sMutex);

    mProjectPath = file;
    mProjectPath.normalize();

    tl::Path tmp_file = mProjectPath;
    tmp_file.replaceExtension(".bak");

    try {

        std::ifstream src(mProjectPath.toString(), std::ios::binary);
        std::ofstream dst(tmp_file.toString(), std::ios::binary);
        dst << src.rdbuf();
        src.close();
        dst.close();

        QFile output(QString::fromStdWString(mProjectPath.toWString()));
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
                //writeCrs(stream);
                writeFeatures(stream);
                writeMatches(stream);
                writeOrientations(stream);
                writeDensification(stream);
                writeMesh(stream);
                writeDem(stream);
                writeOrthophoto(stream);
            }

            stream.writeEndElement(); // Graphos

            output.close();

        }

    } catch (...) {

        std::ifstream  src(tmp_file.toString(), std::ios::binary);
        std::ofstream  dst(mProjectPath.toString(), std::ios::binary);
        dst << src.rdbuf();
        src.close();
        dst.close();
        tl::Path::removeFile(tmp_file);

        TL_THROW_EXCEPTION_WITH_NESTED("Exception detected when writing the project");

    }

    tl::Path::removeFile(tmp_file);
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

tl::Matrix<double, 4, 4> &ProjectImp::transform()
{
    return mTransform;
}

const tl::Matrix<double, 4, 4> &ProjectImp::transform() const
{
    return mTransform;
}

void ProjectImp::setTransform(const tl::Matrix<double, 4, 4> &transform)
{
    mTransform = transform;
}

void ProjectImp::read(QXmlStreamReader &stream)
{
    if (stream.readNextStartElement()) {
        if (stream.name() == "Graphos") {
            while (stream.readNextStartElement()) {
                if (stream.name() == "General") {
                    this->readGeneral(stream);
                } else if (stream.name() == "Database") {
                    this->readDatabase(stream);
                //} else if (stream.name() == "ProjectCRS") {
                //    this->readCrs(stream);
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
                } else if (stream.name() == "Dem") {
                    readDem(stream);
                } else if (stream.name() == "Orthophoto") {
                    auto ortho = readOrthophoto(stream);
                    mOrthophotos[tl::Path::hash(ortho.path)] = ortho;
                } else
                    stream.skipCurrentElement();
            }
        } else {
            stream.raiseError(QObject::tr("Incorrect project file"));
        }
    }
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

//void ProjectImp::readCrs(QXmlStreamReader &stream)
//{
//    this->setCrs(stream.readElementText());
//}

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
        } else if (stream.name() == "Metadata") {
            readImageMetadata(stream, photo);
        }/*else if (stream.name() == "LongitudeExif") {
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
    tl::Point3<double> position;
    tl::Vector3d accuracy;
    tl::Quaterniond quaternion;

    while (stream.readNextStartElement()) {
        if (stream.name() == "CRS") {
            cameraPose.setCrs(stream.readElementText());
        } else if (stream.name() == "X") {
            position.x = readDouble(stream);
        } else if (stream.name() == "Y") {
            position.y = readDouble(stream);
        } else if (stream.name() == "Z") {
            position.z = readDouble(stream);
        } else if (stream.name() == "SX") {
            accuracy.x() = readDouble(stream);
        } else if (stream.name() == "SY") {
            accuracy.y() = readDouble(stream);
        } else if (stream.name() == "SZ") {
            accuracy.z() = readDouble(stream);
        } else if (stream.name() == "Source") {
            cameraPose.setSource(stream.readElementText());
        } else if (stream.name() == "QX") {
            quaternion.x = readDouble(stream);
        } else if (stream.name() == "QY") {
            quaternion.y = readDouble(stream);
        } else if (stream.name() == "QZ") {
            quaternion.z = readDouble(stream);
        } else if (stream.name() == "QW") {
            quaternion.w = readDouble(stream);
        } else if (stream.name() == "RtkFlag") {
            cameraPose.setRtkFlag(readInt(stream));
        }
    }

    cameraPose.setPosition(position);
    cameraPose.setAccuracy(accuracy);
    cameraPose.setQuaternion(quaternion);
    return cameraPose;
}

void ProjectImp::readImageMetadata(QXmlStreamReader &stream, Image &image)
{
    for (auto &attr : stream.attributes()) {
        image.addMetadata(attr.name().toString().toStdString(),
            attr.value().toString().toStdString());
    }
    stream.skipCurrentElement();
}

void ProjectImp::readCameras(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "Camera") {
            readCamera(stream);
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readCamera(QXmlStreamReader &stream)
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
        } else if (stream.name() == "SerialNumber") {
            camera.setSerialNumber(stream.readElementText().toStdString());
        } else if (stream.name() == "Type") {
            camera.setType(stream.readElementText().toStdString());
        } else if (stream.name() == "Focal") {
            camera.setFocal(readDouble(stream));
        } else if (stream.name() == "Width") {
            camera.setWidth(readInt(stream));
        } else if (stream.name() == "Height") {
            camera.setHeight(readInt(stream));
        } else if (stream.name() == "BitsPerPixel") {
            camera.setBitsPerPixel(readInt(stream));
        } else if (stream.name() == "SensorSize") {
            camera.setSensorSize(readDouble(stream));
        } else if (stream.name() == "BandName") {
            camera.setBandName(stream.readElementText().toStdString());
        } else if (stream.name() == "BlackLevel") {
            camera.setBlackLevel(readInt(stream));
        } /*else if (stream.name() == "VignettingCenter") {
            auto vignetting_center = tl::split<float>(stream.readElementText().toStdString(), ';');
            if (vignetting_center.size() == 2)
                camera.setVignettingCenter(tl::Point<float>(vignetting_center[0], vignetting_center[1]));
        } else if (stream.name() == "VignettingPolynomial") {
            auto vignetting_polynomial = tl::split<float>(stream.readElementText().toStdString(), ';');
            if (vignetting_polynomial.size() == 6)
                camera.setVignettingPolynomial(vignetting_polynomial);
        }*/ else if (stream.name() == "CalibratedHMatrix") {
            auto hmatrix = tl::split<float>(stream.readElementText().toStdString(), ' ');
            if (hmatrix.size() == 9) {
                tl::Matrix3x3f H;
                H(0, 0) = hmatrix[0];
                H(0, 1) = hmatrix[1];
                H(0, 2) = hmatrix[2];
                H(1, 0) = hmatrix[3];
                H(1, 1) = hmatrix[4];
                H(1, 2) = hmatrix[5];
                H(2, 0) = hmatrix[6];
                H(2, 1) = hmatrix[7];
                H(2, 2) = hmatrix[8];
                camera.setCalibratedHMatrix(H);
            }

        } else if (stream.name() == "PriorCalibration") {
            this->readPriorCalibration(stream, camera);
        } else if (stream.name() == "Calibration") {
            this->readCalibration(stream, camera);
        } else if (stream.name() == "Vignetting") {
            this->readVignetting(stream, camera);
        } else
            stream.skipCurrentElement();
    }

    mCameras.try_emplace(id, camera);
    mCameraCount = std::max(mCameraCount, id);
}

void ProjectImp::readPriorCalibration(QXmlStreamReader &stream, Camera &camera)
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

        camera.setPriorCalibration(calibration);

    } catch (std::exception &e) {
        tl::printException(e);
    }
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

void ProjectImp::readVignetting(QXmlStreamReader &stream, Camera &camera)
{
    try {

        QString vignetting_model = 0;
        for (auto &attr : stream.attributes()) {
            if (attr.name().compare(QString("model")) == 0) {
                vignetting_model = attr.value().toString();
                break;
            }
        }

        std::shared_ptr<Vignetting> model;

        if (vignetting_model == "radial") {

            tl::Point2d vignetting_center_point;
            std::vector<float> vignetting_polynomial;

            while (stream.readNextStartElement()) {

                if (stream.name() == "VignettingCenter") {
                    auto vignetting_center = tl::split<float>(stream.readElementText().toStdString(), ';');
                    if (vignetting_center.size() == 2)
                        vignetting_center_point = tl::Point2d(vignetting_center[0], vignetting_center[1]);
                } else if (stream.name() == "VignettingPolynomial") {
                    auto polynomial = tl::split<float>(stream.readElementText().toStdString(), ';');
                    if (polynomial.size() == 6)
                        vignetting_polynomial = polynomial;
                } else
                    stream.skipCurrentElement();
            }

            if (!vignetting_polynomial.empty())
                model = std::make_shared<VignettingRadial>(vignetting_center_point, vignetting_polynomial);

        } else if (vignetting_model == "polynomial2d") {

            std::vector<float> coeffs;
            std::vector<std::pair<int, int>> power_pairs;

            while (stream.readNextStartElement()) {

                if (stream.name() == "VignettingCoeffs") {
                    coeffs = tl::split<float>(stream.readElementText().toStdString(), ';');
                } else if (stream.name() == "VignettingPowers") {
                    auto powers = tl::split<float>(stream.readElementText().toStdString(), ';');
                    power_pairs.reserve(powers.size() / 2);
                    for (size_t i = 0; i < powers.size(); i += 2) {
                        power_pairs.push_back(std::make_pair(static_cast<int>(powers[i]), static_cast<int>(powers[i + 1])));
                    }
                } else
                    stream.skipCurrentElement();
            }

            model = std::make_shared<VignettingPolynomial2D>(coeffs, power_pairs);

        } else {
            stream.skipCurrentElement();
            return; 
        }

        camera.setVignettingModel(model);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void ProjectImp::readFeatures(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "FeatureExtractor") {
            this->readFeatureExtractor(stream);
        } else if (stream.name() == "Report") {
            this->readFeatureExtractorReport(stream);
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

void ProjectImp::readFeatureExtractorReport(QXmlStreamReader& stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "Features") {
            mFeatureExtractorReport.features = readInt(stream);
        } else if (stream.name() == "Time") {
            mFeatureExtractorReport.time = readDouble(stream);
        } else if (stream.name() == "Cuda") {
            mFeatureExtractorReport.cuda = readBoolean(stream);
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readSIFT(QXmlStreamReader &stream)
{
    auto sift = std::make_shared<Sift>();
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
        } else if (stream.name() == "Report") {
            this->readFeatureMatchingReport(stream);
        } else if (stream.name() == "Image") {
            this->readPairs(stream);
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readMatchingMethod(QXmlStreamReader &stream)
{
    auto matching_method = std::make_shared<FeatureMatching>();
    while (stream.readNextStartElement()) {
        if (stream.name() == "Distance") {
            matching_method->setDistance(readDouble(stream));
        } else if (stream.name() == "Ratio") {
            matching_method->setRatio(readDouble(stream));
        } else if (stream.name() == "MaxError") {
            matching_method->setMaxError(readDouble(stream));
        } else if (stream.name() == "Confidence") {
            matching_method->setConfidence(readDouble(stream));
        } else if (stream.name() == "CrossCheck") {
            matching_method->enableCrossCheck(readBoolean(stream));
        } else
            stream.skipCurrentElement();
    }
    this->setFeatureMatching(matching_method);
}

void ProjectImp::readFeatureMatchingReport(QXmlStreamReader& stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "Matches") {
            mFeatureMatchingReport.matches = readInt(stream);
        } else if (stream.name() == "Time") {
            mFeatureMatchingReport.time = readDouble(stream);
        } else if (stream.name() == "Cuda") {
            mFeatureMatchingReport.cuda = readBoolean(stream);
        } else
            stream.skipCurrentElement();
    }
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
        if (stream.name() == "SparseModel") {
            this->readOrientationSparseModel(stream);
        } else if (stream.name() == "EnuCrs") {
            this->readEnuCrs(stream);
        } else if (stream.name() == "GroundPoints") {
            this->readGroundPoints(stream);
        } else if (stream.name() == "Image") {
            this->readPhotoOrientations(stream);
        } else if (stream.name() == "Report") {
            this->readOrientationReport(stream);
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readOrientationSparseModel(QXmlStreamReader &stream)
{
    this->setSparseModel(stream.readElementText().toStdWString());
}

void ProjectImp::readEnuCrs(QXmlStreamReader &stream)
{
    this->setEnuCrs(stream.readElementText());
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

void ProjectImp::readOrientationReport(QXmlStreamReader& stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "OrientedImages") {
            mOrientationReport.orientedImages = readInt(stream);
        } else if (stream.name() == "OrientationType") {
            mOrientationReport.type = stream.readElementText().toStdString();
        } else if (stream.name() == "Iterations") {
            mOrientationReport.iterations = readInt(stream);
        } else if (stream.name() == "InitialCost") {
            mOrientationReport.initialCost = readDouble(stream);
        } else if (stream.name() == "FinalCost") {
            mOrientationReport.finalCost = readDouble(stream);
        } else if (stream.name() == "Termination") {
            mOrientationReport.termination = stream.readElementText().toStdString();
        } else if (stream.name() == "ErrorMean") {
            mOrientationReport.alignmentErrorMean = readDouble(stream);
        } else if (stream.name() == "ErrorMedian") {
            mOrientationReport.alignmentErrorMedian = readDouble(stream);
        } else if (stream.name() == "Time") {
            mOrientationReport.time = readDouble(stream);
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readDensification(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "DenseModel") {
            this->readDenseModel(stream);
        } else if (stream.name() == "DensificationMethod") {
            this->readDensificationMethod(stream);
        } else if (stream.name() == "Report") {
            this->readDenseReport(stream);
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readDenseModel(QXmlStreamReader &stream)
{
    this->setDenseModel(stream.readElementText().toStdWString());
}

void ProjectImp::readDenseReport(QXmlStreamReader& stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "Points") {
            mDenseReport.points = readInt(stream);
        } else if (stream.name() == "Method") {
            mDenseReport.method = stream.readElementText().toStdString();
        } else if (stream.name() == "Time") {
            mDenseReport.time = readDouble(stream);
        } else if (stream.name() == "Cuda") {
            mDenseReport.cuda = readBoolean(stream);
        } else
            stream.skipCurrentElement();
    }
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
    auto smvs = std::make_shared<Smvs>();
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
    auto cmvs_pmvs = std::make_shared<CmvsPmvs>();
    while (stream.readNextStartElement()) {
        if (stream.name() == "Level") {
            cmvs_pmvs->setLevel(readInt(stream));
        } else if (stream.name() == "CellSize") {
            cmvs_pmvs->setCellSize(readInt(stream));
        } else if (stream.name() == "Threshold") {
            cmvs_pmvs->setThreshold(readDouble(stream));
        } else if (stream.name() == "Confidence") {
            cmvs_pmvs->setWindowSize(readInt(stream));
        } else if (stream.name() == "ImagesPerCluster") {
            cmvs_pmvs->setImagesPerCluster(readInt(stream));
        } else if (stream.name() == "MinimunImageNumber") {
            cmvs_pmvs->setMinimunImageNumber(readInt(stream));
        } else if (stream.name() == "UseVisibilityInformation") {
            cmvs_pmvs->setUseVisibilityInformation(readBoolean(stream));
        } else
            stream.skipCurrentElement();
    }
    this->setDensification(cmvs_pmvs);
}

void ProjectImp::readMVS(QXmlStreamReader &stream)
{
    auto mvs = std::make_shared<Mvs>();

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
        } else if (stream.name() == "Report") {
            this->readMeshReport(stream);
        }
        else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readMeshModel(QXmlStreamReader &stream)
{
    this->setMeshPath(stream.readElementText().toStdWString());
}

void ProjectImp::readMeshReport(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "Time") {
            mMeshReport.time = readDouble(stream);
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readMeshParameters(QXmlStreamReader &stream)
{
    auto mesh = std::make_shared<PoissonReconProperties>();

    while (stream.readNextStartElement()) {
        if (stream.name() == "Depth") {
            mesh->setDepth(readInt(stream));
        } /*else if (stream.name() == "SolveDepth") {
            mesh->setSolveDepth(readInt(stream));
        } */else if (stream.name() == "BoundaryType") {
            QString bt = stream.readElementText();
            PoissonReconProperties::BoundaryType boundary_type;
            if (bt == "Free"){
                boundary_type = PoissonReconProperties::BoundaryType::free;
            } else if (bt == "Dirichlet") {
                boundary_type = PoissonReconProperties::BoundaryType::dirichlet;
            } else {
                boundary_type = PoissonReconProperties::BoundaryType::neumann;
            }
            mesh->setBoundaryType(boundary_type);
        } else
            stream.skipCurrentElement();
    }

    setMeshProperties(mesh);
}

void ProjectImp::readDem(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "DTMPath") {
            this->mDem.dtmPath = stream.readElementText().toStdWString();
        } else if (stream.name() == "DSMPath") {
            this->mDem.dsmPath = stream.readElementText().toStdWString();
        } else if (stream.name() == "GSD") {
            this->mDem.gsd = stream.readElementText().toDouble();
        } else if (stream.name() == "CRS") {
            this->mDem.epsgCode = stream.readElementText();
        } else if (stream.name() == "Report") {
            this->readDemReport(stream);
        } else
            stream.skipCurrentElement();
    }
}

void ProjectImp::readDemReport(QXmlStreamReader& stream)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "Time") {
            mDemReport.time = readDouble(stream);
        } else if (stream.name() == "GSD") {
            mDemReport.gsd = stream.readElementText().toDouble();
        } else if (stream.name() == "Cols") {
            mDemReport.cols = stream.readElementText().toDouble();
        } else if (stream.name() == "Rows") {
            mDemReport.rows = stream.readElementText().toDouble();
        } else
            stream.skipCurrentElement();
    }
}

auto ProjectImp::readOrthophoto(QXmlStreamReader &stream) -> OrthophotoData
{
    OrthophotoData orthophoto;

    while (stream.readNextStartElement()) {
        if (stream.name() == "Path") {
            orthophoto.path = stream.readElementText().toStdWString();
        } else if (stream.name() == "GSD") {
            orthophoto.gsd = stream.readElementText().toDouble();
        } else if (stream.name() == "CRS") {
            orthophoto.epsgCode = stream.readElementText();
        } else if (stream.name() == "Interpolation") {
            orthophoto.interpolation = stream.readElementText();
        } else if (stream.name() == "Report") {
            orthophoto.report = this->readOrthophotoReport(stream);
        } else
            stream.skipCurrentElement();
    }

    return orthophoto;
}

auto ProjectImp::readOrthophotoReport(QXmlStreamReader &stream) -> OrthophotoReport
{
    OrthophotoReport report;

    while (stream.readNextStartElement()) {
        if (stream.name() == "Time") {
            report.time = readDouble(stream);
        } else if (stream.name() == "GSD") {
            report.gsd = stream.readElementText().toDouble();
        } else if (stream.name() == "Cols") {
            report.cols = stream.readElementText().toInt();
        } else if (stream.name() == "Rows") {
            report.rows = stream.readElementText().toInt();
        } else if (stream.name() == "Channels") {
            report.channels = stream.readElementText().toInt();
        } else
            stream.skipCurrentElement();
    }

    return report;
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

//void ProjectImp::writeCrs(QXmlStreamWriter &stream) const
//{
//    stream.writeTextElement("ProjectCRS", this->crs());
//}

void ProjectImp::writeCameras(QXmlStreamWriter &stream) const
{
    stream.writeStartElement("Cameras");
    {
        const auto &cameras = this->cameras();
        for (const auto &camera : cameras) {
            this->writeCamera(stream, camera.first, camera.second);
        }
    }
    stream.writeEndElement();
}

void ProjectImp::writeCamera(QXmlStreamWriter &stream, int id, const Camera &camera) const
{
    stream.writeStartElement("Camera");
    {
        stream.writeAttribute("id", QString::number(id));

        stream.writeTextElement("Make", QString::fromStdString(camera.make()));
        stream.writeTextElement("Model", QString::fromStdString(camera.model()));
        stream.writeTextElement("SerialNumber", QString::fromStdString(camera.serialNumber()));
        stream.writeTextElement("Type", QString::fromStdString(camera.type()));
        stream.writeTextElement("Focal", QString::number(camera.focal()));
        stream.writeTextElement("Width", QString::number(camera.width()));
        stream.writeTextElement("Height", QString::number(camera.height()));
        stream.writeTextElement("BitsPerPixel", QString::number(camera.bitsPerPixel()));
        stream.writeTextElement("SensorSize", QString::number(camera.sensorSize()));
        if (!camera.bandName().empty()) stream.writeTextElement("BandName", QString::fromStdString(camera.bandName()));
        if (camera.hasBlackLevel()) stream.writeTextElement("BlackLevel", QString::number(camera.blackLevel()));
        if (camera.hasCalibratedHMatrix()) {
            QStringList hmatrix;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    hmatrix.append(QString::number(camera.calibratedHMatrix().at(i, j), 'f', 10));
                }
            }
            stream.writeTextElement("CalibratedHMatrix", hmatrix.join(" "));
        }
        writePriorCalibration(stream, camera.priorCalibration());
        writeCalibration(stream, camera.calibration());
        writeVignetting(stream, camera.vignettingModel());
    }
    stream.writeEndElement(); // Camera
}

void ProjectImp::writePriorCalibration(QXmlStreamWriter &stream, std::shared_ptr<Calibration> calibration) const
{
    if (calibration) {
        stream.writeStartElement("PriorCalibration");
        {
            for (auto param = calibration->begin(); param != calibration->end(); param++) {
                stream.writeTextElement(calibration->parameterName(param->first).c_str(), QString::number(param->second, 'f', 10));
            }
        }
        stream.writeEndElement(); // PriorCalibration
    }
}

void ProjectImp::writeCalibration(QXmlStreamWriter &stream, std::shared_ptr<Calibration> calibration) const
{
    if (calibration) {
        stream.writeStartElement("Calibration");
        {
            for (auto param = calibration->begin(); param != calibration->end(); param++) {
                stream.writeTextElement(calibration->parameterName(param->first).c_str(), QString::number(param->second, 'f', 10));
            }
        }
        stream.writeEndElement(); // Calibration
    }
}

void ProjectImp::writeVignetting(QXmlStreamWriter &stream, std::shared_ptr<Vignetting> vignetting) const
{
    if (vignetting) {
        stream.writeStartElement("Vignetting");

        auto model = vignetting->model();
        switch (model) {
        case Vignetting::Model::radial:
        {
            stream.writeAttribute("model", QString("radial"));

            auto vignetting_radial = std::dynamic_pointer_cast<VignettingRadial>(vignetting);
            stream.writeTextElement("VignettingCenter",
                QString::number(vignetting_radial->center().x) + ";" +
                QString::number(vignetting_radial->center().y));
            QStringList vignetting_polynomial;
            for (const auto &coef : vignetting_radial->polynomial()) {
                vignetting_polynomial.append(QString::number(coef));
            }
            stream.writeTextElement("VignettingPolynomial", vignetting_polynomial.join(";"));
            break;
        }
        case Vignetting::Model::polynomial2d:
        {
            stream.writeAttribute("model", QString("polynomial2d"));

            auto vignetting_polynomial2d = std::dynamic_pointer_cast<VignettingPolynomial2D>(vignetting);
            QStringList coeffs;
            for (const auto &coef : vignetting_polynomial2d->coeffs()) {
                coeffs.append(QString::number(coef));
            }
            stream.writeTextElement("VignettingCoeffs", coeffs.join(";"));
            QStringList _powers;
            for (const auto &power_pair : vignetting_polynomial2d->powers()) {
                _powers.append(QString::number(power_pair.first));
                _powers.append(QString::number(power_pair.second));
            }
            stream.writeTextElement("VignettingPowers", _powers.join(";"));
            break;
        }
        default:
            break;
        }

        stream.writeEndElement(); // Vignetting
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
        writeImageMetadata(stream, image.second);
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
            stream.writeTextElement("X", QString::number(cameraPosition.position().x, 'f', 8));
            stream.writeTextElement("Y", QString::number(cameraPosition.position().y, 'f', 8));
            stream.writeTextElement("Z", QString::number(cameraPosition.position().z, 'f', 3));
            stream.writeTextElement("SX", QString::number(cameraPosition.accuracy().x(), 'f', 3));
            stream.writeTextElement("SY", QString::number(cameraPosition.accuracy().y(), 'f', 3));
            stream.writeTextElement("SZ", QString::number(cameraPosition.accuracy().z(), 'f', 3));
            stream.writeTextElement("QX", QString::number(cameraPosition.quaternion().x, 'f', 10));
            stream.writeTextElement("QY", QString::number(cameraPosition.quaternion().y, 'f', 10));
            stream.writeTextElement("QZ", QString::number(cameraPosition.quaternion().z, 'f', 10));
            stream.writeTextElement("QW", QString::number(cameraPosition.quaternion().w, 'f', 10));
            stream.writeTextElement("Source", cameraPosition.source());
            stream.writeTextElement("RtkFlag", QString::number(cameraPosition.rtkFlag()));
        }
        stream.writeEndElement();
    }
}

void ProjectImp::writeImageMetadata(QXmlStreamWriter &stream, const Image &image) const
{
    for (const auto &meta : image.metadata()) {

        stream.writeStartElement("Metadata");
        stream.writeAttribute(QString::fromStdString(meta.first), QString::fromStdString(meta.second));
        stream.writeEndElement();
    }
}

void ProjectImp::writeFeatures(QXmlStreamWriter &stream) const
{
    stream.writeStartElement("Features");
    {
        this->writeFeatureExtractor(stream);
        this->writeFeatureExtractorReport(stream);
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

void ProjectImp::writeFeatureExtractorReport(QXmlStreamWriter& stream) const
{
    if (!mFeatureExtractorReport.isEmpty()) {

        stream.writeStartElement("Report");

        stream.writeTextElement("Features", QString::number(mFeatureExtractorReport.features));
        stream.writeTextElement("Time", QString::number(mFeatureExtractorReport.time, 'f', 10));
        stream.writeTextElement("Cuda", mFeatureExtractorReport.time ? "true" : "false");

        stream.writeEndElement(); // Report
    }
}

void ProjectImp::writeSIFT(QXmlStreamWriter &stream, const Sift *sift) const
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
        this->writeFeatureMatchingReport(stream);
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

void ProjectImp::writeFeatureMatchingReport(QXmlStreamWriter& stream) const
{
    if (!mFeatureMatchingReport.isEmpty()) {

        stream.writeStartElement("Report");

        stream.writeTextElement("Matches", QString::number(mFeatureMatchingReport.matches));
        stream.writeTextElement("Time", QString::number(mFeatureMatchingReport.time, 'f', 10));
        stream.writeTextElement("Cuda", mFeatureMatchingReport.time ? "true" : "false");

        stream.writeEndElement(); // Report
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
        //this->writeReconstructionPath(stream);
        this->writeOrientationSparseModel(stream);
        this->writeOffset(stream);
        this->writeGroundPoints(stream);
        this->writePhotoOrientations(stream);
        this->writeOrientationReport(stream);
    }
    stream.writeEndElement(); // Orientations
}

//void ProjectImp::writeReconstructionPath(QXmlStreamWriter &stream) const
//{
//    tl::Path reconstruction_path = reconstructionPath();
//    if (!reconstruction_path.empty())
//        stream.writeTextElement("ReconstructionPath", QString::fromStdWString(reconstruction_path.toWString()));
//}

void ProjectImp::writeOrientationSparseModel(QXmlStreamWriter &stream) const
{
    QString sparse_model = QString::fromStdWString(sparseModel().toWString());
    if (!sparse_model.isEmpty())
        stream.writeTextElement("SparseModel", sparse_model);
}

void ProjectImp::writeOffset(QXmlStreamWriter &stream) const
{
    QString enu_crs = this->enuCrs();
    if (!enu_crs.isEmpty())
        stream.writeTextElement("EnuCrs", enu_crs);
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

void ProjectImp::writeOrientationReport(QXmlStreamWriter &stream) const
{
    if (!mOrientationReport.isEmpty()) {

        stream.writeStartElement("Report");

        stream.writeTextElement("OrientedImages", QString::number(mOrientationReport.orientedImages));
        stream.writeTextElement("OrientationType", QString::fromStdString(mOrientationReport.type));
        stream.writeTextElement("Iterations", QString::number(mOrientationReport.iterations));
        stream.writeTextElement("InitialCost", QString::number(mOrientationReport.initialCost, 'f', 10));
        stream.writeTextElement("FinalCost", QString::number(mOrientationReport.finalCost, 'f', 10));
        stream.writeTextElement("Termination", QString::fromStdString(mOrientationReport.termination));
        stream.writeTextElement("ErrorMean", QString::number(mOrientationReport.alignmentErrorMean, 'f', 10));
        stream.writeTextElement("ErrorMedian", QString::number(mOrientationReport.alignmentErrorMedian, 'f', 10));
        stream.writeTextElement("Time", QString::number(mOrientationReport.time, 'f', 10));

        stream.writeEndElement(); // Report
    }
}

void ProjectImp::writeDensification(QXmlStreamWriter &stream) const
{
    stream.writeStartElement("Densification");
    {
        this->writeDenseModel(stream);
        this->writeDensificationMethod(stream);
        this->writeDenseReport(stream);
    }
    stream.writeEndElement(); // Densification
}

void ProjectImp::writeDenseModel(QXmlStreamWriter &stream) const
{
    tl::Path dense_model = denseModel();
    if (!dense_model.empty())
        stream.writeTextElement("DenseModel", QString::fromStdWString(dense_model.toWString()));
}

void ProjectImp::writeDenseReport(QXmlStreamWriter &stream) const
{
    if (!mDenseReport.isEmpty()) {

        stream.writeStartElement("Report");

        stream.writeTextElement("Points", QString::number(mDenseReport.points));
        stream.writeTextElement("Method", QString::fromStdString(mDenseReport.method));
        stream.writeTextElement("Time", QString::number(mDenseReport.time, 'f', 10));
        stream.writeTextElement("Cuda", mDenseReport.time > 0. ? "true" : "false");

        stream.writeEndElement(); // Report
    }
}

void ProjectImp::writeDensificationMethod(QXmlStreamWriter &stream) const
{
    if (auto densification_method = this->densification()) {

        stream.writeStartElement("DensificationMethod");

        if (densification_method->method() == Densification::Method::smvs) {

            stream.writeStartElement("Smvs");

            auto smvs = std::dynamic_pointer_cast<Smvs>(densification_method);
            stream.writeTextElement("InputImageScale", QString::number(smvs->inputImageScale()));
            stream.writeTextElement("OutputDepthScale", QString::number(smvs->outputDepthScale()));
            stream.writeTextElement("SemiGlobalMatching", smvs->semiGlobalMatching() ? "true" : "false");
            stream.writeTextElement("SurfaceSmoothingFactor", QString::number(smvs->surfaceSmoothingFactor()));
            stream.writeTextElement("ShadingBasedOptimization", smvs->shadingBasedOptimization() ? "true" : "false");

            stream.writeEndElement();

        } else if (densification_method->method() == Densification::Method::cmvs_pmvs) {

            stream.writeStartElement("CmvsPmvs");

            auto cmvs_pmvs = std::dynamic_pointer_cast<CmvsPmvs>(densification_method);
            stream.writeTextElement("Level", QString::number(cmvs_pmvs->level()));
            stream.writeTextElement("CellSize", QString::number(cmvs_pmvs->cellSize()));
            stream.writeTextElement("Threshold", QString::number(cmvs_pmvs->threshold()));
            stream.writeTextElement("WindowSize", QString::number(cmvs_pmvs->windowSize()));
            stream.writeTextElement("ImagesPerCluster", QString::number(cmvs_pmvs->imagesPerCluster()));
            stream.writeTextElement("MinimunImageNumber", QString::number(cmvs_pmvs->minimunImageNumber()));
            stream.writeTextElement("UseVisibilityInformation", cmvs_pmvs->useVisibilityInformation() ? "true" : "false");

            stream.writeEndElement();
        } else if (densification_method->method() == Densification::Method::mvs) {

            stream.writeStartElement("MVS");

            auto mvs = std::dynamic_pointer_cast<Mvs>(densification_method);
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
        this->writeMeshReport(stream);
    }
    stream.writeEndElement(); // Densification
}

void ProjectImp::writeMeshModel(QXmlStreamWriter &stream) const
{
    QString mesh_model = QString::fromStdWString(meshPath().toWString());
    if (!mesh_model.isEmpty())
        stream.writeTextElement("MeshModel", mesh_model);
}

void ProjectImp::writeMeshReport(QXmlStreamWriter& stream) const
{
    if (!mMeshReport.isEmpty()) {

        stream.writeStartElement("Report");

        stream.writeTextElement("Time", QString::number(mMeshReport.time, 'f', 10));

        stream.writeEndElement(); // Report
    }
}

void ProjectImp::writeMeshParameters(QXmlStreamWriter &stream) const
{
    if (auto mesh = std::dynamic_pointer_cast<PoissonReconProperties>(meshProperties())) {

        stream.writeStartElement("PoissonParameters");

        stream.writeTextElement("Depth", QString::number(mesh->depth()));
        //stream.writeTextElement("SolveDepth", QString::number(mesh->solveDepth()));
        stream.writeTextElement("BoundaryType", mesh->boundaryTypeAsText());

        stream.writeEndElement();

    }
}

void ProjectImp::writeDem(QXmlStreamWriter &stream) const
{
    if (mDem.dtmPath.empty() && mDem.dsmPath.empty()) return;

    stream.writeStartElement("Dem");
    {
        stream.writeTextElement("DTMPath", QString::fromStdWString(mDem.dtmPath.toWString()));
        stream.writeTextElement("DSMPath", QString::fromStdWString(mDem.dsmPath.toWString()));
        stream.writeTextElement("CRS", mDem.epsgCode);
        stream.writeTextElement("GSD", QString::number(mDem.gsd));

        this->writeDemReport(stream);
    }
    stream.writeEndElement();
}

void ProjectImp::writeDemReport(QXmlStreamWriter &stream) const
{
    if (!mDemReport.isEmpty()) {

        stream.writeStartElement("Report");

        stream.writeTextElement("Time", QString::number(mDemReport.time, 'f', 10));
        stream.writeTextElement("GSD", QString::number(mDemReport.gsd, 'f', 10));
        stream.writeTextElement("Cols", QString::number(mDemReport.cols));
        stream.writeTextElement("Rows", QString::number(mDemReport.rows));

        stream.writeEndElement(); // Report
    }
}

void ProjectImp::writeOrthophoto(QXmlStreamWriter &stream) const
{
    for (const auto &ortho : mOrthophotos) {

        stream.writeStartElement("Orthophoto");
        {
            stream.writeTextElement("CRS", ortho.second.epsgCode);
            stream.writeTextElement("Path", QString::fromStdWString(ortho.second.path.toWString()));
            stream.writeTextElement("GSD", QString::number(ortho.second.gsd));
            stream.writeTextElement("Interpolation", ortho.second.interpolation);

            auto &report = ortho.second.report;
            if (!report.isEmpty()) {

                stream.writeStartElement("Report");

                stream.writeTextElement("Time", QString::number(report.time, 'f', 10));
                stream.writeTextElement("GSD", QString::number(report.gsd, 'f', 10));
                stream.writeTextElement("Cols", QString::number(report.cols));
                stream.writeTextElement("Rows", QString::number(report.rows));
                stream.writeTextElement("Channels", QString::number(report.channels));

                stream.writeEndElement(); // Report
            }
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

} // end namespace graphos
