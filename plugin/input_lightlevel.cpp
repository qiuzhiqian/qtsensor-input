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

#include "input_lightlevel.h"
#include <QDebug>
#include <QTimer>
#include <QString>

#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/input.h>

#include "utils.h"

char const * const InputLightlevel::id("input.lightlevel");

InputLightlevel::InputLightlevel(QSensor *sensor)
    : QSensorBackend(sensor)
    , m_timerid(0)
{
    setReading<QLightReading>(&m_reading);
    //addDataRate(-32768, 32768); // 100Hz
    //addOutputRange(-22.418, 22.418, 0.17651); // 2G
}

InputLightlevel::~InputLightlevel()
{
}

void InputLightlevel::start()
{
    int fd = -1;
    int enable = 1;

    fd = open("/dev/lightsensor", O_RDONLY);
    int result = ioctl(fd, LIGHT_SENSOR_IOCTL_ENABLE, &enable);
    close(fd);

    QByteArray path = qgetenv("QT_LIGHT_INPUT_PATH");
    fd = open(path.data(), O_RDONLY);
    if (fd == -1) {
        qFatal("Failed to open input device");
    }
    qDebug() << "fd=" << fd;
    m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read);
    connect(m_notifier, &QSocketNotifier::activated, [=](){
        struct input_event event;
        if (::read(fd, &event, sizeof(struct input_event)) == sizeof(struct input_event)) {
            if (event.type == EV_ABS) {
                switch (event.code) {
                    case ABS_MISC: {
                        m_reading.setTimestamp(produceTimestamp());
                        m_reading.setLux(event.value);
                        newReadingAvailable();
                        break;
                    }                    
                }
            }
        }
    });
    m_notifier->setEnabled(true);
}

void InputLightlevel::stop()
{
    if (m_timerid) {
        killTimer(m_timerid);
        m_timerid = 0;
    }

    if(m_notifier) {
        m_notifier->setEnabled(false);
        m_notifier->disconnect(SIGNAL(activated(QSocketDescriptor, QSocketNotifier::Type)));
        m_notifier->deleteLater();
    }
}

void InputLightlevel::poll()
{
    m_reading.setTimestamp(produceTimestamp());
    m_reading.setLux(4.0);
    newReadingAvailable();
}

void InputLightlevel::timerEvent(QTimerEvent * /*event*/)
{
    poll();
}

