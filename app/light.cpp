#include "light.h"
#include "brightness.h"
#include "utils.h"
#include <QDebug>
#include <QtMath>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

void DoBrightness(qreal val);
qreal GetCurrentBrightness();

LightAdaptor::LightAdaptor(QObject *parent) :QObject(parent),m_sensor(new QLightSensor(this)){
    if(!service_actived("com.deepin.daemon.Display")) {
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

void LightAdaptor::onReadVal() {
    qreal lux = m_sensor->reading()->lux();
    m_data.enqueue(lux);
    if(m_data.size() > 10) {
        m_data.dequeue();
    }

    qreal sum = 0;
    for(auto d:m_data) {
        sum += d;
    }

    qreal val = sum / m_data.size();

    if(qAbs(m_current_val - val) > 0.3 /*0.8*/ ) {
        // 延迟3s防抖
        if(!m_delay.isActive() || qAbs(m_next_val - val) > 0.2) {
            qDebug() << "brightness will change " << m_current_val << " --> " << val << " (" << m_next_val << ")";
            m_next_val = val;
            m_delay.start();
        }
    } else {
        m_delay.stop();
    }
}

void LightAdaptor::start() {
    qDebug() << "light start...";
    if (QGSettings::isSchemaInstalled("com.deepin.dte.settings")) {
        m_gsettings = new QGSettings("com.deepin.dte.settings");

        //监听key的value是否发生了变化
        connect(m_gsettings, &QGSettings::changed, this, [=] (const QString &key) {
            if (key == "auto-brightness") {
                if(m_gsettings->get("auto-brightness").toBool()) {
                    m_sensor->start();
                } else {
                    qDebug() << "do auto-brightness stop...";
                    m_sensor->stop();
                }
            }
        });

        if(m_gsettings->get("auto-brightness").toBool()) {
            qDebug() << "do sensor start...";
            m_sensor->start();
        } else {
            m_sensor->stop();
        }
    }
    connect(m_sensor, SIGNAL(readingChanged()), this, SLOT(onReadVal()));

    m_delay.setSingleShot(true);
    m_delay.setInterval(2000);
    connect(&m_delay, &QTimer::timeout, [=]() {
        qDebug() << "brightness changed to " << m_next_val;
        m_current_val = m_next_val;
        m_delay.stop();

        BrightnessControl(m_current_val);
    });

    m_ctl = new BrightnessCtl(0,0,0);
    connect(m_ctl, &BrightnessCtl::actived, [=](qreal val) {
        DoBrightness(val);
    });
}

void LightAdaptor::stop() {
    if(m_gsettings) {
        m_gsettings->disconnect(SIGNAL(changed(const QString &key)));
        m_gsettings->deleteLater();
        m_gsettings = nullptr;
    }

    m_sensor->stop();
    m_sensor->disconnect(SIGNAL(readingChanged()));

    m_delay.stop();
    m_delay.disconnect(SIGNAL(timeout()));

    if(m_ctl) {
        m_ctl->stop();
        m_ctl->disconnect(SIGNAL(actived(qreal val)));
        m_ctl->deleteLater();
    }
}

void LightAdaptor::BrightnessControl(qreal lux) {
    qreal dst_val = 0;
    //0.5~3.5
    if(lux < 0.5) {
        dst_val = 0.1;
    } else if (lux < 1.2) {
        dst_val = 0.1 + (lux - 0.5) * (0.4 - 0.1) / (1.2 - 0.5);
        //dst_val = 0.4
    } else if(lux < 2.3) {
        dst_val = 0.4 + (lux - 1.2) * (0.7 - 0.4) / (2.3 - 1.2);
        //dst_val = 0.7
    } else if( lux < 3.5) {
        dst_val = 0.7 + (lux - 2.3) * (0.9 - 0.7) / (3.5 - 2.3);
        //dst_val = 0.9
    } else if (lux < 4) {
        dst_val = 0.95;
    } else {
        dst_val = 1.0;
    }
    
    qreal src_val = GetCurrentBrightness();
    qDebug() << "val: " << src_val << " -->" << dst_val;
    qreal delt = dst_val - src_val;
    if(delt > 0.01) {
        qDebug() << "do brightness + " << dst_val;

        m_ctl->stop();
        m_ctl->setStart(src_val);
        m_ctl->setEnd(dst_val);
        m_ctl->setStep(0.1);
        m_ctl->start();
    } else if (delt < -0.01) {
        qDebug() << "do brightness - " << dst_val;

        m_ctl->stop();
        m_ctl->setStart(src_val);
        m_ctl->setEnd(dst_val);
        m_ctl->setStep(-0.1);
        m_ctl->start();
    }
}

void DoBrightness(qreal val) {
    qDebug() << "do brightness dbus "<< val;
    QDBusInterface display_ifc("com.deepin.daemon.Display", "/com/deepin/daemon/Display",
                                    "com.deepin.daemon.Display",
                                QDBusConnection::sessionBus());
    if (!display_ifc.isValid()) {
        qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        exit(1);
    }

    QDBusInterface monitor_ifc("com.deepin.daemon.Display", "/com/deepin/daemon/Display/Monitor_1",
                                 "com.deepin.daemon.Display.Monitor",
                                 QDBusConnection::sessionBus());
    QString name = monitor_ifc.property("Name").toString();
    display_ifc.call(QLatin1String("SetBrightness"), name, val);
}

qreal GetCurrentBrightness() {
    qreal ret = 0.0;
    QDBusInterface monitor_ifc("com.deepin.daemon.Display", "/com/deepin/daemon/Display/Monitor_1",
                                 "com.deepin.daemon.Display.Monitor",
                                 QDBusConnection::sessionBus());
    QString name = monitor_ifc.property("Name").toString();

    QDBusInterface display_ifc("com.deepin.daemon.Display", "/com/deepin/daemon/Display",
                                 "org.freedesktop.DBus.Properties",
                                 QDBusConnection::sessionBus());
    if (!display_ifc.isValid()) {
        qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        exit(1);
    }

    QDBusReply<QVariant> reply = display_ifc.call(QLatin1String("Get"), "com.deepin.daemon.Display", "Brightness");
    QVariant val = reply.value();
    const QDBusArgument &argument = val.value<QDBusArgument>();
    argument.beginMap();
    while ( !argument.atEnd() ) {
        QString key;
        double value;
        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();
        if(key == name) {
            ret = value;
            break;
        }
    }

    argument.endMap();
    return ret;
}
