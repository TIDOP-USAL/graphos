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

#ifndef GRAPHOS_ORTHOPHOTO_MODEL_INTERFACE_H
#define GRAPHOS_ORTHOPHOTO_MODEL_INTERFACE_H

#include <tidop/core/path.h>

#include "graphos/core/mvp.h"

namespace graphos
{

class Image;
class Camera;

class OrthophotoModel
  : public Model
{

    Q_OBJECT

public:

    using Images = std::vector<Image>;
    using Cameras = std::map<int, Camera>;

public:

    OrthophotoModel(QObject *parent = nullptr) : Model(parent){}
    ~OrthophotoModel() override = default;

    virtual auto images() const -> Images = 0;
    virtual auto cameras() const -> Cameras = 0;
    virtual auto projectFolder() const -> tl::Path = 0;
    virtual auto orthoPath() const -> tl::Path = 0;
    virtual void setOrthoPath(const tl::Path &orthoPath) = 0;
    virtual auto dtmPath() const -> tl::Path = 0;
    virtual auto epsCode() const -> QString = 0;
    virtual void clearProject() = 0;
    virtual auto useCuda() const -> bool = 0;
    virtual auto gsd() const -> double = 0;

public slots:

    virtual void setGSD(double gsd) = 0;

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
};

} // namespace graphos


#endif // GRAPHOS_ORTHOPHOTO_MODEL_INTERFACE_H
