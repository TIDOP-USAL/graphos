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
                     QObject *parent = nullptr);
    ~SettingsModelImp() override;

// SettingsModel interface

public:

    auto languages() const -> QStringList override;
    auto language() const -> QString override;
    auto historyMaxSize() const -> int override;
    auto imageViewerBGcolor() const -> QString override;

    auto keypointsViewerBGColor() const -> QString override;
    auto keypointsViewerMarkerType() const -> int override;
    auto keypointsViewerMarkerSize() const -> int override;
    auto keypointsViewerMarkerWidth() const -> int override;
    auto keypointsViewerMarkerColor() const -> QString override;
    auto keypointsViewerSelectMarkerWidth() const -> int override;
    auto keypointsViewerSelectMarkerColor() const -> QString override;

    auto matchesViewerBGColor() const -> QString override;
    auto matchesViewerMarkerType() const -> int override;
    auto matchesViewerMarkerSize() const -> int override;
    auto matchesViewerMarkerWidth() const -> int override;
    auto matchesViewerMarkerColor() const -> QString override;
    auto matchesViewerSelectMarkerWidth() const -> int override;
    auto matchesViewerSelectMarkerColor() const -> QString override;
    auto matchesViewerLineColor() const -> QString override;
    auto matchesViewerLineWidth() const -> int override;

    auto useCuda() const -> bool override;
    auto checkDevice() const -> bool override;
    void read() override;
    void write() override;

public slots:

    void setLanguage(const QString &language) override;
    void setHistoryMaxSize(int maxSize) override;
    void setImageViewerBGcolor(const QString &bgColor) override;
    void setUseCuda(bool active) override;

    void setKeypointsViewerBGColor(const QString &color) override;
    void setKeypointsViewerMarkerType(int type) override;
    void setKeypointsViewerMarkerSize(int size) override;
    void setKeypointsViewerMarkerWidth(int width) override;
    void setKeypointsViewerMarkerColor(const QString &color) override;
    void setKeypointsViewerSelectMarkerWidth(int width) override;
    void setKeypointsViewerSelectMarkerColor(const QString &color) override;

    void setMatchesViewerBGColor(const QString &color) override;
    void setMatchesViewerMarkerType(int type) override;
    void setMatchesViewerMarkerSize(int size) override;
    void setMatchesViewerMarkerWidth(int width) override;
    void setMatchesViewerMarkerColor(const QString &color) override;
    void setMatchesViewerSelectMarkerWidth(int width) override;
    void setMatchesViewerSelectMarkerColor(const QString &color) override;
    void setMatchesViewerLineColor(const QString &color) override;
    void setMatchesViewerLineWidth(int width) override;

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
