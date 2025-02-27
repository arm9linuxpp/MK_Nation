#include "qtsimpleudp.h"

#include <QDebug>

QtSimpleUdp::QtSimpleUdp(QObject *parent)
    : QObject{parent}
{

}

int QtSimpleUdp::doConnect(QString url, int port)
{
    m_Url = url;
    m_Port = port;
    return 0;
}

void QtSimpleUdp::sendUDPhex(const char *hexString)
{
    qDebug() << "sendUDP base: " << hexString;
    QByteArray cmd = QByteArray::fromHex(hexString);
    sendUDP(cmd.constData(), cmd.size());
}

long QtSimpleUdp::sendUDP(const char *data, size_t len)
{
    qDebug() << "sendUDP2 base: " << len;
    return len;  // all send
}

void QtSimpleUdp::rxUdpDataEvent()
{
    qDebug() << "rxUdpDataEvent ";
}
