#ifndef INSPECTOR_FEATURE_MATCHING_VIEW_H
#define INSPECTOR_FEATURE_MATCHING_VIEW_H

#include "inspector/ui/featmatch/FeatureMatching.h"

class QGridLayout;
class QLabel;
class QComboBox;
class QDialogButtonBox;

namespace inspector
{

namespace ui
{

class FeatureMatchingDialog;

class FeatureMatchingViewImp
  : public FeatureMatchingView
{
  Q_OBJECT

public:

  explicit FeatureMatchingViewImp(QWidget *parent = nullptr);
  ~FeatureMatchingViewImp() override;

// FeatureMatchingView interface

public:

  void addMatchMethod(QWidget *matchMethod) override;
  QString currentMatchMethod() const override;

public slots:

  void setCurrentMatchMethod(const QString &matchMethodName) override;

// IDialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QLabel *mLabelMatcher;
  QComboBox *mComboBoxMatcher;
  QGridLayout *mGridLayoutMatcher;
  QDialogButtonBox *mButtonBox;

};

} // End namespace ui

} // End namespace inspector

#endif // INSPECTOR_FEATURE_MATCHING_VIEW_H
