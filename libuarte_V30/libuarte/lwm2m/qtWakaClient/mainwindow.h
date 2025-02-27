#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "qtipv4ud.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void send2Modem(const char *data,int size);
    int getModemRx(char *data, int maxRx);


    void doLog(const char *data, int len);
    void doErr(const char *data, int len);


private slots:
    void pb1();
    void pb2();
    void pb3();
    void pb4();
    void pb5();


    void time1sec();
    void time50ms();
    void onUdsReceive(const QByteArray ba);
    void rsReadFunction();
    void processRx(void);
    void oneLineRx(QByteArray line);
    void doDisconnect();
    void doConnect(void);
    void doConnect2(int pos);
    void doModemPrepare();
    void CotSend(bool bootstrap,int lmVersion);

private:
    Ui::MainWindow *ui;
    QtIpV4Ud qudp;
    QTimer   *timer1sec;
    QTimer   *timer50ms;

    // from
    QList<QSerialPortInfo> m_Ports;
    QSerialPort            m_Rs232;
    QByteArray             m_RxData;
    QStringList            m_RxList;

    uint16_t modemError;

};
#endif // MAINWINDOW_H
