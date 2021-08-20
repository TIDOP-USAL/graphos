#include "SettingsModel.h"

#include <QDir>
#include <QSettings>
#include <QLocale>
#include <QApplication>

namespace graphos
{

namespace ui
{


SettingsModelImp::SettingsModelImp(Settings *settings,
                                   SettingsController *settingsRW,
                                   QObject *parent)
  : SettingsModel(parent),
    mSettings(settings),
    mSettingsController(settingsRW)
{
  init();
}

SettingsModelImp::~SettingsModelImp()
{
}

QStringList SettingsModelImp::languages() const
{
  QString langPath = QApplication::applicationDirPath();
  langPath.append("/translations");

  QStringList lang_list = QDir(langPath).entryList(QStringList("photomatch_*.qm"));
  if (lang_list.size() == 0)
    lang_list.push_back("photomatch_en.qm");
  return lang_list;
}

QString SettingsModelImp::language() const
{
  return mSettings->language();
}

QStringList SettingsModelImp::history() const
{
  return mSettings->history();
}

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

void graphos::ui::SettingsModelImp::read()
{
  mSettingsController->read(*mSettings);
}

void graphos::ui::SettingsModelImp::write()
{
  mSettingsController->write(*mSettings);
  emit unsavedChanges(false);
}

void SettingsModelImp::setLanguage(const QString &language)
{
  mSettings->setLanguage(language);
  emit unsavedChanges(true);
}

void SettingsModelImp::addToHistory(const QString &project)
{
  mSettings->addToHistory(project);
  mSettingsController->writeHistory(*mSettings);
}

void SettingsModelImp::clearHistory()
{
  mSettings->clearHistory();
  mSettingsController->writeHistory(*mSettings);
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

void SettingsModelImp::init()
{
}

void SettingsModelImp::clear()
{
  mSettings->reset();
  emit unsavedChanges(false);
}

} // namespace ui

} // namespace graphos
