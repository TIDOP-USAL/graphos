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

#ifndef GRAPHOS_FEATURES_MODEL_H
#define GRAPHOS_FEATURES_MODEL_H

#include "graphos/interfaces/mvp.h"
#include "graphos/core/project.h"

namespace graphos
{

class FeaturesModel
  : public Model
{

  Q_OBJECT

public:

  typedef std::map<QString, QString>::iterator features_iterator;
  typedef std::map<QString, QString>::const_iterator features_const_iterator;


public:

  FeaturesModel(QObject *parent = nullptr) : Model(parent) {}
  virtual ~FeaturesModel() override = default;

  virtual QString features(const QString &imgName) const = 0;
  virtual void addFeatures(const QString &imgName, const QString &featureFile) = 0;
  virtual bool removeFeatures(const QString &imgName) const = 0;

  virtual features_iterator begin() = 0;
  virtual features_const_iterator begin() const = 0;
  virtual features_iterator end() = 0;
  virtual features_const_iterator end() const = 0;

};


class FeaturesModelImp
  : public FeaturesModel
{

  Q_OBJECT

public:

  FeaturesModelImp(Project *project,
                   QObject *parent = nullptr);
  ~FeaturesModelImp() override = default;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

// FeaturesModel interface

public:

  QString features(const QString &imgName) const override;
  void addFeatures(const QString &imgName, const QString &featureFile) override;
  bool removeFeatures(const QString &imgName) const override;

  features_iterator begin() override;
  features_const_iterator begin() const override;
  features_iterator end() override;
  features_const_iterator end() const override;

protected:

  Project *mProject;
};

} // namespace graphos

#endif // GRAPHOS_FEATURES_MODEL_H
