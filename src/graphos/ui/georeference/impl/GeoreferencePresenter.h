#ifndef GRAPHOS_GEOREFERENCE_PRESENTER_H
#define GRAPHOS_GEOREFERENCE_PRESENTER_H

#include "graphos/ui/georeference/GeoreferencePresenter.h"

namespace graphos
{

namespace ui
{

class GeoreferenceView;
class GeoreferenceModel;
class HelpDialog;

class GeoreferencePresenterImp
  : public GeoreferencePresenter
{

  Q_OBJECT

public:

  GeoreferencePresenterImp(GeoreferenceView *view,
                           GeoreferenceModel *model);
  ~GeoreferencePresenterImp() override;

private slots:

  void onGeoreferenceFinished();

// ProcessPresenter interface
  
protected slots:

  void onError(int code, const QString &msg) override;
  void onFinished() override;
  bool createProcess() override;

public slots:

  void cancel() override;

// GeoreferencePresenter interface

public slots:

  void setImageActive(const QString &image) override;

// Presenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  GeoreferenceView *mView;
  GeoreferenceModel *mModel;
  HelpDialog *mHelp;
};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_PRESENTER_H
