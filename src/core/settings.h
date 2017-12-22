#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QUrl>

#define define_setting(name, type) \
    void name(const type &value) { _settings.setValue(#name, value); } \
    type  name() const { return _settings.value(#name).value<type>(); }
class Settings
{
public:
    Settings();
    define_setting(server_address, QUrl)
private:
    QSettings _settings;
};

#endif // SETTINGS_H
