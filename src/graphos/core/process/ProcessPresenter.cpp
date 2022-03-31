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

#include "graphos/core/process/ProcessPresenter.h"
#include "graphos/core/process/Progress.h"

#include <tidop/core/progress.h>

#include <QStandardPaths>
#include <QDir>

namespace graphos
{
	
ProcessPresenter::ProcessPresenter()
  : Presenter(),
    mProcess(nullptr),
    mProgressHandler(nullptr)
{
  ProcessPresenter::init();
  ProcessPresenter::initSignalAndSlots();
}

ProcessPresenter::~ProcessPresenter()
{
}

void ProcessPresenter::onError(tl::TaskErrorEvent *event)
{

  if (mProgressHandler){
    mProgressHandler->finish();
    mProgressHandler->reset();
    mProgressHandler->setDescription(QString::fromStdString(event->errorMessage()));

    disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
  }

  emit failed();
}

void ProcessPresenter::onFinished(tl::TaskFinalizedEvent *event)
{
  if (mProgressHandler){
    mProgressHandler->finish();
    mProgressHandler->reset();
    mProgressHandler->setDescription(tr("Process Finished"));

    disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
  }

  emit finished();
}

void ProcessPresenter::onStopped(tl::TaskStoppedEvent *event)
{
  if(mProgressHandler) {
    mProgressHandler->finish();
    mProgressHandler->reset();
    mProgressHandler->setDescription(tr("Process canceled"));

    disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
  }

  emit canceled();
}

ProgressHandler *ProcessPresenter::progressHandler()
{
  return mProgressHandler;
}

void ProcessPresenter::setProgressHandler(ProgressHandler *progressHandler)
{
  mProgressHandler = progressHandler;
}

void ProcessPresenter::run()
{
  try {

    tl::Task *p = mProcess.release();
    delete p;
    p = nullptr;

    mProcess = this->createProcess();

    if(mProcess == nullptr) throw std::runtime_error("");

    mProcess->subscribe(std::bind(&ProcessPresenter::onError, this, std::placeholders::_1));
    mProcess->subscribe(std::bind(&ProcessPresenter::onFinished, this, std::placeholders::_1));
    mProcess->subscribe(std::bind(&ProcessPresenter::onStopped, this, std::placeholders::_1));

    if(mProgressHandler) {
      connect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
      mProgressHandler->init();
    }

    mProcess->runAsync(mProgressHandler);

  } catch(const std::exception &e) {
    /// Imprimir excepciones enlazadas
    msgError(e.what());
  }
}

// Añadir onCancel...
void ProcessPresenter::cancel()
{
  if(mProcess) {
    mProcess->stop();
    if(mProgressHandler) {
      mProgressHandler->setDescription(tr("Stopping process"));
    }
  }

  //emit canceled();
}

void ProcessPresenter::init()
{
}

void ProcessPresenter::initSignalAndSlots()
{
}


} // namespace graphos
