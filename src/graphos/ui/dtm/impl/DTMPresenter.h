#ifndef GRAPHOS_DTM_PRESENTER_H
#define GRAPHOS_DTM_PRESENTER_H

#include "graphos/graphos_global.h"

#include <QObject>

#include <opencv2/core.hpp>

#include "graphos/ui/dtm/DTMPresenter.h"


namespace graphos
{

class DtmInvDistWidget;
class DtmInvDistNNWidget;

namespace ui
{

class DtmView;
class DtmModel;
class HelpDialog;


class DtmPresenterImp
  : public DtmPresenter
{
  Q_OBJECT

public:

  DtmPresenterImp(DtmView *view,
                  DtmModel *model);
  ~DtmPresenterImp() override;

  void setDtmProperties();
  void setInvDistProperties();
  void setInvDistNNProperties();

// Presenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

// ProcessPresenter interface

protected slots:

  void onError(int code, const QString &msg) override;
  void onFinished() override;
  bool createProcess() override;

public slots:

  void cancel() override;

// DtmPresenter interface

public slots:

  void setCurrentDtmMethod(const QString &method) override;

protected:

  DtmView *mView;
  DtmModel *mModel;
  HelpDialog *mHelp;
  DtmInvDistWidget *mDtmInvDistWidget;
  DtmInvDistNNWidget *mDtmInvDistNNWidget;
};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_DTM_PRESENTER_H
