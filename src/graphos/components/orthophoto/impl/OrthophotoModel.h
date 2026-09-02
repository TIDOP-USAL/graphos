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
 
#pragma once

#include "graphos/components/orthophoto/OrthophotoModel.h"
#include "graphos/core/project/Project.h"

class QSettings;

namespace graphos
{

class OrthophotoModelImp
  : public OrthophotoModel
{

    Q_OBJECT

public:

    OrthophotoModelImp(Project *project, QObject *parent = nullptr);
    ~OrthophotoModelImp() override;

// OrthophotoModel interface

public:

    auto properties() const -> std::shared_ptr<OrthophotoProperties> override;
    auto images() const -> const ImageRepository & override;
    auto cameras() const -> const CameraRepository & override;
    auto poses() const -> const CameraPosesRepository & override;
    auto projectFolder() const -> tl::Path override;
    auto dtmPath() const -> tl::Path override;
    auto enuCrs() const -> std::string override;
    auto crs() const -> std::string override;
    void clearProject() override;
    auto useCuda() const -> bool override;
    //void addOrthophoto(const OrthophotoData &ortho_data) override;

    void setProperties(std::shared_ptr<OrthophotoProperties> properties) override;
    void setOrthoReport(OrthophotoReport report) override;
    void setOrthophoto(tl::Path ortho) override;

    void loadSettings() override;
    void saveSettings() override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

protected:

    Project *mProject;
    QSettings *mSettings;
    bool mReadSettings;
};

} // namespace graphos
