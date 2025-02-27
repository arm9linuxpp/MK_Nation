#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>

extern "C" {
#include "vi_lwm2m.h"

#include "../ViApi/vimoapi.h"

}



MainWindow * g_Main = 0;


QtIpV4Ud *g_qudp = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    g_Main = this;
    modemError = -1;

    ui->setupUi(this);
    timer1sec = new QTimer(this);
    timer50ms = new QTimer(this);


    connect(ui->pb1,SIGNAL(clicked()),this,SLOT(pb1()));
    connect(ui->pb2,SIGNAL(clicked()),this,SLOT(pb2()));
    connect(ui->pb3,SIGNAL(clicked()),this,SLOT(pb3()));
    connect(ui->pb4,SIGNAL(clicked()),this,SLOT(pb4()));
    connect(ui->pb5,SIGNAL(clicked()),this,SLOT(pb5()));

    connect(timer1sec,SIGNAL(timeout()),this,SLOT(time1sec()));
    timer1sec->start(1000);

    connect(timer50ms,SIGNAL(timeout()),this,SLOT(time50ms()));
    timer50ms->start(50);



    m_Ports = QSerialPortInfo::availablePorts();
    qDebug() << "Ports" << m_Ports.size();
    foreach( QSerialPortInfo item, m_Ports )
    {
        QString pn = item.portName();
        qDebug() << pn;
        ui->cbPort->addItem(pn);
        // will items always be processed in numerical order by index?
        // do something with "item";
    }

    connect(ui->pbOpen,        SIGNAL (clicked()),this, SLOT (doConnect()));
    connect(ui->pbModePrepare, SIGNAL (clicked()),this, SLOT (doModemPrepare()));
    //  connect(ui->pbClose,   SIGNAL (clicked()),this, SLOT (doDisconnect()));
    connect(&m_Rs232,          SIGNAL(readyRead()), this, SLOT(rsReadFunction()));

}

MainWindow::~MainWindow()
{
    timer1sec->stop();
    delete timer1sec;
    delete ui;
}

extern "C" void xxx_onUdsReceive(uint8_t *b, int len);  // can't include other
extern "C" void doNextStep(void);


void MainWindow::onUdsReceive(const QByteArray ba)
{
    qDebug() << getTs() << " XXXX onUdsReceive len: " << ba.size();
    uint8_t *b = (uint8_t *)ba.data();
    int len = ba.size();
    xxx_onUdsReceive(b, len);

    // from original while loop done on all network rx.
    doNextStep();
  // !!!???  conn_s_updateRxStatistic(objArray[7], numBytes, false);

}

extern "C" {
#include "../examples/client/lwm2mclient.h"

#include "qtlwm2mclient.h"
extern MY_TYPE g_LwM2M;
}

void MainWindow::pb1()
{
    myClientInit();
    qDebug() << " XXXX PB1";

    connect(&qudp,SIGNAL( onReceive(const QByteArray)    ),this,SLOT( onUdsReceive(const QByteArray) ));

  //  qudp.doConnect("192.168.43.200",5683);  // no coaps BS !
    uint32_t port = atoi(g_LwM2M.serverPort);
    qudp.doConnect(g_LwM2M.server,port);  // coaps !
    qDebug() << " XXXX PB1 udp connected" << g_LwM2M.server << " "<< g_LwM2M.serverPort;
    g_qudp = &qudp;
}

void MainWindow::pb2()
{
    qDebug() << " XXXX PB2";

   // qudp.sendUDPhex("0002");

    main_wakaama();   // start lwm2m client

}

void MainWindow::pb3()
{
    qDebug() << " XXXX PB3";
    qudp.doDisconnect();
}

void MainWindow::pb4()
{
    const char * bin = "4145";
    qDebug() << " XXXX PB4 data";
     int ret = upd_wakaama(34799,0,0,bin);
    //int ret = upd_wakaama(19,0,0,bin);
    qDebug() << " XXXX PB4 ret " << ret;
}

extern "C" void lwm2m_oneSec(void);

void MainWindow::pb5()
{
    const char * bin = "123.0";
    qDebug() << " XXXX PB5 data";
    int ret = upd_wakaama(3303,0,5700,bin);

    // 7,0,2 tx data.
    //int ret = upd_wakaama(7,0,2,bin);
    qDebug() << " XXXX PB5 ret " << ret;

  //  lwm2m_oneSec();

}


void MainWindow::send2Modem(const char *data,int size)
{
    // qDebug() << "send2Modem" << data; done insice when debug is on.
    m_Rs232.write(data);
    m_Rs232.flush();
}

int MainWindow::getModemRx(char *data, int maxRx)
{
    int rxLines = m_RxList.size();
    if (rxLines > 0)
    {

        QString s = m_RxList.at(0);
        m_RxList.pop_front();
        QByteArray ba = s.toLocal8Bit();
        int len = ba.size();
        if (len < maxRx)
        {
            for (int i=0;i< len;i++)
            {
                data[i]=ba.at(i);
            }
            data[len]=0; // terminate
            return len;
        } else {
            qDebug() << "MainWindow::getModemRx" << len << " > maxTx" << maxRx;
        }
    }
    return 0;
}


void MainWindow::doLog(const char *data, int len)
{
    if (len == 1)
    {
        // Filter "CR"
        return;
    }
    qDebug() << "doLog" << ":" << data;
}

void MainWindow::doErr(const char *data, int len)
{
    qWarning() << "doErr" << ":" << data;
}

void MainWindow::doDisconnect()
{
    m_Rs232.close();
    qDebug() << "Disconnected";
}


