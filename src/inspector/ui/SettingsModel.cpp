#include "SettingsModel.h"

#include <QDir>
#include <QSettings>
#include <QLocale>
#include <QApplication>

namespace inspector
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

bool SettingsModelImp::useCuda() const
{
  return mSettings->useCuda();
}

void inspector::ui::SettingsModelImp::read()
{
  mSettingsController->read(*mSettings);
}

void inspector::ui::SettingsModelImp::write()
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

} // namespace inspector
