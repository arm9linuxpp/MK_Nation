
#ifndef __VIMOAPI_H
#define __VIMOAPI_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

#define MAX_COT_PAR_MAX 32

#define DEBUG_BUFFER_MIN 80

//#define MZA_FAKE_TIMOUT 1
//#define PRIVATE_APN
//#define BAND_LIMIT_20

#ifndef PRIVATE_APN
//#define MU_WITH_DNS 1
#endif

 // Vi Cot Api Return Code Above V5.
enum vcarc {
    VCARC_RUNNING                     = 0,
    VCARC_DONE                        = 1,
    VCARC_ROAMING_DONE                = 2,

    VCARC_TIMEOUT_GUARD               = 100,
    VCARC_TIMEOUT_CMD                 = 101,
    VCARC_MQTTYSN_NO_CONNACK          = 110,
    VCARC_MQTTYSN_REGACK_WRONG_STATUS = 111,
    VCARC_MQTTYSN_REGACK_WRONG_RC     = 112,
    VCARC_MQTTYSN_GOT_DISCONNECT      = 113,  // e.g. wrong Password
    VCARC_MQTTYSN_PUBLISH_LEN_ERROR   = 114,
    VCARC_MQTTYSN_PUBLISH_LEN_ZERO    = 115,
    VCARC_MQTTYSN_UNKNOWN_OP_CODE     = 116,
    VCARC_MQTTYSN_ERROR_NOT_NONE      = 117,
    VCARC_VOID
          };


typedef struct _CotParameter
{
    uint32_t cops;                        // Operator For Telekom 26201
    char     apnName[MAX_COT_PAR_MAX];    // APN name
    char     clientId[MAX_COT_PAR_MAX];   // client ID for mqtt_sn login. imei len + 8
    char     imei[MAX_COT_PAR_MAX];       // the imei mqttysn ID withoutpw

    // Same as PB name.
    char     mqttsnIp[MAX_COT_PAR_MAX];
    uint32_t mqttsnPort;

    // Timeouts
    uint32_t cmdTimeout;                  // AT command tomeout 1..2 sec
    uint32_t modemConnectTimeoutMs;       //  60 sec
    uint32_t modemSocketOpenTimeoutMs;    // 90 yec


// Out parameter
    uint16_t udpStatus;
// Internal

    // Statistics
    uint32_t moRunCounter;
    uint16_t moLoopRet;

    // internal state
    uint16_t intCmdParameter1;

    // Only internal use
    // all with prefix m_ only internaly use, no init. no modify
    uint16_t m_mode;                        // not used reserved if needed.
    // POS DATA

    int32_t   m_signalStrength; // RSSI
#ifndef BG95
    // QENG only 660
    uint32_t  m_cid;
    uint32_t  m_lac;   // !!!MZA???
    int32_t   m_RSRP;
    int32_t   m_RSRQ;
    int32_t   m_pci;
    int32_t   m_SINR;
    /*
    <lac> String type. Two-byte location area code in hexadecimal format.
    <ci> String type. Four-byte GERAN/E-UTRAN cell ID in hexadecimal format.
    <AcT> Integer type. Access technology of the serving cell.
                           0 GSM
                           8 eMTC
                           9 NB-IoT
    */
    uint8_t m_Mnc[3];
    uint8_t m_Mcc[3];

#endif

#ifdef MU_WITH_DNS
     char     dns_Ip[MAX_COT_PAR_MAX];
#endif
} CotParameter;


