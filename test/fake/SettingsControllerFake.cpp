#include "SettingsControllerFake.h"


namespace inspector
{

SettingsControllerFake::SettingsControllerFake()
  : SettingsController()
{

}

void SettingsControllerFake::read(Settings &settings)
{
  settings.setLanguage(this->data.lang);
}

void SettingsControllerFake::write(const Settings &settings)
{
  this->data.lang = settings.language();
}

void SettingsControllerFake::writeHistory(const Settings &settings)
{
  this->data.history = settings.history();
}




} // end namespace inspector
