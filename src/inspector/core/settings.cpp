#include "settings.h"

#include <QSettings>
#include <QLocale>
#include <QFileInfo>
#include <QPen>

#include <memory>

namespace inspector
{

SettingsImp::SettingsImp()
  : Settings(),
    mHistoyMaxSize(10),
    mImageViewerBGcolor("#dcdcdc"),
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

QStringList SettingsImp::history() const
{
  return mHistory;
}

void SettingsImp::addToHistory(const QString &project)
{
  mHistory.removeAll(project);
  mHistory.prepend(project);

  while (mHistory.size() > mHistoyMaxSize)
    mHistory.removeLast();
}

void SettingsImp::clearHistory()
{
  mHistory.clear();
}

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
  mHistory.clear();
  mImageViewerBGcolor = "#dcdcdc";
  mUseCuda = false;
}


/*----------------------------------------------------------------*/


SettingsControllerImp::SettingsControllerImp()
  : SettingsController(),
    mSettingsController(nullptr)
{
  mSettingsController = new QSettings(QSettings::IniFormat, QSettings::UserScope, "TIDOP", "Inspector");
}

SettingsControllerImp::~SettingsControllerImp()
{
  if (mSettingsController){
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
  QStringList history = mSettingsController->value("HISTORY/RecentProjects", settings.history()).toStringList();
  settings.clearHistory();
  for(auto &prj : history){
    if (QFileInfo(prj).exists()){
      settings.addToHistory(prj);
    }
  }

  settings.setImageViewerBGcolor(mSettingsController->value("ImageViewer/BGColor", settings.imageViewerBGcolor()).toString());

  settings.setUseCuda(mSettingsController->value("General/UseCuda", settings.useCuda()).toBool());
}

void SettingsControllerImp::write(const Settings &settings)
{
  mSettingsController->setValue("lang", settings.language());

  mSettingsController->setValue("HISTORY/MaxSize", settings.historyMaxSize());
  mSettingsController->setValue("HISTORY/RecentProjects", settings.history());

  mSettingsController->setValue("ImageViewer/BGColor", settings.imageViewerBGcolor());

  mSettingsController->setValue("General/UseCuda", settings.useCuda());
}

void SettingsControllerImp::writeHistory(const Settings &settings)
{
  mSettingsController->setValue("HISTORY/RecentProjects", settings.history());
}

} // namespace inspector
