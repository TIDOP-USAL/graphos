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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_DTM_VIEW_INTERFACE_H
#define GRAPHOS_DTM_VIEW_INTERFACE_H

#include "graphos/interfaces/mvp.h"


namespace graphos
{

class DtmView
  : public DialogView
{

  Q_OBJECT

public:

  DtmView(QWidget *parent = nullptr) : DialogView(parent) {}
  ~DtmView() override = default;
 
  virtual double gsd() const = 0;

  virtual void addDtmMethod(QWidget *method) = 0;
  virtual QString currentDtmMethod() const = 0;
  virtual bool isDSM() const = 0;

signals:

  void dtmMethodChange(QString);
  void run();

public slots:

  virtual void setGSD(double gsd) = 0;
  virtual void setCurrentDtmMethod(const QString &method) = 0;
  virtual void setDSM(bool active) = 0;
};

} // namespace graphos

#endif // GRAPHOS_DTM_VIEW_INTERFACE_H