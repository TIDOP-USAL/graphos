#include <QDir>
#include <QWaitCondition>
#include <QDebug>
#include <csignal>

#include <iostream>

#include "ExternalProcess.h"


ExternalProcess::ExternalProcess(const QString &commandPath)
  : mCommandPath(commandPath),
    mWorkingDir(),
    mProcess(new QProcess()),
    mErrorFilePath("pwErrors.log"),
    mStdOutFile(new QFile()),
    mErrOutFile(new QFile()),
    mRunning(false)
{

  setSteps(1);
  QFileInfo fileInfo(mErrorFilePath);
  mErrorFileSize = fileInfo.size();

  mErrorWatcher = new QFileSystemWatcher(QStringList() << mErrorFilePath);

  connect(mErrorWatcher, SIGNAL(fileChanged(QString)),this,SLOT(onError(QString)));
  connect(this, SIGNAL(error(QProcess::ProcessError)),this,SLOT(onError(QProcess::ProcessError)));
  connect(mProcess,SIGNAL(readyReadStandardOutput()),this, SIGNAL(readyReadStandardOutput()));
  connect(mProcess,SIGNAL(readyReadStandardError()),this, SIGNAL(readyReadStandardError()));
  connect(mProcess,SIGNAL(readyReadStandardOutput()),this, SLOT(on_newStdData()));
  connect(mProcess,SIGNAL(readyReadStandardError()),this, SLOT(on_newErrorData()));
//    connect(&mTimer,SIGNAL(timeout()),this,SLOT(onTimeout()));
}

ExternalProcess::~ExternalProcess()
{
  delete mProcess;
  delete mErrorWatcher;
  delete mStdOutFile;
  delete mErrOutFile;
}

void ExternalProcess::start()
{
    run();
}

void ExternalProcess::setWaitForFinished(bool wait)
{
    mProcess->waitForFinished();
}

bool ExternalProcess::isRunning()
{
    return mRunning;
}

void ExternalProcess::run()
{
    mRunning = true;

    //Open log files:
    mStdOutFile->setFileName(mStdOutputFilePath);
    if(mStdOutFile->exists())
        mStdOutFile->open(QFile::WriteOnly);
    mErrOutFile->setFileName(mStdErrorFilePath);
    if(mErrOutFile->exists())
        mErrOutFile->open(QFile::WriteOnly);

    QDir workingDir(mWorkingDir);
    if (workingDir.exists(mWorkingDir))
        mProcess->setWorkingDirectory(mWorkingDir);
    connect(mProcess,SIGNAL(finished(int)),this, SLOT(on_mProcessFinished(int)));
    emit statusChanged(0,getStartupMessage());
//    std::cout<<"Working dir="<<mWorkingDir.toStdString()<<std::endl;
//    std::cout<<"Command path="<<mCommandPath.toStdString()<<std::endl;
//    std::cout<<"Inputs:"<<std::endl;
//    for(int ni=0;ni<mInputs.size();ni++)
//    {
//        if(ni==1)
//        {
//            QString input=mInputs[ni];
//            input=input.replace("\\","/");
//            input=""""+input+"""";
//            mInputs[ni]=input;
//        }
//        std::cout<<"input["<<QString::number(ni).toStdString()<<"]="<<mInputs[ni].toStdString()<<std::endl;
//    }
    mProcess->start(mCommandPath, mInputs);
}

void ExternalProcess::setWorkingDir(const QString &workingDir)
{
  mWorkingDir = workingDir;
}

void ExternalProcess::onError(QProcess::ProcessError commandError)
{
  stop();
  emit Process::error(1, mCommandPath);
}

void ExternalProcess::onError(const QString &path)
{
  QFileInfo fileInfo(mErrorFilePath);
  int errorFileSize = fileInfo.size();
  if (errorFileSize > 0 && (errorFileSize - mErrorFileSize) < 0){
      stop();
      emit Process::error(1, mCommandPath);
  }
}

void ExternalProcess::onTimeout()
{
  mProcess->write("\n");
  mTimer.start(5000);
}

void ExternalProcess::on_newStdData()
{
  QByteArray array = readStdout();
  QString out(array);
  emit newStdData(out);
  if(mStdOutFile->exists()){
      mStdOutFile->write(array);
  }
}

void ExternalProcess::on_newErrorData()
{
  QByteArray array = readStderr();
  QString out(array);
  emit newErrorData(out);
  if(mErrOutFile->exists()){
    mErrOutFile->write(array);
  }
}

void ExternalProcess::on_mProcessFinished(int code)
{
  disconnect(mProcess,SIGNAL(finished(int)),this, SLOT(on_mProcessFinished(int)));
  emit statusChanged(1,getStartupMessage());
  mRunning = false;

  //Close log files:
  if(mStdOutFile->exists())
    mStdOutFile->close();
  if(mErrOutFile->exists())
    mErrOutFile->close();

  qDebug() << QString(" ************** Finishing: ") << mCommandPath <<
              QString(" ExitCode: ") << QString::number(code) << QString(" ExitCode: ") << this->objectName();
  emit finished();
}

void ExternalProcess::stop()
{
  Process::stop();
  cascadeKill(mProcess->pid());
  mProcess->kill();
}

void ExternalProcess::setErroLogFile(const QString &filePath)
{
  mErrorWatcher->removePath(mErrorFilePath);
  mErrorFilePath = filePath;

  QFileInfo fileInfo(mErrorFilePath);
  mErrorFileSize = fileInfo.size();

  mErrorWatcher->addPath(mErrorFilePath);
}

void ExternalProcess::setStdOutputFilePath(const QString &filePath)
{
  mStdOutputFilePath = filePath;
}

void ExternalProcess::setStdErrorFilePath(const QString &filePath)
{
  mStdErrorFilePath = filePath;
}

qint64 ExternalProcess::write(const char *data)
{
    return mProcess->write(data);
}

QByteArray ExternalProcess::readStdout()
{
    return mProcess->readAllStandardOutput();
}

QByteArray ExternalProcess::readStderr()
{
    return mProcess->readAllStandardError();
}

void ExternalProcess::insertEnvironmentVar(const QString &varName, const QString &value)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(varName, value);
    mProcess->setProcessEnvironment(env);
}

void ExternalProcess::appendEnvironmentValue(const QString &varName, const QString &value, int position)
{
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  QString newValue = env.value(varName);
  QString separator = "";
  if (!newValue.isEmpty())
    separator = ";";
  if (position)
    newValue = newValue + separator + value;
  else
    newValue = value + separator + newValue;
  env.insert(varName, newValue);
  mProcess->setProcessEnvironment(env);
}

int ExternalProcess::error()
{
  return mProcess->error();
}

void ExternalProcess::cascadeKill(Q_PID pid){
#ifdef Q_OS_LINUX
    QProcess killer;
    QStringList params;
    params << "--ppid";
    params << QString::number(pid);
    params << "-o";
    params << "pid";
    params << "--noheaders";
    killer.start("/bin/ps",params,QIODevice::ReadOnly);
    if(killer.waitForStarted(-1))
    {
        if(killer.waitForFinished(-1))
        {
            QByteArray temp=killer.readAllStandardOutput();
            QString str=QString::fromLocal8Bit(temp);
            QStringList list=str.split("\n");

            for(int i=0;i<list.size();i++)
            {
                if(!list.at(i).isEmpty())
//                    ::kill(list.at(i).toInt(),SIGKILL);
                    cascadeKill(list.at(i).toInt());
            }
            ::kill(pid,SIGKILL);
        }
    }
#endif
}
