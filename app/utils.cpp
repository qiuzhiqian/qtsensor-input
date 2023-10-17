#include "utils.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

bool service_actived(const QString& name) {
    QDBusInterface dbus_ifc("org.freedesktop.DBus", "/org/freedesktop/DBus",
                                 "org.freedesktop.DBus",
                                 QDBusConnection::sessionBus());

    QDBusReply<bool> reply = dbus_ifc.call(QLatin1String("NameHasOwner"), name);
    if(reply.isValid()) {
        return reply.value();
    }
    return false;
}