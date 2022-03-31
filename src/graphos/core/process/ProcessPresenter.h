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

#ifndef GRAPHOS_PROCESS_PRESENTER_H
#define GRAPHOS_PROCESS_PRESENTER_H

#include <QObject>

#include <tidop/core/task.h>

#include "graphos/interfaces/mvp.h"

namespace tl
{
class TaskErrorEvent;
class TaskFinalizedEvent;
}


namespace graphos
{

class ProgressHandler;

class ProcessPresenter
  : public Presenter
{

  Q_OBJECT

public:

  ProcessPresenter();
  ~ProcessPresenter() override;

protected slots:

  virtual void onError(tl::TaskErrorEvent *event);
  virtual void onFinished(tl::TaskFinalizedEvent *event);
  virtual void onStopped(tl::TaskStoppedEvent *event);

  ProgressHandler *progressHandler();

  /*!
   * \brief Create task
   * return Task
   */
  virtual std::unique_ptr<tl::Task> createProcess() = 0;

public slots:

  virtual void setProgressHandler(ProgressHandler *progressHandler);
  virtual void run();
  virtual void cancel();

signals:

  void running();
  void finished();
  void failed();
  void canceled();

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  std::unique_ptr<tl::Task> mProcess;
  ProgressHandler *mProgressHandler;
};


} // namespace graphos


#endif // GRAPHOS_PROCESS_PRESENTER_H
