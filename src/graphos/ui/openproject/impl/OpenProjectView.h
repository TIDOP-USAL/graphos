#ifndef GRAPHOS_OPEN_PROJECT_VIEW_H
#define GRAPHOS_OPEN_PROJECT_VIEW_H

#include "graphos/ui/openproject/OpenProjectView.h"

namespace graphos
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

} // namespace graphos

#endif // GRAPHOS_OPEN_PROJECT_VIEW_H