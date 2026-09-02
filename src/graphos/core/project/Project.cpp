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

#define GRAPHOS_PROJECT_FILE_VERSION "1.0"

namespace graphos
{

//std::mutex Project::sMutex;

Project::Project()
    : mMatchingRepository(mProjectInfo.database()),
      mTransform(tl::Matrix<double, 4, 4>::identity())
{
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
    mCameraPosesRepository.reset();
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

auto Project::dsmEnu() const -> tl::Path
{
    return mDsmEnu;
}

void Project::setDsmEnu(tl::Path dsmEnu)
{
    mDsmEnu = std::move(dsmEnu);
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
    mCameraPosesRepository.reset();
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
    mTransform = tl::Matrix<double, 4, 4>::identity();
}

auto Project::transform() -> tl::Matrix<double, 4, 4> &
{
    return mTransform;
}

auto Project::transform() const -> const tl::Matrix<double, 4, 4> &
{
    return mTransform;
}

void Project::setTransform(tl::Matrix<double, 4, 4> transform)
{
    mTransform = std::move(transform);
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

} // end namespace graphos
