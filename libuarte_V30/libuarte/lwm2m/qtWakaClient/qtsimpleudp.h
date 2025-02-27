#ifndef QTSIMPLEUDP_H
#define QTSIMPLEUDP_H

#include <QObject>


class QtSimpleUdp : public QObject
{
    Q_OBJECT
public:
    explicit QtSimpleUdp( QObject *parent = nullptr);  // create a RX socket

    virtual int doConnect(QString url, int port);
    // 2 ways of sending
    virtual void sendUDPhex(const char *hexString);
    virtual long sendUDP(const char *data, size_t len);
signals:
    // all received data.
    void onReceive(const QByteArray ba);
public slots:
    virtual void rxUdpDataEvent();


protected:
   QString m_Url;
   int     m_Port;

};

#endif // QTSIMPLEUDP_H
