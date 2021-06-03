#ifndef INSPECTOR_ORIENTATION_VIEW_H
#define INSPECTOR_ORIENTATION_VIEW_H

#include "inspector/ui/orientation/OrientationView.h"

class QCheckBox;
class QDialogButtonBox;

namespace inspector
{

namespace ui
{

class OrientationViewImp
  : public OrientationView
{

public:

  OrientationViewImp(QWidget *parent = nullptr);
  ~OrientationViewImp() override = default;

// OrientationView interface

public:

  bool refinePrincipalPoint() const override;
  bool absoluteOrientation() const override;
  bool isEnabledAbsoluteOrientation() const override;

public slots:

  void setRefinePrincipalPoint(bool refine) override;
  void setAbsoluteOrientation(bool active) override;
  void enabledAbsoluteOrientation(bool enabled) override;

// DialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QCheckBox *mCheckBoxRefinePrincipalPoint;
  QCheckBox *mCheckBoxAbsoluteOrientation;
  QDialogButtonBox *mButtonBox;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURE_EXTRACTOR_VIEW_H
