#include "bsp_iic.h"

#define TEST_BUFFER_SIZE  256
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))
#define I2C_SLAVE_ADDR    0x010

#define I2C2_TEST
#define I2C2_REMAP
#define I2Cx I2C2
#define I2Cx_SCL_PIN GPIO_PIN_10
#define I2Cx_SDA_PIN GPIO_PIN_11
#define GPIOx        GPIOB

uint8_t data_buf[TEST_BUFFER_SIZE] = {0};
static __IO uint32_t I2CTimeout;
uint8_t flag_slave_recv_finish         = 0;
uint8_t flag_slave_send_finish         = 0;
static uint8_t rxDataNum = 0;
static uint8_t RCC_RESET_Flag = 0;

void CommTimeOut_CallBack(ErrCode_t errcode);

static void NVIC_Configuration(uint8_t ch)
{
    NVIC_InitType NVIC_InitStructure;
    if (ch == 1)
    {
        NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
    }
    if (ch == 2)
    {
        NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
    }

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    if (ch == 1)
    {
        NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn; // test err
    }
    if (ch == 2)
    {
        NVIC_InitStructure.NVIC_IRQChannel = I2C2_ER_IRQn; // test err
    }
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

int i2c_slave_init(void)
{
    I2C_InitType i2c2_slave;
    GPIO_InitType i2c1_gpio;
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_I2C1, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
#ifdef I2C1_REMAP
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    GPIO_ConfigPinRemap(GPIO_RMP_I2C1, ENABLE);
#endif
    /*PB6/PB8 -- SCL; PB7/PB9 -- SDA*/
    i2c1_gpio.Pin        = I2Cx_SCL_PIN | I2Cx_SDA_PIN;
    i2c1_gpio.GPIO_Speed = GPIO_Speed_2MHz;
    i2c1_gpio.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_InitPeripheral(GPIOx, &i2c1_gpio);

    I2C_DeInit(I2C2);
    i2c2_slave.BusMode     = I2C_BUSMODE_I2C;
    i2c2_slave.FmDutyCycle = I2C_FMDUTYCYCLE_2;
    i2c2_slave.OwnAddr1    = I2C_SLAVE_ADDR;
    i2c2_slave.AckEnable   = I2C_ACKEN;
    i2c2_slave.AddrMode    = I2C_ADDR_MODE_7BIT;
    i2c2_slave.ClkSpeed    = 100000; // 100000 100K

    I2C_Init(I2C2, &i2c2_slave);
    // int enable
    I2C_ConfigInt(I2C2, I2C_INT_EVENT | I2C_INT_BUF | I2C_INT_ERR, ENABLE);
    NVIC_Configuration(2);
    I2C_Enable(I2C2, ENABLE);
    return 0;
}

void I2C2_EV_IRQHandler(void)
{
    uint8_t timeout_flag = 0;
    uint32_t last_event = 0;
    
    last_event = I2C_GetLastEvent(I2C2);
    if ((last_event & I2C_ROLE_MASTER) != I2C_ROLE_MASTER) // MSMODE = 0:I2C slave mode
    {
        switch (last_event)
        {
        case I2C_EVT_SLAVE_RECV_ADDR_MATCHED:
            // clear flag,ready to receive data
            rxDataNum = 0;
            break;
        case I2C_EVT_SLAVE_SEND_ADDR_MATCHED:
            rxDataNum = 0;
            break;
        case I2C_EVT_SLAVE_DATA_SENDING:
            I2C2->DAT = data_buf[rxDataNum++];
            break;
        case I2C_EVT_SLAVE_DATA_SENDED:
            I2C2->DAT = data_buf[rxDataNum++];
            break;
        case I2C_EVT_SLAVE_DATA_RECVD:
            data_buf[rxDataNum++] = I2C2->DAT;
            break;
        case I2C_EVT_SLAVE_STOP_RECVD:
            I2C_Enable(I2C2, ENABLE);
            if(rxDataNum != 0)
            {
                flag_slave_recv_finish = 1; // The STOPF bit is not set after a NACK reception
            }
            break;
        default:
            I2C_Enable(I2C2, ENABLE);
            timeout_flag = 1;
            break;
        }
    }
    
    if (timeout_flag)
    {
        if ((I2CTimeout--) == 0)
        {
            CommTimeOut_CallBack(SLAVE_UNKNOW);
        }
    }
    else
    {
        I2CTimeout = I2CT_LONG_TIMEOUT;
    }
}

void I2C2_ER_IRQHandler(void)
{
    uint32_t last_event;
    last_event = I2C_GetLastEvent(I2C2);
    if(last_event == I2C_EVT_SLAVE_ACK_MISS)   
    {   
        I2C_ClrFlag(I2C2, I2C_FLAG_ACKFAIL);
        if(rxDataNum != 0)  //slave send the last data and recv NACK 
        {
            flag_slave_send_finish = 1;
        }
        else //not the last data recv nack, send fail
        {
        }
    }
}

void SystemNVICReset(void)
{
    __set_FAULTMASK((uint32_t)1);
    log_info("***** NVIC system reset! *****\r\n");
    __NVIC_SystemReset();
}

void IIC_RCCReset(void)
{
    if (RCC_RESET_Flag >= 3)
    {
        SystemNVICReset();
    }
    else
    {
        RCC_RESET_Flag++;
        
        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C2, ENABLE);
        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C2, DISABLE);
        
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_I2C2, DISABLE );
        GPIO_ConfigPinRemap(GPIO_RMP_I2C1, DISABLE);
        GPIOB->PH_CFG &= 0xFFFFFF00;
        RCC_EnableAPB2PeriphClk( RCC_APB2_PERIPH_AFIO, DISABLE);
        RCC_EnableAPB2PeriphClk (RCC_APB2_PERIPH_GPIOB, DISABLE );
        
        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C2, ENABLE);
        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C2, DISABLE);
        
        log_info("***** IIC module by RCC reset! *****\r\n");
        i2c_slave_init();
    }
}

void IIC_SWReset(void)
{
    GPIO_InitType i2cx_gpio;
    
    i2cx_gpio.Pin        = I2Cx_SCL_PIN | I2Cx_SDA_PIN;
    i2cx_gpio.GPIO_Speed = GPIO_Speed_50MHz;
    i2cx_gpio.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitPeripheral(GPIOx, &i2cx_gpio);
    
    I2CTimeout = I2CT_LONG_TIMEOUT;
    for (;;)
    {
        if ((I2Cx_SCL_PIN | I2Cx_SDA_PIN) == (GPIOx->PID & (I2Cx_SCL_PIN | I2Cx_SDA_PIN)))
        {
            I2Cx->CTRL1 |= 0x8000;
            __NOP();
            __NOP();
            __NOP();
            __NOP();
            __NOP();
            I2Cx->CTRL1 &= ~0x8000;
            
            log_info("***** IIC module self reset! *****\r\n");
            break;
        }
        else
        {
            if ((I2CTimeout--) == 0)
            {
                IIC_RCCReset();
            }
        }
    }
}

void CommTimeOut_CallBack(ErrCode_t errcode)
{
    log_info("...ErrCode:%d\r\n", errcode);
    
#if (COMM_RECOVER_MODE == MODULE_SELF_RESET)
    IIC_SWReset();
#elif (COMM_RECOVER_MODE == MODULE_RCC_RESET)
    IIC_RCCReset();
#elif (COMM_RECOVER_MODE == SYSTEM_NVIC_RESET)
    SystemNVICReset();
#endif
}