#ifndef GRAPHOS_WIDGET_H
#define GRAPHOS_WIDGET_H

#include "graphos/graphos_global.h"

#include <memory>

#include <QWidget>
#include <QEvent>

namespace graphos
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
      this->retranslate();
    }
  }

};


} // namespace graphos

#endif // GRAPHOS_WIDGET_H
