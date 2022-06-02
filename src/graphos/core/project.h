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

#include "graphos/core/camera/Camera.h"
#include "graphos/core/image.h"
#include "graphos/core/features/features.h"
#include "graphos/core/sfm/poses.h"
#include "graphos/core/dense/dense.h"

class QXmlStreamWriter;
class QXmlStreamReader;

namespace graphos
{

class Dtm;

/*!
 * \brief Interfaz Project
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
  virtual QString projectFolder() const = 0;

  /*!
   * \brief Establece el directorio del proyecto
   * \param[in] path Directorio del proyecto
   */
  virtual void setProjectFolder(const QString &dir) = 0;

  /*!
   * \brief Devuelve el ruta del proyecto
   * \return Ruta del proyecto
   */
  virtual QString projectPath() const = 0;

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
  virtual QString database() const = 0;

  /*!
   * \brief Establece la base de datos
   * \param[in] database Base de datos
   */
  virtual void setDatabase(const QString &database) = 0;

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
  
  virtual QString features(size_t imageId) const = 0;
  virtual void addFeatures(size_t imageId, const QString &featureFile) = 0;
  virtual void removeFeatures() = 0;
  virtual void removeFeatures(size_t imageId) = 0;
  virtual const std::unordered_map<size_t, QString> &features() const  = 0;

  virtual std::shared_ptr<FeatureMatching> featureMatching() const = 0;
  virtual void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching) = 0;
  
  virtual void addMatchesPair(size_t imageLeftId, size_t imageRightId) = 0;
  virtual const std::vector<size_t> matchesPairs(size_t imageLeftId) const = 0;
  virtual void removeMatchesPair() = 0;
  virtual void removeMatchesPair(size_t imageLeftId) = 0;
  
  virtual QString sparseModel() const = 0;
  virtual void setSparseModel(const QString &sparseModel) = 0;
  virtual QString offset() const = 0;
  virtual void setOffset(const QString &offset) = 0;
  virtual QString reconstructionPath() const = 0;
  virtual void setReconstructionPath(const QString &reconstructionPath) = 0;
  virtual bool isPhotoOriented(size_t imageId) const = 0;
  virtual CameraPose photoOrientation(size_t imageId) const = 0;
  virtual const std::unordered_map<size_t, CameraPose> &poses() const = 0;
  virtual void addPhotoOrientation(size_t imageId, const CameraPose &photoOrientation) = 0;
  virtual void clearReconstruction() = 0;
  
  virtual std::shared_ptr<Densification> densification() const = 0;
  virtual void setDensification(const std::shared_ptr<Densification> &densification) = 0;
  
  virtual QString denseModel() const = 0;
  virtual void setDenseModel(const QString &denseModel) = 0;
  virtual void clearDensification() = 0;

  virtual std::shared_ptr<Dtm> dtmMethod() const = 0;
  virtual void setDtmMethod(const std::shared_ptr<Dtm> &dtm) = 0;
  virtual QString dtmPath() const = 0;
  virtual void setDtmPath(const QString &dtmPath) = 0;
  virtual void clearDTM() = 0;

  virtual QString orthophotoPath() const = 0;
  virtual void setOrthophotoPath(const QString &orthophotoPath) = 0;
  virtual void clearOrthophotoDTM() = 0;

  /*!
   * \brief Limpia el proyecto
   */
  virtual void clear() = 0;

  /*!
   * \brief Carga el proyecto
   * \param[in] file Ruta del fichero de proyecto
   * \return
   */
  virtual bool load(const QString &file) = 0;

  /*!
   * \brief Guarda el proyecto
   * \param file
   * \return
   */
  virtual bool save(const QString &file) = 0;

  /*!
   * \brief checkOldVersion
   * \param file
   * \return
   */
  virtual bool checkOldVersion(const QString &file) const = 0;

  /*!
   * \brief Crea una copia de un proyecto antiguo con el mismo nombre y un sufijo con la versión
   * \param[in] file
   */
  virtual void oldVersionBak(const QString &file) const = 0;
};


///*!
// * \brief Interfaz para las operaciones de lectura y escritura del proyecto
// */
//class GRAPHOS_EXPORT ProjectController
//{

//public:

//  ProjectController() {}
//  virtual ~ProjectController() = default;

//  /*!
//   * \brief read
//   * \param[in] file Ruta del fichero de proyecto
//   * \param[out] prj
//   * \return
//   */
//  virtual bool read(const QString &file, Project &prj) = 0;

//  /*!
//   * \brief write
//   * \param prj
//   * \param file
//   * \return
//   */
//  virtual bool write(const QString &file, const Project &prj) const = 0;

//  /*!
//   * \brief checkOldVersion
//   * \param file
//   * \return
//   */
//  virtual bool checkOldVersion(const QString &file) const = 0;

//  /*!
//   * \brief Crea una copia de un proyecto antiguo con el mismo nombre y un sufijo con la versión
//   * \param[in] file
//   */
//  virtual void oldVersionBak(const QString &file) const = 0;

