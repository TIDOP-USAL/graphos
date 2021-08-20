#include "controller/CreateProjectController.h"


#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  utility::string_t port = U("34568");
  if (argc == 2) {
    port = argv[1];
  }

  utility::string_t address = U("http://localhost:");
  address.append(port);
  address.append(L"/inspector/createproj");

  try {

    inspector::microservice::CreateProjectController controller;
    controller.initialize(address);
    controller.open().wait();
    ucout << utility::string_t(U("Listening for requests at: ")) << address << std::endl;

    std::cout << "Press ENTER to exit." << std::endl;
    std::string line;
    std::getline(std::cin, line);

    controller.close().wait();

  } catch (std::exception &e) {
    std::cerr << "something wrong happen! :(" << '\n';
  }

  return 0;
}