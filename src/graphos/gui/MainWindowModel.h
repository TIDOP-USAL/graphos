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

    QString projectName() const;
    tl::Path projectPath() const;

    const std::unordered_map<size_t, Image> &images() const;
    Image image(size_t imageId) const;
    const std::map<int, Camera> &cameras() const;
    Camera camera(int id) const;
    void deleteImages(const std::vector<size_t> &imageIds);
    QImage readImage(const tl::Path &imageFile);

    const std::unordered_map<size_t, QString> &features() const;
    std::vector<size_t> imagePairs(size_t imageId) const;

    tl::Path sparseModel() const;
    bool isAbsoluteOrientation() const;

    const std::unordered_map<size_t, CameraPose> &poses() const;
    tl::Matrix<double, 4, 4> transform() const;

    tl::Path denseModel() const;
    tl::Path mesh() const;
    tl::Path dtm() const;
    tl::Path dsm() const;
    tl::Path orthophoto() const;

    QString graphicViewerBackgroundColor() const;

    //bool checkUnsavedChanges() const;
    bool checkOldVersion(const tl::Path &file) const;
    void oldVersionBackup(const tl::Path &file) const;

public slots:

    void load(const tl::Path &file);
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
