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

#include "graphos/core/project/ProjectInfo.h"


namespace graphos
{

ProjectInfo::ProjectInfo(tl::Path projectPath, std::string description)
  : mProjectPath(std::move(projectPath)),
    mDescription(std::move(description))
{
    mProjectPath.normalize();
}

auto ProjectInfo::projectPath() const -> tl::Path
{
    return mProjectPath;
}

void ProjectInfo::setProjectPath(tl::Path path)
{ 
    mProjectPath = std::move(path);
    mProjectPath.normalize();
}

auto ProjectInfo::name() const -> std::string
{
    return mProjectPath.baseName().toString();
}

auto ProjectInfo::description() const -> std::string
{
    return mDescription;
}

void ProjectInfo::setDescription(std::string description)
{
    mDescription = std::move(description);
}

[[nodiscard]]
auto ProjectInfo::enuCrs() const -> std::string
{
    return mEnuCrs;
}

void ProjectInfo::setEnuCrs(std::string enuCrs)
{
    mEnuCrs = enuCrs;
}

auto ProjectInfo::projectFolder() const -> tl::Path
{
    return mProjectPath.parentPath();
}

auto ProjectInfo::database() const -> const tl::Path
{
    return tl::Path(mProjectPath).replaceExtension(".db");
}

auto ProjectInfo::version() const -> std::string
{
    return mVersion;
}

void ProjectInfo::clear()
{
    mProjectPath.clear();
    mDescription = "";
}

} // end namespace graphos
