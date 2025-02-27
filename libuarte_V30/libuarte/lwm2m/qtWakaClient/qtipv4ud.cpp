#include "qtipv4ud.h"

#include <QDebug>
#include <QHostInfo>
#include <QThread>
extern "C" {
#include "vi_lwm2m.h"
}

QtIpV4Ud::QtIpV4Ud(QObject *parent)
    : QtSimpleUdp{ parent}
{
    qDebug() << "QtIpV4Ud";
}

void QtIpV4Ud::doDisconnect()
{
    qDebug() << "doDisconnect";
    udpSocket->close();
}





int QtIpV4Ud::doConnect(QString url, int port)
{
    QtSimpleUdp::doConnect(url,port);
    udpSocket = new QUdpSocket(this);
    bool result = false;
    result =  udpSocket->bind(QHostAddress::AnyIPv4, m_Port);

    qDebug() << result;
    if(result)
    {
        qDebug() << "socket->bind PASS * at port: " << m_Port;
    }
    else
    {
        qDebug() << "socket->bind FAIL";
        return 0;
    }

    //        connect(udpSocket, SIGNAL(readyRead()), this, SLOT(rxUdpDataEvent()), Qt::QueuedConnection);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(rxUdpDataEvent()));

    qDebug() << "socket-> connected";
    return 1;
}

void QtIpV4Ud::rxUdpDataEvent()
{
    qDebug() << "QtIpV4Ud::rxUdpDataEvent in !";

    QHostAddress sender;
    uint16_t port;
    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size(),&sender,&port);

        QString ip = sender.toString();
        int size = datagram.size();
        qDebug() << getTs() << "Message: "<< size << " -> " << datagram.toHex() <<" From: " << ip << port;

        //QThread::msleep(4100);
        //qDebug() << getTs() << "XXXX";

        emit onReceive(datagram);

    }
}

long QtIpV4Ud::sendUDP(const char *data, size_t len)
{
    QByteArray cmd(data,len);


    QHostInfo info = QHostInfo::fromName(m_Url);

    QHostAddress dst = info.addresses().first(); // virtueintelligence.test-ram.m2m.telekom.com ()


    //fprintf(stderr, "%s Sending %d bytes to [%s]:%d\r\n" , getTs(), cmd.size(), m_Url.toStdString().c_str(), m_Port );

    qDebug() << "sendUDP(QT)" << len;

    std::string s = cmd.toHex().toStdString();

    fprintf(stderr, "%s QtIpV4Ud::sendUDP: %ld %s\n" , getTs(),s.size(),s.c_str());


    // Sends the datagram datagram
    // to the host address and at port.
    // qint64 QUdpSocket::writeDatagram(const QByteArray & datagram,
    //                      const QHostAddress & host, quint16 port)
    qint64 ret = udpSocket->writeDatagram(cmd, dst, m_Port);
    udpSocket->flush();
  /*
Sending 105 bytes to [176.9.148.44]:7685
16 FE FF 00  00 00 00 00  00 00 01 00  5C 01 00 00   ............\...
50 00 01 00  00 00 00 00  50 FE FD C3  85 AE 4C DD   P.......P.....L.
7E 78 E3 85  B8 21 C7 7B  2E D6 4E DC  11 33 32 14   ~x...!.{..N..32.
3D E2 C8 58  BC DD 5D 52  33 A3 7F 00  20 8A 23 C1   =..X..]R3... .#.
C5 F4 0D DB  14 DC 99 98  57 AA 56 00  91 05 63 B7   ........W.V...c.
A5 D6 E1 52  8B 77 BD 16  92 6B 6F C5  55 00 02 C0   ...R.w...ko.U...
A8 01 00 00  04 00 17 00  00                         .........
[lwm2m_step:384] timeoutP: 60
*/
    if (ret != len) {
        qDebug() << getTs() << "sendUDP2 send error : " << ret;
    }
    //    qDebug() << "qt5 sendUDP1: " << cmd.toHex()   << " send ret: " << ret << " url " << dst << " port: " << m_Port;
//
    return ret;
}
