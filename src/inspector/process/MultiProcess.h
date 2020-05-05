#ifndef INSPECTOR_MULTIPROCESS_H
#define INSPECTOR_MULTIPROCESS_H

#include <QList>

#include "inspector/process/Process.h"


class INSPECTOR_EXPORT MultiProcess
  : public Process
{

    Q_OBJECT

public:

    MultiProcess();
    explicit MultiProcess(bool isSequential);
    ~MultiProcess() override;

    virtual void start() override;
    virtual void setWaitForFinished(bool wait) override;
    virtual bool isRunning() override;

    virtual QByteArray readStdout() override;
    virtual QByteArray readStderr() override;

    void appendProcess(const std::shared_ptr<Process> &process);
    void appendProcess(const QList<std::shared_ptr<Process> > &processList);
    void clearProcessList();
    int count();
    Process *at(int i);

    virtual int getSteps() override;

public slots:

    virtual void stop() override;

protected:

    void run();

private:

    QList<std::shared_ptr<Process>> mProcessList;
    bool mIsSequential;
    int mCurrentProcess;
    int mRunningCount;
    int mIdealThreadCount;
    int mFinishedCount;
    int mCurrentStep;
    bool mConcatenateMessages;
    bool mWaitForFinished;

private slots:

    void onError(int code, QString cause);
    void onAProcessFinished();
    void OnChildStatusChanged(int step, QString childMessage);
    void OnChildStatusChangedNext();
};


#endif // INSPECTOR_MULTIPROCESS_H
