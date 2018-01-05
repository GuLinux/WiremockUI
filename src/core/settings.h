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
    static Settings *instance();
    define_setting(server_address, QUrl)
    define_setting(geometry, QByteArray)
    define_setting(state, QByteArray)
    define_setting(saveDir, QString)
private:
    QSettings _settings;
    Settings();
};

#endif // SETTINGS_H
