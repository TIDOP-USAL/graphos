#ifndef INSPECTOR_CREATE_PROJECT_CONTROLER_H
#define INSPECTOR_CREATE_PROJECT_CONTROLER_H



#include "controller/Controller.h"

namespace inspector
{

namespace microservice
{

class CreateProjectController 
  : public Controller
{

public:

  CreateProjectController();
  CreateProjectController(const utility::string_t &url);
  ~CreateProjectController();

//  Controller interface

public:

  void initialize(const utility::string_t &url) override;
  pplx::task<void> open() override;
  pplx::task<void> close() override;

private:

  void handleGet(web::http::http_request message) override;
  void handlePost(web::http::http_request message) override;

private:

  web::http::experimental::listener::http_listener *mListener;
  utility::string_t mURL;

};

} // namespace microservice

} // namespace inspector

#endif // INSPECTOR_CREATE_PROJECT_CONTROLER_H