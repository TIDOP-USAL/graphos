#ifndef INSPECTOR_PROCESS_CONCURRENT_H
#define INSPECTOR_PROCESS_CONCURRENT_H



#include "Process.h"
#include <QFutureWatcher>
//template <typename T> class QFutureWatcher;

class  INSPECTOR_EXPORT ProcessConcurrent
  : public Process
{   
    Q_OBJECT

public:

  ProcessConcurrent();
  ~ProcessConcurrent() override;

  virtual void start() override;
  virtual void setWaitForFinished(bool wait) override;
  virtual bool isRunning() override;

  virtual QByteArray readStdout() override;
  virtual QByteArray readStderr() override;

public slots:

  virtual void stop() override;
  void on_mProcessFinished();

protected:

  virtual void run() = 0;
  int runLauncher();

private:

  bool mRunning;
  QFutureWatcher<int> *mWatcher;
  bool mWait;

};

#endif // INSPECTOR_PROCESS_CONCURRENT_H
