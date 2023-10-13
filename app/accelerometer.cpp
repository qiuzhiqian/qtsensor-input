#include "accelerometer.h"
#include <QDebug>
#include <QtMath>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

ORIENTATION orientation_calc(qreal x,qreal y,qreal z);
void RotateScreen(ORIENTATION orient);
bool service_active(const QString& name);

AccelerometerAdaptor::AccelerometerAdaptor(QObject *parent) :QObject(parent),m_sensor(new QAccelerometer(this)){
    if(!service_active("com.deepin.daemon.Display")) {
        m_watcher = new QDBusServiceWatcher("com.deepin.daemon.Display", QDBusConnection::sessionBus(), 
            QDBusServiceWatcher::WatchForRegistration);
        connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, [=](const QString &serviceName) {
            if(serviceName == "com.deepin.daemon.Display") {
                qDebug() << "has wait display active...";
                start();
                m_watcher->deleteLater();
                m_watcher = nullptr;
            }
        });
    } else {
        qDebug() << "display actived...";
        start();
    }
}

AccelerometerAdaptor::~AccelerometerAdaptor() {
    qDebug() << "accelerometer stop...";
    if(m_sensor) {
        m_sensor->stop();
        m_sensor->disconnect(SIGNAL(readingChanged()));
        m_sensor->deleteLater();
        m_sensor = nullptr;
    }
}

void AccelerometerAdaptor::start() {
    qDebug() << "accelerometer start...";
    if (QGSettings::isSchemaInstalled("com.deepin.due.shell")) {
        m_gsettings = new QGSettings("com.deepin.due.shell");

        //监听key的value是否发生了变化
        connect(m_gsettings, &QGSettings::changed, this, [=] (const QString &key) {
            if (key == "rotationislock") {
                if(m_gsettings->get("rotationislock").toBool()) {
                    m_sensor->start();
                } else {
                    qDebug() << "do rotationislock stop...";
                    m_sensor->stop();
                    m_orient = ORIENTATION::UNKNOWN;
                }
            }
        });

        if(m_gsettings->get("rotationislock").toBool()) {
            qDebug() << "do sensor start...";
            m_sensor->start();
        } else {
            m_sensor->stop();
            m_orient = ORIENTATION::UNKNOWN;
        }
    }
    connect(m_sensor, SIGNAL(readingChanged()), this, SLOT(onReadVal()));
}

void AccelerometerAdaptor::stop() {
    if(m_gsettings) {
        m_gsettings->disconnect(SIGNAL(changed(const QString &key)));
        m_gsettings->deleteLater();
        m_gsettings = nullptr;
    }

    m_sensor->stop();
    m_orient = ORIENTATION::UNKNOWN;
    m_sensor->disconnect(SIGNAL(readingChanged()));
}

void AccelerometerAdaptor::onReadVal() {
    qreal x = m_sensor->reading()->x();
    qreal y = m_sensor->reading()->y();
    qreal z = m_sensor->reading()->z();
    //qDebug() << "onReadVal x=" << x << "y=" << y << "z=" << z;
    ORIENTATION orient = orientation_calc(x, y, z);
    if(orient != this->m_orient && orient != ORIENTATION::UNKNOWN) {
        RotateScreen(orient);
        this->m_orient = orient;
    }
}

ORIENTATION orientation_calc(qreal x,qreal y,qreal z) {
    qreal absx = qAbs(x);
    qreal absy = qAbs(y);
    qreal absz = qAbs(z);
    if (absx > absy && absx > absz) {
        if (x > OFFSET) {
            return ORIENTATION::R_90;
        } else if (x < -OFFSET) {
            return ORIENTATION::R_270;
        }
    } else if (absy > absx && absy > absz) {
        if (y > OFFSET) {
            return ORIENTATION::R_0;
        } else if (y < -OFFSET) {
            return ORIENTATION::R_180;
        }
    } else if (absz > absx && absz > absy) {
        return ORIENTATION::UNKNOWN;
    } else {
        return ORIENTATION::UNKNOWN;
    }
    return ORIENTATION::UNKNOWN;
}

void RotateScreen(ORIENTATION orient) {
    int val = 0;
    switch (orient) {
    case ORIENTATION::R_0:
        qDebug() << "Rotate 0 degrees";
        val= 1;
        break;
    case ORIENTATION::R_90:
        qDebug() << "Rotate 90 degrees";
        val = 8;
        break;
    case ORIENTATION::R_180:
        qDebug() << "Rotate 180 degrees";
        val = 4;
        break;
    case ORIENTATION::R_270:
        qDebug() << "Rotate 270 degrees";
        val = 2;
        break;
    default:
        //qDebug() << "unknown degrees";
        return;
    }

    QDBusInterface monitor_ifc("com.deepin.daemon.Display", "/com/deepin/daemon/Display/Monitor_1",
                                 "com.deepin.daemon.Display.Monitor",
                                 QDBusConnection::sessionBus());
    if (!monitor_ifc.isValid()) {
        qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        exit(1);
    }

    if(monitor_ifc.property("Rotation").toInt() != val) {
        monitor_ifc.call(QLatin1String("SetRotation"), val);
        QDBusInterface display_ifc("com.deepin.daemon.Display", "/com/deepin/daemon/Display",
                                    "com.deepin.daemon.Display",
                                    QDBusConnection::sessionBus());
        if (!display_ifc.isValid()) {
            qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
            exit(1);
        }
        display_ifc.call(QLatin1String("ApplyChanges"));
        display_ifc.call(QLatin1String("Save"));
    }
}

bool service_active(const QString& name) {
    QDBusInterface dbus_ifc("org.freedesktop.DBus", "/org/freedesktop/DBus",
                                 "org.freedesktop.DBus",
                                 QDBusConnection::sessionBus());

    QDBusReply<bool> reply = dbus_ifc.call(QLatin1String("NameHasOwner"), name);
    if(reply.isValid()) {
        return reply.value();
    }
    return false;
}
