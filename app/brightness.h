#pragma once

#include <QObject>
#include <QTimer>

class BrightnessCtl : public QObject {
    Q_OBJECT
public:
    BrightnessCtl(qreal start, qreal end, qreal step, QObject *parent=nullptr);

    void setStart(qreal val);
    void setEnd(qreal val);
    void setStep(qreal val);
    void start();
    void stop();
private:
    QTimer m_ctl;

    qreal m_start = 0;
    qreal m_end = 0;
    qreal m_step = 0;

signals:
    void actived(qreal);
};