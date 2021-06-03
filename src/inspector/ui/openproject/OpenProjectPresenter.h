#ifndef INSPECTOR_OPEN_PROJECT_PRESENTER_INTERFACE_H
#define INSPECTOR_OPEN_PROJECT_PRESENTER_INTERFACE_H

#include "inspector/ui/process/ProcessPresenter.h"

namespace inspector
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

} // namespace inspector

#endif // INSPECTOR_OPEN_PROJECT_PRESENTER_INTERFACE_H
