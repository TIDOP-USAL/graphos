#ifndef GRAPHOS_OPEN_PROJECT_PRESENTER_INTERFACE_H
#define GRAPHOS_OPEN_PROJECT_PRESENTER_INTERFACE_H

#include "graphos/ui/process/ProcessPresenter.h"

namespace graphos
{

namespace ui
{


class OpenProjectPresenter
  : public Presenter
{

  Q_OBJECT

public:

  OpenProjectPresenter(){}
  virtual ~OpenProjectPresenter() override = default;

protected slots:

  virtual void setProjectFile(const QString &file) = 0;

signals:

  void projectLoaded();
  //void loadingImages(bool);
  //void imageLoaded(QString);
};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_OPEN_PROJECT_PRESENTER_INTERFACE_H
