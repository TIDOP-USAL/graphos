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

#ifndef GRAPHOS_MATCH_VIEWER_PRESENTER_INTERFACE_H
#define GRAPHOS_MATCH_VIEWER_PRESENTER_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

class MatchViewerPresenter
  : public Presenter
{

  Q_OBJECT

public:

  MatchViewerPresenter(){}
  virtual ~MatchViewerPresenter() {}

public slots:

  //virtual void openFromImages(const QString &imageLeft, const QString &imageRight) = 0;
  virtual void setLeftImage(const QString &image) = 0;
  virtual void setRightImage(const QString &image) = 0;

protected slots:


  virtual void loadMatches(const QString &imageLeft, const QString &imageRight) = 0;
//  virtual void deleteMatch(const QString &imageLeft, const QString &imageRight, int query_id, int train_id) = 0;
};

} // namespace graphos

#endif // GRAPHOS_MATCH_VIEWER_PRESENTER_INTERFACE_H
