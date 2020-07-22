#ifndef INSPECTOR_MVP_H
#define INSPECTOR_MVP_H

#include <memory>

#include <QObject>
#include <QDialog>
#include <QEvent>

namespace inspector
{

namespace ui
{

class HelpDialog;
class ProgressHandler;

class IDialogView
  : public QDialog
{
  Q_OBJECT

public:

  IDialogView(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
    : QDialog(parent, f)
  {
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~helpFlag);
    QWidget::setWindowFlags(flags);
  }

  virtual ~IDialogView() override = default;

private:

  /*!
   * \brief Dialog initialization UI
   */
  virtual void initUI() = 0;

  /*!
   * \brief Signal and slots initialization
   */
  virtual void initSignalAndSlots() = 0;

signals:

  void help();
  void restore();

public slots:

  /*!
   * \brief Clear dialog
   */
  virtual void clear() = 0;

private slots:

  virtual void update() = 0;
  virtual void retranslate() = 0;

// QWidget interface

protected:

  void changeEvent(QEvent *event) override
  {
    if (event->type() == QEvent::LanguageChange){
      retranslate();
    }
  }
};



class IModel
  : public QObject
{

  Q_OBJECT

public:

  IModel(QObject *parent = nullptr) : QObject(parent){}
  virtual ~IModel() = default;

private:

  /*!
   * \brief Class Initialization
   */
  virtual void init() = 0;

public slots:

  virtual void clear() = 0;

};


class IPresenter
  : public QObject
{
  Q_OBJECT

public:

  IPresenter() : QObject(){}
  virtual ~IPresenter() = default;

public slots:

 /*!
  * \brief Show help
  */
  virtual void help() = 0;

 /*!
  * \brief Open
  */
  virtual void open() = 0;

  /*!
   * \brief Set Help
   * \param[in] help
   */
  virtual void setHelp(HelpDialog *help) = 0;

private:

  /*!
   * \brief Class Initialization
   */
  virtual void init() = 0;

  /*!
   * \brief Signal and slots initialization
   */
  virtual void initSignalAndSlots() = 0;
};


class ProcessPresenter
  : public IPresenter
{

  Q_OBJECT

public:

  ProcessPresenter() {}
  ~ProcessPresenter() override = default;

private slots:

  //TODO: el código es el mismo salvo setDescription
  virtual void onError(int code, const QString &msg) = 0;
  virtual void onFinished() = 0;

public slots:

  virtual void setProgressHandler(ProgressHandler *progressHandler) = 0;
  virtual void cancel() = 0;

private slots:

  virtual void run() = 0;

signals:

  void running();
  void finished();

};



} // namespace ui

} // namespace inspector

#endif // INSPECTOR_MVP_H
