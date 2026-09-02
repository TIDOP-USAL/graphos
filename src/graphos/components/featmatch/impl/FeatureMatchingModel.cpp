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

#include "FeatureMatchingModel.h"

#include <tidop/core/app/Message.h>

#include <QFileInfo>
#include <QSettings>
#include <QCoreApplication>
#include <tidop/geospatial/crs.h>

namespace graphos
{

FeatureMatchingModelImp::FeatureMatchingModelImp(Project *project,
                                                 QObject *parent)
  : FeatureMatchingModel(parent),
    mProject(project),
    mSettings(new QSettings(QSettings::IniFormat,
                            QSettings::UserScope,
                            QCoreApplication::organizationName(),
                            QCoreApplication::applicationName()))
{
    FeatureMatchingModelImp::init();
}

FeatureMatchingModelImp::~FeatureMatchingModelImp()
{
    if (mSettings) {
        delete mSettings;
        mSettings = nullptr;
    }
}

void FeatureMatchingModelImp::init()
{
}

void FeatureMatchingModelImp::clear()
{

}

auto FeatureMatchingModelImp::featureMatchingConfig() const -> std::shared_ptr<MatchingProperties>
{
    return mProject->matchingConfig();
}

void FeatureMatchingModelImp::setFeatureMatchingConfig(const std::shared_ptr<MatchingProperties> &featureMatching)
{
    mProject->setMatchingConfig(featureMatching);
}

void FeatureMatchingModelImp::setMatchingReport(const MatchingReport &report)
{
    mProject->setMatchingReport(report);
}

auto FeatureMatchingModelImp::database() const -> tl::Path
{
    return mProject->info().database();
}

auto FeatureMatchingModelImp::useCuda() const -> bool
{
    return mSettings->value("UseCuda", true).toBool();
}

auto FeatureMatchingModelImp::spatialMatching() const -> bool
{
    for (const auto &image : mProject->images()) {
        if (!image.second.cameraPose().isEmpty()) {
            return true;
        }
    }

    return false;
}

auto FeatureMatchingModelImp::geodeticCoordinates() const -> bool
{
    for (const auto &image : mProject->images()) {
        if (!image.second.cameraPose().isEmpty()) {
            auto epsg_code = image.second.cameraPose().crs().toStdString();
            tl::Crs crs(epsg_code);
            return crs.isGeographic();
        }
    }

    return false;
}

void FeatureMatchingModelImp::cleanProject()
{
    mProject->clearMatches();
}

size_t FeatureMatchingModelImp::imagesSize() const
{
    return mProject->images().size();
}

} // End namespace graphos
