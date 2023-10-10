#include <QtCore>
#include "manager.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    SensorManager manager;

    return app.exec();
}