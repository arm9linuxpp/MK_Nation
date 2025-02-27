/**
 * Copyright (c) 2018 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 * @defgroup libuarte_example_main main.c
 * @{
 * @ingroup libuarte_example
 * @brief Libuarte Example Application main file.
 *
 * This file contains the source code for a sample application using libuarte.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_libuarte_async.h"
#include "nrf_drv_clock.h"
#include <bsp.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_queue.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrf_ringbuf.h"
#include "nrf_delay.h"

void sendAsciiOverUart(const uint8_t* data );

NRF_LIBUARTE_ASYNC_DEFINE(libuarte, 0, 0, 0, NRF_LIBUARTE_PERIPHERAL_NOT_USED, 255, 3);

static uint8_t text[] = "UART example started.\r\n Loopback:\r\n";
static uint8_t text_size = sizeof(text);
static volatile bool m_loopback_phase;

typedef struct {
    uint8_t * p_data;
    uint32_t length;
} buffer_t;

NRF_QUEUE_DEF(buffer_t, m_buf_queue, 10, NRF_QUEUE_MODE_NO_OVERFLOW);


#define RS232_MAX_RX_BUFFER 1024
#define RS232_MAX_TX_BUFFER 1024

NRF_RINGBUF_DEF(gsUartRx, RS232_MAX_RX_BUFFER);

static int gUartOpen = 0;
static int gUartTxBusy = 0;



static char gsTx[RS232_MAX_TX_BUFFER+2];

#define WITH_LWM2M 1
//#define  WITH_LWM2M 1

#ifdef WITH_LWM2M
#include "lwm2m/ViApi/vimoapi.h"
#endif

#include "dtls_debug.h"



void uart_event_handler(void * context, nrf_libuarte_async_evt_t * p_evt)
{
    nrf_libuarte_async_t * p_libuarte = (nrf_libuarte_async_t *)context;
    ret_code_t ret;

    switch (p_evt->type)
    {
        case NRF_LIBUARTE_ASYNC_EVT_ERROR:
            bsp_board_led_invert(0);
            break;
        case NRF_LIBUARTE_ASYNC_EVT_RX_DATA:
           // NRF_LOG_INFO("NRF_LIBUARTE_ASYNC_EVT_RX_DATA %d", p_evt->data.rxtx.length);             
            //NRF_LOG_HEXDUMP_DEBUG(p_evt->data.rxtx.p_data, p_evt->data.rxtx.length); // causes error?                
            {
              
              int len = p_evt->data.rxtx.length;

              ret_code_t err_code = nrf_ringbuf_cpy_put(&gsUartRx, p_evt->data.rxtx.p_data, &len);           
              if(err_code != NRF_SUCCESS)
              {
                  NRF_LOG_ERROR("uart_block_tx nrf_ringbuf_cpy_put error 0x%04X", err_code);
              }


              nrf_libuarte_async_rx_free(p_libuarte, p_evt->data.rxtx.p_data, p_evt->data.rxtx.length);  // dont forget to free!            
            }

/*
            ret = nrf_libuarte_async_tx(p_libuarte,p_evt->data.rxtx.p_data, p_evt->data.rxtx.length);
            if (ret == NRF_ERROR_BUSY)
            {
                buffer_t buf = {
                    .p_data = p_evt->data.rxtx.p_data,
                    .length = p_evt->data.rxtx.length,
                };

                ret = nrf_queue_push(&m_buf_queue, &buf);
                APP_ERROR_CHECK(ret);
            }
            else
            {
                APP_ERROR_CHECK(ret);
            }
            bsp_board_led_invert(1);
            */
            m_loopback_phase = true;
            break;
        case NRF_LIBUARTE_ASYNC_EVT_TX_DONE:
    //       NRF_LOG_DEBUG("uart_event_handler NRF_LIBUARTE_ASYNC_EVT_TX_DONE");
           gUartTxBusy = 0;
        /*
            if (m_loopback_phase)
            {
                nrf_libuarte_async_rx_free(p_libuarte, p_evt->data.rxtx.p_data, p_evt->data.rxtx.length);
                if (!nrf_queue_is_empty(&m_buf_queue))
                {
                    buffer_t buf;
                    ret = nrf_queue_pop(&m_buf_queue, &buf);
                    APP_ERROR_CHECK(ret);
                    UNUSED_RETURN_VALUE(nrf_libuarte_async_tx(p_libuarte, buf.p_data, buf.length));
                }
            }
            */
            bsp_board_led_invert(2);
            break;
        default:
            break;
    }
} 

