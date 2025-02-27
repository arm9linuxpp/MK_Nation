#ifndef QTIPV4UD_H
#define QTIPV4UD_H

#include "qtsimpleudp.h"

#include <QUdpSocket>

class QtIpV4Ud : public QtSimpleUdp
{
public:
    explicit QtIpV4Ud( QObject *parent = nullptr);
    virtual int doConnect(QString url, int port);
    virtual void doDisconnect();
    long sendUDP(const char *data, size_t len);

public slots:
    void rxUdpDataEvent();

private:
    QUdpSocket* udpSocket;
};

#endif // QTIPV4UD_H
