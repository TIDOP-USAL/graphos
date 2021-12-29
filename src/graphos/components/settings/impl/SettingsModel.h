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

#ifndef GRAPHOS_SETTINGS_MODEL_H
#define GRAPHOS_SETTINGS_MODEL_H

#include <QObject>

#include "graphos/core/settings.h"
#include "graphos/components/settings/SettingsModel.h"

namespace graphos
{


class SettingsModelImp
  : public SettingsModel
{
  Q_OBJECT

public:

  SettingsModelImp(Settings *settings,
                   /*SettingsController *settingsRW,*/
                   QObject *parent = nullptr);
  ~SettingsModelImp() override;

// SettingsModel interface

public:

  QStringList languages() const override;
  QString language() const override;
  //QStringList history() const override;
  int historyMaxSize() const override;
  QString imageViewerBGcolor() const override;

  QString keypointsViewerBGColor() const override;
  int keypointsViewerMarkerType() const override;
  int keypointsViewerMarkerSize() const override;
  int keypointsViewerMarkerWidth() const override;
  QString keypointsViewerMarkerColor() const override;
  int keypointsViewerSelectMarkerWidth() const override;
  QString keypointsViewerSelectMarkerColor() const override;

  QString matchesViewerBGColor() const override;
  int matchesViewerMarkerType() const override;
  int matchesViewerMarkerSize() const override;
  int matchesViewerMarkerWidth() const override;
  QString matchesViewerMarkerColor() const override;
  int matchesViewerSelectMarkerWidth() const override;
  QString matchesViewerSelectMarkerColor() const override;
  QString matchesViewerLineColor() const override;
  int matchesViewerLineWidth() const override;

  bool useCuda() const override;
  bool checkDevice() const override;
  void read() override;
  void write() override;

public slots:

  void setLanguage(const QString &language) override;
  //void addToHistory(const QString &project) override;
  //void clearHistory() override;
  void setHistoryMaxSize(int maxSize) override;
  void setImageViewerBGcolor(const QString &bgColor) override;
  void setUseCuda(bool active) override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Settings *mSettings;
  SettingsController *mSettingsController;

};

} // namespace graphos

#endif // GRAPHOS_SETTINGS_MODEL_H
