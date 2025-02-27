#include "BG77.h"
#include "vimoapi.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vi_lwm2m.h"



CotParameter *gs_MoPar;

// for all standard AT lengh buffer sprintf
#define STD_AT_LEN_BYTE  128


#define UDP_TX_MAX_LEN_BYTE 300
#define UDP_TX_MAX_LEN_HEX UDP_TX_MAX_LEN_BYTE*2

// we must be able to reveiver a complete UDP Packet? 1500
#define UDP_RX_MAX_LEN_BYTE 500
#define UDP_RX_MAX_LEN_HEX UDP_RX_MAX_LEN_BYTE*2


enum MoSm{
    MOSM_SLEEP              = 0,
    MOSM_WAKE               = 1,  // 0
    MOSM_INIT               = 10,    // ASK
    MOSM_REG_URC            = 11,
    MOSM_SET_URC            = 12,
    MOSM_ASK_LAC            = 13,
    MOSM_GET_LAC            = 14,
    MOSM_ASK_IP             = 15,
    MOSM_ASK_TPYE           = 16,  //

    MOSM_QICLOSE2           = 18,
    MOSM_QICLOSE0           = 19,


    MOSM_QICLOSE            = 21,
    MOSM_PWESAVEOFF         = 22,
    MOSM_FUNOFF             = 23,
    MOSM_APNSET             = 24,
    MOSM_BANDS              = 25,
    MOSM_OPTELEKOM          = 26,
    MOSM_OP_INFO            = 27,
    MOSM_OP_SEARCH          = 29,

    // BG95 start with FUN=0
    MOSM_BG95_FUNOFF        = 30,
    MOSM_BG95_MODE          = 31,
    MOSM_BG95_BAND          = 32,
    MOSM_BG95_NWSCANMODE    = 33,
    MOSM_BG95_NWSCANSEQ     = 34,
    MOSM_BG95_SERVICEDOMAIN = 35,
    MOSM_BG95_FUNON         = 36,

//    MOSM_BG95_FUNON_1       = 30,    //https://docs.iotcreators.com/v2.1/docs/quectel-bg95-m3-nusim
    MOSM_BG95_IP            = 40,
    MOSM_BG95_COPS          = 41,
    MOSM_BG95_WAIT          = 42,
    MOSM_BG95_DELAY         = 43,
    MOSM_BG95_ACOPS         = 44,

    MOSM_BG95_FUN_DELAY   = 48,


//    MOSM_BG95_CTX1          = 40,
//    MOSM_BG95_CTX2          = 41,



    MOSM_FUNON              = 50,
    MOSM_IMSI               = 51,  // just for logging
    MOSM_IMEI               = 52,  // just for debugging
    MOSM_FW_ALL             = 53,  //    AT+QGMR        BG77LAR02A04_01.204.01.204


//    MOSM_APN_READ           = 53,   // cause timeout V16
    MOSM_COPS_READ          = 54,
    MOSM_POS2               = 55,
#ifdef MU_WITH_DNS
    MOSM_DNS1               = 67,
    MOSM_DNS2               = 68,
    MOSM_DNS3               = 69,
#endif
    MOSM_QIFMT              = 60,
    MOSM_QIOPEN             = 61,

    MOSM_DISCONNECT_WAIT    = 63,
    MOSM_BG95_QIOPEN_LP     = 65,  // local UDP service. incomeing
    MOSM_BG95_QIOPEN_RP     = 66,  // remote..

    MOSM_BG95_QIOPEN_DLY    = 67,  // delay after open
    MOSM_UDP_READY          = 70,  // we are ready 300sec for next UDP action.
    MOSM_DONE               = 71,

    MOSM_ERR_DISCONNECT     = 80,

    MOSM_ERR_WAIT           = 81   // wait 2 sec... ignore all responses...
};



static enum MoSm     gsMoStateMachine = 0;
static int      gsStateRepeatCnt = 0;
static int      gsBg95WaitCnt = 0;
static uint32_t gsMoCmdTimeoutCnt = 0;      // timeout for one command 101
static uint32_t gsMoGuardTimeoutMs = 0;    // overall timeout. ERROR 100 in ms

#ifdef BG95
static int      gBg95UdpRxStatus = 0;
static char     gBg95UdpBuffer[UDP_RX_MAX_LEN_HEX];

// we need 3 in one count.
static int      gUdpTxLen = 0;  // how many bytes we have in buffer.
static int      gUdpTxcnt = 0;  // how many gUdpTxJunkCnt we have
static int      gUdpTxJunkCnt[5];
static int      gUdpTxDoneLen = 0;
static int      gUdpTxDoneCnt = 0;    // when sending
static char     gUdpTxDta[UDP_TX_MAX_LEN_BYTE];
static int      gUdpTxState = -1;

static int      gBgAfterWaitState = 0;
static int      gBgAfterWaitVal = 0;
static int      gBgAfterWaitCnt = 0;
#endif



#define TIMEOUT_MS_VIAPI 50



#ifdef QT_WIDGETS_LIB
    #define VI_MAX_LOG_LEN 640  // 543 ist volle config 10.1.2025
#else
    #define VI_MAX_LOG_LEN 64
#endif

void sendAt(const char *cmd)
{
    int txLen = strlen(cmd);


#ifdef WITH_VI_DEBUG_LEVEL
    char msg[VI_MAX_LOG_LEN+64];  // 80 log ... 32 sprintf text...
    if (txLen < VI_MAX_LOG_LEN )
    {
        // log all...
        sprintf(msg,"sendAt[%02d]%s",txLen,cmd);
    } else {
        // log only first 64
        char shortCmd[64];
        int i=0;
        for (i=0; i<60;i++)
        {
            shortCmd[i]=cmd[i];
        }
        shortCmd[i]=0; // terminate

        sprintf(msg,"sendAt[%02d]%s...",txLen,shortCmd);
    }
    ModemDebugLog(msg);
#endif
    ModemSend(cmd, txLen);
}

// send with nagle algo ... collect and send later.
void viAppendUdp(const char *rawData, int len)
{
#ifdef MU_APPEND_FULL
    char one[16];
    char dump[UDP_TX_MAX_LEN_HEX];
    dump[0]=0;

    for (int i=0; i<len; i++)
    {
        sprintf(one,"%02X",rawData[i]&0xff);
        strcat(dump,one);
    }
    ModemErrorLog(dump);
#endif




    int rest  = UDP_TX_MAX_LEN_BYTE - gUdpTxLen;
    if (len > rest) {
        ModemErrorLog("viAppendUdp len > rest");
        return;
    }

    for (int i=0;i<len;i++) {
        gUdpTxDta[gUdpTxLen++]=rawData[i];
    }

    char msg[STD_AT_LEN_BYTE];  // 256*2 + AT command
    sprintf(msg,"viAppendUdp %d %d bytes",len, gUdpTxcnt);
    ModemDebugLog(msg);

    gUdpTxJunkCnt[gUdpTxcnt++] = len;

    gUdpTxState   = 0; // idle
    gUdpTxDoneCnt = 0;
    gUdpTxDoneLen = 0;    
}

void viSendUdpPre(int len)
{
   // only one frame.
    char tx[STD_AT_LEN_BYTE];  // 256*2 + AT command
    sprintf(tx,"AT+QISEND=0,%d\r\n",len);  // Length in send byte ( format idependet!)
    sendAt(tx);
    return;
}

void viSendUdpPost(const char *hex)
{    
#ifdef BG95
    int slen=strlen(hex);
    if (slen < UDP_TX_MAX_LEN_HEX) {       
        sendAt(hex);        
        return;
    } else {
        ModemErrorLog("UDP BC68_MAX_LEN oversize");
    }
#endif
}

/* remove 5.2.2025

int8_t  UdpQiSend(uint8_t *data, uint16_t length, uint32_t timeout)
{
    char one[16];
    char tx[UDP_TX_MAX_LEN_HEX+8];

    if (length > UDP_TX_MAX_LEN_HEX ) {
        ModemErrorLog("UdpQiSend BC68_MAX_FULL_LEN error");
        return -1;
    }

    tx[0]=0;
    for (int i=0; i<length; i++)
    {
        sprintf(one,"%02X",data[i]);
        strcat(tx,one);
    }
    viSendUdpPre(tx);
    delayCot50ms();
    viSendUdpPost(tx);
    return 0;
}
*/


