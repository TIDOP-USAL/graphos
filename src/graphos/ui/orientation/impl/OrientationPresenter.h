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

#ifndef GRAPHOS_ORIENTATION_PRESENTER_H
#define GRAPHOS_ORIENTATION_PRESENTER_H

#include "graphos/graphos_global.h"

#include "graphos/ui/orientation/OrientationPresenter.h"
#include "graphos/core/orientation/gcp.h"

namespace graphos
{

class OrientationView;
class OrientationModel;
class HelpDialog;

class OrientationPresenterImp
  : public OrientationPresenter
{
  Q_OBJECT

public:

  OrientationPresenterImp(OrientationView *view,
                          OrientationModel *model);
  ~OrientationPresenterImp() override;

private slots:

  void onRelativeOrientationFinished();
  void onAbsoluteOrientationFinished();

// ProcessPresenter interface
  
protected slots:

  void onError(int code, const QString &msg) override;
  void onFinished() override;
  bool createProcess() override;

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

  OrientationView *mView;
  OrientationModel *mModel;
  HelpDialog *mHelp;

};

} // namespace graphos

#endif // GRAPHOS_ORIENTATION_PRESENTER_H