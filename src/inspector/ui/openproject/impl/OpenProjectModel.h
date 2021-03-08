#ifndef INSPECTOR_OPEN_PROJECT_MODEL_H
#define INSPECTOR_OPEN_PROJECT_MODEL_H

#include "inspector/ui/openproject/OpenProjectModel.h"

namespace inspector
{

class Project;

namespace ui
{

class OpenProjectModelImp
	: public OpenProjectModel
{

  Q_OBJECT

public:

	OpenProjectModelImp(Project *project,
											QObject *parent = nullptr);
	~OpenProjectModelImp();

// OpenProjectModel interface

public:

	QString inspectorDirectory() const override;

public slots:

  void saveProject() override;
  void loadProject(const QString &projectPath) override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;

};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_OPEN_PROJECT_MODEL_H
