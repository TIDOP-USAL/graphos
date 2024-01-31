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

std::shared_ptr<Feature> FeatureExtractorModelImp::featureExtractor() const
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

tl::Path FeatureExtractorModelImp::database() const
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

const std::unordered_map<size_t, Image> &FeatureExtractorModelImp::images() const
{
    return mProject->images();
}

const std::map<int, Camera> &FeatureExtractorModelImp::cameras() const
{
    return mProject->cameras();
}

void FeatureExtractorModelImp::clearProject()
{
    tl::Path::removeFile(database());
    mProject->removeFeatures();
}

} // namespace graphos
