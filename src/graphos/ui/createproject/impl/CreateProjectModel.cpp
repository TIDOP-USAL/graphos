/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
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
    mPrjFile(""),
    bUnsavedChanges(false)
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

QString CreateProjectModelImp::projectFolder() const
{
  return mProject->projectFolder();
}

QString CreateProjectModelImp::projectPath() const
{
  return mPrjFile;
}

void CreateProjectModelImp::setProjectName(const QString &name)
{
  mProject->setName(name);
  bUnsavedChanges = true;
}

void CreateProjectModelImp::setProjectDescription(const QString &description)
{
  mProject->setDescription(description);
  bUnsavedChanges = true;
}

void CreateProjectModelImp::setProjectFolder(const QString &dir)
{
  mProject->setProjectFolder(dir);
  bUnsavedChanges = true;
}

void CreateProjectModelImp::setDatabase(const QString &database)
{
  mProject->setDatabase(database);
  bUnsavedChanges = true;
}

void CreateProjectModelImp::saveAs(const QString &file)
{
  mPrjFile = file;
  mProject->save(file);

  bUnsavedChanges = false;
}

void CreateProjectModelImp::init()
{

}

void CreateProjectModelImp::clear()
{
  mProject->clear();
  mPrjFile = "";
}

} // namespace graphos
