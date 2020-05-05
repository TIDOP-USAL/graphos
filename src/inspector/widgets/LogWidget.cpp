#include "LogWidget.h"

#include <QToolBar>
#include <QGridLayout>
#include <QListWidgetItem>
#include <QAbstractItemModel>

using namespace tl;

namespace inspector
{

EnumFlags<MessageLevel> LogWidget::sLevel = MessageLevel::msg_verbose;
EnumFlags<MessageLevel> LogWidget::sFilterLevel = MessageLevel::msg_verbose;
std::mutex LogWidget::mtx;

LogWidget::LogWidget(QWidget *parent)
  : IWidgetView(parent),
    MessageManager::Listener(),
    mGridLayout(new QGridLayout(this))
{
  this->initUI();
  this->initSignalAndSlots();
  this->retranslate();
}

LogWidget::~LogWidget()
{

}

void LogWidget::filter(tl::MessageLevel level)
{
  sFilterLevel = level;
 
  mMsgErrorAction->setChecked(sFilterLevel.isActive(MessageLevel::msg_error));
  mMsgWarningAction->setChecked(sFilterLevel.isActive(MessageLevel::msg_warning));
  mMsgInfoAction->setChecked(sFilterLevel.isActive(MessageLevel::msg_info));

  refresh();
}

void LogWidget::setLogLevel(MessageLevel level)
{
  sLevel = level;
}

void LogWidget::print(const char *msg, const char *date, MessageLevel level)
{
  std::lock_guard<std::mutex> lck(LogWidget::mtx);

  QString _msg = QString(date) + " " + msg;

  QListWidgetItem *qListWidgetItem = new QListWidgetItem(_msg);
  qListWidgetItem->setData(Qt::UserRole, QVariant(static_cast<int>(level)));

  if (level == MessageLevel::msg_error)
    qListWidgetItem->setForeground(Qt::red);
  else if (level == MessageLevel::msg_warning)
    qListWidgetItem->setForeground(Qt::magenta);

  mListWidget->insertItem(mListWidget->count(), qListWidgetItem);

  if (!sFilterLevel.isActive(level)) {
    mListWidget->setRowHidden(mListWidget->count() - 1, true);
  }
}

void LogWidget::refresh()
{
  std::lock_guard<std::mutex> lck(LogWidget::mtx);
  MessageLevel level;
  for (int i = 0; i < mListWidget->count(); i++) {
    QListWidgetItem *qListWidgetItem = mListWidget->item(i);
    level = static_cast<tl::MessageLevel>(qListWidgetItem->data(Qt::UserRole).toInt());
    mListWidget->setRowHidden(i, !sFilterLevel.isActive(level));
  }
}

void LogWidget::onPushButtonShowLogWarningToggled(bool active)
{
  sFilterLevel.activeFlag(MessageLevel::msg_warning, active);
  refresh();
}

void LogWidget::onPushButtonShowLogErrorsToggled(bool active)
{
  sFilterLevel.activeFlag(MessageLevel::msg_error, active);
  refresh();
}

void LogWidget::onPushButtonShowLogInfoToggled(bool active)
{
  sFilterLevel.activeFlag(MessageLevel::msg_info, active);
  refresh();
}

void LogWidget::onPushButtonShowLogDebugToggled(bool active)
{
  sFilterLevel.activeFlag(MessageLevel::msg_debug, active);
  refresh();
}

void LogWidget::onRowsInserted(const QModelIndex &parent, int start, int end, LogWidget::QPrivateSignal)
{
  update();
}

void LogWidget::onRowsRemoved(const QModelIndex &parent, int start, int end, LogWidget::QPrivateSignal)
{
  update();
}

void LogWidget::update()
{
  mClearAction->setEnabled(mListWidget->count() > 0);
}

void LogWidget::retranslate()
{

}

void LogWidget::clear()
{
  std::lock_guard<std::mutex> lck(LogWidget::mtx);
  mListWidget->clear();
  update();
}

void LogWidget::initUI()
{
  QToolBar *toolBar = new QToolBar(this);

  mMsgErrorAction = new QAction(QIcon(":/ico/48/img/material/48/icons8-delete.png"), tr("Show errors"), this);
  mMsgErrorAction->setStatusTip(tr("Show errors"));
  mMsgErrorAction->setCheckable(true);
  mMsgErrorAction->setChecked(true);
  toolBar->addAction(mMsgErrorAction);

  mMsgWarningAction = new QAction(QIcon(":/ico/48/img/material/48/icons8-error.png"), tr("Show warnings"), this);
  mMsgWarningAction->setStatusTip(tr("Show warnings"));
  mMsgWarningAction->setCheckable(true);
  mMsgWarningAction->setChecked(true);
  toolBar->addAction(mMsgWarningAction);

  mMsgInfoAction = new QAction(QIcon(":/ico/48/img/material/48/icons8-attention-sign.png"), tr("Show messages"), this);
  mMsgInfoAction->setStatusTip(tr("Show messages"));
  mMsgInfoAction->setCheckable(true);
  mMsgInfoAction->setChecked(true);
  toolBar->addAction(mMsgInfoAction);

  toolBar->addSeparator();

  mClearAction = new QAction(QIcon(":/ico/48/img/material/48/icons8-trash-can.png"), tr("Clean log"), this);
  mClearAction->setStatusTip(tr("Clean log"));
  toolBar->addAction(mClearAction);

  mGridLayout->setMargin(0);
  mGridLayout->addWidget(toolBar);
  mListWidget = new QListWidget(this);
  mGridLayout->addWidget(mListWidget);
}

void LogWidget::initSignalAndSlots()
{
  connect(mMsgErrorAction,      SIGNAL(toggled(bool)),                             this, SLOT(onPushButtonShowLogErrorsToggled(bool)));
  connect(mMsgWarningAction,    SIGNAL(toggled(bool)),                             this, SLOT(onPushButtonShowLogWarningToggled(bool)));
  connect(mMsgInfoAction,       SIGNAL(toggled(bool)),                             this, SLOT(onPushButtonShowLogInfoToggled(bool)));
  connect(mClearAction,         SIGNAL(triggered(bool)),                           this, SLOT(clear()));
  connect(mListWidget->model(), SIGNAL(rowsInserted(const QModelIndex &,int,int)), this, SLOT(onRowsInserted(const QModelIndex &,int,int)));
  connect(mListWidget->model(), SIGNAL(rowsRemoved(const QModelIndex &,int,int)),  this, SLOT(onRowsRemoved(const QModelIndex &,int,int)));
}

void LogWidget::onMsgDebug(const char *msg, const char *date)
{
  if (sLevel.isActive(MessageLevel::msg_debug)) {
    print(msg, date, MessageLevel::msg_debug);
  }
}

void LogWidget::onMsgInfo(const char *msg, const char *date)
{
  if (sLevel.isActive(MessageLevel::msg_info)) {
    print(msg, date, MessageLevel::msg_info);
  }
}

void LogWidget::onMsgWarning(const char *msg, const char *date)
{
  if (sLevel.isActive(MessageLevel::msg_warning)) {
    print(msg, date, MessageLevel::msg_warning);
  }
}

void LogWidget::onMsgError(const char *msg, const char *date)
{
  if (sLevel.isActive(MessageLevel::msg_error)) {
    print(msg, date, MessageLevel::msg_error);
  }
}

} // namespace inspector


