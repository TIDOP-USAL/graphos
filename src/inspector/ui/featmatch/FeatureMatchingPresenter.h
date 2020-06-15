#ifndef INSPECTOR_FEATURE_MATCHING_PRESENTER_H
#define INSPECTOR_FEATURE_MATCHING_PRESENTER_H

#include <QObject>

#include "inspector/ui/featmatch/FeatureMatching.h"

class MultiProcess;

namespace inspector
{

class FeatureMatchingWidget;

namespace ui
{

class SettingsModel;
class ProgressHandler;
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

  void onError(int code, const QString &msg);
  void onFinished();
  void onFinishMatching();

// FeatureMatchingPresenter interface

public slots:

  void setProgressHandler(ProgressHandler *progressHandler) override;
  void cancel() override;

private slots:

  void run() override;
  void setCurrentMatchMethod(const QString &matchMethod) override;

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
  MultiProcess *mMultiProcess;
  ProgressHandler *mProgressHandler;

};

} // End namespace ui

} // End namespace inspector

#endif // INSPECTOR_FEATURE_MATCHING_PRESENTER_H
