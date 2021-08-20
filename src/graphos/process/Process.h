#ifndef GRAPHOS_PROCESS_H
#define GRAPHOS_PROCESS_H

#include "graphos/graphos_global.h"

#include <QThread>
#include <QStringList>
#include <QObject>



class GRAPHOS_EXPORT Process
  : public QObject
{
  Q_OBJECT

public:

  Process();
  virtual ~Process() = default;
  
  virtual void start() = 0;
  virtual void setWaitForFinished(bool wait) = 0;
  virtual bool isRunning() = 0;
  virtual QByteArray readStdout() = 0;
  virtual QByteArray readStderr() = 0;
  
  virtual bool isStopped();
  virtual int getSteps();
  
  void addIntputs (const QStringList &inputs);
  QString startupMessage();
  void setStartupMessage(const QString &message);
  void setSteps(int steps);

  virtual bool isWaitingForFinished() const = 0;

public slots:

  virtual void stop();

signals:

  void statusChanged(int, const QString &);
  void statusChangedNext();
  void finished();
  void error(int, const QString &);

  //For real-time output reading.
  void readyReadStandardOutput();
  void readyReadStandardError();
  void newStdData(const QString &);
  void newErrorData(const QString &);

protected:

  QStringList mInputs;
  bool mStopped;

private:

  int mSteps;
  QString mStartupMessage;

};

#endif // GRAPHOS_PROCESS_H