//};


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
  QString projectFolder() const override;
  void setProjectFolder(const QString &dir) override;
  QString projectPath() const override;
  QString version() const override;
  QString database() const override;
  void setDatabase(const QString &database) override;
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
  
  QString features(size_t imageId) const override;
  void addFeatures(size_t imageId, const QString &featureFile) override;
  void removeFeatures() override;
  void removeFeatures(size_t imageId) override;
  const std::unordered_map<size_t, QString> &features() const override;
  
  std::shared_ptr<FeatureMatching> featureMatching() const override;
  void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching) override;
  
  void addMatchesPair(size_t imageLeftId, size_t imageRightId) override;
  const std::vector<size_t> matchesPairs(size_t imageLeftId) const override;
  void removeMatchesPair() override;
  void removeMatchesPair(size_t imageLeftId) override;
  
  QString sparseModel() const override;
  void setSparseModel(const QString & sparseModel) override;
  QString offset() const override;
  void setOffset(const QString &offset) override;
  QString reconstructionPath() const override;
  void setReconstructionPath(const QString &reconstructionPath) override;
  bool isPhotoOriented(size_t imageId) const override;
  CameraPose photoOrientation(size_t imageId) const override;
  const std::unordered_map<size_t, CameraPose> &poses() const override;
  void addPhotoOrientation(size_t imageId, const CameraPose &photoOrientation) override;
  void clearReconstruction() override;
  
  std::shared_ptr<Densification> densification() const override;
  void setDensification(const std::shared_ptr<Densification> &densification) override;
  
  QString denseModel() const override;
  void setDenseModel(const QString &denseModel) override;
  void clearDensification();
  
  std::shared_ptr<Dtm> dtmMethod() const override;
  void setDtmMethod(const std::shared_ptr<Dtm> &dtm) override;
  QString dtmPath() const override;
  void setDtmPath(const QString &dtmPath) override;
  void clearDTM() override;

  QString orthophotoPath() const override;
  void setOrthophotoPath(const QString &orthophotoPath) override;
  void clearOrthophotoDTM() override;

  void clear() override;

  bool load(const QString &file) override;
  bool save(const QString &file) override;
  bool checkOldVersion(const QString &file) const override;
  void oldVersionBak(const QString &file) const override;

protected:

  bool read(QXmlStreamReader &stream);
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
  void readSIFT(QXmlStreamReader &stream);
  void readFeatureFiles(QXmlStreamReader &stream);
  void readFeatureFile(QXmlStreamReader &stream);
  void readMatches(QXmlStreamReader &stream);
  void readMatchingMethod(QXmlStreamReader &stream);
  void readPairs(QXmlStreamReader &stream);
  void readOrientations(QXmlStreamReader &stream);
  void readReconstructionPath(QXmlStreamReader &stream);
  void readOrientationSparseModel(QXmlStreamReader &stream);
  void readOffset(QXmlStreamReader &stream);
  void readPhotoOrientations(QXmlStreamReader &stream);
  void readDensification(QXmlStreamReader &stream);
  void readDenseModel(QXmlStreamReader &stream);
  void readDensificationMethod(QXmlStreamReader &stream);
  void readSmvs(QXmlStreamReader &stream);
  void readCmvsPmvs(QXmlStreamReader &stream);
  void readMVS(QXmlStreamReader &stream);
  void readDtm(QXmlStreamReader &stream);
  void readDtmPath(QXmlStreamReader &stream);
  void readDtmInterpolation(QXmlStreamReader &stream);
  void readInvDist(QXmlStreamReader &stream);
  void readInvDistNN(QXmlStreamReader &stream);
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
  void writeSIFT(QXmlStreamWriter &stream, Sift *sift) const;
  void writeFeatureFiles(QXmlStreamWriter &stream) const;
  void writeMatches(QXmlStreamWriter &stream) const;
  void writeFeatureMatchingMethod(QXmlStreamWriter &stream) const;
  void writePairs(QXmlStreamWriter &stream) const;
  void writeOrientations(QXmlStreamWriter &stream) const;
  void writeReconstructionPath(QXmlStreamWriter &stream) const;
  void writeOrientationSparseModel(QXmlStreamWriter &stream) const;
  void writeOffset(QXmlStreamWriter &stream) const;
  void writePhotoOrientations(QXmlStreamWriter &stream) const;
  void writeDensification(QXmlStreamWriter &stream) const;
  void writeDenseModel(QXmlStreamWriter &stream) const;
  void writeDensificationMethod(QXmlStreamWriter &stream) const;
  void writeDtm(QXmlStreamWriter &stream) const;
  void writeDtmPath(QXmlStreamWriter &stream) const;
  void writeDtmInterpolation(QXmlStreamWriter &stream) const;
  void writeOrthophoto(QXmlStreamWriter &stream) const;

  QSize readSize(QXmlStreamReader &stream) const;
  int readInt(QXmlStreamReader &stream) const;
  double readDouble(QXmlStreamReader &stream) const;
  bool readBoolean(QXmlStreamReader &stream) const;

protected:

  QString mName;
  QString mDescription;
  QString mProjectFolder;
  QString mProjectPath;
  QString mVersion;
  QString mDatabase;
  QString mCrs;

  std::unordered_map<size_t, Image> mImages;
  std::map<int, Camera> mCameras;
  std::shared_ptr<Feature> mFeatureExtractor;
  std::unordered_map<size_t, QString> mFeatures;
  std::shared_ptr<FeatureMatching> mFeatureMatching;
  std::unordered_map<size_t, std::vector<size_t>> mImagesPairs;
  std::unordered_map<size_t, CameraPose> mPhotoOrientation;

  QString mSparseModel;
  QString mOffset;
  QString mReconstructionPath;
  std::shared_ptr<Densification> mDensification;
  QString mDenseModel;
  std::shared_ptr<Dtm> mDtmMethod;
  QString mDTM;
  static std::mutex sMutex;
  int mCameraCount;
  QString mOrthophoto;
};

} // end namespace graphos

#endif // GRAPHOS_CORE_PROJECT_H
