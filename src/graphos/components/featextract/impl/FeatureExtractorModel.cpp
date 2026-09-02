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

#include "FeatureExtractorModel.h"

#include "graphos/core/project/Project.h"

#include <QFileInfo>
#include <QSettings>
#include <QCoreApplication>

namespace graphos
{

FeatureExtractorModelImp::FeatureExtractorModelImp(Project *project,
                                                   QObject *parent)
  : FeatureExtractorModel(parent),
    mProject(project),
    mSettings(new QSettings(QSettings::IniFormat,
                            QSettings::UserScope,
                            QCoreApplication::organizationName(),
                            QCoreApplication::applicationName()))
{
    init();
}

FeatureExtractorModelImp::~FeatureExtractorModelImp()
{
    if (mSettings) {
        delete mSettings;
        mSettings = nullptr;
    }
}

void FeatureExtractorModelImp::init()
{

}

void FeatureExtractorModelImp::clear()
{

}

auto FeatureExtractorModelImp::featureConfig() const -> std::shared_ptr<FeatureExtractorProperties>
{
    return mProject->featureConfig();
}

void FeatureExtractorModelImp::setFeatureConfig(const std::shared_ptr<FeatureExtractorProperties> &featureExtractor)
{
    mProject->setFeatureConfig(featureExtractor);
}

void FeatureExtractorModelImp::setFeatureExtractorReport(const FeatureExtractorReport &report)
{
    mProject->setFeatureReport(report);
}

auto FeatureExtractorModelImp::database() const -> tl::Path
{
    return mProject->info().database();
}

void FeatureExtractorModelImp::addFeatures(size_t imageId, const QString &featuresFile)
{
    mProject->features().add(imageId, featuresFile.toStdString());
}

bool FeatureExtractorModelImp::useCuda() const
{
    return mSettings->value("UseCuda", true).toBool();
}

auto FeatureExtractorModelImp::images() const -> const ImageRepository &
{
    return mProject->images();
}

auto FeatureExtractorModelImp::cameras() const -> const CameraRepository &
{
    return mProject->cameras();
}

auto FeatureExtractorModelImp::enuCrs() const -> std::string
{
    return mProject->info().enuCrs();
}

void FeatureExtractorModelImp::clearProject()
{
    tl::Path::removeFile(database());
    mProject->clearFeatures();
}

} // namespace graphos