/**@brief Function starting the internal LFCLK oscillator.
 *
 * @details This is needed by RTC1 which is used by the Application Timer
 *          (When SoftDevice is enabled the LFCLK is always running and this is not needed).
 */
static void lfclk_request(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
}

static app_timer_id_t m_timer_id;

static int gsViModemActive = 0;

void sendAsciiOverUart(const uint8_t* data );
int ModemGet(char *data, int maxRx);

static void modem_xxx()
{
      uint8_t rxd[255];
      int maxRx = 255;
      int rx = ModemGet(rxd, maxRx);
      if (rx > 0)
      {        
  //      NRF_LOG_WARNING("Rx %d %02X  %02X  %02X",  rx, rxd[0], rxd[1], rxd[2]);
        NRF_LOG_WARNING("RxModem %s", rxd);   // problem with %d in line?!
  //      log_bytearray(LOG_LEVEL_INFO, "modem_xx: ", rxd, 3, true, true, true);
      }
}

static CotParameter cp;

static void timer_handler_tick(void * p_context)
{
    static int cnt = 0;
    static int counter10s = 0;

    
    cnt++;
    // 200 -> 10 sec
    if (cnt>100) 
    {
      // 10 sec.
      counter10s++;
      cnt = 0;
      NRF_LOG_WARNING("timer_handler_tick %d", counter10s);
      
     
      // sendAsciiOverUart("AT\r\n");
      
      if ( counter10s == 1)
      {
#ifdef WITH_LWM2M
      
      memset(&cp,0,sizeof(cp));
      cp.clientId[0]=0;
      cp.cops = 26201;
      strcpy(cp.apnName    , "internet.m2mportal.de"); 
#ifdef DT_COT
      strcpy(cp.mqttsnIp   , "51.116.97.195"); // "lwm2m.test-ram.m2m.telekom.com");  // mqtt.mu-tec.de UDP 1883 yao kiste hetzner           alias vi-srv-01
      cp.mqttsnPort = 5784;
#else
      strcpy(cp.mqttsnIp   , "168.119.70.243");   // VI Intern test server for Wireshar debug
      cp.mqttsnPort = 7685;      
#endif
      strcpy(cp.clientId    , "0123456789abcdef");
      strcpy(cp.imei    , "0123456789abcdef");

      cp.cmdTimeout               =  2000;   // 2 sec 
      cp.modemConnectTimeoutMs    = 60000;   // 60 sec
      cp.modemSocketOpenTimeoutMs = 90000;   // 90 sec
    

      sendCot(&cp);
      gsViModemActive = 1; // we are now active.
#endif
      }
    }        

    if (gsViModemActive)
    {
      // @return 0 while sending is running. 1 on success or otherweise error (negative)
#ifdef WITH_LWM2M 
      uint16_t cret = loopCot50Ms();
      if (cret == 0) {
        //NRF_LOG_WARNING("gsViModemActive = 0");
      } else {
        gsViModemActive = 0;
        if (cret == 1) {
          NRF_LOG_WARNING("gsViModemActive = 0 success");
        } else {
          NRF_LOG_WARNING("gsViModemActive = 0 error %d",cret);
        }
      }

       if (cp.udpStatus == 1) 
       {
          NRF_LOG_WARNING("udpStatus ready now start LwM2m");
          myClientInit();
          main_wakaama();   // start lwm2m client
          cp.udpStatus =2;
       }

       
       

#endif
    }
    NRF_LOG_FLUSH();
}

#define TICK_TIME_MS 50

