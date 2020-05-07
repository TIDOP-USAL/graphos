#ifndef INSPECTOR_START_PAGE_WIDGET_H
#define INSPECTOR_START_PAGE_WIDGET_H

#include "inspector/widgets/InspectorWidget.h"

class QLabel;
class QSplitter;
class QCommandLinkButton;
class QListWidget;

namespace inspector
{

class StartPageWidget
  : public IWidgetView
{

  Q_OBJECT

public:

  StartPageWidget(QWidget *parent = nullptr);
  ~StartPageWidget() override = default;

  void setHistory(const QStringList &history);

// IWidgetView interface

protected slots:

  void update() override;
  void retranslate() override;

public slots:

  void clear() override;

private:

  void initUI() override;
  void initSignalAndSlots() override;

signals:

  void openNew();
  void openProject();
  void openSettings();
  void clearHistory();
  void openProjectFromHistory(QString);

// QWidget interface

protected:

  void changeEvent(QEvent *event) override;

protected:

  QLabel *mLabelPhotoMatch;
  QLabel *mLabelRecentProjects;
  QSplitter *mSplitter;
  QCommandLinkButton *mCommandLinkButtonNewProject;
  QCommandLinkButton *mCommandLinkButtonOpenProject;
  QCommandLinkButton *mCommandLinkButtonSettings;
  QListWidget *mListWidgetRecentProjects;
  QCommandLinkButton *mCommandLinkButtonClearHistory;
};

} // namespace inspector

#endif // INSPECTOR_START_PAGE_WIDGET_H
