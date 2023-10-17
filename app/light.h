#pragma once

#include <QObject>
#include <QLightSensor>
#include <QQueue>
#include <QTimer>
#include "brightness.h"

class LightAdaptor : public QObject {
    Q_OBJECT
public:
    LightAdaptor(QObject *parent=nullptr);
    void BrightnessControl(qreal lux);

    void start();
    void stop();
private:
    QLightSensor* m_sensor;

    QQueue<qreal> m_data;
    qreal m_current_val = 0;
    qreal m_next_val = 0;

    QTimer m_delay;
    QTimer m_brightness_ctl;

    BrightnessCtl *m_ctl;

public slots:
    void onReadVal();
};