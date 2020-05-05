#ifndef INSPECTOR_PROJECT_H
#define INSPECTOR_PROJECT_H

#include "inspector/inspector_global.h"

#include <memory>
#include <vector>
#include <mutex>

#include <QString>
#include <QSize>

#include "inspector/core/image.h"

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

  typedef std::vector<std::shared_ptr<Image>>::iterator image_iterator;
  typedef std::vector<std::shared_ptr<Image>>::const_iterator image_const_iterator;

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

  /*!
   * \brief Añade una imagen al proyecto
   * \param[in] img Imagen
   */
  virtual void addImage(const std::shared_ptr<Image> &img) = 0;

  /*!
   * \brief Elimina una imagen del proyecto
   * \param[in] img Ruta de la imagen que se quiere eliminar
   */
  virtual void deleteImage(const QString &img) = 0;

  /*!
   * \brief Elimina una imagen del proyecto
   * \param[in] imgId Identificador de la imagen que se quiere eliminar
   */
  virtual void deleteImage(size_t imgId) = 0;

  /*!
   * \brief Busca una imagen en el proyecto
   * \param[in] path Ruta de la imagen a buscar
   * \return Objeto Image correspondiente o puntero nulo si no se encuentra
   */
  virtual std::shared_ptr<Image> findImage(const QString &path) = 0;
  virtual const std::shared_ptr<Image> findImage(const QString &path) const = 0;

  virtual std::shared_ptr<Image> findImageById(size_t id) = 0;
  virtual const std::shared_ptr<Image> findImageById(size_t id) const = 0;
  virtual std::shared_ptr<Image> findImageByName(const QString &imgName) = 0;
  virtual const std::shared_ptr<Image> findImageByName(const QString &imgName) const = 0;

  /*!
   * \brief Busca una imagen en el proyecto
   * \param[in] path Ruta de la imagen a buscar
   * \return Objeto Identificador de la imagen o std::numeric_limits<size_t>().max() si no se encuentra.
   */
  virtual size_t findImageId(const QString &path) const = 0;


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
   * \brief Limpia el proyecto
   */
  virtual void clear() = 0;


};


/*!
 * \brief Interfaz para las operaciones de lectura y escritura del proyecto
 */
class INSPECTOR_EXPORT ProjectController
{

public:

  ProjectController() {}
  virtual ~ProjectController() = default;

  /*!
   * \brief read
   * \param[in] file Ruta del fichero de proyecto
   * \param[out] prj
   * \return
   */
  virtual bool read(const QString &file, Project &prj) = 0;

  /*!
   * \brief write
   * \param prj
   * \param file
   * \return
   */
  virtual bool write(const QString &file, const Project &prj) const = 0;

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
  void addImage(const std::shared_ptr<Image> &img) override;
  void deleteImage(const QString &img) override;
  void deleteImage(size_t imgId) override;
  std::shared_ptr<Image> findImage(const QString &path) override;
  const std::shared_ptr<Image> findImage(const QString &path) const override;
  std::shared_ptr<Image> findImageById(size_t id) override;
  const std::shared_ptr<Image> findImageById(size_t id) const override;
  std::shared_ptr<Image> findImageByName(const QString &imgName) override;
  const std::shared_ptr<Image> findImageByName(const QString &imgName) const override;
  size_t findImageId(const QString &path) const override;
  image_iterator imageBegin() override;
  image_const_iterator imageBegin() const override;
  image_iterator imageEnd() override;
  image_const_iterator imageEnd() const override;
  size_t imagesCount() const override;
  void clear() override;

protected:

  QString mName;
  QString mDescription;
  QString mPath;
  QString mProjectFolder;
  QString mVersion;
  QString mDatabase;
  std::vector<std::shared_ptr<Image>> mImages;

};


class INSPECTOR_EXPORT ProjectControllerImp
  : public ProjectController
{

public:

  ProjectControllerImp();

// ProjectControllerImp interface

public:

  bool read(const QString &file, Project &prj) override;
  bool write(const QString &file, const Project &prj) const override;
  bool checkOldVersion(const QString &file) const override;
  void oldVersionBak(const QString &file) const override;

protected:

  void readGeneral(QXmlStreamReader &stream, Project &prj);
  void readDatabase(QXmlStreamReader &stream, Project &prj);
  void readImages(QXmlStreamReader &stream, Project &prj);
  std::shared_ptr<Image> readImage(QXmlStreamReader &stream);

  void writeVersion(QXmlStreamWriter &stream, const QString &version) const;
  void writeGeneral(QXmlStreamWriter &stream, const Project &prj) const;
  void writeDatabase(QXmlStreamWriter &stream, const Project &prj) const;
  void writeImages(QXmlStreamWriter &stream, const Project &prj) const;
  void writeImage(QXmlStreamWriter &stream, const Image *image) const;

  QSize readSize(QXmlStreamReader &stream) const;
  int readInt(QXmlStreamReader &stream) const;
  double readDouble(QXmlStreamReader &stream) const;
  bool readBoolean(QXmlStreamReader &stream) const;

protected:

  static std::mutex sMutex;

};


} // end namespace inspector

#endif // INSPECTOR_PROJECT_H
