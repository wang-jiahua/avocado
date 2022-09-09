#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include <QMap>
#include <QString>
#include <QVariant>

class Configuration {
public:
    static QMap<QString, QVariant> configuration;

    static void set(const QString &key, const QVariant &value);

    static QVariant get(const QString &key);
};

#endif // _CONFIGURATION_H
