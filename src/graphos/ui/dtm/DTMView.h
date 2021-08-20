#ifndef GRAPHOS_DTM_VIEW_INTERFACE_H
#define GRAPHOS_DTM_VIEW_INTERFACE_H

#include "graphos/interfaces/mvp.h"


namespace graphos
{

namespace ui
{

class DtmView
  : public DialogView
{

  Q_OBJECT

public:

  DtmView(QWidget *parent = nullptr) : DialogView(parent) {}
  ~DtmView() override = default;
 
  virtual double gsd() const = 0;

  virtual void addDtmMethod(QWidget *method) = 0;
  virtual QString currentDtmMethod() const = 0;
  virtual bool isDSM() const = 0;

signals:

  void dtmMethodChange(QString);
  void run();

public slots:

  virtual void setGSD(double gsd) = 0;
  virtual void setCurrentDtmMethod(const QString &method) = 0;
  virtual void setDSM(bool active) = 0;
};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_DTM_VIEW_INTERFACE_H
