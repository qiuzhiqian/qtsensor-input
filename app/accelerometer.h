#pragma once

#include <QObject>
#include <QAccelerometer>

#include <QGSettings>

#define OFFSET 5

enum class ORIENTATION {
    UNKNOWN = -1,
    R_0 = 0,
    R_90 = 1,
    R_180 = 2,
    R_270 = 3,
};

class AccelerometerAdaptor : public QObject {
    Q_OBJECT
public:
    AccelerometerAdaptor(QObject *parent=nullptr);
    ~AccelerometerAdaptor();
private:
    QAccelerometer* m_sensor;
    ORIENTATION m_orient = ORIENTATION::UNKNOWN;
    QGSettings* m_gsettings = nullptr;

public slots:
    void onReadVal();
};