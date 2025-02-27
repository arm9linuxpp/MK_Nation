#include "mqttyudp.h"

//#define CONFIG_DEFAULT_SERVER_IP "51.116.97.192"
//#define CONFIG_SERVER_REMOTE_PORT                       1883

//extern "C" void AT_Fake(const char *tx);
//extern "C" void AT_SetUpdRx(const char *tx);
//extern "C" int8_t  MyUdpSend(uint8_t *data, uint16_t length, Timeout_t timeout);
//#define UDP_SELFTEST 1

MqttyUdp::MqttyUdp( QObject *parent) : QObject(parent) // QString url, int port,
{
 //   m_Url = url;
 //   m_Port = port;

    // create a QUDP socket
    udpSocket = new QUdpSocket(this);

    bool result = false;
    //  QHostAddress my("192.168.43.57"); // virtueintelligence.test-ram.m2m.telekom.com ()
    result =  udpSocket->bind(QHostAddress::AnyIPv4, m_Port);

    qDebug() << result;
    if(result)
    {
        qDebug() << "socket->bind PASS";
    }
    else
    {
        qDebug() << "socket->bind FAIL";
    }

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(rxUdpDataEvent()));
    qDebug() << "socket-> connected";
}

void MqttyUdp::sendUDP(const char *hexString)
{
    //QString MyHexString(hexString);
    //QString MyHexString ="1d040401000a3836373732333033313738343538395047484b6f6b6141";
    QByteArray cmd = QByteArray::fromHex(hexString);
    QHostAddress dst(m_Url); // virtueintelligence.test-ram.m2m.telekom.com ()

    // Sends the datagram datagram
    // to the host address and at port.
    // qint64 QUdpSocket::writeDatagram(const QByteArray & datagram,
    //                      const QHostAddress & host, quint16 port)
    qint64 ret = udpSocket->writeDatagram(cmd, dst, m_Port);
    udpSocket->flush();
    qDebug() << "sendUDP: " << cmd   << " RET: " << ret;
}

void MqttyUdp::sendUDP2(const char *data, size_t len)
{
    QByteArray cmd(data,len);
    QHostAddress dst(m_Url); // virtueintelligence.test-ram.m2m.telekom.com ()

    // Sends the datagram datagram
    // to the host address and at port.
    // qint64 QUdpSocket::writeDatagram(const QByteArray & datagram,
    //                      const QHostAddress & host, quint16 port)
    qint64 ret = udpSocket->writeDatagram(cmd, dst, m_Port);
    udpSocket->flush();
    qDebug() << "qt5 sendUDP1: " << cmd.toHex()   << " send len: " << ret;
 //   qDebug() << "sendUDP2: " << cmd;
}

void MqttyUdp::rxUdpDataEvent()
{
    //qDebug() << "MqttyUdp::readyRead in !";

        QHostAddress sender;
        uint16_t port;
        while (udpSocket->hasPendingDatagrams())
        {
             QByteArray datagram;
             datagram.resize(udpSocket->pendingDatagramSize());
             udpSocket->readDatagram(datagram.data(),datagram.size(),&sender,&port);

             QString ip = sender.toString();
             qDebug() <<"Message: " << datagram <<" From: " << ip << port;
             int size = datagram.size();

#ifdef UDP_SELFTEST
             uint8_t data[512];
             memcpy(data,datagram.constData(),size);
             //uint8_t *data = (uint8_t *) ;
             //MqttsnParseResult_t  ret = MQTTSN_parsePacket(data, size);
             // MqttsnSendFunc_t sendFunc,uint8_t *bdata, size_t len,uint16_t *p_wasPublished

             static uint16_t gs_wasPublished = 0;

             int ret = vi_mqttsn_parsePacket(MyUdpSend, data, size, &gs_wasPublished);
             if (ret != 0)
             {
                 qDebug() <<"vi_mqttsn_parsePacket ret: " << ret;
             }

#else
             QByteArray dh = datagram.toHex();

             /// 1,220.180.239.212,8012,5,1245783132,0
             char buf[128];
             sprintf(buf,"\r\n1,%s,%d,%d,%s,0\r\n\r\nOK\r\n",ip.toLocal8Bit().constData(),port,size,dh.constData());

           //  AT_SetUpdRx(buf);

             // { MODEM_URC_DATA_RECEIVED, "\r\n+NSONMI:%d,%d\r\n" }
             /*
             +NSONMI:1,5 //Received the message
             AT+NSORF=1,5 //Read the message
             1,220.180.239.212,8012,5,1245783132,0
             */
             char msg[512];
             sprintf(msg,"\r\n+NSONMI:1,%d\r\n",size);
            // AT_Fake(msg);
#endif

        }

    /*
    // when data comes in
    QByteArray buffer;
    buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;

    quint16 senderPort = CONFIG_SERVER_REMOTE_PORT;

    // qint64 QUdpSocket::readDatagram(char * data, qint64 maxSize,
    //                 QHostAddress * address = 0, quint16 * port = 0)
    // Receives a datagram no larger than maxSize bytes and stores it in data.
    // The sender's host address and port is stored in *address and *port
    // (unless the pointers are 0).

    socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

    qDebug() << "Message from: " << sender.toString();
    qDebug() << "Message port: " << senderPort;
    qDebug() << "Message: " << buffer;
    */
}

void MqttyUdp::StartConnect(void)
{
// ./mqtt-sn-sub -h 51.116.97.192 -i 867723031784589PGHKokaA -t TOIL/levelmeter/1/out/867723031784589 -v -v -V -d -d -d


    //vi_mqttsn_doConnect(MyUdpSend);

    // qt5 sendUDP1:  "1d04040127103836373732333033313738343538395047484b6f6b6141"  send len:  29
//  char clientId[] = { "867723031784589PGHKokaA" };

    // qt5 sendUDP1:  "1d0404012710393930303030303030303030323032402c2d3f6e336050"  send len:  29
    //char clientId[] = { "990000000000202@,-?n3`P" };


/*
    uint16_t duration = 10000;
    bool cleanSession = true;
    packetId = 1;
    topicId  = 0;
    MqttsnResult_t res = MQTTSN_sendConnect(MyUdpSend, clientId, duration, cleanSession);
    qDebug() << "MQTTSN_sendConnect res: " << res;
*/

}

