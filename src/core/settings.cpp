#include "settings.h"

Settings *Settings::instance()
{
    Settings *instance = nullptr;
    if(! instance)
        instance = new Settings;
    return instance;
}

Settings::Settings() : _settings("Sky", "WiremockUI")
{

}
