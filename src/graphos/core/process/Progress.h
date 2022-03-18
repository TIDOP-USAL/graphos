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

#ifndef GRAPHOS_PROGRESS_H
#define GRAPHOS_PROGRESS_H

#include <QObject>

namespace graphos
{


class ProgressHandler
  : public QObject
{
  Q_OBJECT

public:

  ProgressHandler(QObject *parent = nullptr);
  ~ProgressHandler() override;

public:

  int min() const;
  int max() const;
  int value() const;

signals:

  void rangeChange(int, int);
  void valueChange(int);
  void initialized();
  void finished();
  void titleChange(QString);
  void descriptionChange(QString);
  void cancel();
  void closeAuto(bool);

public slots:

  void setRange(int min, int max);
  void setValue(int value);
  void next();
  void init();
  void finish();
  void setTitle(const QString &title);
  void setDescription(const QString &description);
  void setCloseAuto(bool active);

protected:

  int mMin;
  int mMax;
  int mValue;

};


} // namespace graphos

#endif // GRAPHOS_PROGRESS_H