void newCotState(enum MoSm newState, int timeoutVal)
{
    int oldState = gsMoStateMachine;
    char msg[DEBUG_BUFFER_MIN];
    gsMoCmdTimeoutCnt   = timeoutVal/TIMEOUT_MS_VIAPI;  // 1 sec -> to counts
#ifdef WITH_VI_DEBUG_LEVEL
    char xxx[255];
    int x1 = newState;
    sprintf(xxx,"newCotState %ld with timeout ticks %ld (%d ms)", x1,gsMoCmdTimeoutCnt,timeoutVal);
    ModemDebugLog(xxx);
#endif
    gsMoStateMachine = newState;

//    sprintf(msg,"timeout set %d",gsMoCmdTimeoutCnt);
//    ModemDebugLog(msg); 

    gsStateRepeatCnt = 0;

    switch(gsMoStateMachine)
    {
    case MOSM_UDP_READY:
      gs_MoPar->udpStatus = 1;
      break;
    case MOSM_WAKE:
        sendAt("ATE0\r\n");  // no echo
        break;

    case MOSM_INIT:
        gs_MoPar->intCmdParameter1 = 0;
        sendAt("AT+CGPADDR\r\n");  // Check IP must be online.
        // +CGPADDR: 1,10.157.75.85 Then PIng and work?
        break;

    case MOSM_REG_URC:
        sendAt("AT+CREG?\r\n");  // check for 4
        break;

    case MOSM_SET_URC:
        sendAt("AT+CREG=2\r\n");  // show changes.
        break;


    case MOSM_ASK_LAC:
        sendAt("AT+CREG?\r\n");  // show changes.
        break;


    case MOSM_ASK_IP:
        //sendAt("AT+COPS?\r\n");  // Check IP must be online.
        // +COPS: 1,2,\"26201\",9   ->

        sendAt("AT+CREG?\r\n");  // Check IP must be online.

        //sendAt("AT+CGREG?\r\n");  // Check IP must be online.
        // +CGREG: <n>,<stat>[,[<lac>],[<ci>],[<AcT>],[<rac>][,<cause_type>,<reject_cause>]]


        break;

    case MOSM_QICLOSE2:
        sendAt("AT+QICLOSE=2\r\n");   // //Close a connection whose socket service index is 0.
        break;
    case MOSM_QICLOSE0:
        sendAt("AT+QICLOSE=0\r\n");   // //Close a connection whose socket service index is 0.
        break;


    case MOSM_QICLOSE:
        sendAt("AT+QICLOSE=0\r\n");   // //Close a connection whose socket service index is 0.
        break;


    case MOSM_PWESAVEOFF:
        sendAt("AT+QSCLK=0\r\n");   // diable power save
        break;

    case MOSM_BG95_FUNOFF:
    case MOSM_FUNOFF:
        sendAt("AT+CFUN=0\r\n");
        break;

    case MOSM_APNSET:
        //sendAt("AT+QCGDEFCONT=\"IP\",\"nb-cloud.ic.m2mportal.de\"\r\n");
        if (strlen(gs_MoPar->apnName)>0) {
            sprintf(msg,"AT+QCGDEFCONT=\"IP\",\"%s\"\r\n",gs_MoPar->apnName);
        } else {
            sprintf(msg,"AT+QCGDEFCONT=\"IP\"\r\n");
        }
        sendAt(msg);
        break;

    case MOSM_BANDS:
#ifndef BAND_LIMIT_20
        sendAt("AT+QBAND=2,8,20\r\n");
#else
        sendAt("AT+QBAND=1,20\r\n");  // 20(VODA) roaming test
#endif
        break;


    case MOSM_OPTELEKOM:
        // sendAt("AT+COPS=1,2,\"26201\",9\r\n");
        if (gs_MoPar->cops == 0)
        {
            sprintf(msg,"AT+COPS=0\r\n");
        } else {
            sprintf(msg,"AT+COPS=1,2,\"%d\",9\r\n",gs_MoPar->cops);
        }
        sendAt(msg);
        break;

    case MOSM_OP_INFO:
        sendAt("AT+CEREG=5\r\n");
        break;

    case MOSM_OP_SEARCH:
        //sprintf(msg,"AT+COPS?\r\n");  // dummy just read
        //sprintf(msg,"AT+COPS=1,2,\"%d\",9\r\n",26202);  // Vodaphone Band 20

        // Search... ma 305 sec actual 70 sec. one band
        // 3 band over 180 sec
        gsMoCmdTimeoutCnt = gsMoCmdTimeoutCnt*350; // need time
        sprintf(msg,"timeout XXL %d",gsMoCmdTimeoutCnt);
        ModemDebugLog(msg);

        sendAt("AT+COPS=?\r\n");



        //sprintf(msg,"AT+QBAND?\r\n");  //Query the band that has been set earlier.

        //sprintf(msg,"AT+QBAND=?\r\n"); //List bands


        break;

    case MOSM_BG95_IP:
        //at+cgdcont=1,"IP","internet.nbiot.telekom.de"
        sprintf(msg,"at+cgdcont=1,\"IP\",\"%s\"\r\n",gs_MoPar->apnName);        
        sendAt(msg);
        break;

    case  MOSM_BG95_COPS:
        // =0 does not work here?
        // at+CEREG?
        // at+COPS=?
        // at+COPS?
        // AT+CGPADDR
        // at+COPS=0
        // at+COPS=1,2,"26201"
        // AT+CFUN=1
        // AT+CFUN=0
        if (gs_MoPar->cops == 0 )
        {
            sprintf(msg,"AT+COPS=0\r\n");
        } else {
            sprintf(msg,"AT+COPS=1,2,\"%d\",9\r\n",gs_MoPar->cops);
            //sprintf(msg,"AT+COPS=1,2,\"%d\",9\r\n",26201);
        }
        sendAt(msg);
        //sendAt("AT+COPS=1,2,\"26201\"\r\n");
        //sendAt("AT+COPS=1,2,\"0\"\r\n");
        break;


     case MOSM_BG95_MODE:
        sendAt("at+qcfg=\"iotopmode\",1,1\r\n");  // only NBIOT
        break;


    case MOSM_BG95_BAND:
        //# The band for NB-IoT should be set to 8 (Telekom Germany). If the above query
        //# doesn't end with ",0x80" set it to band 8:
        //at+qcfg="band",0,0,80,1
        sendAt("at+qcfg=\"band\",0,0,80,1\r\n");  // LM only 8(TELEKOM) 20(VODA)
        break;

    case MOSM_BG95_NWSCANMODE:
        sendAt("AT+QCFG=\"nwscanmode\",3,1\r\n");  // only NBIOT
        break;
    case MOSM_BG95_NWSCANSEQ:

#ifdef PRIVATE_APN
        sendAt("at+qcfg=\"nwscanseq\",030303,1\r\n"); // from NB-IOT telekom guide

#else
        sendAt("AT+QCFG=\"nwscanseq\",030201\r\n");  // try all NB-IOT the last.
#endif
        break;
    case MOSM_BG95_SERVICEDOMAIN:
        sendAt("AT+QCFG=\"servicedomain\",1,1\r\n");  // only NBIOT
        break;



//    case MOSM_BG95_FUNON_1:
    case MOSM_BG95_FUNON:
        sendAt("AT+CFUN=1\r\n");  // wait for online
        break;

    case MOSM_BG95_WAIT:
        gsBg95WaitCnt = 60;
        sendAt("AT+CGPADDR\r\n");  // wait IP address
        break;


    case MOSM_BG95_ACOPS:
        sendAt("at+qnwinfo\r\n");  // wait for online
        break;


    case MOSM_FUNON:
        sendAt("AT+CFUN=1\r\n");  // wait for online
        break;

    case MOSM_IMSI:
        sendAt("AT+CIMI\r\n");// IMSI
        break;

    case MOSM_IMEI:
#ifdef BG95
        sendAt("AT+GSN\r\n");  // IMEI
#else
        sendAt("AT+CGSN=1\r\n");  // IMEI
#endif

        break;

    case MOSM_FW_ALL:
        sendAt("AT+QGMR\r\n");// FW extended info
        break;
/*
   case MOSM_APN_READ:
        sendAt("AT+CGCONTRDP\r\n");  // APN read out
        break;
*/
    case MOSM_COPS_READ:
         sendAt("AT+COPS?\r\n");  // APN read out
         break;

#ifdef MU_WITH_DNS
    case MOSM_DNS1:
        // 62.157.169.104 	Essen 	3320  Deutsche Telekom AG
         // 8.8.8.8 google
         sendAt("AT+QIDNSCFG=0,\"62.157.169.104\",\"8.8.8.8\"\r\n");  // set dns server
         //sendAt("AT+QIDNSCFG=0,\"42.42.42.42\",\"43.43.43.43\"\r\n");  // set dns server ( timeout)
         break;
    case MOSM_DNS2:
         sendAt("AT+QIDNSGIP=0,\"lm1.ftpiot.de\"\r\n");  // set dns server
         break;

#endif

    case MOSM_POS2:
#ifdef BG95
        //AT+CGATT=1
        //OK
        //+CGREG: 1,"FFFE","82E76B2",9
   //     sendAt("AT+CGATT=1\r\n");  // CELLID
        sendAt("AT+CGPADDR\r\n");


#else
        sendAt("AT+QENG=0\r\n");  // CELLID
#endif

        break;
/*
    case MOSM_BG95_CTX1:
        {
            char tx[BC68_MAX_LEN];
            //AT+QICSGP=1,1,"CMNBIOT","","",1
            sprintf(tx,"AT+QICSGP=1,1,\"%s\",\"\",\"\",1 \r\n", gs_MoPar->apnName);
            sendAt(tx);

        }
        break;

    case MOSM_BG95_CTX2:
        sendAt("AT+QIACT=1\r\n");
        break;
*/
    case MOSM_QIFMT:
        sendAt("AT+QICFG=\"dataformat\",1,1\r\n");  // Configure to send data in Hex mode both.
//        sendAt("AT+QICFG=\"dataformat\",0,1\r\n");  // Configure to send data in Hex mode both.

        //              OK
        break;

    case MOSM_QIOPEN:
          sendAt("AT+QIOPEN=0,0,\"UDP SERVICE\",\"127.0.0.1\",1,1883,1\r\n");
//        OK
//        +QIOPEN: 0,0 //Connected successfully. It is recommended to wait for 60 s for the URC to be reported.
        break;

    case MOSM_BG95_QIOPEN_LP:
        // Connect ID 2 Context = 1
        sendAt("AT+QIOPEN=1,2,\"UDP SERVICE\",\"127.0.0.1\",0,1883,0\r\n");
        break;

    case MOSM_BG95_QIOPEN_RP:
        // CAN ONLY SEND!!!   Connect ID 0, context 1
        {
            char tx[STD_AT_LEN_BYTE];  // 256*2 + AT command
            sprintf(tx,"AT+QIOPEN=1,0,\"UDP\",\"%s\",%d\r\n",gs_MoPar->mqttsnIp, gs_MoPar->mqttsnPort);
            sendAt(tx);
        }
        break;



    case MOSM_DONE:
        break;

    case MOSM_DISCONNECT_WAIT:
          break;

    case MOSM_ERR_WAIT:
        break;
    }
}


