#ifndef __BG77_H__
#define __BG77_H__
#include "main.h"


#define NAD_PWR_PORT							GPIOB				
#define NAD_PWR_PIN   						GPIO_PIN_12										 //电源

#define NAD_WRITE_DISBLE_PORT			GPIOB
#define NAD_WRITE_DISBLE_PIN   		GPIO_PIN_13										 //写使能

#define NAD_RST_PORT							GPIOA
#define NAD_RST_PIN   						GPIO_PIN_3										 //复位

#define NAD_AP_READY_PORT					GPIOB
#define NAD_AP_READY_PIN   				GPIO_PIN_15										 //app 状态

#define NAD_NET_STATUS_PORT				GPIOA
#define NAD_NET_STATUS_PIN     	 	GPIO_PIN_4										 //网络状态

#define NAD_STATE_PORT						GPIOB
#define NAD_STATE_PIN   					GPIO_PIN_14										 //模块状态

// AT 指令集
#define						AT							"AT\r\n"
#define						ATE0						"ATE0\r\n"											//关闭回显  
#define						CGMM						"AT+CGMM\r\n"										//获取模组名称
#define						CGSN						"AT+CGSN\r\n"										//获取IMME号
#define						CPIN						"AT+CPIN?\r\n"									//检测SIM卡 
#define						CCID						"AT+CCID\r\n"									  //获取卡号 
#define						CSQ							"AT+CSQ\r\n"										//查询信号强度

#define						CREG						"AT+CREG?\r\n"									//注网
#define						QICSGP_SEARCH					"AT+QICSGP=1\r\n"							//查询APN信息 
#define						QICSGP_SET					"AT+QICSGP=1,1,\"CMNNET\",\"\",\"\",1\r\n"							//设置APN信息
#define						QIACT_SET						"AT+QIACT=1\r\n"								//设置上下文
#define						QIACT_SEARCH				"AT+QIACT?\r\n"								//查看上下文
#define						QIOPEN						"AT+QIOPEN=1,0,\"TCP\",\"413051ze4qj9.vicp.fun\",31949,0,2\r\n"			//连接TCP 
#define						UDP_OPEN					"AT+QIOPEN=1,0,\"UDP\",\"8.135.10.183\", 58441,0,2\r\n\r\n"	//UDP 连接
#define						UDP_CLOSE					"AT+QICLOSE=0\r\n"		//UDP 关闭

//AT 状态机
typedef enum
{
	BG_START,								//开机
	BG_RESET,								//重启
	BG_AT,									//开始 AT测试
	BG_ATE0,								//关闭回显
	BG_CGMM,								//获取模组名称
	BG_CGSN,								//获取IMME号
	BG_CPIN,								//检测SIM卡 
	BG_CCID,								//获取卡号 
	BG_CSQ,									//查询信号强度
	BG_CREG,								//注网
	BG_QICSGP_SEARCH,				//查询apn信息
	BG_QICSGP_SET,					//设置APN信息
	BG_QIACT_SET,						//设置上下文
	BG_QIACT_SEARCH,				//查看上下文 IP地址
	BG_UDP_OPEN,						//UDP 连接
	BG_UDP_TRANSMISSION,		//UDP 透传
	BG_UDP_CLOSE,						//UDP 关闭
	BG_IDLE,								//空闲
	BG_POWER_DOWN						//power down 低功耗模式
} BG77_STATE_t;


//BG77 
typedef struct
{
	u8 									AT_step;							//AT 执行流程状态机指示
	u8 									nad_state;						//通信模块状态
	u8 									nad_net_state;				//网络状态
	u8 									ap_ready;							// 模块app状态
	u8									RxBuf[128];					  //数据缓存
  u16									RxCnt;								//长度
  u8									rev_start;						//开始接收
  u8									rev_end;							//接收结束
	
	char								Csq[2];								//信号强度
	char								IMEI[15];							//IMEI
  char								ICCID[20];						//ICCID
  char								APN[10];							//APN
	char								IP[16];								//IP
}bg77_t;

typedef enum
{
	BG_OK,
	START_ERROR,
	SIM_ERROR,
	CSQ_ERROR,
	CREG_ERROR,
	QICSGP_ERROR,
}BG_STATE_T;

extern bg77_t BG77;  //模块句柄

void BG77_FSM(void); //状态机
/***********初始化相关***********/

void BG77_GPIO_Init(void);

/*********  模块引脚相关**************/
void BG77_Power_On(void);
void BG77_Power_Reset(void);
u8 Read_Net_State(void);
u8 Read_State(void);
u8 Read_App_Ready(void);

/*********  发送接收*****************/
ErrorStatus Send_AT_Commend( char  *cmd,char  *rev,u16 TimeOutMs);

/********* AT 指令相关函数***********/
#endif













