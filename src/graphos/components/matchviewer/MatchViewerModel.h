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

#ifndef GRAPHOS_MATCH_VIEWER_MODEL_INTERFACE_H
#define GRAPHOS_MATCH_VIEWER_MODEL_INTERFACE_H

#include <memory>

#include "graphos/interfaces/mvp.h"

namespace graphos
{

class MatchViewerModel
  : public Model
{

  Q_OBJECT

public:

  MatchViewerModel(QObject *parent = nullptr) : Model(parent) {}
  virtual ~MatchViewerModel() = default;

  virtual QString viewerBGColor() const = 0;
  virtual int viewerMarkerType() const = 0;
  virtual int viewerMarkerSize() const = 0;
  virtual int viewerMarkerWidth() const = 0;
  virtual QString viewerMarkerColor() const = 0;
  virtual int viewerSelectMarkerWidth() const = 0;
  virtual QString viewerSelectMarkerColor() const = 0;
  virtual QString viewerLineColor() const = 0;
  virtual int viewerLineWidth() const = 0;

  virtual std::vector<QString> images() const = 0;
  virtual std::vector<QString> imagePairs(const QString &imageName) const = 0;
  virtual std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> loadMatches(const QString &imgName1, const QString &imgName2) const = 0;

public slots:

};

} // namespace graphos

#endif // GRAPHOS_MATCH_VIEWER_MODEL_INTERFACE_H