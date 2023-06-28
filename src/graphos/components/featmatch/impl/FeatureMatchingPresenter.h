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

#ifndef GRAPHOS_FEATURE_MATCHING_PRESENTER_H
#define GRAPHOS_FEATURE_MATCHING_PRESENTER_H

#include <QObject>

#include "graphos/components/featmatch/FeatureMatchingPresenter.h"

namespace graphos
{

class FeatureMatchingWidget;
class FeatureMatchingView;
class FeatureMatchingModel;

class FeatureMatchingPresenterImp
  : public FeatureMatchingPresenter
{

  Q_OBJECT

public:

  FeatureMatchingPresenterImp(FeatureMatchingView *view,
                              FeatureMatchingModel *model);
  ~FeatureMatchingPresenterImp() override;

private:

  void setMatchingProperties();

// FeatureMatchingPresenter interface

public slots:

  void setCurrentMatchMethod(const QString &matchMethod) override;

// TaskPresenter interface
  
protected:

  void onError(tl::TaskErrorEvent *event) override;
  void onFinished(tl::TaskFinalizedEvent *event) override;
  std::unique_ptr<tl::Task>  createProcess() override;

public slots:

  void cancel() override;

// Presenter interface

public slots:

  void open() override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  FeatureMatchingView *mView;
  FeatureMatchingModel *mModel;
  FeatureMatchingWidget *mFeatureMatchingWidget;

};

} // End namespace graphos

#endif // GRAPHOS_FEATURE_MATCHING_PRESENTER_H
