#include "accelerometer.h"
#include <QDebug>
#include <QtMath>

#include <QDBusConnection>
#include <QDBusInterface>

ORIENTATION orientation_calc(qreal x,qreal y,qreal z);
void RotateScreen(ORIENTATION orient);

AccelerometerAdaptor::AccelerometerAdaptor(QObject *parent) :QObject(parent),m_sensor(new QAccelerometer(this)){
    connect(m_sensor, SIGNAL(readingChanged()), this, SLOT(onReadVal()));
    qDebug() << "start...";
    m_sensor->start();
}

void AccelerometerAdaptor::onReadVal() {
    qreal x = m_sensor->reading()->x();
    qreal y = m_sensor->reading()->y();
    qreal z = m_sensor->reading()->z();
    //qDebug() << "onReadVal x=" << x << "y=" << y << "z=" << z;
    ORIENTATION orient = orientation_calc(x, y, z);
    if(orient != this->m_orient) {
        //switch(orientation_calc(x, y, z)) {
        //case ORIENTATION::R_0:
        //    qDebug() << "Rotate 0 degrees";
        //    break;
        //case ORIENTATION::R_90:
        //    qDebug() << "Rotate 90 degrees";
        //    break;
        //case ORIENTATION::R_180:
        //    qDebug() << "Rotate 180 degrees";
        //    break;
        //case ORIENTATION::R_270:
        //    qDebug() << "Rotate 270 degrees";
        //    break;
        //default:
        //    qDebug() << "unknown degrees";
        //    break;
        //}
        RotateScreen(orientation_calc(x, y, z));
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
    QDBusInterface interface("com.deepin.daemon.Display", "/com/deepin/daemon/Display/Monitor_1",
                                 "com.deepin.daemon.Display.Monitor",
                                 QDBusConnection::sessionBus());
    if (!interface.isValid()) {
        qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        exit(1);
    }

    int val = 0;
    switch (orient) {
    case ORIENTATION::R_0:
        val= 1;
        break;
    case ORIENTATION::R_90:
        val = 2;
        break;
    case ORIENTATION::R_180:
        val = 4;
        break;
    case ORIENTATION::R_270:
        val = 8;
        break;
    default:
        break;
    }

    interface.asyncCall(QLatin1String("SetRotation"), val);
}
