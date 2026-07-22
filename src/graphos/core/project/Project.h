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

#ifndef GRAPHOS_CORE_PROJECT_H
#define GRAPHOS_CORE_PROJECT_H

#include "graphos/graphos_global.h"

#include <memory>
#include <vector>
#include <mutex>
#include <map>
#include <unordered_map>

#include <QString>
#include <QSize>

#include "graphos/core/project/ProjectInfo.h"
#include "graphos/core/features/matching.h"
#include "graphos/core/camera/Camera.h"
#include "graphos/core/image.h"
#include "graphos/core/features/features.h"
#include "graphos/core/sfm/poses.h"
#include "graphos/core/sfm/OrientationReport.h"
#include "graphos/core/dense/dense.h"
#include "graphos/core/mesh/PoissonRecon.h"
#include "graphos/core/reports/dem.h"
#include "graphos/core/reports/orthophoto.h"

// extraer de aqui a XmlSerializer.h
class QXmlStreamWriter;
class QXmlStreamReader;

namespace graphos
{

class Sift;
class Vignetting;

struct DemData
{
    QString epsgCode;
    tl::Path dtmPath;
    tl::Path dsmPath;
    double gsd = 0.1;
};

class Project
{

protected:

    //QString mName;
    //QString mDescription;
    //tl::Path mProjectFolder;
    //tl::Path mProjectPath;
    //QString mVersion;
    //tl::Path mDatabase;
    ProjectInfo mProjectInfo;

    QString mCrs;

    std::unordered_map<size_t, Image> mImages;
    std::map<int, Camera> mCameras;
    std::shared_ptr<Feature> mFeatureExtractor;
    FeatureExtractorReport mFeatureExtractorReport;
    std::unordered_map<size_t, QString> mFeatures;
    std::shared_ptr<FeatureMatching> mFeatureMatching;
    FeatureMatchingReport mFeatureMatchingReport;
    std::unordered_map<size_t, std::vector<size_t>> mImagesPairs;
    std::unordered_map<size_t, CameraPose> mPhotoOrientation;

    tl::Path mSparseModel;
    QString mEnuCrs;
    tl::Path mGroundPoints;
    OrientationReport mOrientationReport;
    //tl::Path mReconstructionPath;
    std::shared_ptr<Densification> mDensification;
    tl::Path mDenseModel;
    DenseReport mDenseReport;
    std::shared_ptr<PoissonReconProperties> mMeshProperties;
    tl::Path mMeshModel;
    MeshReport mMeshReport;
    DemData mDem;
    DemReport mDemReport;
    std::map<size_t, OrthophotoData> mOrthophotos;
    static std::mutex sMutex;
    int mCameraCount;
    //tl::Path mOrthophoto;
    tl::Matrix<double, 4, 4> mTransform;

public:

    Project();
    ~Project() = default;

    auto info() -> ProjectInfo &;
    auto info() const -> const ProjectInfo &;

    void addImage(const Image &img);
    bool updateImage(size_t imageId, const Image &image);
    void removeImage(size_t imageId);
    auto findImageById(size_t id) const -> Image;
    auto existImage(size_t imageId) const -> bool;
    auto images() const -> const std::unordered_map<size_t, Image>&;
    auto imagesCount() const -> size_t;

    auto addCamera(const Camera &camera) -> int;
    auto findCamera(const QString &make, 
                    const QString &model, 
                    const QString &serialNumber, 
                    const QString &bandName) const -> Camera;
    auto findCamera(int idCamera) const -> Camera;
    auto cameraId(const QString &make,
                  const QString &model,
                  const QString &serialNumber,
                  const QString &bandName) const -> int;
    auto existCamera(const QString &make,
                     const QString &model, 
                     const QString &serialNumber, 
                     const QString &bandName) const -> bool;
    auto updateCamera(int idCamera, const Camera &camera) -> bool;
    auto removeCamera(int idCamera) -> bool;
    auto cameras() const -> const std::map<int, Camera> &;
    size_t camerasCount() const;

    std::shared_ptr<Feature> featureExtractor() const;
    void setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor);
    FeatureExtractorReport featureExtractorReport() const;
    void setFeatureExtractorReport(const FeatureExtractorReport& report);

    QString features(size_t imageId) const;
    void addFeatures(size_t imageId, const QString &featureFile);
    void removeFeatures();
    void removeFeatures(size_t imageId);
    const std::unordered_map<size_t, QString> &features() const;

