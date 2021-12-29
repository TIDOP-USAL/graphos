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

#ifndef GRAPHOS_FEATURE_MATCHING_VIEW_INTERFACE_H
#define GRAPHOS_FEATURE_MATCHING_VIEW_INTERFACE_H

#include "graphos/interfaces/mvp.h"

class QGridLayout;
class QLabel;
class QComboBox;
class QDialogButtonBox;

namespace graphos
{

class FeatureMatchingView
  : public DialogView
{

  Q_OBJECT

public:

  FeatureMatchingView(QWidget *parent) : DialogView(parent) {}
  ~FeatureMatchingView() override = default;

  virtual void addMatchMethod(QWidget *matchMethod) = 0;
  virtual QString currentMatchMethod() const = 0;

signals:

  void matchMethodChange(QString);
  void run();

public slots:

  virtual void setCurrentMatchMethod(const QString &matchMethodName) = 0;

};

} // End namespace graphos

#endif // GRAPHOS_FEATURE_MATCHING_VIEW_INTERFACE_H
