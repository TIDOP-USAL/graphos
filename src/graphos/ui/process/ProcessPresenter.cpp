#include "ProcessPresenter.h"

#include "graphos/ui/utils/Progress.h"
#include "graphos/process/MultiProcess.h"

#include <QStandardPaths>
#include <QDir>

namespace graphos
{

namespace ui
{
	
ProcessPresenter::ProcessPresenter()
  : Presenter(),
    mMultiProcess(new MultiProcess(true)),
    mProgressHandler(nullptr)
{
}

ProcessPresenter::~ProcessPresenter()
{
  if (mMultiProcess){
    delete mMultiProcess;
    mMultiProcess = nullptr;
  }
}

void ProcessPresenter::setProgressHandler(ProgressHandler *progressHandler)
{
  mProgressHandler = progressHandler;
}

void ProcessPresenter::onFinished()
{
  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->finish();
    mProgressHandler->setDescription(tr("Process Finished"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(finish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(next()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(finish()));
    
  }

  mMultiProcess->clearProcessList();

  emit finished();
}

void ProcessPresenter::onError(int code, const QString &msg)
{
  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->finish();
    mProgressHandler->setDescription(msg);

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(finish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(next()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(finish()));

    disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
  }

  mMultiProcess->clearProcessList();

  emit finished();
}

void ProcessPresenter::run()
{
  try {

    connect(mMultiProcess, SIGNAL(error(int, QString)),          this, SLOT(onError(int, QString)));
    connect(mMultiProcess, SIGNAL(finished()),                   this, SLOT(onFinished()));

    if (mProgressHandler){
      connect(mMultiProcess, SIGNAL(finished()),             mProgressHandler,    SLOT(finish()));
      connect(mMultiProcess, SIGNAL(statusChangedNext()),    mProgressHandler,    SLOT(next()));
      connect(mMultiProcess, SIGNAL(error(int, QString)),    mProgressHandler,    SLOT(finish()));
      
      connect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));

      mProgressHandler->setTitle("Process...");
      mProgressHandler->setDescription("Process...");
    }

    if (createProcess()) {
      if (mProgressHandler){
        if (mProgressHandler->max() == 1)
          mProgressHandler->setRange(0, mMultiProcess->count());
        mProgressHandler->setValue(0);
        mProgressHandler->init();
      }

      emit running();

      mMultiProcess->start();
    } else {
      disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
      disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

      if (mProgressHandler) {
        disconnect(mMultiProcess, SIGNAL(finished()), mProgressHandler, SLOT(finish()));
        disconnect(mMultiProcess, SIGNAL(statusChangedNext()), mProgressHandler, SLOT(next()));
        disconnect(mMultiProcess, SIGNAL(error(int, QString)), mProgressHandler, SLOT(finish()));

        disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
      }
    }

  } catch (std::exception &e) {
    onError(0, e.what());
  } catch (...) {
    onError(0, "Unknown exception");
  }
}

void ProcessPresenter::cancel()
{
  mMultiProcess->stop();

  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->finish();
    mProgressHandler->setDescription(tr("Processing has been canceled by the user"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(finish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(next()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(finish()));
    disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
  }

  mMultiProcess->clearProcessList();

  emit finished();
}

} // namespace ui

} // namespace graphos
