#include "manager.h"
#include <QDebug>

SensorManager::SensorManager(QObject *parent) :QObject(parent),m_sensor(new QAccelerometer(this)){
    connect(m_sensor, SIGNAL(readingChanged()), this, SLOT(onReadVal()));
    qDebug() << "start...";
    m_sensor->start();
}

void SensorManager::onReadVal() {
    qreal x = m_sensor->reading()->x();
    qreal y = m_sensor->reading()->y();
    qreal z = m_sensor->reading()->z();
    qDebug() << "onReadVal x=" << x << "y=" << y << "z=" << z;
}