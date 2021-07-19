#ifndef INSPECTOR_DENSE_VIEW_INTERFACE_H
#define INSPECTOR_DENSE_VIEW_INTERFACE_H

#include "inspector/interfaces/mvp.h"

class QGridLayout;
class QDialogButtonBox;
class QComboBox;
class QLabel;

namespace inspector
{

namespace ui
{

class DensificationView
  : public DialogView
{

  Q_OBJECT


public:

  DensificationView(QWidget *parent) : DialogView(parent) {}
  virtual ~DensificationView() {}

  virtual QString currentDensificationMethod() const = 0;

signals:

  void densificationChanged(QString);
  void run();

public slots:

  virtual void addDensification(QWidget *densification) = 0;
  virtual void setCurrentDensificationMethod(const QString &densificationMethod) = 0;

};

} // End namespace ui

} // End namespace inspector


#endif // INSPECTOR_DENSE_VIEW_INTERFACE_H
