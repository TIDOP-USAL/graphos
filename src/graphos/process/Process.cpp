#include "Process.h"

Process::Process() 
  : mInputs(0),
    mStopped(false),
    mSteps(0),
    mStartupMessage("")
{
}

void Process::addIntputs(const QStringList& inputs)
{
  mInputs = inputs;
}

QString Process::startupMessage()
{
  return mStartupMessage;
}

void Process::setStartupMessage(const QString &message)
{
  mStartupMessage = message;
}

void Process::stop()
{
  mStopped = true;
}

bool Process::isStopped()
{
  return mStopped;
}

int Process::getSteps()
{
  return mSteps;
}

void Process::setSteps(int steps)
{
  mSteps = steps;
}
