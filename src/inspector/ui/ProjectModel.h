#ifndef INSPECTOR_PROJECT_MODEL_H
#define INSPECTOR_PROJECT_MODEL_H

#include <QObject>

#include "inspector/core/project.h"
#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class ProjectModel
  : public IModel
{

  Q_OBJECT

public:

  ProjectModel(QObject *parent = nullptr) : IModel(parent) {}
  ~ProjectModel() override = default;

  /*!
   * \brief Returns the name of the project
   * \return Project name
   */
  virtual QString projectName() const = 0;

  /*!
   * \brief Returns the project description
   * \return Project description
   */
  virtual QString projectDescription() const = 0;

  /*!
   * \brief Returns the project directory
   * \return Project directory
   */
  virtual QString projectFolder() const = 0;

  /*!
   * \brief Returns the project file path
   * \return Project directory
   */
  virtual QString projectPath() const = 0;

  /*!
   * \brief Project File Version
   * The project file version is used to update old projects
   * \return Project File Version
   */
  virtual QString version() const = 0;

  /*!
   * \brief Check for changes without saving in the project
   * \return
   */
  virtual bool checkUnsavedChanges() const = 0;

  /*!
   * \brief Check if a project is an old version
   * \param[in] file Project file
   * \return
   */
  virtual bool checkOldVersion(const QString &file) const = 0;

//  /*!
//   * \brief Add an image to the project
//   * \param[in] img Image
//   */
//  virtual void addImage(const Image &img) = 0;

//  /*!
//   * \brief Delete an image from the project
//   * \param[in] img Image path to be deleted
//   */
//  virtual void deleteImage(const QString &img) = 0;

//  /*!
//   * \brief Delete an image from the project
//   * \param[in] imgId ID image to be deleted
//   */
//  virtual void deleteImage(size_t imgId) = 0;

//  /*!
//   * \brief Search for an image in the project
//   * \param[in] path Image path
//   * \return Image object or null pointer if not found
//   */
//  virtual const Image findImage(const QString &path) const = 0;

//  virtual const Image findImageById(size_t id) const = 0;
//  virtual const Image findImageByName(const QString &imgName) const = 0;

//  /*!
//   * \brief Search for an image in the project
//   * \param[in] path Image path
//   * \return Image ID o std::numeric_limits<size_t>().max() if it is not found
//   */
//  virtual size_t findImageId(const QString &path) const = 0;

//  /*!
//   * \brief Returns a constant iterator at the beginning of the image list
//   * \return Iterator to the first element of the image list
//   */
//  virtual Project::image_const_iterator imageBegin() const = 0;

//  /*!
//   * \brief Return a constant iterator to the next element after the last image
//   * \return Constant Iterator to the next element after the last image
//   */
//  virtual Project::image_const_iterator imageEnd() const = 0;

//  /*!
//   * \brief Number of images of the project
//   * \return
//   */
//  virtual size_t imagesCount() const = 0;

//  /*!
//   * \brief Add images
//   * \param[in] fileNames Image list to be added
//   */
//  virtual void addImages(const QStringList &fileNames) = 0;

//  /*!
//   * \brief Delete images
//   * \param[in] images Image list to be deleted
//   */
//  virtual void deleteImages(const QStringList &images) = 0;

  virtual QString sparseModel() const = 0;
  virtual bool isAbsoluteOriented() const = 0;
  virtual QString denseModel() const = 0;

public slots:

  /*!
   * \brief Set the project name
   * \param[in] name Project's name
   */
  virtual void setProjectName(const QString &name) = 0;

  /*!
   * \brief Set the project description
   * \param[in] description Project description
   */
  virtual void setProjectDescription(const QString &projectDescription) = 0;

  /*!
   * \brief Set the project directory
   * \param[in] path Project Directory
   */
  virtual void setProjectFolder(const QString &dir) = 0;

  virtual void setDatabase(const QString &database) = 0;

  virtual void setSparseModel(const QString &sparseModel) = 0;
  virtual void setDenseModel(const QString &denseModel) = 0;

  /*!
   * \brief Load a project
   * \param[in] file Project file
   */
  virtual void load(const QString &file) = 0;

  /*!
   * \brief Save the project
   */
  virtual void save() = 0;

  /*!
   * \brief Save a project as ...
   * \param[in] file Project file
   */
  virtual void saveAs(const QString &file) = 0;

  /*!
   * \brief Create a copy of a project from an old version with the same name and a suffix with the version
   * \param[in] file Project file
   */
  virtual void oldVersionBackup(const QString &file) const = 0;

};



class ProjectModelImp
  : public ProjectModel
{

  Q_OBJECT

public:

  explicit ProjectModelImp(Project *project,
                           QObject *parent = nullptr);
  ~ProjectModelImp() override;

signals:



// IProject interface

public:

  QString projectName() const override;
  QString projectDescription() const override;
  QString projectFolder() const override;
  QString projectPath() const override;
  QString version() const override;
  bool checkUnsavedChanges() const override;
  bool checkOldVersion(const QString &file) const override;
  void oldVersionBackup(const QString &file) const override;
//  void addImage(const Image &img) override;
//  void deleteImage(const QString &img) override;
//  void deleteImage(size_t imgId) override;
//  const Image findImage(const QString &path) const override;
//  const Image findImageById(size_t id) const override;
//  const Image findImageByName(const QString &imgName) const override;
//  size_t findImageId(const QString &path) const override;
//  Project::image_const_iterator imageBegin() const override;
//  Project::image_const_iterator imageEnd() const override;
//  size_t imagesCount() const override;
  //void addImages(const QStringList &fileNames) override;
  //void deleteImages(const QStringList &images) override;
  QString sparseModel() const override;
  bool isAbsoluteOriented() const override;
  QString denseModel() const override;

public slots:

  void setProjectName(const QString &name) override;
  void setProjectDescription(const QString &projectDescription) override;
  void setProjectFolder(const QString &dir) override;
  void setDatabase(const QString &database) override;
  void setSparseModel(const QString &sparseModel) override;
  void setDenseModel(const QString &denseModel) override;
  void load(const QString &file) override;
  void save() override;
  void saveAs(const QString &file) override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
  //QString mPrjFile;
  bool bUnsavedChanges;

};

} // namespace ui

} // namespace photomatch

#endif // PHOTOMATCH_PROJECT_MODEL_H

