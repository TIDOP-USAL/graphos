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

#include "graphos/core/project.h"

#include <QFileInfo>
#include <QSettings>

namespace graphos
{

FeatureExtractorModelImp::FeatureExtractorModelImp(Project *project,
                                                   QObject *parent)
  : FeatureExtractorModel(parent),
    mProject(project),
    mSettings(new QSettings(QSettings::IniFormat,
                            QSettings::UserScope,
                            qApp->organizationName(),
                            qApp->applicationName()))
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

auto FeatureExtractorModelImp::featureExtractor() const -> std::shared_ptr<Feature>
{
    return mProject->featureExtractor();
}

void FeatureExtractorModelImp::setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor)
{
    mProject->setFeatureExtractor(featureExtractor);
}

void FeatureExtractorModelImp::setFeatureExtractorReport(const FeatureExtractorReport &report)
{
    mProject->setFeatureExtractorReport(report);
}

auto FeatureExtractorModelImp::database() const -> tl::Path
{
    return mProject->database();
}

void FeatureExtractorModelImp::addFeatures(size_t imageId, const QString &featuresFile)
{
    mProject->addFeatures(imageId, featuresFile);
}

bool FeatureExtractorModelImp::useCuda() const
{
    return mSettings->value("UseCuda", true).toBool();
}

auto FeatureExtractorModelImp::images() const -> const std::unordered_map<size_t, Image>&
{
    return mProject->images();
}

auto FeatureExtractorModelImp::cameras() const -> const std::map<int, Camera>&
{
    return mProject->cameras();
}

void FeatureExtractorModelImp::clearProject()
{
    tl::Path::removeFile(database());
    mProject->removeFeatures();
}

} // namespace graphos
