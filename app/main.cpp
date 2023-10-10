#include <QtCore>
#include "accelerometer.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    AccelerometerAdaptor accel_adapter;

    return app.exec();
}