#pragma once

#include <QObject>
#include <QAccelerometer>

class SensorManager : public QObject {
    Q_OBJECT
public:
    SensorManager(QObject *parent = nullptr);

public slots:
    void onReadVal();
private:
    QAccelerometer* m_sensor;
};