static void start50msTimer()
{
  
      if(TICK_TIME_MS>0)
      {
          NRF_LOG_INFO("start tick timer at %dms", TICK_TIME_MS);
          ret_code_t err_code;
          APP_TIMER_DEF(m_timer_id);
          err_code = app_timer_create(&m_timer_id, APP_TIMER_MODE_REPEATED, timer_handler_tick);
          APP_ERROR_CHECK(err_code);
          err_code = app_timer_start(m_timer_id, APP_TIMER_TICKS(TICK_TIME_MS), NULL);
          APP_ERROR_CHECK(err_code);
      }
}



/**
 * @brief Function for main application entry.
 */
int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);
    
    ret_code_t ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);
  
    nrf_drv_clock_lfclk_request(NULL);

    ret_code_t err_code = NRF_LOG_INIT(app_timer_cnt_get);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

   

    nrf_libuarte_async_config_t nrf_libuarte_async_config = {
            .tx_pin     = TX_PIN_NUMBER,
            .rx_pin     = RX_PIN_NUMBER,
            .baudrate   = NRF_UARTE_BAUDRATE_115200,
            .parity     = NRF_UARTE_PARITY_EXCLUDED,
            .hwfc       = NRF_UARTE_HWFC_DISABLED,
            .timeout_us = 100,
            .int_prio   = APP_IRQ_PRIORITY_LOW
    };

    nrf_ringbuf_init(&gsUartRx);

    err_code = nrf_libuarte_async_init(&libuarte, &nrf_libuarte_async_config, uart_event_handler, (void *)&libuarte);

    APP_ERROR_CHECK(err_code);

    gUartOpen = 1;

    nrf_libuarte_async_enable(&libuarte);

    err_code = nrf_libuarte_async_tx(&libuarte, text, text_size);
    APP_ERROR_CHECK(err_code);

//    lfclk_request();
    app_timer_init();
    start50msTimer();

    while (true)
    {
        //NRF_LOG_FLUSH();
    }
}


/** @} */


uint32_t uart_block_tx(uint8_t *byte, size_t len)
{
    if (gUartOpen == 0)
    {
        NRF_LOG_ERROR("uart_block_tx call when closed; abort!");
        return NRF_ERROR_INVALID_STATE;
    }

    if (len > RS232_MAX_TX_BUFFER)
    {
      NRF_LOG_ERROR("uart_block_tx too large len: ",len );    
      return NRF_ERROR_DATA_SIZE;
    }

    int bret = 0;
    CRITICAL_REGION_ENTER();
    if (gUartTxBusy) 
    {        
        NRF_LOG_INFO("gUartTxBusy uart_block_tx add to ring buffer len = %d",len ); 
        /*
        // add to que.. and send on event TX_done.   send later                 
        ret_code_t err_code = nrf_ringbuf_cpy_put(&m_UartResend, byte, &len);           
        if(err_code != NRF_SUCCESS)
        {
            NRF_LOG_ERROR("uart_block_tx nrf_ringbuf_cpy_put error 0x%04X", err_code);
        }
        APP_ERROR_CHECK(err_code);
        if (!gUartTxBusy) 
        {
          NRF_LOG_ERROR("!!!MZA???? gUartTxBusy change during add!"); // TODO: resend!
        }     
        */
        bret = 1;
    }
    CRITICAL_REGION_EXIT();
    if (bret) 
    {
        return NRF_ERROR_BUSY;   // must be outside of critical section.
    }

    uint32_t ret = 0;
    CRITICAL_REGION_ENTER();
    gUartTxBusy = 1;  // needed before nrf_libuarte_async_tx API call reentrance!!! 07/2021 MZA

    memcpy(gsTx,byte,len+1);  // must copy data shall no changed during transfere ( +1 also termiantion!) 

    ret = nrf_libuarte_async_tx(&libuarte,gsTx, len);    
    CRITICAL_REGION_EXIT();
    if (ret == NRF_SUCCESS)
    {
        //gUartTxBusy = 1;
        return ret;  // success
    }
    gUartTxBusy = 0;

    if (ret == NRF_ERROR_BUSY)
    {
        NRF_LOG_ERROR("uart_block_tx busy not sended",len );         
        APP_ERROR_CHECK(ret);
    } 
    else 
    {
        NRF_LOG_ERROR("uart_block_tx failed 0x%04X",ret); 
        APP_ERROR_CHECK(ret);        
    }
    return ret;
}



