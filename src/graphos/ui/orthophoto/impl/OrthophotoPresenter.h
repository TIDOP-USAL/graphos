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

#ifndef GRAPHOS_ORTHOPHOTO_PRESENTER_H
#define GRAPHOS_ORTHOPHOTO_PRESENTER_H

#include "graphos/ui/orthophoto/OrthophotoPresenter.h"

namespace graphos
{

class HelpDialog;


class OrthophotoView;
class OrthophotoModel;

class OrthophotoPresenterImp
  : public OrthophotoPresenter
{

  Q_OBJECT

public:

  OrthophotoPresenterImp(OrthophotoView *view,
                              OrthophotoModel *model);
  ~OrthophotoPresenterImp() override;

private slots:

  void onOrthophotoFinished();
  
// Presenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

// ProcessPresenter interface

protected slots:

  void onError(int code, const QString &msg) override;
  void onFinished() override;
  bool createProcess() override;
  
public slots:

  void cancel() override;

private:

  OrthophotoView *mView;
  OrthophotoModel *mModel;
  HelpDialog *mHelp;

};

} // namespace graphos

#endif // GRAPHOS_ORTHOPHOTO_PRESENTER_H
