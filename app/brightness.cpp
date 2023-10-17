#include "brightness.h"
#include <QDebug>

BrightnessCtl::BrightnessCtl(qreal start, qreal end, qreal step, 
        QObject *parent) : QObject(parent),m_start(start), m_end(end), m_step(step) {
    m_ctl.setSingleShot(true);
    m_ctl.setInterval(100);

    connect(&m_ctl, &QTimer::timeout, [=]() {
        emit actived(m_start);
        if(m_start < m_end) {
            m_start += m_step;
            if( (m_step >0 && m_start > m_end) || (m_step <0 && m_start < m_end) ) {
                m_start = m_end;
            }
            m_ctl.start();
        }
    });
}

void BrightnessCtl::setStart(qreal val) {
    m_start = val;
}

void BrightnessCtl::setEnd(qreal val) {
    m_end = val;
}

void BrightnessCtl::setStep(qreal val) {
    m_step = val;
}

void BrightnessCtl::start() {
    emit actived(m_start);
    m_start += m_step;
    if( (m_step >0 && m_start > m_end) || (m_step <0 && m_start < m_end) ) {
        m_start = m_end;
    }
    qDebug() << "BrightnessCtl " << m_start;
    m_ctl.start();
}

void BrightnessCtl::stop() {
    m_ctl.stop();
}