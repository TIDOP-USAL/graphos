#ifndef INSPECTOR_SETTINGS_PRESENTER_H
#define INSPECTOR_SETTINGS_PRESENTER_H

#include <QObject>

#include "inspector/ui/Settings.h"

namespace inspector
{

class HelpDialog;

namespace ui
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
  ~SettingsPresenterImp() override;

public slots:

  void openViewSettings() override;
  void openToolSettings() override;

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

// ISettingsPresenter interface

private slots:

  void setLanguage(const QString &language) override;
  void save() override;
  void discart() override;

protected:

  SettingsView *mView;
  SettingsModel *mModel;
  HelpDialog *mHelp;
  std::map<QString, QString> mLang;
};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_SETTINGS_PRESENTER_H
