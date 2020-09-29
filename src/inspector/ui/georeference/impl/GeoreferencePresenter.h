#ifndef INSPECTOR_GEOREFERENCE_PRESENTER_H
#define INSPECTOR_GEOREFERENCE_PRESENTER_H

#include "inspector/ui/georeference/GeoreferencePresenter.h"

namespace inspector
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

// IPresenter interface

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

} // namespace inspector

#endif // INSPECTOR_GEOREFERENCE_PRESENTER_H
