#include "ProcessConcurrent.h"


#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>
//#include <QFutureWatcher>

#include <iostream>


ProcessConcurrent::ProcessConcurrent()
  : mRunning(false),
    mWatcher(nullptr),
    mWait(false)
{
}

ProcessConcurrent::~ProcessConcurrent()
{
  if (mWatcher) {
    delete mWatcher;
    mWatcher = nullptr;
  }
}

void ProcessConcurrent::start()
{
  mWatcher = new QFutureWatcher<int>();

  connect(mWatcher, SIGNAL(finished()), this, SLOT(on_mProcessFinished()));

  mRunning = true;
  QFuture<int> future = QtConcurrent::run(this, &ProcessConcurrent::runLauncher);
  mWatcher->setFuture(future);
  if (mWait)
    future.waitForFinished();
}

void ProcessConcurrent::setWaitForFinished(bool wait)
{
  mWait = wait;
}

bool ProcessConcurrent::isRunning()
{
  return mRunning;
}

QByteArray ProcessConcurrent::readStdout()
{
  return "";
}

QByteArray ProcessConcurrent::readStderr()
{
  return "";
}

void ProcessConcurrent::stop()
{
  mWatcher->cancel();
  if (mWatcher->isRunning())
    mWatcher->waitForFinished();
  mRunning = false;

  if (mWatcher) {
    delete mWatcher;
    mWatcher = nullptr;
  }
}

void ProcessConcurrent::on_mProcessFinished()
{
  mRunning = false;

  if (mWatcher) {
    delete mWatcher;
    mWatcher = nullptr;
  }

  emit finished();
}

int ProcessConcurrent::runLauncher()
{
  run();
  return 0;
}
