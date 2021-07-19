#ifndef INSPECTOR_ORIENTATION_VIEW_INTERFACE_H
#define INSPECTOR_ORIENTATION_VIEW_INTERFACE_H

#include "inspector/interfaces/mvp.h"


namespace inspector
{

namespace ui
{

class OrientationView
  : public DialogView
{

  Q_OBJECT

public:

  OrientationView(QWidget *parent = nullptr) : DialogView(parent) {}
  ~OrientationView() override = default;

  virtual bool refinePrincipalPoint() const = 0;
  virtual bool absoluteOrientation() const = 0;
  virtual bool isEnabledAbsoluteOrientation() const = 0;

public slots:

  virtual void setRefinePrincipalPoint(bool refine) = 0;
  virtual void setAbsoluteOrientation(bool active) = 0;
  virtual void enabledAbsoluteOrientation(bool enabled) = 0;

signals:

  void run();
  void refinePrincipalPoint(bool);
  void absoluteOrientationChange(bool);
  void enabledAbsoluteOrientationChange(bool);
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURE_EXTRACTOR_VIEW_INTERFACE_H
