#ifndef GRAPHOS_CONSOLE_WIDGET_H
#define GRAPHOS_CONSOLE_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"

#include <mutex>

#include <tidop/core/defs.h>
#include <tidop/core/flags.h>
#include <tidop/core/messages.h>
#include <tidop/core/log.h>

class QDateTimeEdit;
class QListWidget;
class QGridLayout;

namespace graphos
{

class GRAPHOS_EXPORT LogWidget
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

  void print(const std::string &msg, const std::string &date, tl::MessageLevel level);
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

  void onMsgDebug(const std::string &msg, const std::string &date) override;
  void onMsgInfo(const std::string &msg, const std::string &date) override;
  void onMsgWarning(const std::string &msg, const std::string &date) override;
  void onMsgError(const std::string &msg, const std::string &date) override;

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

} // namespace graphos

#endif // GRAPHOS_CONSOLE_WIDGET_H
