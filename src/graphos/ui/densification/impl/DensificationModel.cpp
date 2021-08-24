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

#include "DensificationModel.h"

#include "graphos/core/project.h"
#include "graphos/core/densification/densification.h"


#include <tidop/core/console.h>

#include <QSettings>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace graphos
{


DensificationModelImp::DensificationModelImp(Project *project,
                                             QObject *parent)
  : DensificationModel(parent),
    mProject(project)
{
  init();
}

DensificationModelImp::~DensificationModelImp()
{

}

void DensificationModelImp::init()
{
}

void DensificationModelImp::clear()
{
  mProject->clearDensification();
}

std::shared_ptr<Densification> DensificationModelImp::densification() const
{
  return mProject->densification();
}

QString DensificationModelImp::projectFolder() const
{
  return mProject->projectFolder();
}


QString DensificationModelImp::reconstructionPath() const
{
  return mProject->reconstructionPath();
}

bool DensificationModelImp::useCuda() const
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TIDOP", "Graphos");
  return settings.value("General/UseCuda", true).toBool();
}

bool DensificationModelImp::existDenseModel() const
{
  QString dense_model = mProject->denseModel();
  return !dense_model.isEmpty();
}

void DensificationModelImp::setDensification(const std::shared_ptr<Densification> &densification)
{
  mProject->setDensification(densification);
}

void DensificationModelImp::setDenseModel(const QString &denseModel)
{
  mProject->setDenseModel(denseModel);
}


} // End namespace graphos
