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
 

#include "HelpModel.h"

#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QCoreApplication>

#define GRAPHOS_HELP_URL_BASE "qthelp://tidop.com.graphos.2.0/doc/"
#define GRAPHOS_HELP_HOME_PAGE "index.html"

namespace graphos
{

HelpModelImp::HelpModelImp(QObject *parent)
  : HelpModel(parent),
    mSettings(new QSettings(QSettings::IniFormat,
                            QSettings::UserScope,
                            qApp->organizationName(),
                            qApp->applicationName()))
{
    HelpModelImp::init();
}

HelpModelImp::~HelpModelImp()
{
    if (mSettings) {
        delete mSettings;
        mSettings = nullptr;
    }
}

auto HelpModelImp::urlBase() const -> QString
{
    return {GRAPHOS_HELP_URL_BASE};
}

auto HelpModelImp::homePage() const -> QString
{
    return QString(GRAPHOS_HELP_URL_BASE).append(GRAPHOS_HELP_HOME_PAGE);
}

void HelpModelImp::loadSettings()
{
    if (mReadSettings) {

        /* Read Settings here

        Example (replace PropertyName):

          mParameters->setPropertyName(mSettings->value("HELP/PropertyName", mParameters->propertyName()).toInt());

        */

    }
}

void HelpModelImp::saveSettings()
{
    if (mReadSettings) {

        /* Write Settings here

        Example:

          mSettings->setValue("HELP/PropertyName", mParameters->propertyName());

        */

    }
}

void HelpModelImp::init()
{
    mReadSettings = mSettings->value("GENERAL/SAVE_PARAMETERS", false).toBool();
}

void HelpModelImp::clear()
{

}

} // namespace graphos
