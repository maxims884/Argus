#include "udpspot.h"
#include <QHostAddress>
#include "log.h"
#include "logaddon.h"
#include <QNetworkDatagram>
#include <QJsonDocument>
#include <QJsonObject>
#include "logaddon.h"

UdpSpot::UdpSpot(int port)
    : m_port(port)
{
    TRACELOG;
    reopenUdp();
    LOGD() << "Will log first 30 pack from every peer and every 60 sec";
}

static bool isCanNoQuoute(const QByteArray &arr)
{
    for(const auto c : qAsConst(arr)){
        if((c < 32 || c > 126) && c != '\t'){
            return false;
        }
    }
    return true;
}

void UdpSpot::reopenUdp()
{
    TRACELOG;
    m_udp.close();
    QString ip = Controller::getInstance()->getKobusIp();
    LOGI() << "KOBUS IP " << ip;
    m_udp.connectToHost(ip, m_port);
}

void UdpSpot::sendBack(const QByteArray &data)
{
    TRACELOG;
            if(isCanNoQuoute(data))
                LOGDnq() << "Send TO" << /*m_lastEndPoint <<*/ data.size() << "bytes" << data;
            else
                LOGD()   << "Send TO" << /*m_lastEndPoint <<*/ data.size() << "bytes" << data;
//        }
        m_udp.write(data/*,m_lastEndPoint.host(),m_lastEndPoint.port()*/);
}

void UdpSpot::sendJson(const QJsonObject &json)
{
    QJsonDocument doc(json);
    auto data = doc.toJson(QJsonDocument::Compact);
    sendBack(data);
}
