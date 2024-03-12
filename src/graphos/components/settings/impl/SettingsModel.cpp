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
    init();
}

SettingsModelImp::~SettingsModelImp()
{
    if (mSettingsController) {
        delete mSettingsController;
        mSettingsController = nullptr;
    }
}

QStringList SettingsModelImp::languages() const
{
    QString langPath = QApplication::applicationDirPath();
    langPath.append("/translations");

    QStringList lang_list = QDir(langPath).entryList(QStringList("graphos_*.qm"));
    if (lang_list.size() == 0)
        lang_list.push_back("graphos_en.qm");
    return lang_list;
}

QString SettingsModelImp::language() const
{
    return mSettings->language();
}

//QStringList SettingsModelImp::history() const
//{
//  return mSettings->history();
//}

int SettingsModelImp::historyMaxSize() const
{
    return mSettings->historyMaxSize();
}

QString SettingsModelImp::imageViewerBGcolor() const
{
    return mSettings->imageViewerBGcolor();
}

QString SettingsModelImp::keypointsViewerBGColor() const
{
    return mSettings->keypointsViewerBGColor();
}

int SettingsModelImp::keypointsViewerMarkerType() const
{
    return mSettings->keypointsViewerMarkerType();
}

int SettingsModelImp::keypointsViewerMarkerSize() const
{
    return mSettings->keypointsViewerMarkerSize();
}

int SettingsModelImp::keypointsViewerMarkerWidth() const
{
    return mSettings->keypointsViewerMarkerWidth();
}

QString SettingsModelImp::keypointsViewerMarkerColor() const
{
    return mSettings->keypointsViewerMarkerColor();
}

int SettingsModelImp::keypointsViewerSelectMarkerWidth() const
{
    return mSettings->keypointsViewerSelectMarkerWidth();
}

QString SettingsModelImp::keypointsViewerSelectMarkerColor() const
{
    return mSettings->keypointsViewerSelectMarkerColor();
}

QString SettingsModelImp::matchesViewerBGColor() const
{
    return mSettings->matchesViewerBGColor();
}

int SettingsModelImp::matchesViewerMarkerType() const
{
    return mSettings->matchesViewerMarkerType();
}

int SettingsModelImp::matchesViewerMarkerSize() const
{
    return mSettings->matchesViewerMarkerSize();
}

int SettingsModelImp::matchesViewerMarkerWidth() const
{
    return mSettings->matchesViewerMarkerWidth();
}

QString SettingsModelImp::matchesViewerMarkerColor() const
{
    return mSettings->matchesViewerMarkerColor();
}

int SettingsModelImp::matchesViewerSelectMarkerWidth() const
{
    return mSettings->matchesViewerSelectMarkerWidth();
}

QString SettingsModelImp::matchesViewerSelectMarkerColor() const
{
    return mSettings->matchesViewerSelectMarkerColor();
}

QString SettingsModelImp::matchesViewerLineColor() const
{
    return mSettings->matchesViewerLineColor();
}

int SettingsModelImp::matchesViewerLineWidth() const
{
    return mSettings->matchesViewerLineWidth();
}

bool SettingsModelImp::useCuda() const
{
    return mSettings->useCuda();
}

bool SettingsModelImp::checkDevice() const
{
    bool bUseGPU = false;

#ifdef HAVE_CUDA
    tl::Message::pauseMessages();
    bUseGPU = cudaEnabled(10.0, 3.0);
    tl::Message::resumeMessages();
#endif //HAVE_CUDA

    return bUseGPU;
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

//void SettingsModelImp::addToHistory(const QString &project)
//{
//  mSettings->addToHistory(project);
//  mSettingsController->writeHistory(*mSettings);
//}
//
//void SettingsModelImp::clearHistory()
//{
//  mSettings->clearHistory();
//  mSettingsController->writeHistory(*mSettings);
//}

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
