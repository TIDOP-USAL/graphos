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

#include "features/matching.h"
#include "graphos/core/camera/Camera.h"
#include "graphos/core/image.h"
#include "graphos/core/features/features.h"
#include "graphos/core/sfm/poses.h"
#include "graphos/core/sfm/OrientationReport.h"
#include "graphos/core/dense/dense.h"
#include "graphos/core/mesh/PoissonRecon.h"

class QXmlStreamWriter;
class QXmlStreamReader;

namespace graphos
{
class Sift;

struct DTMData
{
    tl::Path dtmPath;
    tl::Path dsmPath;
    double gsd = 0.1;
};

struct OrthophotoData
{
    tl::Path path;
    double gsd = 0.05;
};


/*!
 * \brief Interface Project
 */
class Project
{

public:

    Project() {}
    virtual ~Project() = default;

    /*!
     * \brief Devuelve el nombre del proyecto
     * \return Nombre del proyecto
     */
    virtual QString name() const = 0;

    /*!
     * \brief Establece el nombre del proyecto
     * \param[in] name Nombre del proyecto
     */
    virtual void setName(const QString &name) = 0;

    /*!
     * \brief Devuelve la descripción del proyecto
     * \return Descripción del proyecto
     */
    virtual QString description() const = 0;

    /*!
     * \brief Establece la descripción del proyecto
     * \param[in] description Descripción del proyecto
     */
    virtual void setDescription(const QString &description) = 0;

    /*!
     * \brief Devuelve el directorio del proyecto
     * \return Directorio del proyecto
     */
    virtual tl::Path projectFolder() const = 0;

    /*!
     * \brief Establece el directorio del proyecto
     * \param[in] path Directorio del proyecto
     */
    virtual void setProjectFolder(const tl::Path &folder) = 0;

    /*!
     * \brief Devuelve el ruta del proyecto
     * \return Ruta del proyecto
     */
    virtual tl::Path projectPath() const = 0;

    /*!
     * \brief Versión del fichero de proyecto
     * La versión del fichero de proyecto se utiliza para actualizar proyectos antiguos
     * \return Versión del fichero de proyecto
     */
    virtual QString version() const = 0;

    /*!
     * \brief Base de datos
     * Base de datos de Colmap
     * \return Ruta de la base de datos
     */
    virtual tl::Path database() const = 0;

    /*!
     * \brief Establece la base de datos
     * \param[in] database Base de datos
     */
    virtual void setDatabase(const tl::Path &database) = 0;

    /*!
     * \brief Sistema de referencia (código EPSG)
     */
    virtual QString crs() const = 0;

    /*!
     * \brief Establece el sistema de referencia (código EPSG)
     */
    virtual void setCrs(const QString &crs) = 0;

    /*!
     * \brief Añade una imagen al proyecto
     * \param[in] img Imagen
     */
    virtual void addImage(const Image &img) = 0;

    virtual bool updateImage(size_t imageId,
                             const Image &image) = 0;
    /*!
     * \brief Elimina una imagen del proyecto
     * \param[in] imgId Identificador de la imagen que se quiere eliminar
     */
    virtual void removeImage(size_t imgId) = 0;

    /*!
     * \brief Busca una imagen en el proyecto
     * \param[in] path Ruta de la imagen a buscar
     * \return Objeto Image correspondiente o puntero nulo si no se encuentra
     */
    virtual Image findImageById(size_t id) const = 0;

    virtual bool existImage(size_t imageId) const = 0;

    virtual const std::unordered_map<size_t, Image> &images() const = 0;

    /*!
     * \brief Número de imagenes cargadas en el proyecto
     * \return
     */
    virtual size_t imagesCount() const = 0;

    /*!
     * \brief Añade una cámara al proyecto
     * \param[in] camera Cámara
     */
    virtual int addCamera(const Camera &camera) = 0;

    /*!
     * \brief Busca una cámara en el proyecto
     * \param[in] make Marca de la cámara
     * \param[in] model Modelo de la cámara
     * \return Objeto Camera correspondiente o puntero nulo si no se encuentra
     */
    virtual Camera findCamera(const QString &make, const QString &model) const = 0;

