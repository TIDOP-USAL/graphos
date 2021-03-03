#ifndef INSPECTOR_FEATURE_MATCHING_VIEW_INTERFACE_H
#define INSPECTOR_FEATURE_MATCHING_VIEW_INTERFACE_H

#include "inspector/interfaces/mvp.h"

class QGridLayout;
class QLabel;
class QComboBox;
class QDialogButtonBox;

namespace inspector
{

namespace ui
{

class FeatureMatchingView
  : public IDialogView
{

  Q_OBJECT

public:

  FeatureMatchingView(QWidget *parent) : IDialogView(parent) {}
  ~FeatureMatchingView() override = default;

  virtual void addMatchMethod(QWidget *matchMethod) = 0;
  virtual QString currentMatchMethod() const = 0;

signals:

  void matchMethodChange(QString);
  void run();

public slots:

  virtual void setCurrentMatchMethod(const QString &matchMethodName) = 0;

};

} // End namespace ui

} // End namespace inspector

#endif // INSPECTOR_FEATURE_MATCHING_VIEW_INTERFACE_H
