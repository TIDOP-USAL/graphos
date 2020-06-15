#ifndef INSPECTOR_PROJECT_H
#define INSPECTOR_PROJECT_H

#include "inspector/inspector_global.h"

#include <memory>
#include <vector>
#include <mutex>
#include <map>

#include <QString>
#include <QSize>

#include "inspector/core/image.h"
#include "inspector/core/camera.h"
#include "inspector/core/features/features.h"
#include "inspector/core/orientation/photoorientation.h"
#include "inspector/core/densification/densification.h"

class QXmlStreamWriter;
class QXmlStreamReader;

namespace inspector
{

/*!
 * \brief Interfaz Project
 */
class Project
{

public:

  typedef std::vector<Image>::iterator image_iterator;
  typedef std::vector<Image>::const_iterator image_const_iterator;
  typedef std::map<int, Camera>::iterator camera_iterator;
  typedef std::map<int, Camera>::const_iterator camera_const_iterator;
  typedef std::map<QString, QString>::iterator features_iterator;
  typedef std::map<QString, QString>::const_iterator features_const_iterator;

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

  virtual QString imageDirectory() const = 0;
  virtual void setImageDirectory(const QString &imageDirectory) = 0;

  /*!
   * \brief Añade una imagen al proyecto
   * \param[in] img Imagen
   */
  virtual void addImage(const Image &img) = 0;

  virtual bool updateImage(size_t imageId, const Image &image) = 0;

  /*!
   * \brief Elimina una imagen del proyecto
   * \param[in] imgPath Ruta de la imagen que se quiere eliminar
   */
  virtual bool removeImage(const QString &imgPath) = 0;

  /*!
   * \brief Elimina una imagen del proyecto
   * \param[in] imgId Identificador de la imagen que se quiere eliminar
   */
  virtual bool removeImage(size_t imgId) = 0;

  /*!
   * \brief Busca una imagen en el proyecto
   * \param[in] path Ruta de la imagen a buscar
   * \return Objeto Image correspondiente o puntero nulo si no se encuentra
   */
  virtual Image findImage(const QString &path) const = 0;

  virtual Image findImageById(size_t id) const = 0;
  virtual Image findImageByName(const QString &imgName) const = 0;

  /*!
   * \brief Busca una imagen en el proyecto
   * \param[in] imgName Nombre de la imagen a buscar
   * \return Objeto Identificador de la imagen o std::numeric_limits<size_t>().max() si no se encuentra.
   */
  virtual size_t imageId(const QString &imgName) const = 0;

  virtual bool existImage(const QString &imgName) const = 0;

  /// Iteradores para el acceso a las imágenes y las cámaras

  /*!
   * \brief Devuelve un iterador al inicio del listado de imágenes
   * \return Iterador al primer elemento del listado de imágenes
   */
  virtual image_iterator imageBegin() = 0;

  /*!
   * \brief Devuelve un iterador constante al inicio del listado de imágenes
   * \return Iterador al primer elemento del listado de imágenes
   */
  virtual image_const_iterator imageBegin() const = 0;

  /*!
   * \brief Devuelve un iterador al siguiente elemento después después de la última imagen
   * Este elemento actúa como un marcador de posición, intentar acceder a él resulta en un comportamiento no definido
   * \return Iterador al siguiente elemento después de la última imagen
   */
  virtual image_iterator imageEnd() = 0;

  /*!
   * \brief Devuelve un iterador constante al siguiente elemento después de la última imagen
   * Este elemento actúa como un marcador de posición, intentar acceder a él resulta en un comportamiento no definido
   * \return Iterador constante al siguiente elemento después de la última imagen
   */
  virtual image_const_iterator imageEnd() const = 0;

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

  /*!
   * \brief Devuelve un iterador al inicio del listado de cámaras
   * \return Iterador al primer elemento del listado de cámaras
   */
  virtual camera_iterator cameraBegin() = 0;

  /*!
   * \brief Devuelve un iterador constante al inicio del listado de cámaras
   * \return Iterador al primer elemento del listado de cámaras
   */
  virtual camera_const_iterator cameraBegin() const = 0;

  /*!
   * \brief Devuelve un iterador al siguiente elemento después después de la última cámara
   * Este elemento actúa como un marcador de posición, intentar acceder a él resulta en un comportamiento no definido
   * \return Iterador al siguiente elemento después de la última cámara
   */
  virtual camera_iterator cameraEnd() = 0;

  /*!
   * \brief Devuelve un iterador constante al siguiente elemento después de la última cámara
   * Este elemento actúa como un marcador de posición, intentar acceder a él resulta en un comportamiento no definido
   * \return Iterador constante al siguiente elemento después de la última cámara
   */
  virtual camera_const_iterator cameraEnd() const = 0;

  virtual std::shared_ptr<Feature> featureExtractor() const = 0;
  virtual void setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor) = 0;

