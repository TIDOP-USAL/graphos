#ifndef INSPECTOR_ORIENTATION_PRESENTER_H
#define INSPECTOR_ORIENTATION_PRESENTER_H

#include "inspector/inspector_global.h"

#include "inspector/ui/orientation/Orientation.h"


class MultiProcess;

namespace inspector
{

class HelpDialog;
class ProgressHandler;

namespace ui
{

class OrientationPresenterImp
  : public OrientationPresenter
{
  Q_OBJECT

public:

  OrientationPresenterImp(OrientationView *view,
                          OrientationModel *model);
  ~OrientationPresenterImp() override;

private slots:

  void onError(int code, const QString &msg);
  void onFinished();
  void onOrientationFinished();

// OrientationPresenter interface

public slots:

  void setProgressHandler(ProgressHandler *progressHandler) override;
  void cancel() override;

private slots:

  void run() override;

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

protected:

  OrientationView *mView;
  OrientationModel *mModel;
  HelpDialog *mHelp;
  MultiProcess *mMultiProcess;
  ProgressHandler *mProgressHandler;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_ORIENTATION_PRESENTER_H
