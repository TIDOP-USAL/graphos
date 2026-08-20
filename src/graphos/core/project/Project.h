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
//#include <mutex>
#include <map>
#include <unordered_map>

#include "graphos/core/project/ProjectInfo.h"
#include "graphos/core/image/ImageRepository.h"
#include "graphos/core/camera/CameraRepository.h"
#include "graphos/core/features/FeatureExtractorProperties.h"
#include "graphos/core/features/FeatureExtractorReport.h"
#include "graphos/core/features/FeaturesRepository.h"
#include "graphos/core/matching/MatchingProperties.h"
#include "graphos/core/matching/MatchingReport.h"
#include "graphos/core/matching/MatchingRepository.h"
#include "graphos/core/orientation/OrientationProperties.h"
#include "graphos/core/orientation/OrientationReport.h"
#include "graphos/core/orientation/CameraPosesRepository.h"
#include "graphos/core/dense/DensificationProperties.h"
#include "graphos/core/dense/DensificationReport.h"
#include "graphos/core/mesh/MeshReport.h"
#include "graphos/core/mesh/PoissonReconProperties.h"
#include "graphos/core/dem/DemProperties.h"
#include "graphos/core/dem/DemReport.h"
#include "graphos/core/ortho/OrthophotoProperties.h"
#include "graphos/core/ortho/OrthophotoReport.h"

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

    ProjectInfo mProjectInfo;
    CameraRepository mCameraRepository;
    ImageRepository mImageRepository;

    std::shared_ptr<FeatureExtractorProperties> mFeaturesConfig;
    FeatureExtractorReport mFeaturesReport;
    FeaturesRepository mFeaturesRepository;

    std::shared_ptr<MatchingProperties> mMatchingConfig;
    MatchingReport mMatchingReport;
    MatchingRepository mMatchingRepository;

    QString mCrs;

    std::shared_ptr<OrientationProperties> mOrientationConfig;
    OrientationReport mOrientationReport;
    tl::Path mSparseModel;
    tl::Path mGroundPoints;
    tl::Path mPosesFile;
    mutable std::unique_ptr<CameraPosesRepository> mCameraPosesRepository;

    std::shared_ptr<DensificationProperties> mDensificationConfig;
    tl::Path mDenseModel;
    DensificationReport mDenseReport;

    std::shared_ptr<PoissonReconProperties> mMeshProperties;
    tl::Path mMeshModel;
    MeshReport mMeshReport;

    std::shared_ptr<DemProperties> mDemProperties;
    tl::Path mDtm;
    tl::Path mDsm;
    DemReport mDemReport;

    std::shared_ptr<OrthophotoProperties> mOrthoProperties;
    OrthophotoReport mOrthoReport;
    tl::Path mOrthoMosaic;
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

    auto featureConfig() const -> std::shared_ptr<FeatureExtractorProperties>;
    void setFeatureConfig(std::shared_ptr<FeatureExtractorProperties> config);

    auto featureReport() const -> const FeatureExtractorReport &;
    void setFeatureReport(FeatureExtractorReport report);

    auto features() -> FeaturesRepository &;
    auto features() const -> const FeaturesRepository &;

    void clearFeatures();

    // Matching
    
    auto matchingConfig() const -> std::shared_ptr<MatchingProperties>;
    void setMatchingConfig(std::shared_ptr<MatchingProperties> config);

    auto matchingReport() const -> MatchingReport;
    void setMatchingReport(MatchingReport report);

    auto matches() -> MatchingRepository &;
    auto matches() const -> const MatchingRepository &;

    void clearMatches();
    
    // Orientation

    auto orientationConfig() const -> std::shared_ptr<OrientationProperties>;
    void setOrientationConfig(std::shared_ptr<OrientationProperties> config);

    auto orientationReport() const -> OrientationReport;
    void setOrientationReport(OrientationReport report);

    auto sparseModel() const -> tl::Path;
    void setSparseModel(tl::Path sparseModel);

    auto groundPoints() const -> tl::Path;
    void setGroundPoints(tl::Path groundPoints);
    
    auto cameraPosesFile() const -> tl::Path;
    void setCameraPosesFile(tl::Path poses);

    auto cameraPoses() -> CameraPosesRepository &;

    void clearOrientation();

    // Densification

    auto densificationConfig() const -> std::shared_ptr<DensificationProperties>;
    void setDensificationConfig(std::shared_ptr<DensificationProperties> config);
     
    auto denseModel() const -> tl::Path;
    void setDenseModel(tl::Path denseModel);

    auto densificationReport() const -> DensificationReport;
    void setDensificationReport(DensificationReport densificationReport);
    
    void clearDensification();

    // Mesh

    auto meshConfig() const -> std::shared_ptr<PoissonReconProperties>;
    void setMeshConfig(std::shared_ptr<PoissonReconProperties> config);
    
    auto meshModel() const -> tl::Path;
    void setMeshModel(tl::Path meshModel);

    auto meshReport() const -> MeshReport;
    void setMeshReport(MeshReport meshReport);

    void clearMesh();

    // DEM

    auto demConfig() const -> std::shared_ptr<DemProperties>;
    void setDemConfig(std::shared_ptr<DemProperties> config);

    auto dsm() const -> tl::Path;
    void setDsm(tl::Path dsm);

    auto dtm() const -> tl::Path;
    void setDtm(tl::Path dtm);

    auto demReport() const -> DemReport;
    void setDemReport(DemReport demReport);

    void clearDem();

    // Orthophoto

    auto orthoConfig() const -> std::shared_ptr<OrthophotoProperties>;
    void setOrthoConfig(std::shared_ptr<OrthophotoProperties> config);

    auto orthoReport() const -> OrthophotoReport;
    void setOrthoReport(OrthophotoReport orthoReport);

    auto orthophoto() const -> tl::Path;
    void setOrthophoto(tl::Path orthophoto);

    void clearOrthophoto();

    void clear();

    //void load(const tl::Path &file);
    //void save(const tl::Path &file);
    //void exportCameras(const tl::Path &file);
    //bool checkOldVersion(const tl::Path &file) const;
    //void oldVersionBak(const tl::Path &file) const;

    //tl::Matrix<double, 4, 4> &transform();
    //const tl::Matrix<double, 4, 4> &transform() const;
    //void setTransform(const tl::Matrix<double, 4, 4> &transform);

};

} // end namespace graphos