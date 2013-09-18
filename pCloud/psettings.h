#ifndef PSETTINGS_H
#define PSETTINGS_H

#include <QString>
#include <QSettings>

#include <stdint.h>

class PCloudApp;

class PSettings
{
private:
    PCloudApp *app;
    QSettings *settings;
public:
    PSettings(PCloudApp *a);
    ~PSettings();
    QString get(const QString &key);
    void set(const QString &key, const QString &val);
    int geti(const QString &key);
    void seti(const QString &key, int val);
    bool isSet(const QString &key);
    void unset(const QString &key);
    void setu64(const QString &key, uint64_t val);
    uint64_t getu64(const QString &key);
};

#ifdef Q_OS_WIN
char getFirstFreeDevice();
#endif

#endif // PSETTINGS_H
