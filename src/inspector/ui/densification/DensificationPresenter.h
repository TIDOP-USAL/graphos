#ifndef INSPECTOR_DENSE_PRESENTER_H
#define INSPECTOR_DENSE_PRESENTER_H

#include <QObject>

#include "inspector/ui/densification/Densification.h"

#include <memory>

class MultiProcess;

namespace inspector
{

class CmvsPmvsWidget;
class SmvsWidget;

namespace ui
{

class HelpDialog;
class ProgressHandler;

class DensificationPresenterImp
  : public DensificationPresenter
{
  Q_OBJECT

public:

  DensificationPresenterImp(DensificationView *view,
                    DensificationModel *model);
  ~DensificationPresenterImp() override;

private:

  void setCmvsPmvsProperties();
  void setSmvsProperties();

private slots:

  void onDensificationChanged(const QString &densification);
  void onError(int code, const QString &msg);
  void onFinished();
  void onFinishDensification();

// DensificationPresenter interface

public slots:

  void setProgressHandler(ProgressHandler *progressHandler) override;
  void cancel() override;

private slots:

  void run() override;
  void setCurrentDensifier(const QString &desnsifier) override;

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  DensificationView *mView;
  DensificationModel *mModel;
  CmvsPmvsWidget *mCmvsPmvs;
  SmvsWidget *mSmvs;
  MultiProcess *mMultiProcess;
  ProgressHandler *mProgressHandler;
  HelpDialog *mHelp;
};

} // End namespace ui

} // End namespace inspector


#endif // INSPECTOR_DENSE_PRESENTER_H
