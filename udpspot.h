#ifndef UDPSPOT_H
#define UDPSPOT_H

#include <QObject>
#include <QUdpSocket>
//#include "networkconstants.h"
//#include "endpoint.h"
#include <controller.h>

class UdpSpot : public QObject
{
    Q_OBJECT
public:
    explicit UdpSpot(int port);
    void clearStat();
    void reopenUdp();

signals:
    void receiveJson(const QJsonObject &json);
public slots:
    void sendBack(const QByteArray &data);
    void sendJson(const QJsonObject &json);
private:
    const int m_port;
    QUdpSocket m_udp;
};
#endif // UDPSPOT_H