// Implemented by VI germany
 /**
 * @brief sendCot send data to COT via MQTT and PtrotoBuffer
 * @note all parameters must be set before calling this function.
 *       and must be allocated untille loopCotOneMs returns != 0
 * @note the BC660 must be activated and ready before calling this function
 * @param toCot data send to COT
 *        for bootstrap you need to send:
 *        toCot.has_deviceConfiguration = true;
 *        toCot.deviceConfiguration.version = 0;
 *        toCot.has_deviceDescriptorEvent = true;
 *        and config + deviceDescriptor data.
 *
 *        after that for level meter events you need to send
 *        toCot.has_deviceConfiguration = true;
 *        toCot.deviceConfiguration.version = 1; // or whatever COT retuned last.
 *
 *        to send NB-IOT LAC+CELLID just set
 *        toCot.has_devicePositionUpdate = true;  // Auto fill data inside from modem!
 *
 * @param fromCot data reveived from COT
 *        for success full bootstrap you get
 *        fromCot.has_deviceConfiguration = true;
 *        fromCot.deviceConfiguration.version = 1;
 *
 * @param parameter Paramter for sending.
 * @return 0 on success otherwise error code
 */
uint16_t sendCot( CotParameter *parameter);

/**
 * @brief roamingDetectCot trigger Provider ability
 * @return 0 on success otherwise error code
 * @param parameter pointer the paramter, result will be written there in cops
 *        also set Cotparameter.modemConnectTimeoutMs = 320000;
 *        take longer than 300 seconds for a scan.
 *
 *
 * starts a network search.
 * then select from the responses the best cops to use
 * send a VCARC_ROAMING_DONE when detection is done can take up to
 * you need to call loopCot50Ms until  VCARC_ROAMING_DONE oder error comes.
 */
uint16_t roamingDetectCot(CotParameter *parameter);


/**
 * @brief loopCot50Ms does background job to send to COT
 * @note call after sendCot every 50 milli second from main loop untill it returns != 0
 * @return 0 while sending is running. 1 on success or otherweise error (negative)
 */
uint16_t loopCot50Ms(void);



// from now one
// implemented by China Team only for declaration no other system depending functions are used.
/**
 * @brief ModemSend send data to the modem with \r\n
 * @note  transfere should be done by interrupt.
 * @param data ponter to the data to send.
 * @param txLen how many bytes
 * @return  0 on a error, txLen on success.
 */
int ModemSend(const char *data, int txLen);  // will be called by TEAM-DE to send data to modem, return 0 on success

/**
 * @brief ModemGet reads one line from the modem Terminated \r\n
 * @note  the reveive should be done in the interrupt or DMA what ever you system support.
 * @note  if multiple \n are reveived they must be separated. by calls
 * @note  do not return all bytes received... ONLY LINE by LINE!
 * @param data   pointer to the data.
 * @param maxRx  max number of bytes to read.
 * @return 0 if no rx, the bytes copied on data
 * Receive Buffer e.g.
Receive Buffer e.g.
Normal
-> AT\r\n
   first  call -> 4 -> AT\r\n
   second call -> 0
2 line are in the buffer
-> AT\r\nOK\r\n
   first  call -> 4 -> AT\r\n
   second call -> 4 -> OK\r\n

2 line are in the buffer + empty line
-> AT\r\n\r\nOK\r\n
   first  call -> 4 -> AT\r\n
   second call -> 2 -> \r\n
   third  call -> 4 -> OK\r\n

incomplete buffer
-> AT\r
   frist  call -> 0

junk in the buffer
-> \r\nOK\r\n
   first  call -> 2 -> \r\n      -> ignored by VIAPI
   second call -> 4 -> OK\r\n    -> This will accepcted as ACK.

 */
int ModemGet(char *data, int maxRx);   // data reveived by the modem, returns 0 on success

/**
 * @brief ModemErrorLog reports a error message should be vissible all time in the log
 * @param msg message to log.
 */
void ModemErrorLog(const char *msg);

#ifdef WITH_VI_DEBUG_LEVEL
/**
 * @brief ModemDebugLog verbose log can be filtered out in production
 * @param msg to log
 */
void ModemDebugLog(const char *msg);



void viAppendUdp(const char *rawData, int len);

#endif


#ifdef __cplusplus
 }
#endif

#endif