    std::shared_ptr<FeatureMatching> featureMatching() const;
    void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching);
    FeatureMatchingReport featureMatchingReport() const;
    void setFeatureMatchingReport(const FeatureMatchingReport &report);

    void addMatchesPair(size_t imageLeftId, size_t imageRightId);
    const std::vector<size_t> matchesPairs(size_t imageLeftId) const;
    void removeMatchesPair();
    void removeMatchesPair(size_t imageLeftId);

    tl::Path sparseModel() const;
    void setSparseModel(const tl::Path &sparseModel);
    QString enuCrs() const;
    void setEnuCrs(const QString &enuCrs);
    tl::Path groundPoints() const;
    void setGroundPoints(const tl::Path &groundPoints);
    tl::Path reconstructionPath() const;
    //void setReconstructionPath(const tl::Path &reconstructionPath);
    bool isPhotoOriented(size_t imageId) const;
    CameraPose photoOrientation(size_t imageId) const;
    const std::unordered_map<size_t, CameraPose> &poses() const;
    void addPhotoOrientation(size_t imageId, const CameraPose &photoOrientation);
    void clearReconstruction();
    OrientationReport orientationReport() const;
    void setOrientationReport(const OrientationReport &orientationReport);

    std::shared_ptr<Densification> densification() const;
    void setDensification(const std::shared_ptr<Densification> &densification);
    tl::Path denseModel() const;
    void setDenseModel(const tl::Path &denseModel);
    DenseReport denseReport() const;
    void setDenseReport(const DenseReport &denseReport);
    void clearDensification();

    std::shared_ptr<PoissonReconProperties> meshProperties() const;
    void setMeshProperties(const std::shared_ptr<PoissonReconProperties> &meshProperties);
    tl::Path meshPath() const;
    void setMeshPath(const tl::Path &meshPath);
    MeshReport meshReport() const;
    void setMeshReport(const MeshReport &report);
    void clearMesh();

    const DemData &dem() const;
    DemData &dem();
    void setDem(const DemData &dem);
    DemReport demReport() const;
    void setDemReport(const DemReport &report);
    void clearDem();

    auto orthophotos() const -> const std::map<size_t, OrthophotoData>& ;
    auto orthophotos() -> std::map<size_t, OrthophotoData>&;
    void setOrthophoto(const OrthophotoData &orthophoto);
    void clearOrthophoto();

    void clear();

    void load(const tl::Path &file);
    void save(const tl::Path &file);
    void exportCameras(const tl::Path &file);
    bool checkOldVersion(const tl::Path &file) const;
    void oldVersionBak(const tl::Path &file) const;

    tl::Matrix<double, 4, 4> &transform();
    const tl::Matrix<double, 4, 4> &transform() const;
    void setTransform(const tl::Matrix<double, 4, 4> &transform);

