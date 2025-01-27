/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSensors module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "input_accelerometer.h"
#include <QDebug>
#include <QTimer>
#include <QString>

#include <cstddef>
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/input.h>

#include "utils.h"

char const * const InputAccelerometer::id("input.accelerometer");

InputAccelerometer::InputAccelerometer(QSensor *sensor)
    : QSensorBackend(sensor)
    , m_timerid(0)
{
    setReading<QAccelerometerReading>(&m_reading);
    addDataRate(-32768, 32768); // 100Hz
    addOutputRange(-22.418, 22.418, 0.17651); // 2G
}

InputAccelerometer::~InputAccelerometer()
{
}

void InputAccelerometer::start()
{
    if(m_notifier) {
        return;
    }

    qDebug() << "open gsensor...";
    int fd = -1;
    fd = open("/dev/mma8452_daemon", O_RDONLY);
    int result = ioctl(fd, GSENSOR_IOCTL_START);
    result = ioctl(fd, GSENSOR_IOCTL_GET_CALIBRATION, &accel_offset);
    close(fd);

    QByteArray path = qgetenv("QT_ACCEL_INPUT_PATH");
    fd = open(path.data(), O_RDONLY);
    if (fd == -1) {
        qFatal("Failed to open input device");
    }
    
    m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read);
    connect(m_notifier, &QSocketNotifier::activated, [=](){
        struct input_event event;
        if (::read(fd, &event, sizeof(struct input_event)) == sizeof(struct input_event)) {
            if (event.type == EV_ABS) {
                switch (event.code) {
                    case EVENT_TYPE_ACCEL_X: {
                        m_reading.setTimestamp(produceTimestamp());
                        m_reading.setX(event.value - accel_offset[0]);
                        valid_flag |= 0x01;
                        if(valid_flag>= 0x07) {
                            newReadingAvailable();
                        }
                        break;
                    }
                    case EVENT_TYPE_ACCEL_Y:
                    {
                        m_reading.setTimestamp(produceTimestamp());
                        m_reading.setY(event.value - accel_offset[1]);
                        valid_flag |= 0x02;
                        if(valid_flag>= 0x07) {
                            newReadingAvailable();
                        }
                        break;
                    }
                    case EVENT_TYPE_ACCEL_Z:
                    {
                        m_reading.setTimestamp(produceTimestamp());
                        m_reading.setZ(event.value - accel_offset[2]);
                        valid_flag |= 0x04;
                        if(valid_flag>= 0x07) {
                            newReadingAvailable();
                        }
                        break;
                    }
                }
            }
        }
    });
    m_notifier->setEnabled(true);
}

void InputAccelerometer::stop()
{
    if (m_timerid) {
        killTimer(m_timerid);
        m_timerid = 0;
    }

    if(m_notifier) {
        m_notifier->setEnabled(false);
        m_notifier->disconnect(SIGNAL(activated(QSocketDescriptor, QSocketNotifier::Type)));
        m_notifier->deleteLater();
        m_notifier = nullptr;
    }

    qDebug() << "close gsensor...";
    int fd = -1;
    fd = open("/dev/mma8452_daemon", O_RDONLY);
    qDebug() << "fd="<< fd;
    int result = ioctl(fd, GSENSOR_IOCTL_CLOSE);
    qDebug() << "result="<< result;
    close(fd);
}

void InputAccelerometer::poll()
{
    m_reading.setTimestamp(produceTimestamp());
    m_reading.setX(12.1);
    m_reading.setY(14.3);
    m_reading.setZ(21.3);

    newReadingAvailable();
}

void InputAccelerometer::timerEvent(QTimerEvent * /*event*/)
{
    poll();
}
