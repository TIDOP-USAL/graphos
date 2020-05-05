#include <inspector/core/project.h>

namespace inspector
{

class ProjectControllerFake
  : public ProjectControllerImp
{

public:

  ProjectControllerFake();

  ~ProjectControllerFake() override {}

// ProjectControllerImp interface

public:

  bool read(const QString &file, Project &prj) override;
  bool write(const QString &file, const Project &prj) const override;
  bool checkOldVersion(const QString &file) const override;
  void oldVersionBak(const QString &file) const override;

protected:

  QString mProjectFileText;
};

} // end namespace inspector
