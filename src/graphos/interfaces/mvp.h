#ifndef GRAPHOS_MVP_H
#define GRAPHOS_MVP_H

#include <memory>

#include <QObject>
#include <QDialog>
#include <QEvent>

namespace graphos
{

namespace ui
{

class HelpDialog;
//class ProgressHandler;

class DialogView
  : public QDialog
{
  Q_OBJECT

public:

  DialogView(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
    : QDialog(parent, f)
  {
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~helpFlag);
    QWidget::setWindowFlags(flags);
  }

  virtual ~DialogView() override = default;

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



class Model
  : public QObject
{

  Q_OBJECT

public:

  Model(QObject *parent = nullptr) : QObject(parent){}
  virtual ~Model() = default;

private:

  /*!
   * \brief Class Initialization
   */
  virtual void init() = 0;

public slots:

  virtual void clear() = 0;

};


class Presenter
  : public QObject
{
  Q_OBJECT

public:

  Presenter() : QObject(){}
  virtual ~Presenter() = default;

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

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_MVP_H
