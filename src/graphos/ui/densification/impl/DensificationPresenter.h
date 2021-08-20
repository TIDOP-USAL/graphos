#ifndef GRAPHOS_DENSE_PRESENTER_H
#define GRAPHOS_DENSE_PRESENTER_H

#include "graphos/ui/densification/DensificationPresenter.h"

namespace graphos
{

class CmvsPmvsWidget;
class SmvsWidget;

namespace ui
{

class DensificationView;
class DensificationModel;
class HelpDialog;

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
  void onFinishDensification();

// DensificationPresenter interface

public slots:

  void setCurrentDensifier(const QString &densifier) override;

// ProcessPresenter interface
  
protected slots:

  void onError(int code, const QString &msg) override;
  void onFinished() override;
  bool createProcess() override;

public slots:

  void cancel() override;

// Presenter interface

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
  HelpDialog *mHelp;
};

} // End namespace ui

} // End namespace graphos


#endif // GRAPHOS_DENSE_PRESENTER_H