    virtual Camera findCamera(int idCamera) const = 0;
    virtual int cameraId(const QString &make, const QString &model) const = 0;
    virtual bool existCamera(const QString &make, const QString &model) const = 0;
    virtual bool updateCamera(int idCamera, const Camera &camera) = 0;
    virtual bool removeCamera(int idCamera) = 0;
    virtual const std::map<int, Camera> &cameras() const = 0;
    virtual size_t camerasCount() const = 0;

    virtual std::shared_ptr<Feature> featureExtractor() const = 0;
    virtual void setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor) = 0;
    virtual FeatureExtractorReport featureExtractorReport() const = 0;
    virtual void setFeatureExtractorReport(const FeatureExtractorReport &report) = 0;

    virtual QString features(size_t imageId) const = 0;
    virtual void addFeatures(size_t imageId, const QString &featureFile) = 0;
    virtual void removeFeatures() = 0;
    virtual void removeFeatures(size_t imageId) = 0;
    virtual const std::unordered_map<size_t, QString> &features() const = 0;

    virtual std::shared_ptr<FeatureMatching> featureMatching() const = 0;
    virtual void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching) = 0;
    virtual FeatureMatchingReport featureMatchingReport() const = 0;
    virtual void setFeatureMatchingReport(const FeatureMatchingReport &report) = 0;

    virtual void addMatchesPair(size_t imageLeftId, size_t imageRightId) = 0;
    virtual const std::vector<size_t> matchesPairs(size_t imageLeftId) const = 0;
    virtual void removeMatchesPair() = 0;
    virtual void removeMatchesPair(size_t imageLeftId) = 0;

    virtual tl::Path sparseModel() const = 0;
    virtual void setSparseModel(const tl::Path &sparseModel) = 0;
    virtual tl::Path offset() const = 0;
    virtual void setOffset(const tl::Path &offset) = 0;
    virtual tl::Path groundPoints() const = 0;
    virtual void setGroundPoints(const tl::Path &groundPoints) = 0;
    virtual tl::Path reconstructionPath() const = 0;
    //virtual void setReconstructionPath(const tl::Path &reconstructionPath) = 0;
    virtual bool isPhotoOriented(size_t imageId) const = 0;
    virtual CameraPose photoOrientation(size_t imageId) const = 0;
    virtual const std::unordered_map<size_t, CameraPose> &poses() const = 0;
    virtual void addPhotoOrientation(size_t imageId, const CameraPose &photoOrientation) = 0;
    virtual void clearReconstruction() = 0;
    virtual OrientationReport orientationReport() const = 0;
    virtual void setOrientationReport(const OrientationReport &orientationReport) = 0;

    virtual std::shared_ptr<Densification> densification() const = 0;
    virtual void setDensification(const std::shared_ptr<Densification> &densification) = 0;
    virtual tl::Path denseModel() const = 0;
    virtual void setDenseModel(const tl::Path &denseModel) = 0;
    virtual DenseReport denseReport() const = 0;
    virtual void setDenseReport(const DenseReport &denseReport) = 0;
    virtual void clearDensification() = 0;

    virtual std::shared_ptr<PoissonReconProperties> meshProperties() const = 0;
    virtual void setProperties(const std::shared_ptr<PoissonReconProperties> &meshProperties) = 0;
    virtual tl::Path meshPath() const = 0;
    virtual void setMeshPath(const tl::Path &meshPath) = 0;
    virtual MeshReport meshReport() const = 0;
    virtual void setMeshReport(const MeshReport &report) = 0;
    virtual void clearMesh() = 0;

    //virtual std::shared_ptr<Dtm> dtmMethod() const = 0;
    //virtual void setDtmMethod(const std::shared_ptr<Dtm> &dtm) = 0;
    //virtual tl::Path dtmPath() const = 0;
    //virtual void setDtmPath(const tl::Path &dtmPath) = 0;
    virtual const DTMData &dtm() const = 0;
    virtual DTMData &dtm() = 0;
    virtual void setDtm(const DTMData &dtm) = 0;
    virtual void clearDTM() = 0;

    //virtual tl::Path orthophotoPath() const = 0;
    //virtual void setOrthophotoPath(const tl::Path &orthophotoPath) = 0;
    virtual const OrthophotoData &orthophoto() const = 0;
    virtual OrthophotoData &orthophoto() = 0;
    virtual void setOrthophoto(const OrthophotoData &orthophoto) = 0;
    virtual void clearOrthophoto() = 0;

    virtual tl::Matrix<double, 4, 4> &transform() = 0;
    virtual const tl::Matrix<double, 4, 4> &transform() const = 0;
    virtual void setTransform(const tl::Matrix<double, 4, 4> &transform) = 0;

    virtual void clear() = 0;
    virtual void load(const tl::Path &file) = 0;
    virtual void save(const tl::Path &file) = 0;

    /*!
     * \brief checkOldVersion
     * \param file
     * \return
     */
    virtual bool checkOldVersion(const tl::Path &file) const = 0;

    /*!
     * \brief Crea una copia de un proyecto antiguo con el mismo nombre y un sufijo con la versión
     * \param[in] file
     */
    virtual void oldVersionBak(const tl::Path &file) const = 0;
};


