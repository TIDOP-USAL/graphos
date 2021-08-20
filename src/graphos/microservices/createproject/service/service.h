#ifndef INSPECTOR_CREATE_PROJECT_SERVICE_H
#define INSPECTOR_CREATE_PROJECT_SERVICE_H

#include <QString>

namespace inspector
{

namespace microservice
{

class Service
{

private:

public:

  Service();
  ~Service();

  void createProject(const QString &name, 
                     const QString &description,
                     bool overwrite = false);

private:

};

} // namespace microservice

} // namespace inspector

#endif // INSPECTOR_CREATE_PROJECT_SERVICE_H