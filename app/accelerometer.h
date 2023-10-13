#pragma once

#include <QObject>
#include <QAccelerometer>

#include <QGSettings>
#include <QDBusServiceWatcher>

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

    QDBusServiceWatcher* m_watcher = nullptr;

    void start();
    void stop();

public slots:
    void onReadVal();
};