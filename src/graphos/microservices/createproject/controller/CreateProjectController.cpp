#include "CreateProjectController.h"

#include "service/service.h"

#include "opencv2/imgcodecs.hpp"

using namespace web;
using namespace http;
using namespace experimental;
using namespace listener;

namespace inspector
{

namespace microservice
{

CreateProjectController::CreateProjectController()
  : Controller(),
    mListener(nullptr),
    mURL()
{
}

CreateProjectController::CreateProjectController(const utility::string_t &url)
  : Controller(),
    mListener(nullptr),
    mURL(url)
{
  initialize(url);
}

CreateProjectController::~CreateProjectController()
{
  if (mListener) {
    mListener->close().wait();
    delete mListener;
    mListener = nullptr;
  }
}

void CreateProjectController::initialize(const utility::string_t &url)
{
  mURL = url;
  uri_builder uri(url);

  if (mListener) {
    mListener->close().wait();
  }

  mListener = new http_listener(uri.to_uri());

  mListener->support(methods::GET, std::bind(&CreateProjectController::handleGet, this, std::placeholders::_1));
  mListener->support(methods::POST, std::bind(&CreateProjectController::handlePost, this, std::placeholders::_1));
}

pplx::task<void> CreateProjectController::open() 
{ 
  if (mListener == nullptr) throw "The controller has not been initialized. Call 'initialize()' method";
  return mListener->open();
}

pplx::task<void> CreateProjectController::close() 
{ 
  if (mListener == nullptr) throw "The controller has not been initialized. Call 'initialize()' method";
  return mListener->close();
}

void CreateProjectController::handleGet(web::http::http_request message)
{
  utility::string_t name;
  utility::string_t description;
  bool overwrite = false;

  try {

    web::json::value response = web::json::value::object();

    web::uri _uri = message.absolute_uri();
    auto query = uri::split_query(uri::decode(_uri.query()));

    utility::string_t upperCaseParameter;
    for (auto &parameter : query) {
      upperCaseParameter.resize(parameter.first.size());
      std::transform(parameter.first.begin(), parameter.first.end(), upperCaseParameter.begin(), ::towupper);
      if (upperCaseParameter.compare(U("NAME")) == 0) {
        name = parameter.second;
      } else if (upperCaseParameter.compare(U("DESCRIPTION")) == 0) {
        description = parameter.second;
      } else if (upperCaseParameter.compare(U("OVERWRITE")) == 0) {
        upperCaseParameter.resize(parameter.second.size());
        std::transform(parameter.second.begin(), parameter.second.end(), upperCaseParameter.begin(), ::towupper);
        overwrite = upperCaseParameter.compare(U("TRUE")) == 0;
      }
    }

    if (name.empty()) {
       response[U("ERROR")] = json::value::string(U("Invalid request. Required parameter 'name' not found"));
       message.reply(status_codes::BadRequest, response);
       return;
    }

    Service service;
    service.createProject(QString::fromStdWString(name), 
                          QString::fromStdWString(description),
                          overwrite);

    response[U("NAME")] = json::value::string(name);
    response[U("DESCRIPTION")] = json::value::string(description);
    message.reply(status_codes::OK, response);

  } catch (const std::exception &e) {
    message.reply(status_codes::BadRequest, e.what());
  } catch (...) {
    message.reply(status_codes::InternalError, U(""));
  }

}

void CreateProjectController::handlePost(web::http::http_request message)
{
  auto paths = uri::split_path(uri::decode(message.relative_uri().path()));
  auto query = uri::split_query(uri::decode(message.relative_uri().query()));
  auto queryItr = query.find(U("request"));

  message.reply(status_codes::OK);
}

} // namespace microservice

} // namespace inspector