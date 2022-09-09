#include "configuration.h"

QMap<QString, QVariant> Configuration::configuration = QMap<QString, QVariant>();

void Configuration::set(const QString &key, const QVariant &value) {
    configuration[key] = value;
}

QVariant Configuration::get(const QString &key) {
    return configuration[key];
}
