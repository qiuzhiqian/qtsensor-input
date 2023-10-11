#pragma once

#include <QObject>
#include <QLightSensor>


class LightAdaptor : public QObject {
    Q_OBJECT
public:
    LightAdaptor(QObject *parent=nullptr);
private:
    QLightSensor* m_sensor;

public slots:
    void onReadVal();
};