void sendAsciiOverUart(const uint8_t* data )
{
    // think must be statis segment!
    uint8_t m_uart_tx_buf[RS232_MAX_TX_BUFFER];

    //NRF_LOG_INFO("sendAsciiOverUart: %s len: %d",data,strlen(data));  VERY LONG!!! with UDP
    int len = strlen(data);
    if (len > (RS232_MAX_TX_BUFFER-2) ) 
    {
      NRF_LOG_WARNING("UART TX: len error=0x%04X", len);
      return;
    }
    int uart_pos_tx = 0;       
    for (uint32_t i = 0; i < len; i++)
    {
        // write everything received into rx buffer
        m_uart_tx_buf[uart_pos_tx++] = data[i];
    }     
    // send whole rx buffer as one message
    uint32_t ret = uart_block_tx(m_uart_tx_buf, uart_pos_tx);

    // Expected error codes:
    // * SUCCESS:     sent immediately
    // * ERROR_BUSY:  added to ringbuffer to be sent later
    // everything else --> WARNING
    if((ret != NRF_SUCCESS) && (ret != NRF_ERROR_BUSY))
    {
        //TODO: Send ErrorMessage to DSPIC
        NRF_LOG_WARNING("UART TX: err_code=0x%04X", ret);
    } else {
        //NRF_LOG_INFO("sendAsciiOverUart: leave");
    }
}





// implemented by China Team
int ModemSend(const char *data, int txLen)
{
    sendAsciiOverUart(data);
    return txLen;
}

/**
return 0 if no data there. or how many 
*/
int ModemTriggerInBuffer(const char *data, int *inOut)
{
  int cnt = *inOut;
  //log_bytearray(LOG_LEVEL_INFO, "ModemTriggerInBuffer: ", data, cnt, true, true, true);
  if (cnt >= RS232_MAX_RX_BUFFER) 
  {
    return cnt;
  }

  int dataLen = 0;
  int removeLen = 0; // with

  for (int i=0; i<cnt; i++)
  {
     char c = data[i];
     //NRF_LOG_INFO("ModemGet %d --> 0x%02X",i,c);
     if ((dataLen == 0) && (i > 0) && (c == '\r') )
     {
         dataLen = i;   // excude the CR
         removeLen = dataLen;
         *inOut = removeLen;
     } else {
         if ( (c == '\r') || ( c == '\n') ) 
         {
            removeLen = i+1;
            *inOut = removeLen;
         } else {
           // not first hit 
           if (dataLen != 0 ) {
              // we have some rx... and also one CR... 
              // kill all CR/LF behind
              return dataLen;
            }
         }
     }    
  }
  //NRF_LOG_INFO("ModemGet ret %d --> %d",dataLen,removeLen);
  *inOut = removeLen;  // 
  // e.g. we have CR without CR or LF... we wait and do not remove
  return dataLen;

}

