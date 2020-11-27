#ifndef INSPECTOR_DTM_VIEW_INTERFACE_H
#define INSPECTOR_DTM_VIEW_INTERFACE_H

#include "inspector/interfaces/mvp.h"


namespace inspector
{

namespace ui
{

class DtmView
  : public IDialogView
{

  Q_OBJECT

public:

  DtmView(QWidget *parent = nullptr) : IDialogView(parent) {}
  ~DtmView() override = default;
 
  virtual double gsd() const = 0;

  virtual void addDtmMethod(QWidget *method) = 0;
  virtual QString currentDtmMethod() const = 0;

signals:

  void dtmMethodChange(QString);
  void run();

public slots:

  virtual void setGSD(double gsd) = 0;
  virtual void setCurrentDtmMethod(const QString &method) = 0;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_DTM_VIEW_INTERFACE_H
