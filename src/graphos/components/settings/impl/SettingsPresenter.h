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
 
#ifndef GRAPHOS_SETTINGS_PRESENTER_H
#define GRAPHOS_SETTINGS_PRESENTER_H

#include "graphos/components/settings/SettingsPresenter.h"

namespace graphos
{

class SettingsView;
class SettingsModel;

class SettingsPresenterImp
  : public SettingsPresenter
{

  Q_OBJECT

public:

  SettingsPresenterImp(SettingsView *view,
                       SettingsModel *model);
  ~SettingsPresenterImp() override = default;

// Presenter interface

public slots:

  void open() override;

private:

  void init() override;
  void initSignalAndSlots() override;

// SettingsPresenter interface

private slots:

  void setLanguage(const QString &language) override;
  void save() override;
  void discart() override;

protected:

  SettingsView *mView;
  SettingsModel *mModel;
  std::map<QString, QString> mLang;

};


} // namespace graphos

#endif // GRAPHOS_SETTINGS_PRESENTER_H