void newCotStateWithDelay(enum MoSm newState, int timeoutVal)
{
#ifdef BG95
    ModemDebugLog("MOSM_BG95_FUN_DELAY start");
    gBgAfterWaitState = newState;
    gBgAfterWaitVal = timeoutVal;
    gBgAfterWaitCnt = 100;
    newCotState(MOSM_BG95_FUN_DELAY, 20000);  // MAX 20 sec

#else
    newCotState(newState, timeoutVal);
#endif
}


void logLongData(const char *data, int rxLen, int line)
{

#ifdef WITH_VI_DEBUG_LEVEL
    // int rxLen = strlen(data);
    char msg[64+32];
    if (rxLen < 64 )
    {
        // log all...
        sprintf(msg,"logLongData: mrx[%02d]%s",rxLen,data);
    } else {
        // log only first 64
        char shortCmd[64];
        int i=0;
        for (i=0; i<60;i++)
        {
            shortCmd[i]=data[i];
        }
        shortCmd[i]=0; // terminate

        sprintf(msg,"logLongDatA: mrx[%02d]%s...",rxLen,shortCmd);
    }
    ModemDebugLog(msg);
#endif

}

void setErrorAndDisconnect(int err)
{
    char msg[DEBUG_BUFFER_MIN];
    sprintf(msg,"setErrorAndDisconnect %d State: %d",err, gsMoStateMachine);
    ModemErrorLog(msg);
    newCotState(MOSM_ERR_DISCONNECT,2000);  // close the UDP?
    gs_MoPar->moLoopRet = err;
}

void UdpRxHex(const char * hex)
{
    int slen = strlen(hex);
    int len = slen/2;
    uint8_t bdata[UDP_RX_MAX_LEN_BYTE];  // 2 bytes in hex... are only one byte in bdata.
    if (slen >= UDP_RX_MAX_LEN_HEX)
    {
        ModemErrorLog("UdpRxHex len to high");
        return;
    }
    int spos = 0;
    for (int i=0; i < len; i++)
    {
        char buf[16];
        buf[0]= hex[spos++];
        buf[1]= hex[spos++];
        buf[2]= 0;
        long val = strtol(buf,0,16); // 2 bytes hex
        bdata[i]=val&0xff;
    }

    // bdata ready send to upper ?
    xxx_onUdsReceive(bdata, len);
    // from original while loop done on all network rx.
    doNextStep();
    // !!!???  conn_s_updateRxStatistic(objArray[7], numBytes, false);

    if( gsMoStateMachine == MOSM_DISCONNECT_WAIT  )
    {
#ifdef WITH_VI_DEBUG_LEVEL
    char msg[DEBUG_BUFFER_MIN];
    sprintf(msg,"UdpRxHex MOSM_DISCONNECT_ON_OPEN: %d",gsMoStateMachine);
    ModemDebugLog(msg);
#endif
        //newCotState(MOSM_DISCONNECT_WAIT,gs_MoPar->cmdTimeout); // then  also cause error if disconnect too fast.
        newCotState(MOSM_DONE,gs_MoPar->cmdTimeout);
        return;
    }

}


uint16_t sendCot( CotParameter *parameter)
{
    gs_MoPar   = parameter;

    gs_MoPar->m_mode = 0;

    gs_MoPar->moRunCounter = 0;
    gs_MoPar->moLoopRet = 0;
    gs_MoPar->udpStatus = 0;

    // log this allways!
    ModemErrorLog("sendCot(VIAPI V31 R1800)");  // VERSION-TAG
#ifdef UDP_SELFTEST
    // only to init gsMoPar
#else
    gsMoGuardTimeoutMs = gs_MoPar->modemConnectTimeoutMs; // 60 ssec
    newCotState(MOSM_WAKE,gs_MoPar->cmdTimeout);

#endif
    return 0;  // Accepted
}



bool ContainsCops(uint32_t *foundCops, int foundCount, uint32_t testCops)
{
    for (int i=0; i <foundCount; i++ )
    {
        if (foundCops[i]==testCops)
        {
            return true;
        }
    }
    return false;
}

const uint32_t CopsPrios[] = {
	26201,26202,26203,20416,20404,23203,23202,29340,23001,23102,26002,21901,21630,20201,22801,22801,22210,22201,21401,23415,24001,24491,23820,24202,24201,20620,20610,20810,27877,310260,310410,302610,302720,45008,44010,53005,46692,310260,25001,46692
	};

void selectBestCops(uint32_t *foundCops, int foundCount)
{
    uint32_t telekomCops = 26201;
    if (foundCount == 0)
    {
        // nothing found... still try telekom ( epecxt we are in germany)
        gs_MoPar->cops = telekomCops;
        return;
    }

    if (foundCount == 1)
    {
        // only one found -> must take this.
        gs_MoPar->cops = foundCops[0];
        return;
    }

    // more than 1 ( 2,3,4
    size_t PrioCnt = sizeof(CopsPrios)/sizeof(CopsPrios[0]);
    for (size_t i=0; i<PrioCnt;i++)
    {
        uint32_t actPrio = CopsPrios[i]; // 26202;

        #ifdef WITH_VI_DEBUG_LEVEL
        char msg[DEBUG_BUFFER_MIN];
        sprintf(msg,"extractCopsEntry: check Prio for %d",actPrio );
        ModemErrorLog(msg);
        #endif

        if (ContainsCops(foundCops, foundCount, actPrio))
        {
            gs_MoPar->cops = actPrio;
            return;
        }
    }

    gs_MoPar->cops = foundCops[0];  // nothing found in prio list... take just first -:)
}

