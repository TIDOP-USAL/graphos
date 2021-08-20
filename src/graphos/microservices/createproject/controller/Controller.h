#ifndef INSPECTOR_CONTROLLER_H
#define INSPECTOR_CONTROLLER_H

#include "cpprest/http_listener.h"

class Controller
{

public:

  Controller(){}
  virtual ~Controller() = default;

  virtual void initialize(const utility::string_t &url) = 0;
  virtual pplx::task<void> open() = 0;
  virtual pplx::task<void> close() = 0;

private:

  virtual void handleGet(web::http::http_request message) = 0;
  virtual void handlePost(web::http::http_request message) = 0;

};




#endif // INSPECTOR_CONTROLLER_H