class ProjectImp
    : public Project
{

public:

    ProjectImp();
    ~ProjectImp() override = default;

    // Project interface

public:

    QString name() const override;
    void setName(const QString &name) override;
    QString description() const override;
    void setDescription(const QString &description) override;
    tl::Path projectFolder() const override;
    void setProjectFolder(const tl::Path &folder) override;
    tl::Path projectPath() const override;
    QString version() const override;
    tl::Path database() const override;
    void setDatabase(const tl::Path &database) override;
    QString crs() const override;
    void setCrs(const QString &crs) override;
    void addImage(const Image &img) override;
    bool updateImage(size_t imageId, const Image &image) override;
    void removeImage(size_t imageId) override;
    Image findImageById(size_t id) const override;
    bool existImage(size_t imageId) const override;
    const std::unordered_map<size_t, Image> &images() const override;
    size_t imagesCount() const override;

    int addCamera(const Camera &camera) override;
    const std::map<int, Camera> &cameras() const override;
    Camera findCamera(const QString &make, const QString &model) const override;
    Camera findCamera(int idCamera) const override;
    int cameraId(const QString &make, const QString &model) const override;
    bool existCamera(const QString &make, const QString &model) const override;
    bool updateCamera(int idCamera, const Camera &camera) override;
    bool removeCamera(int idCamera) override;
    size_t camerasCount() const override;

    std::shared_ptr<Feature> featureExtractor() const override;
    void setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor) override;
    FeatureExtractorReport featureExtractorReport() const override;
    void setFeatureExtractorReport(const FeatureExtractorReport& report) override;

    QString features(size_t imageId) const override;
    void addFeatures(size_t imageId, const QString &featureFile) override;
    void removeFeatures() override;
    void removeFeatures(size_t imageId) override;
    const std::unordered_map<size_t, QString> &features() const override;

    std::shared_ptr<FeatureMatching> featureMatching() const override;
    void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching) override;
    FeatureMatchingReport featureMatchingReport() const override;
    void setFeatureMatchingReport(const FeatureMatchingReport &report) override;

    void addMatchesPair(size_t imageLeftId, size_t imageRightId) override;
    const std::vector<size_t> matchesPairs(size_t imageLeftId) const override;
    void removeMatchesPair() override;
    void removeMatchesPair(size_t imageLeftId) override;

    tl::Path sparseModel() const override;
    void setSparseModel(const tl::Path &sparseModel) override;
    tl::Path offset() const override;
    void setOffset(const tl::Path &offset) override;
    tl::Path groundPoints() const override;
    void setGroundPoints(const tl::Path &groundPoints) override;
    tl::Path reconstructionPath() const override;
    //void setReconstructionPath(const tl::Path &reconstructionPath) override;
    bool isPhotoOriented(size_t imageId) const override;
    CameraPose photoOrientation(size_t imageId) const override;
    const std::unordered_map<size_t, CameraPose> &poses() const override;
    void addPhotoOrientation(size_t imageId, const CameraPose &photoOrientation) override;
    void clearReconstruction() override;
    OrientationReport orientationReport() const override;
    void setOrientationReport(const OrientationReport &orientationReport) override;

    std::shared_ptr<Densification> densification() const override;
    void setDensification(const std::shared_ptr<Densification> &densification) override;
    tl::Path denseModel() const override;
    void setDenseModel(const tl::Path &denseModel) override;
    DenseReport denseReport() const override;
    void setDenseReport(const DenseReport &denseReport) override;
    void clearDensification() override;

    std::shared_ptr<PoissonReconProperties> meshProperties() const override;
    void setProperties(const std::shared_ptr<PoissonReconProperties> &meshProperties) override;
    tl::Path meshPath() const override;
    void setMeshPath(const tl::Path &meshPath) override;
    MeshReport meshReport() const override;
    void setMeshReport(const MeshReport &report) override;
    void clearMesh() override;

    const DTMData &dtm() const override;
    DTMData &dtm() override;
    void setDtm(const DTMData &dtm) override;
    //void setDtmMethod(const std::shared_ptr<Dtm> &dtm) override;
    //tl::Path dtmPath() const override;
    //void setDtmPath(const tl::Path &dtmPath) override;
    void clearDTM() override;

    //tl::Path orthophotoPath() const override;
    //void setOrthophotoPath(const tl::Path &orthophotoPath) override;
    const OrthophotoData &orthophoto() const override;
    OrthophotoData &orthophoto() override;
    void setOrthophoto(const OrthophotoData &orthophoto) override;
    void clearOrthophoto() override;

    void clear() override;

    void load(const tl::Path &file) override;
    void save(const tl::Path &file) override;
    bool checkOldVersion(const tl::Path &file) const override;
    void oldVersionBak(const tl::Path &file) const override;

    tl::Matrix<double, 4, 4> &transform() override;
    const tl::Matrix<double, 4, 4> &transform() const override;
    void setTransform(const tl::Matrix<double, 4, 4> &transform) override;

