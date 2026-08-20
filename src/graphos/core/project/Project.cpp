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

#include "graphos/core/project/Project.h"

#include "graphos/core/orientation/io/CameraPosesReader.h"

//#include "graphos/core/features/SiftProperties.h"
//#include "graphos/core/features/matching.h"
//#include "graphos/core/dense/Smvs.h"
//#include "graphos/core/dense/CmvsPmvs.h"
//#include "graphos/core/dense/mvs.h"
//#include "graphos/core/camera/Colmap.h"
//#include "graphos/core/mesh/PoissonRecon.h"
//#include "graphos/core/multispectral/Vignetting.h"
//#include "graphos/core/orientation/GroundPoint.h"
//
//#include <tidop/core/app/Message.h>
//#include <tidop/core/base/Exception.h>
//#include <tidop/core/base/Split.h>

//#include <colmap/scene/database.h>
//
//#include <QFile>
//#include <QFileInfo>
//#include <QXmlStreamWriter>
//
//#include <fstream>


#define GRAPHOS_PROJECT_FILE_VERSION "1.0"

namespace graphos
{

//std::mutex Project::sMutex;

Project::Project()
    : mMatchingRepository(mProjectInfo.database())
  //: mCrs(""),
    //mCameraCount(0),
    //mTransform(tl::Matrix<double, 4, 4>::identity())
{
    //mImages.reserve(1000);
}

auto Project::info() -> ProjectInfo &
{
    return mProjectInfo;
}

auto Project::info() const -> const ProjectInfo &
{
    return mProjectInfo;
}

auto Project::cameras() -> CameraRepository &
{
    return mCameraRepository;
}

auto Project::cameras() const -> const CameraRepository &
{
    return mCameraRepository;
}

auto Project::images() -> ImageRepository &
{
    return mImageRepository;
}

auto Project::images() const -> const ImageRepository &
{
    return mImageRepository;
}

auto Project::featureConfig() const -> std::shared_ptr<FeatureExtractorProperties> 
{
    return mFeaturesConfig;
}

void Project::setFeatureConfig(std::shared_ptr<FeatureExtractorProperties> config) { 
    mFeaturesConfig = std::move(config); 
}

auto Project::featureReport() const -> const FeatureExtractorReport & 
{ 
    return mFeaturesReport; 
}

void Project::setFeatureReport(FeatureExtractorReport report)
{
    mFeaturesReport = std::move(report);
}

auto Project::features() -> FeaturesRepository & 
{ 
    return mFeaturesRepository;
}

auto Project::features() const -> const FeaturesRepository & 
{
    return mFeaturesRepository;
}

void Project::clearFeatures()
{
    mFeaturesConfig.reset();
    mFeaturesReport.clear();
    mFeaturesRepository.clear();
    clearMatches();
}

auto Project::matchingConfig() const -> std::shared_ptr<MatchingProperties>
{
    return mMatchingConfig;
}

void Project::setMatchingConfig(std::shared_ptr<MatchingProperties> config)
{
    mMatchingConfig = std::move(config);
}

auto Project::matchingReport() const -> MatchingReport
{
    return mMatchingReport;
}

void Project::setMatchingReport(MatchingReport report)
{
    mMatchingReport = std::move(report);
}

auto Project::matches() -> MatchingRepository &
{
    return mMatchingRepository;
}

auto Project::matches() const -> const MatchingRepository &
{
    return mMatchingRepository;
}

void Project::clearMatches()
{
    mMatchingConfig.reset();
    mMatchingReport.clear();
    mMatchingRepository.clear();
    clearOrientation();
}

auto Project::orientationConfig() const -> std::shared_ptr<OrientationProperties>
{
    return mOrientationConfig;
}

void Project::setOrientationConfig(std::shared_ptr<OrientationProperties> config)
{
    mOrientationConfig = std::move(config);
}

auto Project::orientationReport() const -> OrientationReport
{
    return mOrientationReport;
}

void Project::setOrientationReport(OrientationReport report)
{
    mOrientationReport = std::move(report);
}


auto Project::sparseModel() const -> tl::Path
{
    return mSparseModel;
}

void Project::setSparseModel(tl::Path sparseModel)
{
    mSparseModel = std::move(sparseModel);
}

auto Project::groundPoints() const -> tl::Path
{
    return mGroundPoints;
}

void Project::setGroundPoints(tl::Path groundPoints)
{
    mGroundPoints = std::move(groundPoints);
}

auto Project::cameraPosesFile() const -> tl::Path
{
    return mPosesFile;
}

void Project::setCameraPosesFile(tl::Path poses)
{
    mPosesFile = std::move(poses);
}

auto Project::cameraPoses() -> CameraPosesRepository &
{
    if (!mCameraPosesRepository) {
        if (auto reader = CameraPosesReaderFactory::create("GRAPHOS")) {
            reader->read(mPosesFile);
            auto poses = reader->cameraPoses();
            mCameraPosesRepository = std::make_unique<CameraPosesRepository>();
            mCameraPosesRepository->populate(poses);
        }
    }

    return *mCameraPosesRepository;
}

void Project::clearOrientation()
{
    mSparseModel.clear();
    mGroundPoints.clear();
    mPosesFile.clear();
    mOrientationReport.clear();
    mOrientationConfig.reset();
    clearDensification();
}



auto Project::densificationConfig() const -> std::shared_ptr<DensificationProperties>
{
    return mDensificationConfig;
}

void Project::setDensificationConfig(std::shared_ptr<DensificationProperties> config)
{
    mDensificationConfig = std::move(config);
}

auto Project::denseModel() const -> tl::Path
{
    return mDenseModel;
}

void Project::setDenseModel(tl::Path denseModel)
{
    mDenseModel = std::move(denseModel);
}

auto Project::densificationReport() const -> DensificationReport
{
    return mDenseReport;
}

void Project::setDensificationReport(DensificationReport densificationReport)
{
    mDenseReport = std::move(densificationReport);
}

void Project::clearDensification()
{
    mDensificationConfig.reset();
    mDenseModel.clear();
    mDenseReport.clear();
    clearMesh();
    clearDem();
}

auto Project::meshConfig() const -> std::shared_ptr<PoissonReconProperties>
{
    return mMeshProperties;
}

void Project::setMeshConfig(std::shared_ptr<PoissonReconProperties> config)
{
    mMeshProperties = std::move(config);
}

auto Project::meshModel() const -> tl::Path
{
    return mMeshModel;
}

void Project::setMeshModel(tl::Path meshModel)
{
    mMeshModel = std::move(meshModel);
}

auto Project::meshReport() const -> MeshReport
{
    return mMeshReport;
}

void Project::setMeshReport(MeshReport meshReport)
{
    mMeshReport = std::move(meshReport);
}


void Project::clearMesh()
{
    mMeshProperties.reset();
    mMeshModel.clear();
    mMeshReport.clear();
}

auto Project::demConfig() const -> std::shared_ptr<DemProperties>
{
    return mDemProperties;
}

void Project::setDemConfig(std::shared_ptr<DemProperties> config)
{
    mDemProperties = std::move(config);
}

auto Project::dsm() const -> tl::Path
{
    return mDsm;
}

void Project::setDsm(tl::Path dsm)
{
    mDsm = std::move(dsm);
}

auto Project::dtm() const -> tl::Path
{
    return mDtm;
}

void Project::setDtm(tl::Path dtm)
{
    mDtm = std::move(dtm);
}

auto Project::demReport() const -> DemReport
{
    return mDemReport;
}

void Project::setDemReport(DemReport demReport)
{
    mDemReport = std::move(demReport);
}

void Project::clearDem()
{
    mDemProperties.reset();
    mDsm.clear();
    mDtm.clear();
    mDemReport.clear();
    clearOrthophoto();
}

auto Project::orthoConfig() const -> std::shared_ptr<OrthophotoProperties>
{
    return mOrthoProperties;
}

void Project::setOrthoConfig(std::shared_ptr<OrthophotoProperties> config)
{
    mOrthoProperties = std::move(config);
}

auto Project::orthoReport() const -> OrthophotoReport
{
    return mOrthoReport;
}

void Project::setOrthoReport(OrthophotoReport orthoReport)
{
    mOrthoReport = std::move(orthoReport);
}

auto Project::orthophoto() const -> tl::Path
{
    return mOrthoMosaic;
}

void Project::setOrthophoto(tl::Path orthophoto)
{
    mOrthoMosaic = std::move(orthophoto);
}

void Project::clearOrthophoto()
{
    mOrthoProperties.reset();
    mOrthoReport.clear();
    mOrthoMosaic.clear();
}

void Project::clear()
{
    mProjectInfo.clear();
    mCrs = "";
    mImageRepository.clear();
    mCameraRepository.clear();
    mFeaturesConfig.reset();
    mFeaturesReport.clear();
    mFeaturesRepository.clear();
    mMatchingConfig.reset();
    mMatchingReport.clear();
    mMatchingRepository.clear();
    mSparseModel.clear();
    mGroundPoints.clear();
    mPosesFile.clear();
    mOrientationReport.clear();
    mOrientationConfig.reset();
    mDensificationConfig.reset();
    mDenseModel.clear();
    mDenseReport.clear();
    mMeshProperties.reset();
    mMeshModel.clear();
    mMeshReport.clear();
    mDemProperties.reset();
    mDsm.clear();
    mDtm.clear();
    mDemReport.clear();
    mOrthoProperties.reset();
    mOrthoReport.clear();
    mOrthoMosaic.clear();
}

//void Project::exportCameras(const tl::Path &file)
//{
//    std::lock_guard<std::mutex> lck(Project::sMutex);
//
//    try {
//
//        QFile output(QString::fromStdWString(file.toWString()));
//        if (output.open(QFile::WriteOnly)) {
//            QXmlStreamWriter stream(&output);
//            stream.setAutoFormatting(true);
//            stream.writeStartDocument();
//
//            stream.writeStartElement("GraphosCameras");
//            {
//                stream.writeStartElement("Cameras");
//                {
//                    const auto &cameras = this->cameras();
//                    for (const auto &camera_map : cameras) {
//
//                        auto id = camera_map.first;
//                        const auto &camera = camera_map.second;
//
//                        stream.writeStartElement("Camera");
//                        {
//                            stream.writeAttribute("id", QString::number(id));
//
//                            stream.writeTextElement("Make", QString::fromStdString(camera.make()));
//                            stream.writeTextElement("Model", QString::fromStdString(camera.model()));
//                            stream.writeTextElement("SerialNumber", QString::fromStdString(camera.serialNumber()));
//                            stream.writeTextElement("Type", QString::fromStdString(camera.type()));
//                            stream.writeTextElement("Focal", QString::number(camera.focal()));
//                            stream.writeTextElement("Width", QString::number(camera.width()));
//                            stream.writeTextElement("Height", QString::number(camera.height()));
//                            stream.writeTextElement("BitsPerPixel", QString::number(camera.bitsPerPixel()));
//                            stream.writeTextElement("SensorSize", QString::number(camera.sensorSize()));
//                            if (!camera.bandName().empty()) stream.writeTextElement("BandName", QString::fromStdString(camera.bandName()));
//                            if (camera.hasBlackLevel()) stream.writeTextElement("BlackLevel", QString::number(camera.blackLevel()));
//                            if (camera.hasCalibratedHMatrix()) {
//                                QStringList hmatrix;
//                                for (int i = 0; i < 3; i++) {
//                                    for (int j = 0; j < 3; j++) {
//                                        hmatrix.append(QString::number(camera.calibratedHMatrix().at(i, j), 'f', 10));
//                                    }
//                                }
//                                stream.writeTextElement("CalibratedHMatrix", hmatrix.join(" "));
//                            }
//                            writePriorCalibration(stream, camera.priorCalibration());
//                            writeCalibration(stream, camera.calibration());
//                            Undistort undistort(camera);
//                            writeCalibrationUndistorted(stream, undistort.undistortCamera().calibration());
//                            writeVignetting(stream, camera.vignettingModel());
//                        }
//                        stream.writeEndElement(); // Camera
//                    }
//                }
//                stream.writeEndElement();
//                writeImages(stream);
//
//                stream.writeStartElement("Orientations");
//                {
//                    this->writeOffset(stream);
//                    this->writePhotoOrientations(stream);
//                    this->writeOrientationReport(stream);
//                }
//                stream.writeEndElement(); // Orientations
//
//                tl::Path gcp_file = mProjectInfo.projectFolder();
//                gcp_file.append("sfm");
//                gcp_file.append("georef.xml");
//
//                if (gcp_file.exists()) {
//
//                    auto reader = GCPsReaderFactory::create("GRAPHOS");
//                    //reader->setImages(images());
//                    reader->read(gcp_file);
//                    auto crs = reader->epsgCode();
//                    auto gcps = reader->gcps();
//
//                    stream.writeStartElement("GroundControlPoints");
//                    {
//                        stream.writeTextElement("Crs", QString::fromStdString(crs));
//
//                        for (const auto &gcp : gcps) {
//
//
//                            stream.writeStartElement("GroundControlPoint");
//                            stream.writeTextElement("Name", QString::fromStdString(gcp.name()));
//                            stream.writeTextElement("x", QString::number(gcp.x(), 'f', 6));
//                            stream.writeTextElement("y", QString::number(gcp.y(), 'f', 6));
//                            stream.writeTextElement("z", QString::number(gcp.z(), 'f', 6));
//                            stream.writeTextElement("error", QString::number(gcp.error(), 'f', 3));
//                            stream.writeStartElement("ImagePoints");
//
//                            for (const auto &point : gcp.track().points()) {
//
//                                stream.writeStartElement("ImagePoint");
//                                stream.writeAttribute("image_id", QString::number(point.first));
//                                stream.writeTextElement("x", QString::number(point.second.x()));
//                                stream.writeTextElement("y", QString::number(point.second.y()));
//                                stream.writeTextElement("ex", QString::number(gcp.track().error(point.first).x()));
//                                stream.writeTextElement("ey", QString::number(gcp.track().error(point.first).y()));
//                                stream.writeEndElement();
//
//                            }
//
//                            stream.writeEndElement();
//                            stream.writeEndElement();
//                        }
//                    }
//                    stream.writeEndElement();
//
//                }
//            }
//
//            stream.writeEndElement(); // GraphosCameras
//
//            output.close();
//
//        }
//
//    } catch (...) {
//        TL_THROW_EXCEPTION_WITH_NESTED("An exception has been detected when exporting the cameras.");
//    }
//}
//
//bool Project::checkOldVersion(const tl::Path &file) const
//{
//    std::lock_guard<std::mutex> lck(Project::sMutex);
//    bool bUpdateVersion = false;
//
//    QFile input(QString::fromStdWString(file.toWString()));
//
//    if (input.open(QIODevice::ReadOnly)) {
//        QXmlStreamReader stream;
//        stream.setDevice(&input);
//
//        if (stream.readNextStartElement()) {
//            if (stream.name() == "Graphos") {
//                QString version = "0";
//                for (auto &attr : stream.attributes()) {
//                    if (attr.name().compare(QString("version")) == 0) {
//                        version = attr.value().toString();
//                        break;
//                    }
//                }
//                if (version.compare(GRAPHOS_PROJECT_FILE_VERSION) < 0) {
//                    // Es una versión mas antigua
//                    bUpdateVersion = true;
//                }
//            } else
//                stream.raiseError(QObject::tr("Incorrect file"));
//        }
//    }
//
//    return bUpdateVersion;
//}
//
//void Project::oldVersionBak(const tl::Path &file) const
//{
//    // Versión antigua
//    QString version = "0";
//    QFile input(QString::fromStdWString(file.toWString()));
//
//    if (input.open(QIODevice::ReadOnly)) {
//        QXmlStreamReader stream;
//        stream.setDevice(&input);
//
//        if (stream.readNextStartElement()) {
//            if (stream.name() == "Graphos") {
//                for (auto &attr : stream.attributes()) {
//                    if (attr.name().compare(QString("version")) == 0) {
//                        version = attr.value().toString();
//                        break;
//                    }
//                }
//            }
//        }
//    }
//
//    tl::Path tmp_file = file;
//    std::string bak_file = file.baseName().toString();
//    bak_file.append("_v").append(version.toStdString()).append(".bak");
//    tmp_file.replaceFileName(bak_file);
//    std::ifstream  src(file.toString(), std::ios::binary);
//    std::ofstream  dst(tmp_file.toString(), std::ios::binary);
//    dst << src.rdbuf();
//    src.close();
//    dst.close();
//}

//void Project::readCamera(QXmlStreamReader &stream)
//{
//    Camera camera;
//
//    int id = 0;
//    for (auto &attr : stream.attributes()) {
//        if (attr.name().compare(QString("id")) == 0) {
//            id = attr.value().toInt();
//            break;
//        }
//    }
//
//    while (stream.readNextStartElement()) {
//        if (stream.name() == "Make") {
//            camera.setMake(stream.readElementText().toStdString());
//        } else if (stream.name() == "Model") {
//            camera.setModel(stream.readElementText().toStdString());
//        } else if (stream.name() == "SerialNumber") {
//            camera.setSerialNumber(stream.readElementText().toStdString());
//        } else if (stream.name() == "Type") {
//            camera.setType(stream.readElementText().toStdString());
//        } else if (stream.name() == "Focal") {
//            camera.setFocal(readDouble(stream));
//        } else if (stream.name() == "Width") {
//            camera.setWidth(readInt(stream));
//        } else if (stream.name() == "Height") {
//            camera.setHeight(readInt(stream));
//        } else if (stream.name() == "BitsPerPixel") {
//            camera.setBitsPerPixel(readInt(stream));
//        } else if (stream.name() == "SensorSize") {
//            camera.setSensorSize(readDouble(stream));
//        } else if (stream.name() == "BandName") {
//            camera.setBandName(stream.readElementText().toStdString());
//        } else if (stream.name() == "BlackLevel") {
//            camera.setBlackLevel(readInt(stream));
//        } /*else if (stream.name() == "VignettingCenter") {
//            auto vignetting_center = tl::split<float>(stream.readElementText().toStdString(), ';');
//            if (vignetting_center.size() == 2)
//                camera.setVignettingCenter(tl::Point2f(vignetting_center[0], vignetting_center[1]));
//        } else if (stream.name() == "VignettingPolynomial") {
//            auto vignetting_polynomial = tl::split<float>(stream.readElementText().toStdString(), ';');
//            if (vignetting_polynomial.size() == 6)
//                camera.setVignettingPolynomial(vignetting_polynomial);
//        }*/ else if (stream.name() == "CalibratedHMatrix") {
//            auto hmatrix = tl::split<float>(stream.readElementText().toStdString(), ' ');
//            if (hmatrix.size() == 9) {
//                tl::Matrix3x3f H;
//                H(0, 0) = hmatrix[0];
//                H(0, 1) = hmatrix[1];
//                H(0, 2) = hmatrix[2];
//                H(1, 0) = hmatrix[3];
//                H(1, 1) = hmatrix[4];
//                H(1, 2) = hmatrix[5];
//                H(2, 0) = hmatrix[6];
//                H(2, 1) = hmatrix[7];
//                H(2, 2) = hmatrix[8];
//                camera.setCalibratedHMatrix(H);
//            }
//
//        } else if (stream.name() == "PriorCalibration") {
//            this->readPriorCalibration(stream, camera);
//        } else if (stream.name() == "Calibration") {
//            this->readCalibration(stream, camera);
//        } else if (stream.name() == "Vignetting") {
//            this->readVignetting(stream, camera);
//        } else
//            stream.skipCurrentElement();
//    }
//
//    mCameras.try_emplace(id, camera);
//    mCameraCount = std::max(mCameraCount, id);
//}
//
//void Project::readPriorCalibration(QXmlStreamReader &stream, Camera &camera)
//{
//    try {
//
//        std::shared_ptr<Calibration> calibration;
//        calibration = CalibrationFactory::create(camera.type());
//        while (stream.readNextStartElement()) {
//            if (stream.name().compare(QString("f")) == 0) {
//                calibration->setParameter(Calibration::Parameters::focal, readDouble(stream));
//            } else if (stream.name() == "fx") {
//                calibration->setParameter(Calibration::Parameters::focalx, readDouble(stream));
//            } else if (stream.name() == "fy") {
//                calibration->setParameter(Calibration::Parameters::focaly, readDouble(stream));
//            } else if (stream.name() == "cx") {
//                calibration->setParameter(Calibration::Parameters::cx, readDouble(stream));
//            } else if (stream.name() == "cy") {
//                calibration->setParameter(Calibration::Parameters::cy, readDouble(stream));
//            } else if (stream.name() == "k1") {
//                calibration->setParameter(Calibration::Parameters::k1, readDouble(stream));
//            } else if (stream.name() == "k2") {
//                calibration->setParameter(Calibration::Parameters::k2, readDouble(stream));
//            } else if (stream.name() == "k3") {
//                calibration->setParameter(Calibration::Parameters::k3, readDouble(stream));
//            } else if (stream.name() == "k4") {
//                calibration->setParameter(Calibration::Parameters::k4, readDouble(stream));
//            } else if (stream.name() == "k5") {
//                calibration->setParameter(Calibration::Parameters::k5, readDouble(stream));
//            } else if (stream.name() == "k6") {
//                calibration->setParameter(Calibration::Parameters::k6, readDouble(stream));
//            } else if (stream.name() == "p1") {
//                calibration->setParameter(Calibration::Parameters::p1, readDouble(stream));
//            } else if (stream.name() == "p2") {
//                calibration->setParameter(Calibration::Parameters::p2, readDouble(stream));
//            } else
//                stream.skipCurrentElement();
//        }
//
//        camera.setPriorCalibration(calibration);
//
//    } catch (std::exception &e) {
//        tl::printException(e);
//    }
//}
//
//void Project::readCalibration(QXmlStreamReader &stream, Camera &camera)
//{
//    try {
//
//        std::shared_ptr<Calibration> calibration;
//        calibration = CalibrationFactory::create(camera.type());
//        while (stream.readNextStartElement()) {
//            if (stream.name().compare(QString("f")) == 0) {
//                calibration->setParameter(Calibration::Parameters::focal, readDouble(stream));
//            } else if (stream.name() == "fx") {
//                calibration->setParameter(Calibration::Parameters::focalx, readDouble(stream));
//            } else if (stream.name() == "fy") {
//                calibration->setParameter(Calibration::Parameters::focaly, readDouble(stream));
//            } else if (stream.name() == "cx") {
//                calibration->setParameter(Calibration::Parameters::cx, readDouble(stream));
//            } else if (stream.name() == "cy") {
//                calibration->setParameter(Calibration::Parameters::cy, readDouble(stream));
//            } else if (stream.name() == "k1") {
//                calibration->setParameter(Calibration::Parameters::k1, readDouble(stream));
//            } else if (stream.name() == "k2") {
//                calibration->setParameter(Calibration::Parameters::k2, readDouble(stream));
//            } else if (stream.name() == "k3") {
//                calibration->setParameter(Calibration::Parameters::k3, readDouble(stream));
//            } else if (stream.name() == "k4") {
//                calibration->setParameter(Calibration::Parameters::k4, readDouble(stream));
//            } else if (stream.name() == "k5") {
//                calibration->setParameter(Calibration::Parameters::k5, readDouble(stream));
//            } else if (stream.name() == "k6") {
//                calibration->setParameter(Calibration::Parameters::k6, readDouble(stream));
//            } else if (stream.name() == "p1") {
//                calibration->setParameter(Calibration::Parameters::p1, readDouble(stream));
//            } else if (stream.name() == "p2") {
//                calibration->setParameter(Calibration::Parameters::p2, readDouble(stream));
//            } else
//                stream.skipCurrentElement();
//        }
//
//        camera.setCalibration(calibration);
//
//    } catch (std::exception &e) {
//        tl::printException(e);
//    }
//}
//
//void Project::readVignetting(QXmlStreamReader &stream, Camera &camera)
//{
//    try {
//
//        QString vignetting_model = 0;
//        for (auto &attr : stream.attributes()) {
//            if (attr.name().compare(QString("model")) == 0) {
//                vignetting_model = attr.value().toString();
//                break;
//            }
//        }
//
//        std::shared_ptr<Vignetting> model;
//
//        if (vignetting_model == "radial") {
//
//            tl::Point2f vignetting_center_point;
//            std::vector<float> vignetting_polynomial;
//
//            while (stream.readNextStartElement()) {
//
//                if (stream.name() == "VignettingCenter") {
//                    auto vignetting_center = tl::split<float>(stream.readElementText().toStdString(), ';');
//                    if (vignetting_center.size() == 2)
//                        vignetting_center_point = tl::Point2f(vignetting_center[0], vignetting_center[1]);
//                } else if (stream.name() == "VignettingPolynomial") {
//                    auto polynomial = tl::split<float>(stream.readElementText().toStdString(), ';');
//                    if (polynomial.size() == 6)
//                        vignetting_polynomial = polynomial;
//                } else
//                    stream.skipCurrentElement();
//            }
//
//            if (!vignetting_polynomial.empty())
//                model = std::make_shared<VignettingRadial>(vignetting_center_point, vignetting_polynomial);
//
//        } else if (vignetting_model == "polynomial2d") {
//
//            std::vector<float> coeffs;
//            std::vector<std::pair<int, int>> power_pairs;
//
//            while (stream.readNextStartElement()) {
//
//                if (stream.name() == "VignettingCoeffs") {
//                    coeffs = tl::split<float>(stream.readElementText().toStdString(), ';');
//                } else if (stream.name() == "VignettingPowers") {
//                    auto powers = tl::split<float>(stream.readElementText().toStdString(), ';');
//                    power_pairs.reserve(powers.size() / 2);
//                    for (size_t i = 0; i < powers.size(); i += 2) {
//                        power_pairs.push_back(std::make_pair(static_cast<int>(powers[i]), static_cast<int>(powers[i + 1])));
//                    }
//                } else
//                    stream.skipCurrentElement();
//            }
//
//            model = std::make_shared<VignettingPolynomial2D>(coeffs, power_pairs);
//
//        } else {
//            stream.skipCurrentElement();
//            return; 
//        }
//
//        camera.setVignettingModel(model);
//
//    } catch (std::exception &e) {
//        tl::printException(e);
//    }
//}

//
//auto Project::readOrthophoto(QXmlStreamReader &stream) -> OrthophotoData
//{
//    OrthophotoData orthophoto;
//
//    while (stream.readNextStartElement()) {
//        if (stream.name() == "Path") {
//            orthophoto.path = stream.readElementText().toStdWString();
//        } else if (stream.name() == "GSD") {
//            orthophoto.gsd = stream.readElementText().toDouble();
//        } else if (stream.name() == "CRS") {
//            orthophoto.epsgCode = stream.readElementText();
//        } else if (stream.name() == "Interpolation") {
//            orthophoto.interpolation = stream.readElementText();
//        } else if (stream.name() == "Report") {
//            orthophoto.report = this->readOrthophotoReport(stream);
//        } else
//            stream.skipCurrentElement();
//    }
//
//    return orthophoto;
//}
//
//auto Project::readOrthophotoReport(QXmlStreamReader &stream) -> OrthophotoReport
//{
//    OrthophotoReport report;
//
//    while (stream.readNextStartElement()) {
//        if (stream.name() == "Time") {
//            report.time = readDouble(stream);
//        } else if (stream.name() == "GSD") {
//            report.gsd = stream.readElementText().toDouble();
//        } else if (stream.name() == "Cols") {
//            report.cols = stream.readElementText().toInt();
//        } else if (stream.name() == "Rows") {
//            report.rows = stream.readElementText().toInt();
//        } else if (stream.name() == "Channels") {
//            report.channels = stream.readElementText().toInt();
//        } else
//            stream.skipCurrentElement();
//    }
//
//    return report;
//}

//void Project::writeDem(QXmlStreamWriter &stream) const
//{
//    if (mDem.dtmPath.empty() && mDem.dsmPath.empty()) return;
//
//    stream.writeStartElement("Dem");
//    {
//        stream.writeTextElement("DTMPath", QString::fromStdWString(mDem.dtmPath.toWString()));
//        stream.writeTextElement("DSMPath", QString::fromStdWString(mDem.dsmPath.toWString()));
//        stream.writeTextElement("CRS", mDem.epsgCode);
//        stream.writeTextElement("GSD", QString::number(mDem.gsd));
//
//        this->writeDemReport(stream);
//    }
//    stream.writeEndElement();
//}
//
//void Project::writeDemReport(QXmlStreamWriter &stream) const
//{
//    if (!mDemReport.isEmpty()) {
//
//        stream.writeStartElement("Report");
//
//        stream.writeTextElement("Time", QString::number(mDemReport.time, 'f', 10));
//        stream.writeTextElement("GSD", QString::number(mDemReport.gsd, 'f', 10));
//        stream.writeTextElement("Cols", QString::number(mDemReport.cols));
//        stream.writeTextElement("Rows", QString::number(mDemReport.rows));
//
//        stream.writeEndElement(); // Report
//    }
//}
//
//void Project::writeOrthophoto(QXmlStreamWriter &stream) const
//{
//    for (const auto &ortho : mOrthophotos) {
//
//        stream.writeStartElement("Orthophoto");
//        {
//            stream.writeTextElement("CRS", ortho.second.epsgCode);
//            stream.writeTextElement("Path", QString::fromStdWString(ortho.second.path.toWString()));
//            stream.writeTextElement("GSD", QString::number(ortho.second.gsd));
//            stream.writeTextElement("Interpolation", ortho.second.interpolation);
//
//            auto &report = ortho.second.report;
//            if (!report.isEmpty()) {
//
//                stream.writeStartElement("Report");
//
//                stream.writeTextElement("Time", QString::number(report.time, 'f', 10));
//                stream.writeTextElement("GSD", QString::number(report.gsd, 'f', 10));
//                stream.writeTextElement("Cols", QString::number(report.cols));
//                stream.writeTextElement("Rows", QString::number(report.rows));
//                stream.writeTextElement("Channels", QString::number(report.channels));
//
//                stream.writeEndElement(); // Report
//            }
//        }
//        stream.writeEndElement();
//    }
//}
//
//QSize Project::readSize(QXmlStreamReader &stream) const
//{
//    QSize blockSize;
//    while (stream.readNextStartElement()) {
//        if (stream.name() == "Width") {
//            blockSize.setWidth(readInt(stream));
//        } else if (stream.name() == "Height") {
//            blockSize.setHeight(readInt(stream));
//        } else
//            stream.skipCurrentElement();
//    }
//    return blockSize;
//}
//
//int Project::readInt(QXmlStreamReader &stream) const
//{
//    QString string = stream.readElementText();
//    if (!string.isEmpty()) return string.toInt();
//    else return 0;
//}
//
//double Project::readDouble(QXmlStreamReader &stream) const
//{
//    QString string = stream.readElementText();
//    if (!string.isEmpty()) return string.toDouble();
//    else return 0.;
//}
//
//bool Project::readBoolean(QXmlStreamReader &stream) const
//{
//    return stream.readElementText().compare("true") == 0 ? true : false;
//}

} // end namespace graphos
