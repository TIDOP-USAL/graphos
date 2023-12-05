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

#ifndef GRAPHOS_LOADER_MODEL_H
#define GRAPHOS_LOADER_MODEL_H

#include "graphos/components/images/ImageLoaderModel.h"

namespace graphos
{

class Project;

class ImageLoaderModelImp
  : public ImageLoaderModel
{

    Q_OBJECT

public:

    ImageLoaderModelImp(Project *project,
                        QObject *parent = nullptr);
    ~ImageLoaderModelImp() override = default;

// ImagesModel interface

public:

    QString projectCRS() const override;
    void setProjectCRS(const QString &crs) override;
    void addImage(const Image &image) override;
    bool existImage(size_t imageId) const;
    tl::Path imagesDirectory() const override;
    const std::map<int, Camera> &cameras() const override;
    int addCamera(const Camera &camera) override;
    int cameraID(const Camera &camera) const override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

protected:

    Project *mProject;
};

} // namespace graphos

#endif // GRAPHOSS_MODEL_H
