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
#include "graphos/process/MultiProcess.h"

#include <tidop/core/progress.h>

#include <QStandardPaths>
#include <QDir>

namespace graphos
{
	
//ProcessPresenter::ProcessPresenter()
//  : Presenter(),
//    mMultiProcess(new MultiProcess(true)),
//    mProgressHandler(nullptr)
//{
//}
//
//ProcessPresenter::~ProcessPresenter()
//{
//  if (mMultiProcess){
//    delete mMultiProcess;
//    mMultiProcess = nullptr;
//  }
//}
//
//void ProcessPresenter::setProgressHandler(ProgressHandler *progressHandler)
//{
//  mProgressHandler = progressHandler;
//}
//
//void ProcessPresenter::onFinished()
//{
//  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
//  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));
//
//  if (mProgressHandler){
//    mProgressHandler->setRange(0,1);
//    mProgressHandler->setValue(1);
//    mProgressHandler->finish();
//    mProgressHandler->setDescription(tr("Process Finished"));
//
//    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(finish()));
//    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(next()));
//    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(finish()));
//    
//  }
//
//  mMultiProcess->clearProcessList();
//
//  emit finished();
//}
//
//void ProcessPresenter::onError(int code, const QString &msg)
//{
//  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
//  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));
//
//  if (mProgressHandler){
//    mProgressHandler->setRange(0,1);
//    mProgressHandler->setValue(1);
//    mProgressHandler->finish();
//    mProgressHandler->setDescription(msg);
//
//    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(finish()));
//    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(next()));
//    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(finish()));
//
//    disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
//  }
//
//  mMultiProcess->clearProcessList();
//
//  emit failed();
//}
//
//void ProcessPresenter::run()
//{
//  try {
//
//    connect(mMultiProcess, SIGNAL(error(int, QString)),          this, SLOT(onError(int, QString)));
//    connect(mMultiProcess, SIGNAL(finished()),                   this, SLOT(onFinished()));
//
//    if (mProgressHandler){
//      connect(mMultiProcess, SIGNAL(finished()),             mProgressHandler,    SLOT(finish()));
//      connect(mMultiProcess, SIGNAL(statusChangedNext()),    mProgressHandler,    SLOT(next()));
//      connect(mMultiProcess, SIGNAL(error(int, QString)),    mProgressHandler,    SLOT(finish()));
//      
//      connect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
//
//      mProgressHandler->setTitle("Process...");
//      mProgressHandler->setDescription("Process...");
//    }
//
//    if (createProcess()) {
//      if (mProgressHandler){
//        if (mProgressHandler->max() == 1)
//          mProgressHandler->setRange(0, mMultiProcess->count());
//        mProgressHandler->setValue(0);
//        mProgressHandler->init();
//      }
//
//      emit running();
//
//      mMultiProcess->start();
//    } else {
//      disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
//      disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));
//
//      if (mProgressHandler) {
//        disconnect(mMultiProcess, SIGNAL(finished()), mProgressHandler, SLOT(finish()));
//        disconnect(mMultiProcess, SIGNAL(statusChangedNext()), mProgressHandler, SLOT(next()));
//        disconnect(mMultiProcess, SIGNAL(error(int, QString)), mProgressHandler, SLOT(finish()));
//
//        disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
//      }
//    }
//
//  } catch (std::exception &e) {
//    onError(0, e.what());
//  } catch (...) {
//    onError(0, "Unknown exception");
//  }
//}
//
//void ProcessPresenter::cancel()
//{
//  mMultiProcess->stop();
//
//  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
//  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));
//
//  if (mProgressHandler){
//    mProgressHandler->setRange(0,1);
//    mProgressHandler->setValue(1);
//    mProgressHandler->finish();
//    mProgressHandler->setDescription(tr("Processing has been canceled by the user"));
//
//    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(finish()));
//    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(next()));
//    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(finish()));
//    disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
//  }
//
//  mMultiProcess->clearProcessList();
//
//  emit canceled();
//}




ProcessPresenter::ProcessPresenter()
  : Presenter(),
    mProcess(nullptr)
{
}

ProcessPresenter::~ProcessPresenter()
{
}

void ProcessPresenter::onError(tl::ProcessErrorEvent *event)
{

  if (mProgressHandler){
    //mProgressHandler->setRange(0,1);
    mProgressHandler->finish();
    mProgressHandler->reset();
    mProgressHandler->setDescription(QString::fromStdString(event->errorMessage()));

    disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
  }

  emit failed();
}

void ProcessPresenter::onFinished(tl::ProcessFinalizedEvent *event)
{
  if (mProgressHandler){
    //mProgressHandler->setRange(0,1);
    mProgressHandler->finish();
    mProgressHandler->reset();
    mProgressHandler->setDescription(tr("Process Finished"));

    disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
  }

  emit finished();
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

    mProcess = this->createProcess();

    if(mProcess == nullptr) throw std::runtime_error("");

    mProcess->subscribe(std::bind(&ProcessPresenter::onError, this, std::placeholders::_1));
    mProcess->subscribe(std::bind(&ProcessPresenter::onFinished, this, std::placeholders::_1));
    
    connect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));

    mProcess->runAsync(mProgressHandler);

  } catch(const std::exception &e) {
    /// Imprimir excepciones enlazadas
    msgError(e.what());
  }
}

void ProcessPresenter::cancel()
{
  if(mProcess) {
    mProcess->stop();
  }

  emit canceled();
}






} // namespace graphos