// //1,"","","26201",9
uint32_t extractCopsEntry(char *oneEntry)
{
    // Log allways
    char msg[DEBUG_BUFFER_MIN];
    sprintf(msg,"extractCopsEntry: %s",oneEntry );
    ModemErrorLog(msg);

    uint32_t ret = 0;
    const char *sepColon = { ","};
    char * pch = strtok (oneEntry,sepColon);
    int no = 0;

      while (pch != NULL)
      {
        switch(no)
        {
        case 3:
            ret = strtol(pch+1,NULL,10);  // is decimal
            break;
        }

        pch = strtok (NULL, sepColon);
        no++;
      }
      #ifdef WITH_VI_DEBUG_LEVEL
      sprintf(msg,"extractCopsEntry ret: %d", ret);
      ModemErrorLog(msg);
      #endif
      return ret;
}

uint16_t muEndRunLoop(const char *txt)
{

    long mSec = TIMEOUT_MS_VIAPI * gs_MoPar->moRunCounter;
    char msg[DEBUG_BUFFER_MIN];
    sprintf(msg,"muEndRunLoop %d after %ld ms %s",gs_MoPar->moLoopRet,mSec,txt);
    ModemErrorLog(msg);

    return gs_MoPar->moLoopRet;
}

// 500 byte UDO data + x
#define BC660_MAX_RX 650

void decrementCounter() {
    // ModemDebugLog("loopCot50Ms decrementCounter");
    gsMoCmdTimeoutCnt--;

    if (gsMoStateMachine == MOSM_BG95_DELAY)
    {
        if (gsMoCmdTimeoutCnt == 1)
        {
            if (gsBg95WaitCnt)
            {
                newCotState(MOSM_BG95_WAIT,gs_MoPar->cmdTimeout);
            }
        }
    }
}


void internUdpPrepare(char *hexTx, int len)
{
    char buf[32];

    hexTx[0]=0; // terminate
    int dst = 0;
    // 100ms no change do sending
    for (int i=0;i<len;i++)
    {
        int data = gUdpTxDta[gUdpTxDoneLen++]&0xff; // if not expaneds 0xfe -> 0xfffffe
        sprintf(buf,"%02X",data);
        hexTx[dst++]=buf[0];
        hexTx[dst++]=buf[1];
    }
    hexTx[dst]=0;
}

