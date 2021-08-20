#ifndef GRAPHOS_OPEN_PROJECT_MODEL_H
#define GRAPHOS_OPEN_PROJECT_MODEL_H

#include "graphos/ui/openproject/OpenProjectModel.h"

namespace graphos
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

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;

};


} // namespace ui

} // namespace graphos

#endif // GRAPHOS_OPEN_PROJECT_MODEL_H
