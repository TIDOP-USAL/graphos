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

#ifndef GRAPHOS_FEATURE_EXTRACTOR_PRESENTER_H
#define GRAPHOS_FEATURE_EXTRACTOR_PRESENTER_H

#include "graphos/graphos_global.h"

#include <QObject>

#include <opencv2/core.hpp>

#include "graphos/components/featextract/FeatureExtractorPresenter.h"


namespace graphos
{

class SiftWidget;
class FeatureExtractorView;
class FeatureExtractorModel;
class HelpDialog;

class FeatureExtractorPresenterImp
  : public FeatureExtractorPresenter
{
  Q_OBJECT

public:

  FeatureExtractorPresenterImp(FeatureExtractorView *view,
                               FeatureExtractorModel *model);
  ~FeatureExtractorPresenterImp() override;

private:

  void setDetectorAndDescriptorProperties();
  void setSiftProperties();

private slots:

  void onFeaturesExtracted(const QString &imageName, 
                           const QString &featuresFile);

// FeatureExtractorPresenter interface

public slots:

  void setCurrentDetectorDescriptor(const QString &detectorDescriptor) override;

// ProcessPresenter interface
  
protected slots:

  void onError(tl::TaskErrorEvent *event) override;
  void onFinished(tl::TaskFinalizedEvent *event) override;

  std::unique_ptr<tl::Task> createProcess() override;

public slots:

  void cancel() override;

// Presenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

protected:

  FeatureExtractorView *mView;
  FeatureExtractorModel *mModel;
  HelpDialog *mHelp;
  SiftWidget *mSift;

};

} // namespace graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_PRESENTER_H