uint16_t loopCot50Ms(void)
{
    viPlattformTick50ms();

    if (gsMoStateMachine == MOSM_SLEEP) {
        return VCARC_RUNNING; // exit here when no init was called.
    }

    if (gUdpTxcnt>0)
    {
        switch(gUdpTxState)
        {
        case 0:
            {
                // start transere of junk
                int len = gUdpTxJunkCnt[gUdpTxDoneCnt];
                viSendUdpPre(len);
                gUdpTxState++;
            }
            break;
        case 1:
            {
                // send..
                int len = gUdpTxJunkCnt[gUdpTxDoneCnt++];

                char hexTx2[UDP_RX_MAX_LEN_HEX+8];
                internUdpPrepare(hexTx2,len);
                viSendUdpPost(hexTx2);
                gUdpTxState++;
            }
            break;
        case 2:
            {
                // check end?
                char msg[STD_AT_LEN_BYTE];  // 256*2 + AT command
                sprintf(msg,"Background send %d / %d ",gUdpTxDoneCnt,gUdpTxcnt);  // Length in send byte ( format idependet!)
                viDebugLog(msg);

                if (gUdpTxDoneCnt >= gUdpTxcnt)
                {
                    gUdpTxcnt = 0;
                    gUdpTxLen = 0;
                    gUdpTxJunkCnt[0]=0;
                    gUdpTxJunkCnt[2]=0;
                    gUdpTxJunkCnt[3]=0;
                    gUdpTxJunkCnt[4]=0;
                    gUdpTxJunkCnt[5]=0;
                    gUdpTxDoneCnt = 0;


                    viDebugLog("Background send done");
                } else {
                    gUdpTxState = 0; // next.
                }
            }
            break;
        }
    }


    //ModemDebugLog("loopCot50Ms");
    gs_MoPar->moRunCounter++;

    if (gsMoStateMachine >= MOSM_ERR_DISCONNECT)
    {
        // in timeout shutdown
        if (gsMoCmdTimeoutCnt>0)
        {

            decrementCounter(); // gsMoCmdTimeoutCnt--;

            // Read out and do not care
            char data[BC660_MAX_RX];  // UDP 255*2 + wrapping
            int ms = ModemGet(data, BC660_MAX_RX);
            if (ms > 0)
            {
                logLongData(data, ms,__LINE__);
            }
        } else {
            return muEndRunLoop("ERROR delayed");   // return save error code.
        }
        return VCARC_RUNNING;
    }

    const char *sepColon = { ","};

    if (gs_MoPar->moLoopRet != 0)
    {
        char msg[255];
        sprintf(msg,"UDS callback moLoopRet: %d",gs_MoPar->moLoopRet);
        return  muEndRunLoop(msg); // "UDS callback");
    }

    if (gsMoGuardTimeoutMs > TIMEOUT_MS_VIAPI)
    {
        gsMoGuardTimeoutMs-=TIMEOUT_MS_VIAPI;

        if (gsMoCmdTimeoutCnt>0)
        {
            decrementCounter(); // gsMoCmdTimeoutCnt--;
        } else {
#ifdef MU_WITH_DNS
            if (gsMoStateMachine == MOSM_DNS2)
            {
                newCotState(MOSM_QIFMT,gs_MoPar->cmdTimeout);  // no DNS response continue
            } else {
                setErrorAndDisconnect(VCARC_TIMEOUT_CMD);
                return VCARC_RUNNING;
            }
#else
            setErrorAndDisconnect(VCARC_TIMEOUT_CMD);
            return VCARC_RUNNING;
#endif
        }

        // with no Rx..
        switch (gsMoStateMachine)
        {
        case MOSM_WAKE:
            if (gsMoCmdTimeoutCnt == 5)
            {
                newCotState(MOSM_INIT,gs_MoPar->cmdTimeout);
            }
            break;

        case MOSM_DISCONNECT_WAIT:
            if (gsMoCmdTimeoutCnt == 5)
            {
               newCotState(MOSM_DONE,gs_MoPar->cmdTimeout);
            }
             break;

        case MOSM_BG95_FUN_DELAY:
            if (gBgAfterWaitCnt) {
                gBgAfterWaitCnt--;
            } else {
                ModemDebugLog("MOSM_BG95_FUN_DELAY done");
                newCotState(gBgAfterWaitState, gBgAfterWaitVal);
            }
            break;



        default:
            break;
        }

        char data[BC660_MAX_RX];  // UDP 255*2 + wrapping

        int ms = ModemGet(data, BC660_MAX_RX);
        if (ms > 0)
        {
            if (data[ms-1]=='\n')
            {
                data[ms-1]=0; // remove \n
                ms--;
            }
        }

        if (ms > 0)
        {
            if (data[ms-1]=='\r')
            {
                data[ms-1]=0; // remove \r
                ms--;
            }
        }

#ifdef BG95
        if (gBg95UdpRxStatus != 0)
        {
            /*
                21:53:49.710	DBG	doLog : mrx[14]AT+QIRD=0,200    1
                21:53:49.773	DBG	doLog : mrx[08]+QIRD: 3       1->2
                21:53:49.835	DBG	doLog : mrx[06]030500    2
                21:53:49.961	DBG	doLog : mrx[02]OK 2->0
                we get all at once!!!
                */

            // do not change any position below!
            /*
            if (gBg95UdpRxStatus >=2) {
                char msg[255];
                sprintf(msg,"gBg95UdpRxStatus %d [%s],",gBg95UdpRxStatus,data);
                ModemDebugLog(msg);
            }
*/

            if (strstr(data, "AT+QIRD=") == data)
            {
                gBg95UdpRxStatus=2;
                //ModemDebugLog("gBg95UdpRxStatus = 2");
            }

            if (gBg95UdpRxStatus == 3)
            {
                gBg95UdpRxStatus = 4;
                ModemDebugLog("copy UDS data");
                strcpy(gBg95UdpBuffer,data);       // TODO check lwn? !!!MZA???
            }

            if (strstr(data, "OK") == data)
            {
                gBg95UdpRxStatus=0;
                //ModemDebugLog("gBg95UdpRxStatus = 0");
                UdpRxHex(gBg95UdpBuffer);                 // all done with reading... no process the UDP data.
                return VCARC_RUNNING;
            }

            if (strstr(data, "+QIRD:") == data)
            {
                gBg95UdpRxStatus=3;
                //ModemDebugLog("gBg95UdpRxStatus = 3");
            }
        }
#endif

        if ( ms > 0 )
        {
            logLongData(data, ms,__LINE__);
            bool isOk = false;
            if (strstr(data, "OK") == data)
            {
                #ifdef WITH_VI_DEBUG_LEVEL
                ModemDebugLog("GOT OK");
                #endif
                isOk = true;
            }
#ifdef BG95
            if (strstr(data, "SEND OK") == data)
            {
                // qisendex -> BG95 only
                #ifdef WITH_VI_DEBUG_LEVEL
                ModemDebugLog("GOT SEND OK");
                #endif
                isOk = true;
            }


#endif

            bool isIpConnected = false;
            //"+IP: 10.1.70.194\r\n"
            if (strstr(data, "+IP:") == data)
            {
                #ifdef WITH_VI_DEBUG_LEVEL
                ModemDebugLog("GOT +IP");
                #endif
                isIpConnected = true;
            }

            if (strstr(data, "+CEREG: 3") == data)
            {

                if (gsMoStateMachine ==  MOSM_FUNON)
                {
                    if (gsStateRepeatCnt < 10)
                    {
                        gsStateRepeatCnt++;
                        if (gsStateRepeatCnt < 5)
                        {
                             // only if we wait for connect.
                            char msg[128];
                            uint32_t newTV = gs_MoPar->modemSocketOpenTimeoutMs/TIMEOUT_MS_VIAPI;
                            gsMoGuardTimeoutMs += gs_MoPar->modemSocketOpenTimeoutMs;
                            sprintf(msg,"CEREG Timeout extend %d -> %d (x50ms) cnt: %d GT:%d",  gsMoCmdTimeoutCnt,newTV,gsStateRepeatCnt,gsMoGuardTimeoutMs);
                            ModemErrorLog(msg);
                            gsMoCmdTimeoutCnt = newTV;
                        }
                    }
                }
            }


            //+CEREG: 5,"D321","01AB830A",9 -> not used in V13 ( does not come allways)

            /*
            mrx[10]AT+COPS=?
                                                   01234567
            22:34:40.957    DBG     doLog : mrx[79]+COPS: (1,"","","26201",9),(0,"","","26202",9),(0,"","","262...
            22:34:40.969    DBG     doLog : MCC 2 2 ""
            22:34:40.974    DBG     doLog : MCC 262 MNC ?01
            22:34:41.079    DBG     doLog : mrx[02]OK
            22:34:41.085    DBG     doLog : GOT OK
            */
            // 20 pro eintrag
            //
            //                12345678
            if (strstr(data, "+COPS: (") == data)
            {
                // roaming search answer

                // 255 char -> 20 each entry maxes 10 max
                uint32_t foundCops[10];
                int      foundCount = 0;

                char msg[DEBUG_BUFFER_MIN];

                int rlen = strlen(data);
                sprintf(msg,"COPS Roaming: %d",rlen );
                ModemDebugLog(msg);

                // +COPS: (2,"CHINA MOBILE","CMCC","46000",9),(3,"CHINA UNICOMM","CUCC","46001",9),(1,"",
                //"","21405",9),(1,"CHINA TELECOM","CTCC","46011",9),,(0-4),(0-2)


                if (rlen > 10)
                {
                    /*
                    COPS Roaming: 79
                   23:18:39.614    DBG     doLog : COPS Roaming xx: 7 18
                   23:18:39.622    DBG     doLog : COPS Roaming xx: 27 18
                   23:18:39.629    DBG     doLog : COPS Roaming xx: 47 18
                   23:18:39.636    DBG     doLog : COPS Roaming xx: 68 4
                      */
                    int start = 0;
                    int sepMode = 0; // searching.

                    // at least one entry
                    for (int i=6; i<(rlen-2);i++)
                    {
                        if (data[i]=='(')
                        {
                           sepMode = 1; // found (
                           start = i;

                        } else {

                            if (data[i]==')')
                            {
                                if (sepMode == 1)
                                {
                                     int len = i - start -1;
                                     if ( (len > 10) && ( len < 25) )
                                     {
                                        //sprintf(msg,"COPS Roaming xx: %d %d",start,len );
                                        //ModemDebugLog(msg);

                                        char oneEntry[30];
                                        int k=0;
                                        int startPos=start+1;
                                        while(len>0)
                                        {
                                            oneEntry[k++]=data[startPos++];
                                            len--;
                                        }
                                        oneEntry[k++]=0; // terminate
                                        uint32_t act = extractCopsEntry(oneEntry);
                                        foundCops[foundCount++]= act;
                                     }
                                }
                                sepMode = 0; // closed
                            }

                        }
                    }

                }

                selectBestCops(foundCops,foundCount);
                sprintf(msg,"selectBestCops: %d",gs_MoPar->cops );
                ModemDebugLog(msg);


            } else {

                //             +COPS: 1,2,"26201",9
                //if (strstr(data, "+COPS: 1,") == data)


                // reponse to AT+COPS?
                if (strstr(data, "+COPS: ") == data)
                {
                    //
                    char * pch = strtok (data,sepColon);
                    int no = 0;
                    #ifdef WITH_VI_DEBUG_LEVEL
                    char msg[DEBUG_BUFFER_MIN];
                    #endif
                      while (pch != NULL)
                      {
                        switch(no)
                        {
#ifndef BG95
                        case 2: // "xxx"
                            {
                               int len = strlen(pch);
                               if (len == 7)
                               {
                                   gs_MoPar->m_Mcc[0]=pch[1];
                                   gs_MoPar->m_Mcc[1]=pch[2];
                                   gs_MoPar->m_Mcc[2]=pch[3];
                                   gs_MoPar->m_Mnc[0]=pch[4];
                                   gs_MoPar->m_Mnc[1]=pch[5];
                                   gs_MoPar->m_Mnc[2]=0xff;
                               }
                               if (len == 8)
                               {
                                   gs_MoPar->m_Mcc[0]=pch[1];
                                   gs_MoPar->m_Mcc[1]=pch[2];
                                   gs_MoPar->m_Mcc[2]=pch[3];
                                   gs_MoPar->m_Mnc[0]=pch[4];
                                   gs_MoPar->m_Mnc[1]=pch[5];
                                   gs_MoPar->m_Mnc[2]=pch[6];
                               }
                               #ifdef WITH_VI_DEBUG_LEVEL
                               sprintf(msg,"MCC %d %d %s",no,len,pch);
                               ModemDebugLog(msg);
                               #endif
                            }
                            break;
#else
// BG96/77    +COPS: 0,0,"CHN-UNICOM",0
// last -> <Act> Integer type. Access technology selected.  0 GSM /  8 eMTC /  9 NB-IoT
#endif


                        }

                        pch = strtok (NULL, sepColon);
                        no++;
                    }
                    #ifdef WITH_VI_DEBUG_LEVEL
                    #ifndef BG95

                    sprintf(msg,"MCC %c%c%c MNC %c%c%c"
                              ,gs_MoPar->m_Mcc[0]
                              ,gs_MoPar->m_Mcc[1]
                              ,gs_MoPar->m_Mcc[2]
                              ,gs_MoPar->m_Mnc[0]
                              ,gs_MoPar->m_Mnc[1]
                              ,gs_MoPar->m_Mnc[2]
                              );
                    ModemDebugLog(msg);
                    #endif
                    #endif
                }

            }
#ifndef BG95
            // V13 we need to take this because CERG 5 is not stable in CN!!! at lease
            // 19:21:20.909    DBG     doLog : +QENG: 0,3740,9,392,"01AB830A",-95,-6,-89,11,8,"D321",0,12,0
            if (strstr(data, "+QENG:") == data)
            {
                char * pch = strtok (data,sepColon);
                int no = 0;
                #ifdef WITH_VI_DEBUG_LEVEL
                char msg[DEBUG_BUFFER_MIN];
                #endif

                  while (pch != NULL)
                  {
                    switch(no)
                    {
                    case 4:
                        #ifdef WITH_VI_DEBUG_LEVEL
                        sprintf(msg,"CELLID %d %s",no,pch);
                        ModemDebugLog(msg);
                        #endif
                        gs_MoPar->m_cid = strtol(pch+1,NULL,16);
                        break;


                   //+QENG: 0,<sc_EARFCN>,<sc_EARFCN_offset>,<sc_pci>,<sc_cellID>,
                   //+QENG: 0,3740,9,392,"01AB830A",-100,-6,-94,12,8,"D321",0,16,0                          7
                   //[<sc_RSRP>],[<sc_RSRQ>],[<sc_RSSI>],[<sc_SINR>],<sc_band>,<sc_TAC>,[<sc_ECL>],[<sc_Tx_pwr>],<operation_mode>
// sc_pci 3
                        // sc_SINR  #8

                    case 3:
                        gs_MoPar->m_pci = strtol(pch,NULL,10);
                        break;
                    case 8:
                        gs_MoPar->m_SINR = strtol(pch,NULL,10);
                        break;



                    case 5:
                        gs_MoPar->m_RSRP = strtol(pch,NULL,10);
                        break;

                    case 6:
                        gs_MoPar->m_RSRQ = strtol(pch,NULL,10);
                        break;

                    case 7:
                        gs_MoPar->m_signalStrength = strtol(pch,NULL,10);
                        break;

                    case 10:
                        #ifdef WITH_VI_DEBUG_LEVEL
                        sprintf(msg,"LAC %d %s",no,pch);
                        ModemDebugLog(msg);
                        #endif
                        gs_MoPar->m_lac = strtol(pch+1,NULL,16);
                        break;
                    }

                    pch = strtok (NULL, sepColon);
                    no++;
                  }
                  #ifdef WITH_VI_DEBUG_LEVEL
                  sprintf(msg,"CELLID %08X RSSI %d",  gs_MoPar->m_cid,gs_MoPar->m_signalStrength);
                  ModemDebugLog(msg);
                  #endif
            }
#endif



//            "+QIURC: \"recv\",0,2,\"172.25.102.151\",1883,\"\x02\x18\"\r\n"
            if (strstr(data, "+QIURC:") == data)
            {
                if (strstr(data, "+QIURC: \"pdpdeact\"") == data)
                {
                // Just ignore happens on reboots
                //  mrx[20]+QIURC: "pdpdeact",1
               //09:16:09.084	DBG	doLog : UDP: 0 []
               //09:16:09.088	DBG	doLog : vi_mqttsn_parsePacket ERROR ret.result: 6
                } else {



                    //ModemDebugLog("GOT +QUURC");
                    char * pch = strtok (data,sepColon);
                    int no = 0;
                    int count = 0;
                    char udp[BC660_MAX_RX];  // UDP 255*2 + wrapping
                    #ifdef WITH_VI_DEBUG_LEVEL
                    char msg[DEBUG_BUFFER_MIN];
                    #endif

                  while (pch != NULL)
                  {
                    /*
17:24:30.934    DBG     doLog 10 : GOT +QUURC
17:24:30.945    DBG     doLog 16 : 0 +QIURC: "recv"
17:24:30.955    DBG     doLog 3 : 1 0
17:24:30.962    DBG     doLog 3 : 2 2
17:24:30.968    DBG     doLog 18 : 3 "172.25.102.151"
17:24:30.980    DBG     doLog 6 : 4 1883

                     */
                    switch(no)
                    {
                    case 2:
                        count = strtol(pch,0,10);
                        break;
                    case 5:
                        if (*pch == '\"')
                        {
                            strcpy(udp,pch+1);
                        } else {
                            strcpy(udp,pch);  // Or error
                        }
                        break;

                    }
                    //sprintf(msg,"%i %s",no,pch);
                    //ModemDebugLog(msg);

                    pch = strtok (NULL, sepColon);
                    no++;
                  }



#ifdef BG95
                      // BG95 we only have the trigger to read data.
                      /* XXXX
                    21:30:36.241	DBG	doLog : mrx[16]+QIURC: "recv",0
                    21:30:36.241	DBG	doLog : UDP: 0 []
                    AT+QIRD=0,1500 //Retrieve the data, and the maximum length of data to be retrieved is 1500 bytes.
                    +QIRD: 5 //The length of actually retrieved data is 5 bytes.
                    test1
                    OK

21:53:49.710	DBG	doLog : mrx[14]AT+QIRD=0,200
21:53:49.773	DBG	doLog : mrx[08]+QIRD: 3
21:53:49.835	DBG	doLog : mrx[06]030500
21:53:49.961	DBG	doLog : mrx[02]OK

          */
                    gBg95UdpBuffer[0]=0;         // terminate.
                    gBg95UdpRxStatus = 1;
                    sendAt("AT+QIRD=0,700\r\n");  // BG95 no hex receive...

#else
                    // BG660 we have the data...
                    udp[count*2]=0;  // cut off "
#ifdef WITH_VI_DEBUG_LEVEL
                    sprintf(msg,"UDP: %i [%s]",count,udp);
                    ModemDebugLog(msg);
#endif
                    UdpRxHex(udp);
#endif

                    }
                }




            switch(gsMoStateMachine)
            {
            case MOSM_WAKE:
                break;


            case MOSM_PWESAVEOFF:
                if (isOk)
                {
                    newCotState(MOSM_QICLOSE,gs_MoPar->modemSocketOpenTimeoutMs);
                }
                break;

            case MOSM_QICLOSE2:
                if (isOk)
                {
                    newCotState(MOSM_QICLOSE0,gs_MoPar->modemSocketOpenTimeoutMs);
                }
                break;

            case MOSM_QICLOSE0:
                if (isOk)
                {
                    newCotState(MOSM_QIFMT ,gs_MoPar->modemSocketOpenTimeoutMs);
                }
                break;


            case MOSM_QICLOSE:
                if (isOk)
                {
                    newCotState(MOSM_FUNOFF,gs_MoPar->modemSocketOpenTimeoutMs);
                }
                break;

            case MOSM_FUNOFF:
                if (isOk)
                {
#ifdef BG95
                    newCotState(MOSM_BG95_FUNOFF,gs_MoPar->modemSocketOpenTimeoutMs);
#else

                    newCotState(MOSM_APNSET,gs_MoPar->cmdTimeout);
#endif
                }
                break;
            case MOSM_APNSET:
                if (isOk)
                {
                    newCotState(MOSM_BANDS,gs_MoPar->cmdTimeout);
                }
                break;

            case MOSM_BANDS:
                if (isOk)
                {
                    if ( gs_MoPar->m_mode == 0xAA55)
                    {
                        newCotState(MOSM_OP_SEARCH,gs_MoPar->cmdTimeout);
                    } else {
                        newCotState(MOSM_OPTELEKOM,gs_MoPar->cmdTimeout);
                    }
                }
                break;

            case MOSM_OPTELEKOM:
                if (isOk)
                {
                    newCotState(MOSM_OP_INFO,gs_MoPar->modemSocketOpenTimeoutMs);
                }
                break;

            case MOSM_OP_INFO:
                if (isOk)
                {
                    newCotState(MOSM_FUNON,gs_MoPar->modemSocketOpenTimeoutMs);
                }
                break;



            case MOSM_OP_SEARCH:
                if (isOk)
                {
                    gs_MoPar->moLoopRet = VCARC_ROAMING_DONE;
                    newCotState(MOSM_DONE,gs_MoPar->modemSocketOpenTimeoutMs);
                }
                break;

            case MOSM_BG95_FUNOFF:
                if (isOk)
                {
                    newCotStateWithDelay(MOSM_BG95_MODE,gs_MoPar->modemSocketOpenTimeoutMs);
                }
                break;


            case MOSM_BG95_MODE:
                if (isOk)
                {
                    newCotState(MOSM_BG95_BAND,gs_MoPar->cmdTimeout);
                }
                break;

            case MOSM_BG95_BAND:
                if (isOk)
                {
                    newCotState(MOSM_BG95_NWSCANMODE,gs_MoPar->cmdTimeout);
                }
                break;

            case MOSM_BG95_NWSCANMODE:
                if (isOk)
                {
                    newCotState(MOSM_BG95_NWSCANSEQ,gs_MoPar->cmdTimeout);
                }
                break;

            case MOSM_BG95_NWSCANSEQ:
                if (isOk)
                {
                    newCotState(MOSM_BG95_SERVICEDOMAIN,gs_MoPar->cmdTimeout);
                }
                break;

            case MOSM_BG95_SERVICEDOMAIN:
                if (isOk)
                {
                    newCotState(MOSM_BG95_FUNON,gs_MoPar->cmdTimeout);
                }
                break;


            case MOSM_BG95_FUNON:
                if (isOk)
                {
                    newCotStateWithDelay(MOSM_BG95_IP,gs_MoPar->cmdTimeout);
#ifdef X_PRIVATE_APN
                    gsBg95WaitCnt = 60; // retry for IP address
                    newCotState(MOSM_BG95_WAIT,gs_MoPar->cmdTimeout);  // no COPS!!
#endif

                }
                break;

            case MOSM_BG95_IP:
                if (isOk)
                {
                    newCotState(MOSM_BG95_COPS,gs_MoPar->modemSocketOpenTimeoutMs);  // take time? BG95 waits ehre
                }
                break;

            case MOSM_BG95_COPS:
                if (isOk)
                {
                    newCotState(MOSM_BG95_WAIT,gs_MoPar->cmdTimeout);
                }
                break;


            case MOSM_INIT:
            {
                if (strstr(data, "+CGPADDR: 1") == data)
                {
                    int initForce = false;

                    if (strstr(data, "+CGPADDR: 1,0.0.0.0") == data)
                    {
                        initForce = true;
                    } else {
                        int len = strlen(data);
                        if (len > 12)
                        {
                            // has a address that is not null
                            initForce = false;
                        } else {
                            initForce = true;
                        }
                    }

                    if (initForce)
                    {
                        gs_MoPar->intCmdParameter1 = 1;
                    } else {
                        gs_MoPar->intCmdParameter1 = 2;
                    }
                }
                if (strstr(data, "ERROR") == data)
                {
                    newCotState(MOSM_PWESAVEOFF,gs_MoPar->cmdTimeout);   // not online -> ERROR or INIT? TODO !!!MZA???
                }

                if (isOk)
                {
                    if (gs_MoPar->intCmdParameter1 == 2)
                    {
                        newCotState(MOSM_REG_URC,gs_MoPar->cmdTimeout);    // next we are online. close first in case of restart.
                    } else {
                        newCotState(MOSM_PWESAVEOFF,gs_MoPar->cmdTimeout);   // not online -> ERROR or INIT? TODO !!!MZA???
                    }
                }
            }
            break;

            case  MOSM_REG_URC:
                if (strstr(data, "+CREG: 2") == data)
                {
                    gs_MoPar->intCmdParameter1 = 1;  // is fine.
                }

                if (isOk)
                {
                    if (gs_MoPar->intCmdParameter1 == 42)
                    {
                        newCotState(MOSM_QICLOSE2,gs_MoPar->cmdTimeout);    // fine
                    } else {
                        newCotState(MOSM_SET_URC,gs_MoPar->cmdTimeout);   // not online -> ERROR or INIT? TODO !!!MZA???
                    }
                }
                break;

            case MOSM_SET_URC:  // SREG URC is on.
                if (isOk)
                {
                    newCotState(MOSM_ASK_LAC,gs_MoPar->cmdTimeout);   // reconnect.
                }
                break;

            case MOSM_ASK_LAC:
            {
                if (strstr(data, "+COPS: ") == data)
                {
                    //
                    char * pch = strtok (data,sepColon);
                    int no = 0;
#ifdef WITH_VI_DEBUG_LEVEL
                    char msg[DEBUG_BUFFER_MIN];
#endif
#ifdef WITH_VI_DEBUG_LEVEL
                    sprintf(msg,"COPS %d  %s",pch);
                    ModemDebugLog(msg);
#endif

                    while (pch != NULL)
                    {
                        switch(no)
                        {
                        case 2: // "xxx"
                            break;
                        }
                    }
                }
                if (isOk)
                {
                    newCotState(MOSM_PWESAVEOFF,gs_MoPar->cmdTimeout);   // reconnect.
                }
            }
            break;


            case MOSM_BG95_WAIT:
                {
                    if (strstr(data, "+CGPADDR: 1") == data)
                    {
                        int len = strlen(data);
                       // char msg[32];
                       // sprintf(msg,"xxxx: %i ",len);
                       // ModemDebugLog(msg);

                        if (strstr(data, "+CGPADDR: 1,0.0.0.0") == data)
                        {
                            if (gsBg95WaitCnt)
                            {
                                gsBg95WaitCnt--;

                                newCotState(MOSM_BG95_DELAY,gs_MoPar->cmdTimeout);
                            }
                        } else {
                            if (len > 15)
                            {
                                newCotState(MOSM_BG95_ACOPS,gs_MoPar->cmdTimeout);    // next
                            } else {
                                newCotState(MOSM_BG95_DELAY,gs_MoPar->cmdTimeout);
                            }
                        }
                    }
                }
                    break;

            case MOSM_BG95_ACOPS:
                if (isOk)
                {

                        gsBg95WaitCnt = 60;
                    newCotState(MOSM_IMSI,gs_MoPar->cmdTimeout);   // done with BG95 secia startup
                }
                break;






            case MOSM_FUNON:
                if (isIpConnected)
                {
                    // 19:21:20.660    DBG     doLog : +COPS: 1,2,"26201",9
                    newCotState(MOSM_IMSI,gs_MoPar->cmdTimeout);
                }
                break;


            case MOSM_IMSI:
                if (isOk)
                {                    
                    newCotState(MOSM_IMEI,gs_MoPar->cmdTimeout);
                }
                break;

            case MOSM_IMEI:
                if (isOk)
                {
                    //newCotState(MOSM_APN_READ,gs_MoPar->cmdTimeout);
                    newCotState(MOSM_FW_ALL,gs_MoPar->cmdTimeout);
                }
                break;

            case MOSM_FW_ALL:
                if (isOk)
                {
                    //newCotState(MOSM_APN_READ,gs_MoPar->cmdTimeout);
                    newCotState(MOSM_COPS_READ,gs_MoPar->cmdTimeout);
                }
                break;


/*
            case MOSM_APN_READ:
                if (isOk)
                {
                    newCotState(MOSM_COPS_READ,gs_MoPar->cmdTimeout);
                }
                break;
*/
            case MOSM_COPS_READ:
                if (isOk)
                {
                    // 19:21:20.909    DBG     doLog : +QENG: 0,3740,9,392,"01AB830A",-95,-6,-89,11,8,"D321",0,12,0

                    newCotState(MOSM_POS2,gs_MoPar->cmdTimeout);
                }
                break;
/*
            case MOSM_BG95_CTX1:
                if (isOk)
                {
                    newCotState(MOSM_BG95_CTX2,gs_MoPar->cmdTimeout);
                }
                break;
            case MOSM_BG95_CTX2:
                if (isOk)
                {
                    newCotState(MOSM_QIFMT,gs_MoPar->cmdTimeout);
                }
                break;
*/

            case MOSM_POS2:
#ifdef BG95
/*
                if (strstr(data, "+QENG:") == data)
                {
                    char * pch = strtok (data,sepColon);
                    int no = 0;
                    #ifdef WITH_VI_DEBUG_LEVEL
                    char msg[DEBUG_BUFFER_MIN];
                    #endif

                      while (pch != NULL)
                      {
                        switch(no)
                        {
                        case 4:
                            #ifdef WITH_VI_DEBUG_LEVEL
                            sprintf(msg,"CELLID %d %s",no,pch);
                            ModemDebugLog(msg);
                            #endif
                            gs_MoPar->m_cid = strtol(pch+1,NULL,16);
                            break;


                       //+QENG: 0,<sc_EARFCN>,<sc_EARFCN_offset>,<sc_pci>,<sc_cellID>,
                       //+QENG: 0,3740,9,392,"01AB830A",-100,-6,-94,12,8,"D321",0,16,0                          7
                       //[<sc_RSRP>],[<sc_RSRQ>],[<sc_RSSI>],[<sc_SINR>],<sc_band>,<sc_TAC>,[<sc_ECL>],[<sc_Tx_pwr>],<operation_mode>
    // sc_pci 3
                            // sc_SINR  #8

                        case 3:
                            gs_MoPar->m_pci = strtol(pch,NULL,10);
                            break;
                        case 8:
                            gs_MoPar->m_SINR = strtol(pch,NULL,10);
                            break;



                        case 5:
                            gs_MoPar->m_RSRP = strtol(pch,NULL,10);
                            break;

                        case 6:
                            gs_MoPar->m_RSRQ = strtol(pch,NULL,10);
                            break;

                        case 7:
                            gs_MoPar->m_signalStrength = strtol(pch,NULL,10);
                            break;

                        case 10:
                            #ifdef WITH_VI_DEBUG_LEVEL
                            sprintf(msg,"LAC %d %s",no,pch);
                            ModemDebugLog(msg);
                            #endif
                            gs_MoPar->m_lac = strtol(pch+1,NULL,16);
                            break;
                        }

                        pch = strtok (NULL, sepColon);
                        no++;
                      }
                      #ifdef WITH_VI_DEBUG_LEVEL
                      sprintf(msg,"CELLID %08X RSSI %d",  gs_MoPar->m_cid,gs_MoPar->m_signalStrength);
                      ModemDebugLog(msg);
                      #endif
                }
                */
                if (isOk)
                {
#ifdef MU_WITH_DNS
                    newCotState(MOSM_DNS1,gs_MoPar->cmdTimeout);
#else
                    newCotState(MOSM_QIFMT,gs_MoPar->cmdTimeout);


#endif
                }
#else


                if (isOk)
                {
#ifdef MU_WITH_DNS
                    newCotState(MOSM_DNS1,gs_MoPar->cmdTimeout);
#else
                    newCotState(MOSM_QIFMT,gs_MoPar->cmdTimeout);
#endif
                }
#endif
                break;



#ifdef MU_WITH_DNS
            case MOSM_DNS1:
                if (isOk)
                {
                    newCotState(MOSM_DNS2,10000);  // DNS query 5 sec.
                }
                break;



            case MOSM_DNS2:
                if (strstr(data, "+QIDNSGIP: 0,") == data)
                {
                    // 0,1,0 -> 1 IP...
                    newCotState(MOSM_DNS3,1000);
                }
                break;

            case MOSM_DNS3:
                if (strstr(data, "+QIDNSGIP: ") == data)
                {
                    const char *pIp = data + 11;
                    int len = strlen(pIp) +1;

                    if ( len < MAX_COT_PAR_MAX )
                    {
                            strcpy(gs_MoPar->mqttsnIp,pIp);
                            // +QIDNSGIP: 14.215.177.39
                            ModemErrorLog("GOT DNS DATA");
                            ModemErrorLog(gs_MoPar->mqttsnIp);
                    }
                    newCotState(MOSM_QIFMT,gs_MoPar->cmdTimeout);
                }
                break;
#endif

            case MOSM_QIFMT:
                if (isOk)
                {
#ifndef BG95
                    // BG95
                    if (gs_toCot->has_devicePositionUpdate)
                    {
                        // We add the data

                        gs_toCot->devicePositionUpdate.has_gpsPosition = false;
                        gs_toCot->devicePositionUpdate.cellHints_count = 1;
                        gs_toCot->devicePositionUpdate.cellHints[0].type = toil_geolocation_CellPositionHint_CellType_NBIOT;
                        gs_toCot->devicePositionUpdate.cellHints[0].has_mcc = true;
                        gs_toCot->devicePositionUpdate.cellHints[0].mcc[2]  = gs_MoPar->m_Mcc[2];
                        gs_toCot->devicePositionUpdate.cellHints[0].mcc[1]  = gs_MoPar->m_Mcc[1];
                        gs_toCot->devicePositionUpdate.cellHints[0].mcc[0]  = gs_MoPar->m_Mcc[0];
                        gs_toCot->devicePositionUpdate.cellHints[0].has_mnc = true;
                        gs_toCot->devicePositionUpdate.cellHints[0].mnc[2]  = gs_MoPar->m_Mnc[2];
                        gs_toCot->devicePositionUpdate.cellHints[0].mnc[1]  = gs_MoPar->m_Mnc[1];
                        gs_toCot->devicePositionUpdate.cellHints[0].mnc[0]  = gs_MoPar->m_Mnc[0]; //0xff;  // 3 bytes are always necessary, unused bytes must be set to 0xff
                        gs_toCot->devicePositionUpdate.cellHints[0].has_signalStrength = true;
                        gs_toCot->devicePositionUpdate.cellHints[0].signalStrength= gs_MoPar->m_signalStrength;
                        gs_toCot->devicePositionUpdate.cellHints[0].has_rsrp = true;
                        gs_toCot->devicePositionUpdate.cellHints[0].rsrp     =  gs_MoPar->m_RSRP;
                        gs_toCot->devicePositionUpdate.cellHints[0].has_rsrq = true;
                        gs_toCot->devicePositionUpdate.cellHints[0].rsrq = gs_MoPar->m_RSRQ;
                        gs_toCot->devicePositionUpdate.cellHints[0].has_pci = true;
                        gs_toCot->devicePositionUpdate.cellHints[0].pci = gs_MoPar->m_pci;
                        gs_toCot->devicePositionUpdate.cellHints[0].has_sinr = true;
                        gs_toCot->devicePositionUpdate.cellHints[0].sinr = gs_MoPar->m_SINR;

                        gs_toCot->devicePositionUpdate.cellHints[0].cid = gs_MoPar->m_cid;
                        gs_toCot->devicePositionUpdate.cellHints[0].has_lac = false;
                        //gs_toCot->devicePositionUpdate.cellHints[0].has_lac = true;
                        //gs_toCot->devicePositionUpdate.cellHints[0].lac = gs_MoPar->lac;

                        //gs_toCot->devicePositionUpdate.cellHints[0].cid = 29391115;
                        // working sample from telekom
                        /*
                        gs_toCot->devicePositionUpdate.cellHints[0].cid = 29391115;
                        gs_toCot->devicePositionUpdate.cellHints[0].mcc[2]  = '2';
                        gs_toCot->devicePositionUpdate.cellHints[0].mcc[1]  = '6';
                        gs_toCot->devicePositionUpdate.cellHints[0].mcc[0]  = '2';
                        gs_toCot->devicePositionUpdate.cellHints[0].mnc[0]  = 0xFF;
                        gs_toCot->devicePositionUpdate.cellHints[0].mnc[2]  = '1';
                        gs_toCot->devicePositionUpdate.cellHints[0].mnc[1]  = '0';
                        gs_toCot->devicePositionUpdate.cellHints[0].signalStrength=-89;
*/
                        //gs_toCot->devicePositionUpdate.cellHints[0].h
                    }
#endif

                    #ifdef BG95
                      newCotState(MOSM_BG95_QIOPEN_LP,gs_MoPar->cmdTimeout);
                    #else
                      newCotState(MOSM_QIOPEN,gs_MoPar->cmdTimeout);
                    #endif

                }
                break;
            case MOSM_QIOPEN:
                if (isOk)
                {
                    // Workarround allway disconnect before open.
                    // if not every second connect will be failed.
#ifdef BG95
                    newCotState(MOSM_DONE,9000); // need more time?! unclear why
#else
                    newCotState(MOSM_DISCONNECT_ON_OPEN,3000); // more than 1 sec needed -> 3 sec...
#endif
                    // there is maybe no response! ( not defined in spec)
                }
                break;

            case MOSM_BG95_QIOPEN_LP:
                //if (isOk)
                if (strstr(data, "+QIOPEN: 2,0") == data)
                {
                    newCotState(MOSM_BG95_QIOPEN_RP,3000); // more than 1 sec needed -> 3 sec...
                }
                break;

            case MOSM_BG95_QIOPEN_RP:
                // OK + +QIOPEN   must be 0,0 first the channel than the status.
                if (strstr(data, "+QIOPEN: 0,0") == data)
                {
                    // Workarround allway disconnect before open.
                    // if not every second connect will be failed.
                    newCotState(MOSM_UDP_READY,300000); // more than 1 sec needed -> 3 sec...
                    // there is maybe no response! ( not defined in spec)
                }
                break;



            case MOSM_DONE:
                break;
            case MOSM_ERR_WAIT:
                break;
            }
        }

        return VCARC_RUNNING;
    }
    setErrorAndDisconnect(VCARC_TIMEOUT_GUARD);
    return VCARC_RUNNING;

}


