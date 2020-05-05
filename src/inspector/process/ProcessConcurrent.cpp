#include "ProcessConcurrent.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>
#include <iostream>


ProcessConcurrent::ProcessConcurrent():
    mRunning(false),
    mWait(false)
{
}

void ProcessConcurrent::start()
{
    mWatcher = new QFutureWatcher<int>();
    connect(mWatcher, SIGNAL(finished()), this, SLOT(on_mProcessFinished()));
//  connect(mWatcher, SIGNAL(finished()), this, SIGNAL(finished()));

    mRunning = true;
    QFuture<int> future = QtConcurrent::run(this, &ProcessConcurrent::runLauncher);
    mWatcher->setFuture(future);
    if (mWait)
        future.waitForFinished();
}

void ProcessConcurrent::setWaitForFinished(bool wait)
{
    mWait=wait;
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
    delete mWatcher;
}

void ProcessConcurrent::on_mProcessFinished()
{
    mRunning = false;
    delete mWatcher;
    emit finished();
}

int ProcessConcurrent::runLauncher()
{
    run();
    return 0;
}


//template <class T>  ProcessConcurrentList<T>::ProcessConcurrentList(QVector<T> v):
//    mVector(v)
//{

//}

//template <class T> ProcessConcurrentList<T>::~ProcessConcurrentList()
//{

//}

//template <class T>void ProcessConcurrentList<T>::run()
//{
//    mWatcher = new QFutureWatcher<int>();
//    connect(mWatcher, SIGNAL(finished()), this, SLOT(on_mProcessFinished()));
////  connect(mWatcher, SIGNAL(finished()), this, SIGNAL(finished()));

//    mRunning = true;
//    QFuture<int> future = QtConcurrent::map(mVector,&ProcessConcurrentList::run);
//    mWatcher->setFuture(future);
//}
