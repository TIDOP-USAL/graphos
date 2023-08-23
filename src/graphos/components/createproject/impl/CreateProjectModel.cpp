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

#include "CreateProjectModel.h"

#include "graphos/core/project.h"

namespace graphos
{


CreateProjectModelImp::CreateProjectModelImp(Project *project,
                                             QObject *parent)
  : CreateProjectModel(parent),
    mProject(project),
    mPrjFile("")
{
    init();
}

CreateProjectModelImp::~CreateProjectModelImp()
{

}

QString CreateProjectModelImp::projectName() const
{
    return mProject->name();
}

QString CreateProjectModelImp::projectDescription() const
{
    return mProject->description();
}

tl::Path CreateProjectModelImp::projectFolder() const
{
    return mProject->projectFolder();
}

tl::Path CreateProjectModelImp::projectPath() const
{
    return mPrjFile;
}

void CreateProjectModelImp::setProjectName(const QString &name)
{
    mProject->setName(name);
}

void CreateProjectModelImp::setProjectDescription(const QString &description)
{
    mProject->setDescription(description);
}

void CreateProjectModelImp::setProjectFolder(const tl::Path &folder)
{
    mProject->setProjectFolder(folder);
}

void CreateProjectModelImp::setDatabase(const tl::Path &database)
{
    mProject->setDatabase(database);
}

void CreateProjectModelImp::saveAs(const tl::Path &file)
{
    mPrjFile = file;
    mProject->save(file);
}

void CreateProjectModelImp::init()
{

}

void CreateProjectModelImp::clear()
{
    mProject->clear();
    mPrjFile.clear();
}

} // namespace graphos
