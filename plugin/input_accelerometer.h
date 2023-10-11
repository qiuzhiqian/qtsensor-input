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

#pragma once

#include <QSensorBackend>
#include <QAccelerometerReading>
#include <QAccelerometer>
#include <QtCore/QFile>
#include <QSocketNotifier>

#include <linux/ioctl.h>
#define ACCELERATION_RATIO_ANDROID_TO_HW        (9.80665f / 16384)
#define GSENSOR_IOCTL_MAGIC 'a'
#define GSENSOR_IOCTL_CLOSE                _IO(GSENSOR_IOCTL_MAGIC, 0x02)
#define GSENSOR_IOCTL_START                _IO(GSENSOR_IOCTL_MAGIC, 0x03)
#define GSENSOR_IOCTL_GET_CALIBRATION      _IOR(GSENSOR_IOCTL_MAGIC, 0x11, int[3])
#define EVENT_TYPE_ACCEL_X          ABS_X
#define EVENT_TYPE_ACCEL_Y          ABS_Y
#define EVENT_TYPE_ACCEL_Z          ABS_Z
#define RADIANS_TO_DEGREES 180.0/M_PI
#define OFFSET 5

class InputAccelerometer : public QSensorBackend
{
public:
    static char const * const id;

    InputAccelerometer(QSensor *sensor);
    ~InputAccelerometer();

    void start() override;
    void stop() override;
    void poll();
    void timerEvent(QTimerEvent * /*event*/) override;

private:
    QAccelerometerReading m_reading;
    QSocketNotifier *m_notifier;
    int m_timerid;

    quint8 valid_flag = 0x00;

    int accel_offset[3];
};
