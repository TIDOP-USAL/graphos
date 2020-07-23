#ifndef INSPECTOR_FEATURE_MATCHING_PRESENTER_H
#define INSPECTOR_FEATURE_MATCHING_PRESENTER_H

#include <QObject>

#include "inspector/ui/featmatch/FeatureMatching.h"

namespace inspector
{

class FeatureMatchingWidget;

namespace ui
{

class SettingsModel;
class HelpDialog;

class FeatureMatchingPresenterImp
  : public FeatureMatchingPresenter
{

  Q_OBJECT

public:

  FeatureMatchingPresenterImp(FeatureMatchingView *view,
                              FeatureMatchingModel *model,
                              SettingsModel *settingsModel);
  ~FeatureMatchingPresenterImp() override;

private:

  void setMatchingProperties();

private slots:

  void onFinishMatching();

// FeatureMatchingPresenter interface

public slots:

  void setCurrentMatchMethod(const QString &matchMethod) override;

// ProcessPresenter interface
  
protected slots:

  void onError(int code, const QString &msg) override;
  void onFinished() override;
  void createProcess() override;

public slots:

  void cancel() override;

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  FeatureMatchingView *mView;
  FeatureMatchingModel *mModel;
  SettingsModel *mSettingsModel;
  HelpDialog *mHelp;
  FeatureMatchingWidget *mFeatureMatchingWidget;

};

} // End namespace ui

} // End namespace inspector

#endif // INSPECTOR_FEATURE_MATCHING_PRESENTER_H
