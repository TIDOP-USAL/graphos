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
#include "graphos/core/camera/Camera.h"
#include "graphos/core/sfm/poses.h"

#include <QImage>

class QSettings;

namespace graphos
{

class MainWindowModel
  : public Model
{
    Q_OBJECT

public:

    explicit MainWindowModel(Project *project);
    ~MainWindowModel();

    auto projectName() const -> QString;
    auto projectPath() const -> tl::Path;

    auto images() const -> const std::unordered_map<size_t, Image> &;
    auto image(size_t imageId) const -> Image;
    auto cameras() const -> const std::map<int, Camera> &;
    auto camera(int id) const -> Camera;
    void deleteImages(const std::vector<size_t> &imageIds);
    auto readImage(const tl::Path &imageFile) -> QImage;

    auto features() const -> const std::unordered_map<size_t, QString> &;
    auto imagePairs(size_t imageId) const -> std::vector<size_t>;

    auto sparseModel() const -> tl::Path;
    auto isAbsoluteOrientation() const -> bool;

    auto poses() const -> const std::unordered_map<size_t, CameraPose>&;
    auto transform() const -> tl::Matrix<double, 4, 4>;

    auto denseModel() const -> tl::Path;
    auto mesh() const -> tl::Path;
    auto dtm() const -> tl::Path;
    auto dsm() const -> tl::Path;
    auto orthophotos() const -> const std::map<size_t, OrthophotoData> &;

    auto graphicViewerBackgroundColor() const -> QString;

    //bool checkUnsavedChanges() const;
    auto checkOldVersion(const tl::Path &file) const -> bool;
    void oldVersionBackup(const tl::Path &file) const;

public slots:

    void load(const tl::Path &file) const;
    //void save();
    //void saveAs(const tl::Path &file);

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

protected:

    Project *mProject;
    QSettings *mSettings;
    //bool bUnsavedChanges;
};

} // namespace graphos

#endif // GRAPHOS_MAINWINDOW_MODEL_H
