#ifndef INSPECTOR_WIDGET_H
#define INSPECTOR_WIDGET_H

#include <memory>

#include <QObject>
#include <QDialog>
#include <QEvent>

namespace inspector
{

class HelpDialog;


class IWidgetView
  : public QWidget
{

  Q_OBJECT

public:

  IWidgetView(QWidget *parent)
    : QWidget(parent)
  {
  }

  virtual ~IWidgetView() override {}

private:

  /*!
   * \brief Widget Initialization
   */
  virtual void initUI() = 0;
  virtual void initSignalAndSlots() = 0;

public slots:

  /*!
   * \brief Clear the widget
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


} // namespace inspector

#endif // INSPECTOR_WIDGET_H