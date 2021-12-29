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

#ifndef GRAPHOS_POINT_CLOUD_EXPORT_PRESENTER_H
#define GRAPHOS_POINT_CLOUD_EXPORT_PRESENTER_H

#include "graphos/components/export/densemodel/ExportPointCloudPresenter.h"

namespace graphos
{

class PointCloudCSVFormatWidget;
class ExportPointCloudView;
class ExportPointCloudModel;
class HelpDialog;

class ExportPointCloudPresenterImp
  : public ExportPointCloudPresenter
{

  Q_OBJECT

public:

  ExportPointCloudPresenterImp(ExportPointCloudView *view,
                               ExportPointCloudModel *model);
  ~ExportPointCloudPresenterImp() override;

// ExportPointCloudPresenter interface

public slots:

  void save() override;
  void setCurrentFormat(const QString &format) override;

// Presenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  ExportPointCloudView *mView;
  ExportPointCloudModel *mModel;
  //PointCloudCSVFormatWidget *mPointCloudCSVFormatWidget;
  HelpDialog *mHelp;
  QString mFile;
  QString mFormat;

};

} // namespace graphos

#endif // GRAPHOS_POINT_CLOUD_EXPORT_PRESENTER_H
