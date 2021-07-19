#ifndef INSPECTOR_FEATURES_VIEWER_PRESENTER_H
#define INSPECTOR_FEATURES_VIEWER_PRESENTER_H

#include <memory>

#include <QObject>

#include "inspector/ui/featviewer/FeaturesViewerPresenter.h"

namespace inspector
{

namespace ui
{

class FeaturesViewerView;
class FeaturesViewerModel;
class SettingsModel;
class HelpDialog;

class FeaturesViewerPresenterImp
  : public FeaturesViewerPresenter
{

  Q_OBJECT

public:

  FeaturesViewerPresenterImp(FeaturesViewerView *view,
                             FeaturesViewerModel *model);
  ~FeaturesViewerPresenterImp() override;

// FeaturesViewerPresenter interface

public slots:

  void setImageActive(const QString &image) override;
  //void openKeypointsFromImage(const QString &image) override;

protected slots:

  void loadKeypoints(const QString &image) override;

// Presenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  FeaturesViewerView *mView;
  FeaturesViewerModel *mModel;
  SettingsModel *mSettingsModel;
  HelpDialog *mHelp;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURES_VIEWER_PRESENTER_H
