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

#ifndef GRAPHOS_EXPORT_ORIENTATIONS_PRESENTER_H
#define GRAPHOS_EXPORT_ORIENTATIONS_PRESENTER_H

#include "graphos/components/export/orientations/ExportOrientationsPresenter.h"

namespace graphos
{

class NvmFormatWidget;
class BundlerFormatWidget;
class MveFormatWidget;
class OriTxtFormatWidget;
class ExportOrientationsView;
class ExportOrientationsModel;
class HelpDialog;

class ExportOrientationsPresenterImp
  : public ExportOrientationsPresenter
{

public:

  ExportOrientationsPresenterImp(ExportOrientationsView *view,
                                 ExportOrientationsModel *model);
  ~ExportOrientationsPresenterImp() override;

// ExportOrientationsPresenter interface

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

  ExportOrientationsView *mView;
  ExportOrientationsModel *mModel;
  NvmFormatWidget *mNvmFormatWidget;
  BundlerFormatWidget *mBundlerFormatWidget;
  MveFormatWidget *mMveFormatWidget;
  OriTxtFormatWidget *mOriTxtFormatWidget;
  HelpDialog *mHelp;
};

} // namespace graphos

#endif // GRAPHOS_EXPORT_ORIENTATIONS_PRESENTER_H
