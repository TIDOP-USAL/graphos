#ifndef INSPECTOR_SETTINGS_VIEW_H
#define INSPECTOR_SETTINGS_VIEW_H

#include "inspector/ui/Settings.h"

class QListWidget;
class QStackedWidget;
class QComboBox;
class QDialogButtonBox;
class QTabWidget;
class QGridLayout;
class QSpinBox;
class QLineEdit;
class QCheckBox;
class QLabel;

namespace inspector
{

namespace ui
{


class SettingsViewImp
  : public SettingsView
{
  Q_OBJECT

public:

  SettingsViewImp(QWidget *parent = nullptr);
  ~SettingsViewImp() override;

protected slots:

  void onPushButtonImageViewerBGColorClicked();

// IDialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

// ISettingsView interface

public:

  QString activeLanguage() const override;
  int historyMaxSize() const override;
  QString imageViewerBGColor() const override;
  bool useCuda() const override;

public slots:

  void setPage(int page) override;
  void setLanguages(const QStringList &languages) override;
  void setActiveLanguage(const QString &language) override;
  void setHistoryMaxSize(int size) override;
  void setImageViewerBGcolor(const QString &color) override;
  void setUseCuda(bool active) override;
  void setCudaEnabled(bool enabled) override;
  void setUnsavedChanges(bool unsaveChanges) override;

protected:

  QListWidget *mListWidget;
  QStackedWidget *mStackedWidget;
  QLabel *mLabelLanguages;
  QComboBox *mLanguages;
  QLabel *mLabelHistoryMaxSize;
  QSpinBox *mHistoryMaxSize;
  QTabWidget *mTabWidgetTools;
  QLabel *mLabelUseCuda;
  QCheckBox *mCheckBoxUseCuda;
  QLabel *mLabelImageViewerBGcolor;
  QLineEdit *mLineEditImageViewerBGcolor;
  QPushButton *mPushButtonImageViewerBGcolor;
  QDialogButtonBox *mButtonBox;

  bool bUnsaveChanges;
};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_SETTINGS_VIEW_H