int ModemGet(char *data, int maxRx)
{
  // find the Trigger or over max -:) 
  uint8_t * actRxDta;
  size_t actRxLen = maxRx;
  
  bool start = true;
  ret_code_t  err_code = nrf_ringbuf_get(&gsUartRx, &actRxDta, &actRxLen, start); 
  if (err_code == NRF_SUCCESS) 
  {
    // successfull  
    if (actRxLen > 0)
    {
      // data in ring buffer. check how may we can use?
      int du = actRxLen; // input max...
      int want = ModemTriggerInBuffer(actRxDta, &du);

       //NRF_LOG_WARNING("ModemTriggerInBuffer  %d -> %d ", want, du);
      if (want > 0)
      {
        int maxCopy = want;
        if (maxCopy > maxRx)
        {
           maxCopy = maxRx; // more requested then we can handle
        }
        int i=0;
        int dst = 0;
        for ( i=0; i<maxCopy;i++) 
        {
          char c = actRxDta[i];
   //       NRF_LOG_WARNING("uart xxx %d -> %02X ", i,c);
          switch(c) {
          case '\r':
          case '\n':
              break;
              default:
              data[dst++] = c;
              break;
          }
          
        }
        data[dst] = 0;  // Terminate

        //NRF_LOG_WARNING("uart %s du=%d ", data,du);
        
        err_code = nrf_ringbuf_free(&gsUartRx, du);  // must release !!!        
        if(err_code != NRF_SUCCESS)
        {
          NRF_LOG_WARNING("uart_send_queue error nrf_ringbuf_free 0x%04X %d", err_code, du);
        } else {
           //NRF_LOG_INFO("ModemGet nrf_ringbuf_free %d with data %d",du,dst);
        }
        return dst;  // what we have filled
      
      } else {
        // we need to call free allways! even with 0! 
        // we may have data received but no trigger! be send udp data with "> " 
          
          err_code = nrf_ringbuf_free(&gsUartRx, du);  // must release !!!        
          if(err_code != NRF_SUCCESS)
          {
            NRF_LOG_WARNING("uart_send_queue error nrf_ringbuf_free 0x%04X %dm", err_code, du);
          } else {
            NRF_LOG_WARNING("ModemGet nrf_ringbuf_free %d with no data ", du);          
          }
        
      }
    }
    else
    {
       // act rax len < 0
    }
    return 0;
  }
  else
  {
    NRF_LOG_WARNING("uart_send_queue error nrf_ringbuf_cpy_get 0x%04X", err_code);  // NRF_ERROR_BUSY
  }
  
    return 0;
}



int ip_addr_cmp(int a,int b)
{
  return 1; 
}

void viFailureLog(const char *error) 
{
   NRF_LOG_INFO("viFailureLog %s",error);    
   NRF_LOG_FLUSH();
}
void viDebugLog(const char *error) 
{
   /*
   int len = strlen(error);
   if (len < 5) 
   {
    NRF_LOG_INFO("viDebugLog <5");    
    NRF_LOG_FLUSH();
    return;
   }
   if (len > 45) 
   {
    NRF_LOG_INFO("viDebugLog >45");    
    NRF_LOG_FLUSH();
    return;
   }
   */
   NRF_LOG_INFO("viDL %s",error);      
   NRF_LOG_FLUSH();
}

void ModemErrorLog(const char *msg) {
  NRF_LOG_INFO("ModemErrorLog %s",msg); 
  NRF_LOG_FLUSH();
}

 

void ModemDebugLog(const char *msg) {
  NRF_LOG_INFO("ModemDebugLog %s",msg); 
  NRF_LOG_FLUSH(); 
}




log_t
dtls_get_log_level(void) {
  return 0;
}

void
dtls_set_log_level(log_t level) {
  (void)level;
}

void
hexdump(const unsigned char *packet, int length) {
  (void)packet;
  (void)length;
}

void
dump(unsigned char *buf, size_t len) {
  (void)buf;
  (void)len;
}

void
dtls_dsrv_hexdump_log(log_t level, const char *name, const unsigned char *buf, size_t length, int extend) {
  (void)level;
  (void)name;
  (void)buf;
  (void)length;
  (void)extend;
}


void
dtls_dsrv_log_addr(log_t level, const char *name, const session_t *addr) {
  (void)level;
  (void)name;
  (void)addr;
//   NRF_LOG_WARNING("dtls_dsrv_log_addr %d %s",level,name);  
//   NRF_LOG_FLUSH();
}


int Xsendto(int sockfd, const void *buf, size_t len, int flags )
{
  //NRF_LOG_WARNING("Xsendto %d",len);  
  //NRF_LOG_FLUSH();
  viAppendUdp(buf, len);
  return 0;
}


void dsrv_log(log_t level, const char *format, ...) {
}