  virtual QString features(const QString &imgName) const = 0;
  virtual void addFeatures(const QString &imgName, const QString &featureFile) = 0;
  virtual void removeFeatures() = 0;
  virtual bool removeFeatures(const QString &imgName) = 0;

  virtual features_iterator featuresBegin() = 0;
  virtual features_const_iterator featuresBegin() const = 0;
  virtual features_iterator featuresEnd() = 0;
  virtual features_const_iterator featuresEnd() const = 0;

  virtual std::shared_ptr<FeatureMatching> featureMatching() const = 0;
  virtual void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching) = 0;

  virtual void addMatchesPair(const QString &imageLeft, const QString &imageRight) = 0;
  virtual const std::vector<QString> matchesPairs(const QString &imageLeft) const = 0;
  virtual void removeMatchesPair() = 0;
  virtual void removeMatchesPair(const QString &imageLeft) = 0;

  virtual bool refinePrincipalPoint() const = 0;
  virtual void setRefinePrincipalPoint(bool refine) = 0;

  virtual QString sparseModel() const = 0;
  virtual void setSparseModel(const QString &sparseModel) = 0;

  virtual PhotoOrientation photoOrientation(const QString &imgName) const = 0;
  virtual void addPhotoOrientation(const QString &imgName, const PhotoOrientation &photoOrientation) = 0;

  virtual std::shared_ptr<Densification> densification() const = 0;
  virtual void setDensification(const std::shared_ptr<Densification> &densification) = 0;

  virtual QString denseModel() const = 0;
  virtual void setDenseModel(const QString &denseModel) = 0;

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
//class INSPECTOR_EXPORT ProjectController
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


class INSPECTOR_EXPORT ProjectImp
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
  QString version() const override;
  QString database() const override;
  void setDatabase(const QString &database) override;
  QString imageDirectory() const override;
  void setImageDirectory(const QString &imageDirectory) override;
  void addImage(const Image &img) override;
  bool updateImage(size_t imageId, const Image &image) override;
  bool removeImage(const QString &imgPath) override;
  bool removeImage(size_t imgId) override;
  Image findImage(const QString &imgName) const override;
  Image findImageById(size_t id) const override;
  Image findImageByName(const QString &imgName) const override;
  bool existImage(const QString &imgName) const override;
  size_t imageId(const QString &imageName) const override;
  image_iterator imageBegin() override;
  image_const_iterator imageBegin() const override;
  image_iterator imageEnd() override;
  image_const_iterator imageEnd() const override;
  size_t imagesCount() const override;

  int addCamera(const Camera &camera) override;
  Camera findCamera(const QString &make, const QString &model) const override;
  Camera findCamera(int idCamera) const override;
  int cameraId(const QString &make, const QString &model) const override;
  bool existCamera(const QString &make, const QString &model) const override;
  bool updateCamera(int idCamera, const Camera &camera) override;
  bool removeCamera(int idCamera) override;
  camera_iterator cameraBegin() override;
  camera_const_iterator cameraBegin() const override;
  camera_iterator cameraEnd() override;
  camera_const_iterator cameraEnd() const override;

  std::shared_ptr<Feature> featureExtractor() const override;
  void setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor) override;

  QString features(const QString &imgName) const override;
  void addFeatures(const QString &imgName, const QString &featureFile) override;
  void removeFeatures() override;
  bool removeFeatures(const QString &imgName) override;

  features_iterator featuresBegin() override;
  features_const_iterator featuresBegin() const override;
  features_iterator featuresEnd() override;
  features_const_iterator featuresEnd() const override;

  std::shared_ptr<FeatureMatching> featureMatching() const override;
  void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching) override;

  void addMatchesPair(const QString &imageLeft, const QString &imageRight) override;
  const std::vector<QString> matchesPairs(const QString &imageLeft) const override;
  void removeMatchesPair() override;
  void removeMatchesPair(const QString &imageLeft) override;

  bool refinePrincipalPoint() const override;
  void setRefinePrincipalPoint(bool refine) override;

  QString sparseModel() const override;
  void setSparseModel(const QString & sparseModel) override;
  PhotoOrientation photoOrientation(const QString &imgName) const override;
  void addPhotoOrientation(const QString &imgName, const PhotoOrientation &photoOrientation) override;

  std::shared_ptr<Densification> densification() const override;
  void setDensification(const std::shared_ptr<Densification> &densification) override;

  QString denseModel() const override;
  void setDenseModel(const QString &denseModel) override;

  void clear() override;

  bool load(const QString &file) override;
  bool save(const QString &file) override;
  bool checkOldVersion(const QString &file) const override;
  void oldVersionBak(const QString &file) const override;

