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

#ifndef GRAPHOS_DENSE_PRESENTER_H
#define GRAPHOS_DENSE_PRESENTER_H

#include "graphos/components/dense/DensificationPresenter.h"

namespace graphos
{

class CmvsPmvsWidget;
class SmvsWidget;
class MvsWidget;
class DensificationView;
class DensificationModel;
class HelpDialog;

class DensificationPresenterImp
  : public DensificationPresenter
{
  Q_OBJECT

public:

  DensificationPresenterImp(DensificationView *view,
                            DensificationModel *model);
  ~DensificationPresenterImp() override;

private:

  void setCmvsPmvsProperties();
  void setSmvsProperties();
  void setMvsProperties();

private slots:

  void onDensificationChanged(const QString &densification);
  void onFinishDensification();

// DensificationPresenter interface

public slots:

  void setCurrentDensifier(const QString &densifier) override;

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

private:

  DensificationView *mView;
  DensificationModel *mModel;
  CmvsPmvsWidget *mCmvsPmvs;
  SmvsWidget *mSmvs;
  MvsWidget *mMVS;
  HelpDialog *mHelp;
};

} // End namespace graphos


#endif // GRAPHOS_DENSE_PRESENTER_H
