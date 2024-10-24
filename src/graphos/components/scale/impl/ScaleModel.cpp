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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "ScaleModel.h"

#include <QFile>
#include <QTextStream>
#include <QSettings>

namespace graphos
{

ScaleModelImp::ScaleModelImp(Project *project, QObject *parent)
  : ScaleModel(parent),
    mProject(project),
    mSettings(new QSettings(QSettings::IniFormat,
                            QSettings::UserScope,
                            qApp->organizationName(),
                            qApp->applicationName()))
{
    ScaleModelImp::init();
}

ScaleModelImp::~ScaleModelImp()
{
    if (mSettings) {
        delete mSettings;
        mSettings = nullptr;
    }
}

void ScaleModelImp::loadSettings()
{
    if (mReadSettings) {

        /* Read Settings here

        Example (replace PropertyName):

        mParameters->setPropertyName(mSettings->value("SCALE/PropertyName", mParameters->propertyName()).toInt());

        */

    }
}

void ScaleModelImp::setTransform(const tl::Matrix<double, 4, 4> &transform)
{
    auto &_transform = mProject->transform();
    _transform = _transform * transform;
}

void ScaleModelImp::saveSettings()
{
    if (mReadSettings) {

        /* Write Settings here

        Example:

        mSettings->setValue("SCALE/PropertyName", mParameters->propertyName());

        */

    }
}

void ScaleModelImp::init()
{
    mReadSettings = mSettings->value("GENERAL/SAVE_PARAMETERS", false).toBool();
}

void ScaleModelImp::clear()
{

}

} // namespace graphos
