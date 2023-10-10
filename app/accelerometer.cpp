#include "accelerometer.h"
#include <QDebug>
#include <QtMath>

ORIENTATION orientation_calc(qreal x,qreal y,qreal z);

AccelerometerAdaptor::AccelerometerAdaptor(QObject *parent) :QObject(parent),m_sensor(new QAccelerometer(this)){
    connect(m_sensor, SIGNAL(readingChanged()), this, SLOT(onReadVal()));
    qDebug() << "start...";
    m_sensor->start();
}

void AccelerometerAdaptor::onReadVal() {
    qreal x = m_sensor->reading()->x();
    qreal y = m_sensor->reading()->y();
    qreal z = m_sensor->reading()->z();
    qDebug() << "onReadVal x=" << x << "y=" << y << "z=" << z;
    switch(orientation_calc(x, y, z)) {
    case ORIENTATION::R_0:
        qDebug() << "Rotate 0 degrees";
        break;
    case ORIENTATION::R_90:
        qDebug() << "Rotate 90 degrees";
        break;
    case ORIENTATION::R_180:
        qDebug() << "Rotate 180 degrees";
        break;
    case ORIENTATION::R_270:
        qDebug() << "Rotate 270 degrees";
        break;
    default:
        qDebug() << "unknown degrees";
        break;
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