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

#ifndef GRAPHOS_MAINWINDOW_MODEL_H
#define GRAPHOS_MAINWINDOW_MODEL_H

#include "graphos/interfaces/mvp.h"
#include "graphos/core/project.h"
#include "graphos/core/image.h"
#include "graphos/core/sfm/poses.h"

namespace graphos
{

class MainWindowModel
  : public Model
{
  Q_OBJECT

public:

  explicit MainWindowModel(Project *project);

  QString projectName() const;
  QString projectPath() const;
  bool checkUnsavedChanges() const;
  bool checkOldVersion(const QString &file) const;
  void oldVersionBackup(const QString &file) const;

  const std::unordered_map<size_t, Image> &images() const;
  const Image &image(size_t imageId) const;
  void deleteImages(const std::vector<size_t> &imageIds);

  const std::unordered_map<size_t, QString> &features() const;
  std::vector<size_t> imagePairs(size_t imageId) const;

  QString sparseModel() const;
  bool isAbsoluteOrientation() const;

  std::list<std::pair<QString, QString>> exif(const QString &image) const;

  /*!
   * \brief Devuelve la ruta por defecto donde se situan los proyectos
   * \return
   */
  //QString defaultPath() const;

  bool isPhotoOriented(size_t imageId) const;
  CameraPose cameraPose(size_t imageId) const;

  QString denseModel() const;

  //Image findImageByName(const QString &imageName) const;
  //size_t imageID(const QString &imageName) const;
  ////bool removeImage(size_t id);
  //bool removeImage(const QString &imageName);
  //void removeImages(const QStringList &images);
  //image_iterator imageBegin();
  //image_const_iterator imageBegin() const;
  //image_iterator imageEnd();
  //image_const_iterator imageEnd() const;


signals:

public slots:

  void load(const QString &file);
  void save();
  void saveAs(const QString &file);

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  //QString mPrjDefaultPath;
  Project *mProject;
  bool bUnsavedChanges;
};

} // namespace graphos

#endif // GRAPHOS_MAINWINDOW_MODEL_H
