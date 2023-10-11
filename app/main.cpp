#include <QtCore>
#include "accelerometer.h"
#include "light.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    AccelerometerAdaptor accel_adapter;
    LightAdaptor light_adapter;

    return app.exec();
}