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

#pragma once

#include "graphos/graphos_global.h"

#include <memory>
#include <vector>
#include <mutex>
#include <map>
#include <unordered_map>

#include <QString>
#include <QSize>

#include "graphos/core/project/ProjectInfo.h"
//
//#include "graphos/core/camera/Camera.h"
//#include "graphos/core/Image.h"

//#include "graphos/core/sfm/poses.h"
//#include "graphos/core/sfm/OrientationReport.h"
//#include "graphos/core/dense/dense.h"
//#include "graphos/core/mesh/PoissonRecon.h"
//#include "graphos/core/reports/dem.h"
//#include "graphos/core/reports/orthophoto.h"
#include "graphos/core/repositories/CameraRepository.h"
#include "graphos/core/repositories/FeaturesRepository.h"
#include "graphos/core/repositories/ImageRepository.h"
#include "graphos/core/repositories/MatchingRepository.h"
#include "graphos/core/features/Features.h"
#include "graphos/core/features/FeatureExtractorReport.h"
#include "graphos/core/features/FeatureMatching.h"
#include "graphos/core/features/FeatureMatchingReport.h"

// extraer de aqui a XmlSerializer.h
//class QXmlStreamWriter;
//class QXmlStreamReader;

namespace graphos
{

//class Sift;
//class Vignetting;
//
//struct DemData
//{
//    QString epsgCode;
//    tl::Path dtmPath;
//    tl::Path dsmPath;
//    double gsd = 0.1;
//};

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
    CameraRepository mCameraRepository;
    ImageRepository mImageRepository;

    std::shared_ptr<Feature> mFeatConfig;
    FeatureExtractorReport mFeatReport;
    FeaturesRepository mFeatureRepo;

    std::shared_ptr<FeatureMatching> mFeatureMatchingConfig;
    FeatureMatchingReport mFeatureMatchingReport;
    MatchingRepository mMatchingRepository;

    QString mCrs;

    //std::shared_ptr<FeatureMatching> mFeatureMatching;
    
    //std::unordered_map<size_t, std::vector<size_t>> mImagesPairs;
    //std::unordered_map<size_t, CameraPose> mPhotoOrientation;

    //tl::Path mSparseModel;
    //QString mEnuCrs;
    //tl::Path mGroundPoints;
    //OrientationReport mOrientationReport;
    ////tl::Path mReconstructionPath;
    //std::shared_ptr<Densification> mDensification;
    //tl::Path mDenseModel;
    //DenseReport mDenseReport;
    //std::shared_ptr<PoissonReconProperties> mMeshProperties;
    //tl::Path mMeshModel;
    //MeshReport mMeshReport;
    //DemData mDem;
    //DemReport mDemReport;
    //std::map<size_t, OrthophotoData> mOrthophotos;
    //static std::mutex sMutex;
    ////int mCameraCount;
    ////tl::Path mOrthophoto;
    //tl::Matrix<double, 4, 4> mTransform;

public:

    Project();
    ~Project() = default;

    auto info() -> ProjectInfo &;
    auto info() const -> const ProjectInfo &;

    auto cameras() -> CameraRepository &;
    auto cameras() const -> const CameraRepository &;

    auto images() -> ImageRepository &;
    auto images() const -> const ImageRepository &;

    // Features

    auto featureConfig() const -> std::shared_ptr<Feature>;
    void setFeatureConfig(std::shared_ptr<Feature> config);

    auto featureReport() const -> const FeatureExtractorReport &;
    void setFeatureReport(FeatureExtractorReport report);

    auto features() -> FeaturesRepository &;
    auto features() const -> const FeaturesRepository &;

    void clearFeatures();

    // Matching
    
    // Configuración dinámica del Matcher (hereda de Properties)
    auto featureMatcherConfig() const -> std::shared_ptr<FeatureMatching>;
    void setFeatureMatcherConfig(std::shared_ptr<FeatureMatching> config);

    auto featureMatchingReport() const -> FeatureMatchingReport;
    void setFeatureMatchingReport(FeatureMatchingReport report);

    auto matches() -> MatchingRepository &;
    auto matches() const -> const MatchingRepository &;

    void clearMatches();
    
    //std::shared_ptr<FeatureMatching> featureMatching() const;
    //void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching);
    //FeatureMatchingReport featureMatchingReport() const;
    //void setFeatureMatchingReport(const FeatureMatchingReport &report);

    //void addMatchesPair(size_t imageLeftId, size_t imageRightId);
    //const std::vector<size_t> matchesPairs(size_t imageLeftId) const;
    //void removeMatchesPair();
    //void removeMatchesPair(size_t imageLeftId);

    //tl::Path sparseModel() const;
    //void setSparseModel(const tl::Path &sparseModel);
    //QString enuCrs() const;
    //void setEnuCrs(const QString &enuCrs);
    //tl::Path groundPoints() const;
    //void setGroundPoints(const tl::Path &groundPoints);
    //tl::Path reconstructionPath() const;
    ////void setReconstructionPath(const tl::Path &reconstructionPath);
    //bool isPhotoOriented(size_t imageId) const;
    //CameraPose photoOrientation(size_t imageId) const;
    //const std::unordered_map<size_t, CameraPose> &poses() const;
    //void addPhotoOrientation(size_t imageId, const CameraPose &photoOrientation);
    void clearReconstruction();
    //OrientationReport orientationReport() const;
    //void setOrientationReport(const OrientationReport &orientationReport);

