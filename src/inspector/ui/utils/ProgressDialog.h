#ifndef INSPECTOR_PROGRESS_DIALOG_H
#define INSPECTOR_PROGRESS_DIALOG_H

#include <QDialog>
#include <QTextEdit>

#include "inspector/interfaces/mvp.h"
#include "inspector/process/Process.h"

namespace Ui
{
  class ProgressDialog;
}

namespace inspector
{

namespace ui
{
	
class ProgressDialog
  : public IDialogView
{
  Q_OBJECT

public:

  explicit ProgressDialog(QWidget *parent = nullptr) : IDialogView(parent){}
  virtual ~ProgressDialog() = default;

public slots:

  virtual void setRange(int min, int max) = 0;
  virtual void setValue(int value) = 0;
  virtual void setInitialized() = 0;
  virtual void setFinished() = 0;
  virtual void setTitle(QString text) = 0;
  virtual void setStatusText(QString text) = 0;

protected slots:

  virtual void onMinimized() = 0;

signals:

  void cancel();

};



class ProgressDialogImp
  : public ProgressDialog
{
  Q_OBJECT

public:

  explicit ProgressDialogImp(QWidget *parent = nullptr);
  ~ProgressDialogImp()  override;

// ProgressDialog interface

protected slots:

  void onPushButtonCancelClicked();

public slots:

  void setRange(int min, int max) override;
  void setValue(int value) override;
  void setInitialized() override;
  void setFinished() override;
  void setTitle(QString title) override;
  void setStatusText(QString text) override;

protected slots:

  void onMinimized() override;

// IDialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

private:

  Ui::ProgressDialog *ui;
  Process *mProcess;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_PROGRESS_DIALOG_H
