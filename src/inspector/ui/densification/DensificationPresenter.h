#ifndef INSPECTOR_DENSE_PRESENTER_H
#define INSPECTOR_DENSE_PRESENTER_H

#include <QObject>

#include "inspector/ui/densification/Densification.h"

#include <memory>

namespace inspector
{

class CmvsPmvsWidget;
class SmvsWidget;

namespace ui
{

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

  DensificationView *mView;
  DensificationModel *mModel;
  CmvsPmvsWidget *mCmvsPmvs;
  SmvsWidget *mSmvs;
  HelpDialog *mHelp;
};

} // End namespace ui

} // End namespace inspector


#endif // INSPECTOR_DENSE_PRESENTER_H
