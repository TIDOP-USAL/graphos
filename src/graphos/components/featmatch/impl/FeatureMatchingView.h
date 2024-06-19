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

#ifndef GRAPHOS_FEATURE_MATCHING_VIEW_H
#define GRAPHOS_FEATURE_MATCHING_VIEW_H

#include "graphos/components/featmatch/FeatureMatchingView.h"

class QGridLayout;
class QLabel;
class QComboBox;
class QDialogButtonBox;
class QCheckBox;

namespace graphos
{

class FeatureMatchingDialog;

class FeatureMatchingViewImp
  : public FeatureMatchingView
{
  Q_OBJECT

public:

  explicit FeatureMatchingViewImp(QWidget *parent = nullptr);
  ~FeatureMatchingViewImp() override;

// FeatureMatchingView interface

public:

  void addMatchMethod(QWidget *matchMethod) override;
  auto currentMatchMethod() const -> QString override;
  bool spatialMatching() const override;

public slots:

  void setCurrentMatchMethod(const QString &matchMethodName) override;
  void setSpatialMatching(bool active) override;
  void enabledSpatialMatching(bool enabled) override;

// DialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QLabel *mLabelMatcher;
  QComboBox *mComboBoxMatcher;
  QGridLayout *mGridLayoutMatcher;
  QCheckBox *mCheckBoxSpatialMatching;
  QDialogButtonBox *mButtonBox;

};

} // End namespace graphos

#endif // GRAPHOS_FEATURE_MATCHING_VIEW_H
