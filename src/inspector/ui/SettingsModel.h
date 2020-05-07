#ifndef INSPECTOR_SETTINGS_MODEL_H
#define INSPECTOR_SETTINGS_MODEL_H

#include <QObject>

#include "inspector/core/settings.h"
#include "inspector/ui/Settings.h"

namespace inspector
{

namespace ui
{


class SettingsModelImp
  : public SettingsModel
{
  Q_OBJECT

public:

  SettingsModelImp(Settings *settings,
                   SettingsController *settingsRW,
                   QObject *parent = nullptr);
  ~SettingsModelImp() override;

// SettingsModel interface

public:

  QStringList languages() const override;
  QString language() const override;
  QStringList history() const override;
  int historyMaxSize() const override;
  QString imageViewerBGcolor() const override;
  bool useCuda() const override;
  void read() override;
  void write() override;

public slots:

  void setLanguage(const QString &language) override;
  void addToHistory(const QString &project) override;
  void clearHistory() override;
  void setHistoryMaxSize(int maxSize) override;
  void setImageViewerBGcolor(const QString &bgColor) override;
  void setUseCuda(bool active) override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Settings *mSettings;
  SettingsController *mSettingsController;

};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_SETTINGS_MODEL_H
