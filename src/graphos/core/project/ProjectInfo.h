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

#include "graphos/graphos_global.h"

#include <string>

#include <tidop/core/base/Path.h>

namespace graphos
{

constexpr auto project_file_version = "1.0";

class ProjectInfo
{

private:

    tl::Path mProjectPath;
    std::string mDescription;
    std::string mVersion = project_file_version;
	
public: 

    ProjectInfo() = default;
    explicit ProjectInfo(tl::Path projectPath, 
                         std::string description);

    [[nodiscard]]
    auto projectPath() const -> tl::Path;

    void setProjectPath(tl::Path path);
	
    [[nodiscard]]
    auto name() const -> std::string;

    [[nodiscard]]
    auto description() const -> std::string;

    void setDescription(std::string description);

    [[nodiscard]]
    auto projectFolder() const -> tl::Path;

    [[nodiscard]]
    auto database() const -> const tl::Path;

    [[nodiscard]]
    auto version() const -> std::string;

    void clear();

};


} // end namespace graphos
