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

#ifndef GRAPHOS_MATCHES_MODEL_H
#define GRAPHOS_MATCHES_MODEL_H

#include "graphos/interfaces/mvp.h"
#include "graphos/core/project.h"

namespace graphos
{

class MatchesModel
  : public Model
{

  Q_OBJECT

public:

  MatchesModel(QObject *parent = nullptr) : Model(parent) {}
  ~MatchesModel() override = default;

  virtual void addMatchesPair(const QString &imageLeft, const QString &imageRight) = 0;
  virtual const std::vector<QString> matchesPairs(const QString &imageLeft) const = 0;
  virtual void removeMatchesPair() = 0;
  virtual void removeMatchesPair(const QString &imageLeft) = 0;

};


class MatchesModelImp
  : public MatchesModel
{

  Q_OBJECT

public:

  MatchesModelImp(Project *project,
                  QObject *parent = nullptr);
  ~MatchesModelImp() override = default;

// MatchesModel interface

public:

  void addMatchesPair(const QString &imageLeft, const QString &imageRight) override;
  const std::vector<QString> matchesPairs(const QString &imageLeft) const override;
  void removeMatchesPair() override;
  void removeMatchesPair(const QString &imageLeft) override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
};

} // namespace graphos

#endif // GRAPHOS_MATCHES_MODEL_H