/**
  * @brief  Send data through Modem (without timeout mechanism)
  * @param  data: Data to be sent (string)
  * @param  txLen: Length of data to be sent
  * @retval Number of bytes actually sent (returns txLen on success, -1 on failure)
  */
int ModemSend(const char *data, int txLen)
{
  if (data == NULL || txLen <= 0) {
        return -1;  // Invalid parameters
    }
	BG77.rev_start = 0;
	BG77.rev_end = 0;

	memset(BG77.RxBuf,0,BG77.RxCnt);  
	BG77.RxCnt = 0;
	
	if( 128 < txLen){
			return -1;  // Lenth exceeds the maximum length 
		}
	USART1_Send_Len(data,txLen);
	BG77.rev_start = 1;
	// whether Driver need to add \r\n?
	USART1Send1Byte('\r');
	USART1Send1Byte('\n');
	return txLen;
}

//Were the blocking wait and timeout exit handled by me?
//For TIMEOUT we can discuss later  -- Owen
int ModemGet(char *data, int maxRx)
{
	if(BG77.rev_end == 1)
	{
		if(BG77.RxCnt <= maxRx)
		{
		  memcpy(data,BG77.RxBuf,BG77.RxCnt);
		}
		else
		{
		  return 1; //The received data length exceeds the maximum length.
		}	
	}
	else
	{
	  return 2; //The model has not received the data.
	}
  
	return 0;
}

void ModemErrorLog(const char *msg)
{
  printf("%s",msg);
}
