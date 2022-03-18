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

#ifndef GRAPHOS_PROCESS_CONCURRENT_H
#define GRAPHOS_PROCESS_CONCURRENT_H

#include "graphos/process/Process.h"

#include <QFutureWatcher>

class  GRAPHOS_EXPORT ProcessConcurrent
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

  bool isWaitingForFinished() const override;

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

#endif // GRAPHOS_PROCESS_CONCURRENT_H
