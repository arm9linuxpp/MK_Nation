#include "vimoapi.h"

#include "mainwindow.h"

#include <QThread>

extern MainWindow * g_Main;


// implemented by China Team
extern "C" int ModemSend(const char *data, int txLen)
{
    g_Main->send2Modem(data, txLen);
    return 0;
}

extern "C" int ModemGet(char *data, int maxRx)
{

    return g_Main->getModemRx(data,maxRx);
}


extern "C" void ModemErrorLog(const char *msg)
{
    int len = strlen(msg);
    g_Main->doLog(msg,len);  // Use system logging Add Loglevel
}

extern "C" void ModemDebugLog(const char *msg)
{
    int len = strlen(msg);
    g_Main->doLog(msg,len);
}



