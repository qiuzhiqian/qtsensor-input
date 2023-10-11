#include "light.h"
#include <QDebug>
#include <QtMath>

LightAdaptor::LightAdaptor(QObject *parent) :QObject(parent),m_sensor(new QLightSensor(this)){
    connect(m_sensor, SIGNAL(readingChanged()), this, SLOT(onReadVal()));
    qDebug() << "light start...";
    m_sensor->start();
}

void LightAdaptor::onReadVal() {
    qreal lux = m_sensor->reading()->lux();
    qDebug() << "onReadVal lux=" << lux;
}


