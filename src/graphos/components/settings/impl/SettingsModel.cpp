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

#include "SettingsModel.h"

#include "graphos/core/utils.h"

#include "tidop/core/msg/message.h"

#include <QDir>
#include <QSettings>
#include <QLocale>
#include <QApplication>

namespace graphos
{


SettingsModelImp::SettingsModelImp(Settings *settings,
                                   QObject *parent)
  : SettingsModel(parent),
    mSettings(settings),
    mSettingsController(new SettingsControllerImp)
{
    SettingsModelImp::init();
}

SettingsModelImp::~SettingsModelImp()
{
    if (mSettingsController) {
        delete mSettingsController;
        mSettingsController = nullptr;
    }
}

auto SettingsModelImp::languages() const -> QStringList
{
    QString lang_path = QApplication::applicationDirPath();
    lang_path.append("/translations");

    QStringList lang_list = QDir(lang_path).entryList(QStringList("graphos_*.qm"));
    if (lang_list.empty())
        lang_list.push_back("graphos_en.qm");
    return lang_list;
}

auto SettingsModelImp::language() const -> QString
{
    return mSettings->language();
}

auto SettingsModelImp::historyMaxSize() const -> int
{
    return mSettings->historyMaxSize();
}

auto SettingsModelImp::imageViewerBGcolor() const -> QString
{
    return mSettings->imageViewerBGcolor();
}

auto SettingsModelImp::keypointsViewerBGColor() const -> QString
{
    return mSettings->keypointsViewerBGColor();
}

auto SettingsModelImp::keypointsViewerMarkerType() const -> int
{
    return mSettings->keypointsViewerMarkerType();
}

auto SettingsModelImp::keypointsViewerMarkerSize() const -> int
{
    return mSettings->keypointsViewerMarkerSize();
}

auto SettingsModelImp::keypointsViewerMarkerWidth() const -> int
{
    return mSettings->keypointsViewerMarkerWidth();
}

auto SettingsModelImp::keypointsViewerMarkerColor() const -> QString
{
    return mSettings->keypointsViewerMarkerColor();
}

auto SettingsModelImp::keypointsViewerSelectMarkerWidth() const -> int
{
    return mSettings->keypointsViewerSelectMarkerWidth();
}

auto SettingsModelImp::keypointsViewerSelectMarkerColor() const -> QString
{
    return mSettings->keypointsViewerSelectMarkerColor();
}

auto SettingsModelImp::matchesViewerBGColor() const -> QString
{
    return mSettings->matchesViewerBGColor();
}

auto SettingsModelImp::matchesViewerMarkerType() const -> int
{
    return mSettings->matchesViewerMarkerType();
}

auto SettingsModelImp::matchesViewerMarkerSize() const -> int
{
    return mSettings->matchesViewerMarkerSize();
}

auto SettingsModelImp::matchesViewerMarkerWidth() const -> int
{
    return mSettings->matchesViewerMarkerWidth();
}

auto SettingsModelImp::matchesViewerMarkerColor() const -> QString
{
    return mSettings->matchesViewerMarkerColor();
}

auto SettingsModelImp::matchesViewerSelectMarkerWidth() const -> int
{
    return mSettings->matchesViewerSelectMarkerWidth();
}

auto SettingsModelImp::matchesViewerSelectMarkerColor() const -> QString
{
    return mSettings->matchesViewerSelectMarkerColor();
}

auto SettingsModelImp::matchesViewerLineColor() const -> QString
{
    return mSettings->matchesViewerLineColor();
}

auto SettingsModelImp::matchesViewerLineWidth() const -> int
{
    return mSettings->matchesViewerLineWidth();
}

auto SettingsModelImp::useCuda() const -> bool
{
    return mSettings->useCuda();
}

auto SettingsModelImp::checkDevice() const -> bool
{
    bool use_gpu = false;

#ifdef HAVE_CUDA
    tl::Message::pauseMessages();
    use_gpu = cudaEnabled(10.0, 3.0);
    tl::Message::resumeMessages();
#endif //HAVE_CUDA

    return use_gpu;
}

void SettingsModelImp::read()
{
    mSettingsController->read(*mSettings);
}

void SettingsModelImp::write()
{
    mSettingsController->write(*mSettings);
    emit unsavedChanges(false);
}

void SettingsModelImp::setLanguage(const QString &language)
{
    mSettings->setLanguage(language);
    emit unsavedChanges(true);
}

void SettingsModelImp::setHistoryMaxSize(int maxSize)
{
    mSettings->setHistoryMaxSize(maxSize);
    emit unsavedChanges(true);
}

void SettingsModelImp::setImageViewerBGcolor(const QString &bgColor)
{
    mSettings->setImageViewerBGcolor(bgColor);
    emit unsavedChanges(true);
}

void SettingsModelImp::setUseCuda(bool active)
{
    mSettings->setUseCuda(active);
    emit unsavedChanges(true);
}

void SettingsModelImp::setKeypointsViewerBGColor(const QString &color)
{
    mSettings->setKeypointsViewerBGColor(color);
    emit unsavedChanges(true);
}

void SettingsModelImp::setKeypointsViewerMarkerType(int type)
{
    mSettings->setKeypointsViewerMarkerType(type);
    emit unsavedChanges(true);
}

void SettingsModelImp::setKeypointsViewerMarkerSize(int size)
{
    mSettings->setKeypointsViewerMarkerSize(size);
    emit unsavedChanges(true);
}

void SettingsModelImp::setKeypointsViewerMarkerWidth(int width)
{
    mSettings->setKeypointsViewerMarkerWidth(width);
    emit unsavedChanges(true);
}

void SettingsModelImp::setKeypointsViewerMarkerColor(const QString &color)
{
    mSettings->setKeypointsViewerMarkerColor(color);
    emit unsavedChanges(true);
}

void SettingsModelImp::setKeypointsViewerSelectMarkerWidth(int width)
{
    mSettings->setKeypointsViewerSelectMarkerWidth(width);
    emit unsavedChanges(true);
}

void SettingsModelImp::setKeypointsViewerSelectMarkerColor(const QString &color)
{
    mSettings->setKeypointsViewerSelectMarkerColor(color);
    emit unsavedChanges(true);
}

void SettingsModelImp::setMatchesViewerBGColor(const QString &color)
{
    mSettings->setMatchesViewerBGColor(color);
    emit unsavedChanges(true);
}

void SettingsModelImp::setMatchesViewerMarkerType(int type)
{
    mSettings->setMatchesViewerMarkerType(type);
    emit unsavedChanges(true);
}

void SettingsModelImp::setMatchesViewerMarkerSize(int size)
{
    mSettings->setMatchesViewerMarkerSize(size);
    emit unsavedChanges(true);
}

void SettingsModelImp::setMatchesViewerMarkerWidth(int width)
{
    mSettings->setMatchesViewerMarkerWidth(width);
    emit unsavedChanges(true);
}

void SettingsModelImp::setMatchesViewerMarkerColor(const QString &color)
{
    mSettings->setMatchesViewerMarkerColor(color);
    emit unsavedChanges(true);
}

void SettingsModelImp::setMatchesViewerSelectMarkerWidth(int width)
{
    mSettings->setMatchesViewerSelectMarkerWidth(width);
    emit unsavedChanges(true);
}

void SettingsModelImp::setMatchesViewerSelectMarkerColor(const QString &color)
{
    mSettings->setMatchesViewerSelectMarkerColor(color);
    emit unsavedChanges(true);
}

void SettingsModelImp::setMatchesViewerLineColor(const QString &color)
{
    mSettings->setMatchesViewerLineColor(color);
    emit unsavedChanges(true);
}

void SettingsModelImp::setMatchesViewerLineWidth(int width)
{
    mSettings->setMatchesViewerLineWidth(width);
    emit unsavedChanges(true);
}


void SettingsModelImp::init()
{
    read();
}

void SettingsModelImp::clear()
{
    mSettings->reset();
    emit unsavedChanges(false);
}

} // namespace graphos
