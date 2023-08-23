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

#include "graphos/core/settings.h"
#include "graphos/core/utils.h"

#include "tidop/core/msg/message.h"

#include <QSettings>
#include <QLocale>
#include <QFileInfo>
#include <QPen>

#include <memory>

namespace graphos
{

SettingsImp::SettingsImp()
  : Settings(),
    mHistoyMaxSize(20),
    mImageViewerBGcolor("#dcdcdc"),
    mKeypointViewerBGColor("#dcdcdc"),
    mKeypointsViewerMarkerType(2),
    mKeypointViewerMarkerSize(20),
    mKeypointViewerMarkerWidth(2),
    mKeypointViewerMarkerColor("#00aa00"),
    mKeypointViewerSelectMarkerWidth(2),
    mKeypointViewerSelectMarkerColor("#e5097e"),
    mMatchesViewerBGColor("#dcdcdc"),
    mMatchesViewerMarkerType(1),
    mMatchesViewerMarkerColor("#00aa00"),
    mMatchesViewerMarkerSize(20),
    mMatchesViewerMarkerWidth(2),
    mMatchesViewerSelectMarkerWidth(2),
    mMatchesViewerSelectMarkerColor("#e5097e"),
    mMatchesViewerLineColor("#0000ff"),
    mMatchesViewerLineWidth(2),
    mUseCuda(true)
{
    reset();
}

SettingsImp::~SettingsImp()
{

}

QString SettingsImp::language() const
{
    return mLanguage;
}

void SettingsImp::setLanguage(const QString &language)
{
    mLanguage = language;
}

//QStringList SettingsImp::history() const
//{
//  return mHistory;
//}
//
//void SettingsImp::addToHistory(const QString &project)
//{
//  mHistory.removeAll(project);
//  mHistory.prepend(project);
//
//  while (mHistory.size() > mHistoyMaxSize)
//    mHistory.removeLast();
//}

//void SettingsImp::clearHistory()
//{
//  mHistory.clear();
//}

int SettingsImp::historyMaxSize() const
{
    return mHistoyMaxSize;
}

void SettingsImp::setHistoryMaxSize(int maxSize)
{
    mHistoyMaxSize = maxSize;
}

QString SettingsImp::imageViewerBGcolor() const
{
    return mImageViewerBGcolor;
}

void SettingsImp::setImageViewerBGcolor(const QString &bgColor)
{
    mImageViewerBGcolor = bgColor;
}

QString SettingsImp::keypointsViewerBGColor() const
{
    return mKeypointViewerBGColor;
}

void SettingsImp::setKeypointsViewerBGColor(const QString &color)
{
    mKeypointViewerBGColor = color;
}

int SettingsImp::keypointsViewerMarkerType() const
{
    return mKeypointsViewerMarkerType;
}

void SettingsImp::setKeypointsViewerMarkerType(int type)
{
    mKeypointsViewerMarkerType = type;
}

int SettingsImp::keypointsViewerMarkerSize() const
{
    return mKeypointViewerMarkerSize;
}

void SettingsImp::setKeypointsViewerMarkerSize(int size)
{
    mKeypointViewerMarkerSize = size;
}

int SettingsImp::keypointsViewerMarkerWidth() const
{
    return mKeypointViewerMarkerWidth;
}

void SettingsImp::setKeypointsViewerMarkerWidth(int width)
{
    mKeypointViewerMarkerWidth = width;
}

QString SettingsImp::keypointsViewerMarkerColor() const
{
    return mKeypointViewerMarkerColor;
}

void SettingsImp::setKeypointsViewerMarkerColor(const QString &color)
{
    mKeypointViewerMarkerColor = color;
}

int SettingsImp::keypointsViewerSelectMarkerWidth() const
{
    return mKeypointViewerSelectMarkerWidth;
}

void SettingsImp::setKeypointsViewerSelectMarkerWidth(int width)
{
    mKeypointViewerSelectMarkerWidth = width;
}

QString SettingsImp::keypointsViewerSelectMarkerColor() const
{
    return mKeypointViewerSelectMarkerColor;
}

void SettingsImp::setKeypointsViewerSelectMarkerColor(const QString &color)
{
    mKeypointViewerSelectMarkerColor = color;
}

QString SettingsImp::matchesViewerBGColor() const
{
    return mMatchesViewerBGColor;
}

void SettingsImp::setMatchesViewerBGColor(const QString &color)
{
    mMatchesViewerBGColor = color;
}

int SettingsImp::matchesViewerMarkerType() const
{
    return mMatchesViewerMarkerType;
}

void SettingsImp::setMatchesViewerMarkerType(int type)
{
    mMatchesViewerMarkerType = type;
}

int SettingsImp::matchesViewerMarkerSize() const
{
    return mMatchesViewerMarkerSize;
}

void SettingsImp::setMatchesViewerMarkerSize(int size)
{
    mMatchesViewerMarkerSize = size;
}

int SettingsImp::matchesViewerMarkerWidth() const
{
    return mMatchesViewerMarkerWidth;
}

void SettingsImp::setMatchesViewerMarkerWidth(int width)
{
    mMatchesViewerMarkerWidth = width;
}

QString SettingsImp::matchesViewerMarkerColor() const
{
    return mMatchesViewerMarkerColor;
}

void SettingsImp::setMatchesViewerMarkerColor(const QString &color)
{
    mMatchesViewerMarkerColor = color;
}

int SettingsImp::matchesViewerSelectMarkerWidth() const
{
    return mMatchesViewerSelectMarkerWidth;
}

void SettingsImp::setMatchesViewerSelectMarkerWidth(int width)
{
    mMatchesViewerSelectMarkerWidth = width;
}

QString SettingsImp::matchesViewerSelectMarkerColor() const
{
    return mMatchesViewerSelectMarkerColor;
}

void SettingsImp::setMatchesViewerSelectMarkerColor(const QString &color)
{
    mMatchesViewerSelectMarkerColor = color;
}

QString SettingsImp::matchesViewerLineColor() const
{
    return mMatchesViewerLineColor;
}

void SettingsImp::setMatchesViewerLineColor(const QString &color)
{
    mMatchesViewerLineColor = color;
}

int SettingsImp::matchesViewerLineWidth() const
{
    return mMatchesViewerLineWidth;
}

void SettingsImp::setMatchesViewerLineWidth(int width)
{
    mMatchesViewerLineWidth = width;
}

bool SettingsImp::useCuda() const
{
    return mUseCuda;
}

void SettingsImp::setUseCuda(bool active)
{
    mUseCuda = active;
}

void SettingsImp::reset()
{
    mLanguage = "en";
    mHistoyMaxSize = 10;
    //mHistory.clear();
    mImageViewerBGcolor = "#dcdcdc";
    mKeypointViewerBGColor = "#dcdcdc";
    mKeypointsViewerMarkerType = 0;
    mKeypointViewerMarkerSize = 20;
    mKeypointViewerMarkerWidth = 2;
    mKeypointViewerMarkerColor = "#00aa00";
    mKeypointViewerSelectMarkerWidth = 2;
    mKeypointViewerSelectMarkerColor = "#e5097e";
    mMatchesViewerBGColor = "#dcdcdc";
    mMatchesViewerMarkerType = 0;
    mMatchesViewerMarkerColor = "#00aa00";
    mMatchesViewerMarkerSize = 20;
    mMatchesViewerMarkerWidth = 2;
    mMatchesViewerSelectMarkerWidth = 2;
    mMatchesViewerSelectMarkerColor = "#e5097e";
    mMatchesViewerLineColor = "#0000ff";
    mMatchesViewerLineWidth = 2;
    mUseCuda = false;
}


/*----------------------------------------------------------------*/


SettingsControllerImp::SettingsControllerImp()
  : SettingsController(),
    mSettingsController(nullptr)
{
    mSettingsController = new QSettings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
}

SettingsControllerImp::~SettingsControllerImp()
{
    if (mSettingsController) {
        delete mSettingsController;
        mSettingsController = nullptr;
    }
}

void SettingsControllerImp::read(Settings &settings)
{
    QString lang = QLocale::system().name();
    lang.truncate(lang.lastIndexOf('_'));
    settings.setLanguage(mSettingsController->value("lang", lang).toString());

    settings.setHistoryMaxSize(mSettingsController->value("HISTORY/MaxSize", settings.historyMaxSize()).toInt());
    //QStringList history = mSettingsController->value("HISTORY/RecentProjects", settings.history()).toStringList();
    //settings.clearHistory();
    //for(auto &prj : history){
    //  if (QFileInfo(prj).exists()){
    //    settings.addToHistory(prj);
    //  }
    //}

    settings.setImageViewerBGcolor(mSettingsController->value("ImageViewer/BackgroundColor", settings.imageViewerBGcolor()).toString());

    /* Keypoints Viewer */
    settings.setKeypointsViewerBGColor(mSettingsController->value("KeypointsViewer/BackgroundColor", settings.keypointsViewerBGColor()).toString());
    settings.setKeypointsViewerMarkerType(mSettingsController->value("KeypointsViewer/Type", settings.keypointsViewerMarkerType()).toInt());
    settings.setKeypointsViewerMarkerSize(mSettingsController->value("KeypointsViewer/MarkerSize", settings.keypointsViewerMarkerSize()).toInt());
    settings.setKeypointsViewerMarkerWidth(mSettingsController->value("KeypointsViewer/MarkerWidth", settings.keypointsViewerMarkerWidth()).toInt());
    settings.setKeypointsViewerMarkerColor(mSettingsController->value("KeypointsViewer/MarkerColor", settings.keypointsViewerMarkerColor()).toString());
    settings.setKeypointsViewerSelectMarkerWidth(mSettingsController->value("KeypointsViewer/SelectMarkerWidth", settings.keypointsViewerSelectMarkerWidth()).toInt());
    settings.setKeypointsViewerSelectMarkerColor(mSettingsController->value("KeypointsViewer/SelectMarkerColor", settings.keypointsViewerSelectMarkerColor()).toString());

    /* Matches Viewer */
    settings.setMatchesViewerBGColor(mSettingsController->value("MatchesViewer/BackgroundColor", settings.matchesViewerBGColor()).toString());
    settings.setMatchesViewerMarkerType(mSettingsController->value("MatchesViewer/MarkerType", settings.matchesViewerMarkerType()).toInt());
    settings.setMatchesViewerMarkerSize(mSettingsController->value("MatchesViewer/MarkerSize", settings.matchesViewerMarkerSize()).toInt());
    settings.setMatchesViewerMarkerWidth(mSettingsController->value("MatchesViewer/MarkerWidth", settings.matchesViewerMarkerWidth()).toInt());
    settings.setMatchesViewerMarkerColor(mSettingsController->value("MatchesViewer/MarkerColor", settings.matchesViewerMarkerColor()).toString());
    settings.setMatchesViewerSelectMarkerWidth(mSettingsController->value("MatchesViewer/SelectMarkerWidth", settings.matchesViewerSelectMarkerWidth()).toInt());
    settings.setMatchesViewerSelectMarkerColor(mSettingsController->value("MatchesViewer/SelectMarkerColor", settings.matchesViewerSelectMarkerColor()).toString());
    settings.setMatchesViewerLineColor(mSettingsController->value("MatchesViewer/LineColor", settings.matchesViewerLineColor()).toString());
    settings.setMatchesViewerLineWidth(mSettingsController->value("MatchesViewer/LineWidth", settings.matchesViewerLineWidth()).toInt());

    tl::Message::instance().pauseMessages();
    bool bUseGPU = cudaEnabled(10.0, 3.0);
    tl::Message::instance().resumeMessages();

    settings.setUseCuda(mSettingsController->value("UseCuda", bUseGPU).toBool());
}

void SettingsControllerImp::write(const Settings &settings)
{
    mSettingsController->setValue("lang", settings.language());

    mSettingsController->setValue("HISTORY/MaxSize", settings.historyMaxSize());
    //mSettingsController->setValue("HISTORY/RecentProjects", settings.history());

    mSettingsController->setValue("ImageViewer/BackgroundColor", settings.imageViewerBGcolor());

    /* Keypoints Viewer */
    mSettingsController->setValue("KeypointsViewer/BackgroundColor", settings.keypointsViewerBGColor());
    mSettingsController->setValue("KeypointsViewer/Type", settings.keypointsViewerMarkerType());
    mSettingsController->setValue("KeypointsViewer/MarkerSize", settings.keypointsViewerMarkerSize());
    mSettingsController->setValue("KeypointsViewer/MarkerWidth", settings.keypointsViewerMarkerWidth());
    mSettingsController->setValue("KeypointsViewer/MarkerColor", settings.keypointsViewerMarkerColor());
    mSettingsController->setValue("KeypointsViewer/SelectMarkerWidth", settings.keypointsViewerSelectMarkerWidth());
    mSettingsController->setValue("KeypointsViewer/SelectMarkerColor", settings.keypointsViewerSelectMarkerColor());

    /* Matches Viewer */
    mSettingsController->setValue("MatchesViewer/BackgroundColor", settings.matchesViewerBGColor());
    mSettingsController->setValue("MatchesViewer/MarkerType", settings.matchesViewerMarkerType());
    mSettingsController->setValue("MatchesViewer/MarkerSize", settings.matchesViewerMarkerSize());
    mSettingsController->setValue("MatchesViewer/MarkerWidth", settings.matchesViewerMarkerWidth());
    mSettingsController->setValue("MatchesViewer/MarkerColor", settings.matchesViewerMarkerColor());
    mSettingsController->setValue("MatchesViewer/SelectMarkerWidth", settings.matchesViewerSelectMarkerWidth());
    mSettingsController->setValue("MatchesViewer/SelectMarkerColor", settings.matchesViewerSelectMarkerColor());
    mSettingsController->setValue("MatchesViewer/LineColor", settings.matchesViewerLineColor());
    mSettingsController->setValue("MatchesViewer/LineWidth", settings.matchesViewerLineWidth());

    mSettingsController->setValue("UseCuda", settings.useCuda());
}

//void SettingsControllerImp::writeHistory(const Settings &settings)
//{
//  mSettingsController->setValue("HISTORY/RecentProjects", settings.history());
//}

} // namespace graphos
