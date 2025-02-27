#ifndef MQTTYUDP_H
#define MQTTYUDP_H

#include <QObject>
#include <QUdpSocket>
#include <stdint.h>

class MqttyUdp : public QObject
{
    Q_OBJECT
public:
    explicit MqttyUdp( QObject *parent = nullptr); // QString url, int port,

    void sendUDP(const char *hexString);
    void sendUDP2(const char *data, size_t len);
    void StartConnect(void);

public slots:
    void rxUdpDataEvent();


signals:

private:
    QString m_Url;
    int     m_Port;
    QUdpSocket* udpSocket;
    uint16_t packetId;
    uint16_t topicId;
};

#endif // MQTTYUDP_H
