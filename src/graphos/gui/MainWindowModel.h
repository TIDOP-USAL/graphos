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

#include "graphos/core/mvp.h"
#include "graphos/core/project.h"
#include "graphos/core/image.h"
#include "graphos/core/sfm/poses.h"

#include <QImage>

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

  const std::unordered_map<size_t, Image> &images() const;
  Image image(size_t imageId) const;
  void deleteImages(const std::vector<size_t> &imageIds);
  QImage readImage(size_t imageId);

  const std::unordered_map<size_t, QString> &features() const;
  std::vector<size_t> imagePairs(size_t imageId) const;

  QString sparseModel() const;
  bool isAbsoluteOrientation() const;

  std::list<std::pair<QString, QString>> exif(const QString &image) const;
  
  const std::unordered_map<size_t, CameraPose> &poses() const;

  QString denseModel() const;

  bool checkUnsavedChanges() const;
  bool checkOldVersion(const QString &file) const;
  void oldVersionBackup(const QString &file) const;

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

  Project *mProject;
  bool bUnsavedChanges;
};

} // namespace graphos

#endif // GRAPHOS_MAINWINDOW_MODEL_H