protected:

    void read(QXmlStreamReader &stream);
    void readGeneral(QXmlStreamReader &stream);
    //void readDatabase(QXmlStreamReader &stream);
    //void readCrs(QXmlStreamReader &stream);
    void readImages(QXmlStreamReader &stream);
    Image readImage(QXmlStreamReader &stream);
    CameraPose readCameraPosition(QXmlStreamReader &stream);
    void readImageMetadata(QXmlStreamReader &stream, Image &image);
    void readCameras(QXmlStreamReader &stream);
    void readCamera(QXmlStreamReader &stream);
    void readPriorCalibration(QXmlStreamReader &stream, Camera &camera);
    void readCalibration(QXmlStreamReader &stream, Camera &camera);
    void readVignetting(QXmlStreamReader &stream, Camera &camera);
    void readFeatures(QXmlStreamReader &stream);
    void readFeatureExtractor(QXmlStreamReader &stream);
    void readFeatureExtractorReport(QXmlStreamReader &stream);
    void readSIFT(QXmlStreamReader &stream);
    void readFeatureFiles(QXmlStreamReader &stream);
    void readFeatureFile(QXmlStreamReader &stream);
    void readMatches(QXmlStreamReader &stream);
    void readMatchingMethod(QXmlStreamReader &stream);
    void readFeatureMatchingReport(QXmlStreamReader &stream);
    void readPairs(QXmlStreamReader &stream);
    void readOrientations(QXmlStreamReader &stream);
    //void readReconstructionPath(QXmlStreamReader &stream);
    void readOrientationSparseModel(QXmlStreamReader &stream);
    void readEnuCrs(QXmlStreamReader &stream);
    void readGroundPoints(QXmlStreamReader &stream);
    void readPhotoOrientations(QXmlStreamReader &stream);
    void readOrientationReport(QXmlStreamReader& stream);
    void readDensification(QXmlStreamReader &stream);
    void readDenseModel(QXmlStreamReader &stream);
    void readDenseReport(QXmlStreamReader &stream);
    void readDensificationMethod(QXmlStreamReader &stream);
    void readSmvs(QXmlStreamReader &stream);
    void readCmvsPmvs(QXmlStreamReader &stream);
    void readMVS(QXmlStreamReader &stream);
    void readMesh(QXmlStreamReader &stream);
    void readMeshModel(QXmlStreamReader &stream);
    void readMeshReport(QXmlStreamReader &stream);
    void readMeshParameters(QXmlStreamReader &stream);
    void readDem(QXmlStreamReader &stream);
    void readDemReport(QXmlStreamReader &stream);
    auto readOrthophoto(QXmlStreamReader &stream) -> OrthophotoData;
    auto readOrthophotoReport(QXmlStreamReader &stream) -> OrthophotoReport;

    void writeVersion(QXmlStreamWriter &stream) const;
    void writeGeneral(QXmlStreamWriter &stream) const;
    void writeDatabase(QXmlStreamWriter &stream) const;
    //void writeCrs(QXmlStreamWriter &stream) const;
    void writeCameras(QXmlStreamWriter &stream) const;
    void writeCamera(QXmlStreamWriter &stream, int id, const Camera &camera) const;
    void writePriorCalibration(QXmlStreamWriter &stream, std::shared_ptr<Calibration> calibration) const;
    void writeCalibration(QXmlStreamWriter &stream, std::shared_ptr<Calibration> calibration) const;
    void writeCalibrationUndistorted(QXmlStreamWriter &stream, std::shared_ptr<Calibration> calibration) const;
    void writeVignetting(QXmlStreamWriter &stream, std::shared_ptr<Vignetting> vignetting) const;
    void writeImages(QXmlStreamWriter &stream) const;
    void writeImage(QXmlStreamWriter &stream, const std::pair<size_t, Image> &image) const;
    void writeCameraPosition(QXmlStreamWriter &stream, const CameraPose &cameraPosition) const;
    void writeImageMetadata(QXmlStreamWriter &stream, const Image &image) const; 
    void writeFeatures(QXmlStreamWriter &stream) const;
    void writeFeatureExtractor(QXmlStreamWriter &stream) const;
    void writeFeatureExtractorReport(QXmlStreamWriter &stream) const;
    void writeSIFT(QXmlStreamWriter &stream, const Sift *sift) const;
    void writeFeatureFiles(QXmlStreamWriter &stream) const;
    void writeMatches(QXmlStreamWriter &stream) const;
    void writeFeatureMatchingMethod(QXmlStreamWriter &stream) const;
    void writeFeatureMatchingReport(QXmlStreamWriter &stream) const;
    void writePairs(QXmlStreamWriter &stream) const;
    void writeOrientations(QXmlStreamWriter &stream) const;
    //void writeReconstructionPath(QXmlStreamWriter &stream) const;
    void writeOrientationSparseModel(QXmlStreamWriter &stream) const;
    void writeOffset(QXmlStreamWriter &stream) const;
    void writeGroundPoints(QXmlStreamWriter &stream) const;
    void writePhotoOrientations(QXmlStreamWriter &stream) const;
    void writeOrientationReport(QXmlStreamWriter &stream) const;
    void writeDensification(QXmlStreamWriter &stream) const;
    void writeDenseModel(QXmlStreamWriter &stream) const;
    void writeDenseReport(QXmlStreamWriter &stream) const;
    void writeDensificationMethod(QXmlStreamWriter &stream) const;
    void writeMesh(QXmlStreamWriter &stream) const;
    void writeMeshModel(QXmlStreamWriter &stream) const;
    void writeMeshReport(QXmlStreamWriter &stream) const;
    void writeMeshParameters(QXmlStreamWriter &stream) const;
    void writeDem(QXmlStreamWriter &stream) const;
    void writeDemReport(QXmlStreamWriter &stream) const;
    void writeOrthophoto(QXmlStreamWriter &stream) const;

    QSize readSize(QXmlStreamReader &stream) const;
    int readInt(QXmlStreamReader &stream) const;
    double readDouble(QXmlStreamReader &stream) const;
    bool readBoolean(QXmlStreamReader &stream) const;

};

} // end namespace graphos

#endif // GRAPHOS_CORE_PROJECT_H
