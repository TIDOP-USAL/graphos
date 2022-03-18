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

#ifndef GRAPHOS_MULTIPROCESS_H
#define GRAPHOS_MULTIPROCESS_H

#include <memory>

#include <QList>

#include "graphos/process/Process.h"


class GRAPHOS_EXPORT MultiProcess
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
  bool isWaitingForFinished() const override;

public slots:

  virtual void stop() override;

protected:

  void run();

private slots:

  void onError(int code, const QString &cause);
  void onAProcessFinished();
  void OnChildStatusChanged(int step, const QString &childMessage);
  void OnChildStatusChangedNext();

private:

  QList<std::shared_ptr<Process>> mProcessList;
  bool mIsSequential;
  int mCurrentProcess;
  int mRunningCount;
  int mIdealThreadCount;
  int mFinishedCount;
  int mCurrentStep;
  bool mWaitForFinished;

};


#endif // GRAPHOS_MULTIPROCESS_H
