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

#ifndef GRAPHOS_FEATURE_EXTRACTOR_MODEL_H
#define GRAPHOS_FEATURE_EXTRACTOR_MODEL_H

#include "graphos/components/featextract/FeatureExtractorModel.h"

class QSettings;

namespace graphos
{

class Project;
class Feature;

class FeatureExtractorModelImp
  : public FeatureExtractorModel
{

public:

  FeatureExtractorModelImp(Project *project,
                           QObject *parent = nullptr);
  ~FeatureExtractorModelImp() override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

// FeatureExtractorModel interface

public:

  std::shared_ptr<Feature> featureExtractor() const override;
  void setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor) override;
  QString database() const override;
  void addFeatures(const QString &imageName, const QString &featuresFile) override;
  bool useCuda() const override;
  std::vector<Image> images() const override;
  std::map<int, tl::Camera> cameras() const override;
  void clearProject() override;

protected:

  Project *mProject;
  QSettings *mSettings;
};

} // namespace graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_MODEL_H
