#ifndef INSPECTOR_CONSOLE_WIDGET_H
#define INSPECTOR_CONSOLE_WIDGET_H

#include "inspector/inspector_global.h"

#include "inspector/widgets/InspectorWidget.h"

#include <mutex>

#include <tidop/core/defs.h>
#include <tidop/core/flags.h>
#include <tidop/core/messages.h>

class QDateTimeEdit;
class QListWidget;
class QGridLayout;

namespace inspector
{

class INSPECTOR_EXPORT LogWidget
  : public IWidgetView,
    public tl::MessageManager::Listener
{
  Q_OBJECT

public:

  explicit LogWidget(QWidget *parent = nullptr);
  ~LogWidget() override;

  /*!
   * \brief Filter the messages displayed in the console
   * \param[in] level Log level
   */
  void filter(tl::MessageLevel level);

  /*!
   * \brief Set log level
   * \param[in] level Log level
   */
  void setLogLevel(tl::MessageLevel level);

  void print(const char *msg, const char *date, tl::MessageLevel level);
  void refresh();

private slots:

  void onPushButtonShowLogWarningToggled(bool active);
  void onPushButtonShowLogErrorsToggled(bool active);
  void onPushButtonShowLogInfoToggled(bool active);
  void onPushButtonShowLogDebugToggled(bool active);
  void onRowsInserted(const QModelIndex &parent,int start,int end,QPrivateSignal);
  void onRowsRemoved(const QModelIndex &parent,int start,int end,QPrivateSignal);

// IWidgetView interface

protected slots:

  void update() override;
  void retranslate() override;

public slots:

  void clear() override;

private:

  void initUI() override;
  void initSignalAndSlots() override;

// Listener interface

public:

  void onMsgDebug(const char *msg, const char *date) override;
  void onMsgInfo(const char *msg, const char *date) override;
  void onMsgWarning(const char *msg, const char *date) override;
  void onMsgError(const char *msg, const char *date) override;

private:

  static tl::EnumFlags<tl::MessageLevel> sLevel;
  static tl::EnumFlags<tl::MessageLevel> sFilterLevel;
  static std::mutex mtx;

  QGridLayout *mGridLayout;
  QListWidget *mListWidget;
  QAction* mMsgErrorAction;
  QAction* mMsgWarningAction;
  QAction* mMsgInfoAction;
  QAction* mClearAction;

};

} // namespace inspector

#endif // INSPECTOR_CONSOLE_WIDGET_H
