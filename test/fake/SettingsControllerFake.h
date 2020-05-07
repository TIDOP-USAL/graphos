#include <inspector/core/settings.h>

namespace inspector
{

class SettingsControllerFake
  : public SettingsController
{

protected:

  struct Data{
    QString lang = "en";
    QStringList history;
  };

public:

  SettingsControllerFake();
  ~SettingsControllerFake() override {}

// SettingsController interface

public:

  void read(Settings &settings) override;
  void write(const Settings &settings) override;
  void writeHistory(const Settings &settings) override;

protected:

  Data data;

};

} // end namespace inspector