protected:

  bool read(QXmlStreamReader &stream);
  void readGeneral(QXmlStreamReader &stream);
  void readDatabase(QXmlStreamReader &stream);
  void readImages(QXmlStreamReader &stream);
  Image readImage(QXmlStreamReader &stream);
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
  void readOrientationSparseModel(QXmlStreamReader &stream);
  void readPhotoOrientations(QXmlStreamReader &stream);
  void readDensification(QXmlStreamReader &stream);
  void readDenseModel(QXmlStreamReader &stream);
  void readDensificationMethod(QXmlStreamReader &stream);
  void readSmvs(QXmlStreamReader &stream);
  void readCmvsPmvs(QXmlStreamReader &stream);

  void writeVersion(QXmlStreamWriter &stream) const;
  void writeGeneral(QXmlStreamWriter &stream) const;
  void writeDatabase(QXmlStreamWriter &stream) const;
  void writeCameras(QXmlStreamWriter &stream) const;
  void writeCamera(QXmlStreamWriter &stream, int id, const Camera &camera) const;
  void writeCalibration(QXmlStreamWriter &stream, std::shared_ptr<Calibration> calibration) const;
  void writeImages(QXmlStreamWriter &stream) const;
  void writeImage(QXmlStreamWriter &stream, const Image &image) const;
  void writeFeatures(QXmlStreamWriter &stream) const;
  void writeFeatureExtractor(QXmlStreamWriter &stream) const;
  void writeSIFT(QXmlStreamWriter &stream, Sift *sift) const;
  void writeFeatureFiles(QXmlStreamWriter &stream) const;
  void writeFeatureFile(QXmlStreamWriter &stream) const;
  void writeMatches(QXmlStreamWriter &stream) const;
  void writeFeatureMatchingMethod(QXmlStreamWriter &stream) const;
  void writePairs(QXmlStreamWriter &stream) const;
  void writeOrientations(QXmlStreamWriter &stream) const;
  void writeOrientationSparseModel(QXmlStreamWriter &stream) const;
  void writePhotoOrientations(QXmlStreamWriter &stream) const;
  void writeDensification(QXmlStreamWriter &stream) const;
  void writeDenseModel(QXmlStreamWriter &stream) const;
  void writeDensificationMethod(QXmlStreamWriter &stream) const;

  QSize readSize(QXmlStreamReader &stream) const;
  int readInt(QXmlStreamReader &stream) const;
  double readDouble(QXmlStreamReader &stream) const;
  bool readBoolean(QXmlStreamReader &stream) const;

protected:

  QString mName;
  QString mDescription;
  QString mPath;
  QString mProjectFolder;
  QString mVersion;
  QString mDatabase;
  QString mImagesDirectory;
  std::vector<Image> mImages;
  std::map<int, Camera> mCameras;
  std::shared_ptr<Feature> mFeatureExtractor;
  std::map<QString, QString> mFeatures;
  std::shared_ptr<FeatureMatching> mFeatureMatching;
  std::map<QString, std::vector<QString>> mImagesPairs;
  std::map<QString, PhotoOrientation> mPhotoOrientation;
  bool bRefinePrincipalPoint;
  QString mSparseModel;
  std::shared_ptr<Densification> mDensification;
  QString mDenseModel;
  static std::mutex sMutex;
  static int sCameraCount;
};


//class INSPECTOR_EXPORT ProjectControllerImp
//  : public ProjectController
//{

//public:

//  ProjectControllerImp();

//// ProjectControllerImp interface

//public:

//  bool read(const QString &file, Project &prj) override;
//  bool write(const QString &file, const Project &prj) const override;
//  bool checkOldVersion(const QString &file) const override;
//  void oldVersionBak(const QString &file) const override;

//protected:

//  void readGeneral(QXmlStreamReader &stream, Project &prj);
//  void readDatabase(QXmlStreamReader &stream, Project &prj);
//  void readImages(QXmlStreamReader &stream, Project &prj);
//  Image readImage(QXmlStreamReader &stream);
//  void readCameras(QXmlStreamReader &stream, Project &prj);
//  std::shared_ptr<Camera> readCamera(QXmlStreamReader &stream);

//  void writeVersion(QXmlStreamWriter &stream, const QString &version) const;
//  void writeGeneral(QXmlStreamWriter &stream, const Project &prj) const;
//  void writeDatabase(QXmlStreamWriter &stream, const Project &prj) const;
//  void writeCameras(QXmlStreamWriter &stream, const Project &prj) const;
//  void writeCamera(QXmlStreamWriter &stream, int id, const Camera *camera) const;
//  void writeImages(QXmlStreamWriter &stream, const Project &prj) const;
//  void writeImage(QXmlStreamWriter &stream, const Image *image) const;

//  QSize readSize(QXmlStreamReader &stream) const;
//  int readInt(QXmlStreamReader &stream) const;
//  double readDouble(QXmlStreamReader &stream) const;
//  bool readBoolean(QXmlStreamReader &stream) const;

//protected:

//  static std::mutex sMutex;

//};


} // end namespace inspector

#endif // INSPECTOR_PROJECT_H
