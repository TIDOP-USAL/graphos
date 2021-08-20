#ifndef GRAPHOS_FEATURE_MATCHING_VIEW_INTERFACE_H
#define GRAPHOS_FEATURE_MATCHING_VIEW_INTERFACE_H

#include "graphos/interfaces/mvp.h"

class QGridLayout;
class QLabel;
class QComboBox;
class QDialogButtonBox;

namespace graphos
{

namespace ui
{

class FeatureMatchingView
  : public DialogView
{

  Q_OBJECT

public:

  FeatureMatchingView(QWidget *parent) : DialogView(parent) {}
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

} // End namespace graphos

#endif // GRAPHOS_FEATURE_MATCHING_VIEW_INTERFACE_H
