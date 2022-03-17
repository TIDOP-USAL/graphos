/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
