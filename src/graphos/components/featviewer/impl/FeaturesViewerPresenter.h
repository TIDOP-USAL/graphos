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

#ifndef GRAPHOS_FEATURES_VIEWER_PRESENTER_H
#define GRAPHOS_FEATURES_VIEWER_PRESENTER_H

#include <memory>

#include <QObject>

#include "graphos/components/featviewer/FeaturesViewerPresenter.h"

namespace graphos
{

class FeaturesViewerView;
class FeaturesViewerModel;
class SettingsModel;
class HelpDialog;

class FeaturesViewerPresenterImp
  : public FeaturesViewerPresenter
{

  Q_OBJECT

public:

  FeaturesViewerPresenterImp(FeaturesViewerView *view,
                             FeaturesViewerModel *model);
  ~FeaturesViewerPresenterImp() override;

// FeaturesViewerPresenter interface

public slots:

  void setImageActive(const QString &image) override;
  //void openKeypointsFromImage(const QString &image) override;

protected slots:

  void loadKeypoints(const QString &image) override;

// Presenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  FeaturesViewerView *mView;
  FeaturesViewerModel *mModel;
  SettingsModel *mSettingsModel;
  HelpDialog *mHelp;

};

} // namespace graphos

#endif // GRAPHOS_FEATURES_VIEWER_PRESENTER_H