void MainWindow::doModemPrepare()
{
    CotSend(false,false);
    modemError = 0;
}

void MainWindow::doConnect(void)
{
    int pos = ui->cbPort->currentIndex();
    doConnect2(pos);
}

void MainWindow::doConnect2(int pos)
{
    QSerialPortInfo spi = m_Ports.at(pos);
    QString pn = spi.portName();

    m_Rs232.setBaudRate(QSerialPort::Baud115200);
    m_Rs232.setDataBits(QSerialPort::Data8);
    m_Rs232.setParity(QSerialPort::NoParity);
    m_Rs232.setPort(spi);
    bool isOpen = m_Rs232.open(QIODevice::ReadWrite);
    if (isOpen)
    {
        qDebug() << "connect" << pn << " open= " << isOpen;
    } else {
        qDebug() << "FAILED connect" << pn << " open= " << isOpen;
    }
}


void MainWindow::rsReadFunction()
{
    QByteArray arrivedMsg = m_Rs232.readAll();
    //qDebug() << "RX: " << arrivedMsg.toHex();
    qDebug() << "RX: " << arrivedMsg;
    m_RxData.append(arrivedMsg);               // add Data...we may have already some
    processRx();
}

void MainWindow::oneLineRx(QByteArray line)
{
    // qDebug() << "oneLineRx" << line;
    char * pd = line.data();
    uint8_t *cd = (uint8_t*) pd;
}


void MainWindow::processRx(void)
{

    bool more = true;
    while(more)
    {
        int index = m_RxData.indexOf('\n');
        if (index>=0)
        {

            QByteArray line = m_RxData.mid(0,index+1);
            QByteArray rest = m_RxData.mid(index+1);

//       qDebug() << "processRx " << index << line << " rest " << rest;
            oneLineRx(line); // .trimmed());  -> to old log

    //qDebug() << "processRx " << line;
            m_RxList.append(line);

            m_RxData = rest;
        } else {
            more = false;
        }
    }
}

 static CotParameter Cotparameter;

void MainWindow::CotSend(bool bootstrap,int lmVersion)
{
    memset(&Cotparameter,0,sizeof(CotParameter));

    Cotparameter.cmdTimeout               =  1000;   // for normal AT command 1 sec
    Cotparameter.modemConnectTimeoutMs    = 60000;   // 60 sec
    Cotparameter.modemSocketOpenTimeoutMs = 90000;   // 90 sec

    Cotparameter.mqttsnIp[0]=0;
#ifdef MU_WITH_DNS
    Cotparameter.dns_Ip[0]=0;
#endif

#ifdef BG95
    Cotparameter.cops = 26201;          // Operator For Telekom
#ifdef USE_CH_SIM
    Cotparameter.cops = 46000;          // for sample in china
#endif
#else
    Cotparameter.cops = 0;  // AUTO detect for BG660
#endif
    //Cotparameter.cops = 23203;  // AT OK 1.7.2022
    //Cotparameter.cops = 26202;          // Operator For VODA roaming test
    //Cotparameter.cops = 22801;
    // Cotparameter.cops = 22802;// FL ??  1.7.2022   CREG = 3 denyed
    //Cotparameter.cops = 22210; // IT 2.7.2022 via search.
#ifdef DT_COT
    strcpy(Cotparameter.mqttsnIp        , "51.116.97.195"); // "lwm2m.test-ram.m2m.telekom.com");  // mqtt.mu-tec.de UDP 1883 yao kiste hetzner           alias vi-srv-01
    Cotparameter.mqttsnPort = 5784;
#else
    strcpy(Cotparameter.mqttsnIp        , "168.119.70.243");
    Cotparameter.mqttsnPort = 7685;
#endif


#ifdef PRIVATE_APN
    strcpy(Cotparameter.apnName   , "nb-cloud.ic.m2mportal.de");  //Private APN: nb-cloud.ic.m2mportal.de 12/2024
    //strcpy(Cotparameter.apnName   , "");
#else
    //strcpy(Cotparameter.apnName   , "");              // AUTO
    strcpy(Cotparameter.apnName   , "internet.nbiot.telekom.de");   // Yao mein internet.m2mportal.de 2024/30.08
#endif

#ifdef USE_TESTRAM
    strcpy(Cotparameter.apnName   , "internet.m2mportal.de");   // kein NB-IOT.
#endif

    uint16_t ret = sendCot( &Cotparameter);
    qDebug() << " sendCot: " << ret;
}

void MainWindow::time50ms()
{
    // if (modemError == 0)
    {
        modemError = loopCot50Ms();  //call it allways... needed for time ticking.
        if (modemError != 0)
        {

            if ( modemError == VCARC_DONE)
            {

            }
        }
    }
}


extern "C" void viAppendUdp(const char *rawData, int len);

extern "C" int Xsendto(int sockfd, const void *buf, size_t len, int flags ) {
//    sprintf("Xsendto ???? !!!! XXXXX2\r\n");

#ifdef USE_BG95
    const char *dbuf = (const char *) buf;
    viAppendUdp(dbuf,len);
#else
    g_qudp->sendUDP((const char *)buf,len);
#endif
    return len;
}

void MainWindow::time1sec()
{
 //   qDebug() << "MainWindow::time1sec";
    lwm2m_oneSec();
}

extern "C" void log_handler(int level, const char *message)
{
    //fprintf( stderr, "%d %s", level, message);
    // for DTLS logging
    qDebug() << "log_handler: " << message;

}

extern "C" long clock_time() {
    return 42;
}

