#ifndef __BSP_IIC_H__
#define __BSP_IIC_H__
#include "n32g45x.h"
#include "aisc.h"

typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} Status;

typedef enum
{
    MASTER_OK = 0,
    MASTER_BUSY,
    MASTER_MODE,
    MASTER_TXMODE,
    MASTER_RXMODE,
    MASTER_SENDING,
    MASTER_SENDED,
    MASTER_RECVD,
    MASTER_BYTEF,
    MASTER_BUSERR,
    MASTER_UNKNOW,
    SLAVE_OK = 20,
    SLAVE_BUSY,
    SLAVE_MODE,
    SLAVE_BUSERR,
    SLAVE_UNKNOW,

}ErrCode_t;

#define MODULE_SELF_RESET       1
#define MODULE_RCC_RESET        2
#define SYSTEM_NVIC_RESET       3
#define COMM_RECOVER_MODE       0

int i2c_slave_init(void);
extern uint8_t flag_slave_recv_finish;
extern uint8_t flag_slave_send_finish;

void I2C2_EV_IRQHandler(void);
void I2C2_ER_IRQHandler(void);
#endif