protected:

    void read(QXmlStreamReader &stream);
    void readGeneral(QXmlStreamReader &stream);
    void readDatabase(QXmlStreamReader &stream);
    void readCrs(QXmlStreamReader &stream);
    void readImages(QXmlStreamReader &stream);
    Image readImage(QXmlStreamReader &stream);
    CameraPose readCameraPosition(QXmlStreamReader &stream);
    void readCameras(QXmlStreamReader &stream);
    Camera readCamera(QXmlStreamReader &stream);
    void readCalibration(QXmlStreamReader &stream, Camera &camera);
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
    void readOffset(QXmlStreamReader &stream);
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
    void readDtm(QXmlStreamReader &stream);
    void readOrthophoto(QXmlStreamReader &stream);

    void writeVersion(QXmlStreamWriter &stream) const;
    void writeGeneral(QXmlStreamWriter &stream) const;
    void writeDatabase(QXmlStreamWriter &stream) const;
    void writeCrs(QXmlStreamWriter &stream) const;
    void writeCameras(QXmlStreamWriter &stream) const;
    void writeCamera(QXmlStreamWriter &stream, int id, const Camera &camera) const;
    void writeCalibration(QXmlStreamWriter &stream, std::shared_ptr<Calibration> calibration) const;
    void writeImages(QXmlStreamWriter &stream) const;
    void writeImage(QXmlStreamWriter &stream, const std::pair<size_t, Image> &image) const;
    void writeCameraPosition(QXmlStreamWriter &stream, const CameraPose &cameraPosition) const;
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
    void writeDtm(QXmlStreamWriter &stream) const;
    void writeOrthophoto(QXmlStreamWriter &stream) const;

    QSize readSize(QXmlStreamReader &stream) const;
    int readInt(QXmlStreamReader &stream) const;
    double readDouble(QXmlStreamReader &stream) const;
    bool readBoolean(QXmlStreamReader &stream) const;

protected:

    QString mName;
    QString mDescription;
    tl::Path mProjectFolder;
    tl::Path mProjectPath;
    QString mVersion;
    tl::Path mDatabase;
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
    tl::Path mOffset;
    tl::Path mGroundPoints;
    OrientationReport mOrientationReport;
    //tl::Path mReconstructionPath;
    std::shared_ptr<Densification> mDensification;
    tl::Path mDenseModel;
    DenseReport mDenseReport;
    std::shared_ptr<PoissonReconProperties> mMeshProperties;
    tl::Path mMeshModel;
    MeshReport mMeshReport;
    //std::shared_ptr<Dtm> mDtmMethod;
    //tl::Path mDTM;
    DTMData mDTM;
    OrthophotoData mOrthophoto;
    static std::mutex sMutex;
    int mCameraCount;
    //tl::Path mOrthophoto;
    tl::Matrix<double, 4, 4> mTransform;
};

} // end namespace graphos

#endif // GRAPHOS_CORE_PROJECT_H
