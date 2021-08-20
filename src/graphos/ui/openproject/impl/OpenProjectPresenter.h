#ifndef GRAPHOS_OPEN_PROJECT_PRESENTER_H
#define GRAPHOS_OPEN_PROJECT_PRESENTER_H

#include "graphos/ui/openproject/OpenProjectPresenter.h"

namespace graphos
{

namespace ui
{

class OpenProjectView;
class OpenProjectModel;
class HelpDialog;

class OpenProjectPresenterImp
	: public OpenProjectPresenter
{

	Q_OBJECT

public:

	OpenProjectPresenterImp(OpenProjectView *view,
                          OpenProjectModel *model);
	~OpenProjectPresenterImp() override;

 // OpenProjectPresenter interface

protected slots:

  void setProjectFile(const QString &file) override;

// Presenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  OpenProjectView *mView;
  OpenProjectModel *mModel;

};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_OPEN_PROJECT_PRESENTER_H