    //std::shared_ptr<Densification> densification() const;
    //void setDensification(const std::shared_ptr<Densification> &densification);
    //tl::Path denseModel() const;
    //void setDenseModel(const tl::Path &denseModel);
    //DenseReport denseReport() const;
    //void setDenseReport(const DenseReport &denseReport);
    //void clearDensification();

    //std::shared_ptr<PoissonReconProperties> meshProperties() const;
    //void setMeshProperties(const std::shared_ptr<PoissonReconProperties> &meshProperties);
    //tl::Path meshPath() const;
    //void setMeshPath(const tl::Path &meshPath);
    //MeshReport meshReport() const;
    //void setMeshReport(const MeshReport &report);
    //void clearMesh();

    //const DemData &dem() const;
    //DemData &dem();
    //void setDem(const DemData &dem);
    //DemReport demReport() const;
    //void setDemReport(const DemReport &report);
    //void clearDem();

    //auto orthophotos() const -> const std::map<size_t, OrthophotoData>& ;
    //auto orthophotos() -> std::map<size_t, OrthophotoData>&;
    //void setOrthophoto(const OrthophotoData &orthophoto);
    //void clearOrthophoto();

    void clear();

    //void load(const tl::Path &file);
    //void save(const tl::Path &file);
    //void exportCameras(const tl::Path &file);
    //bool checkOldVersion(const tl::Path &file) const;
    //void oldVersionBak(const tl::Path &file) const;

    //tl::Matrix<double, 4, 4> &transform();
    //const tl::Matrix<double, 4, 4> &transform() const;
    //void setTransform(const tl::Matrix<double, 4, 4> &transform);

protected:

    //void readMatches(QXmlStreamReader &stream);
    //void readMatchingMethod(QXmlStreamReader &stream);
    //void readFeatureMatchingReport(QXmlStreamReader &stream);
    //void readPairs(QXmlStreamReader &stream);
    //void readOrientations(QXmlStreamReader &stream);
    ////void readReconstructionPath(QXmlStreamReader &stream);
    //void readOrientationSparseModel(QXmlStreamReader &stream);
    //void readEnuCrs(QXmlStreamReader &stream);
    //void readGroundPoints(QXmlStreamReader &stream);
    //void readPhotoOrientations(QXmlStreamReader &stream);
    //void readOrientationReport(QXmlStreamReader& stream);
    //void readDensification(QXmlStreamReader &stream);
    //void readDenseModel(QXmlStreamReader &stream);
    //void readDenseReport(QXmlStreamReader &stream);
    //void readDensificationMethod(QXmlStreamReader &stream);
    //void readSmvs(QXmlStreamReader &stream);
    //void readCmvsPmvs(QXmlStreamReader &stream);
    //void readMVS(QXmlStreamReader &stream);
    //void readMesh(QXmlStreamReader &stream);
    //void readMeshModel(QXmlStreamReader &stream);
    //void readMeshReport(QXmlStreamReader &stream);
    //void readMeshParameters(QXmlStreamReader &stream);
    //void readDem(QXmlStreamReader &stream);
    //void readDemReport(QXmlStreamReader &stream);
    //auto readOrthophoto(QXmlStreamReader &stream) -> OrthophotoData;
    //auto readOrthophotoReport(QXmlStreamReader &stream) -> OrthophotoReport;

    //void writeMatches(QXmlStreamWriter &stream) const;
    //void writeFeatureMatchingMethod(QXmlStreamWriter &stream) const;
    //void writeFeatureMatchingReport(QXmlStreamWriter &stream) const;
    //void writePairs(QXmlStreamWriter &stream) const;
    //void writeOrientations(QXmlStreamWriter &stream) const;
    ////void writeReconstructionPath(QXmlStreamWriter &stream) const;
    //void writeOrientationSparseModel(QXmlStreamWriter &stream) const;
    //void writeOffset(QXmlStreamWriter &stream) const;
    //void writeGroundPoints(QXmlStreamWriter &stream) const;
    //void writePhotoOrientations(QXmlStreamWriter &stream) const;
    //void writeOrientationReport(QXmlStreamWriter &stream) const;
    //void writeDensification(QXmlStreamWriter &stream) const;
    //void writeDenseModel(QXmlStreamWriter &stream) const;
    //void writeDenseReport(QXmlStreamWriter &stream) const;
    //void writeDensificationMethod(QXmlStreamWriter &stream) const;
    //void writeMesh(QXmlStreamWriter &stream) const;
    //void writeMeshModel(QXmlStreamWriter &stream) const;
    //void writeMeshReport(QXmlStreamWriter &stream) const;
    //void writeMeshParameters(QXmlStreamWriter &stream) const;
    //void writeDem(QXmlStreamWriter &stream) const;
    //void writeDemReport(QXmlStreamWriter &stream) const;
    //void writeOrthophoto(QXmlStreamWriter &stream) const;

    //QSize readSize(QXmlStreamReader &stream) const;
    //int readInt(QXmlStreamReader &stream) const;
    //double readDouble(QXmlStreamReader &stream) const;
    //bool readBoolean(QXmlStreamReader &stream) const;

};

} // end namespace graphos