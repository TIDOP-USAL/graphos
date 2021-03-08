#ifndef INSPECTOR_OPEN_PROJECT_VIEW_H
#define INSPECTOR_OPEN_PROJECT_VIEW_H

#include "inspector/ui/openproject/OpenProjectView.h"

namespace inspector
{

namespace ui
{

class OpenProjectViewImp
	: public OpenProjectView
{

  Q_OBJECT

public:

	explicit OpenProjectViewImp(QWidget *parent = nullptr);
	~OpenProjectViewImp() override = default;

private:

  void init();

private:

// OpenProjectView interface

public:

	void setInspectorPath(const QString &directory) override;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_OPEN_PROJECT_VIEW_H