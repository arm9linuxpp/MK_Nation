#include "BG77.h"
#include "main.h"
//bg77 句柄初始化
bg77_t BG77;

u8 Index_Strength_At_String(char * str,char data1,char data2);
BG_STATE_T BG_Start(void);
BG_STATE_T Wait_CSQ(u16 TimeOut);
BG_STATE_T Net_Register(void);
BG_STATE_T Apn_Set(void);
BG_STATE_T Get_IP(void);
void BG77_Power_Reset(void);

void BG77_FSM(void)
{
	char *Ptr;
	u8 len;
	u8 send[50] = "hello world";
  switch(BG77.AT_step)
	{
		case BG_RESET:		//复位
				BG77_Power_Reset();
				BG77.AT_step = BG_START;
				printf("bg77 reset\n");
				break;
	  case BG_START:		//开始 	
			
		  if(BG_Start() == BG_OK)
			{
			  BG77.AT_step = BG_AT;
				printf("bg44 start success\n");
			}
			else
			{
			  BG77.AT_step = BG_START;
				printf("bg44 start fail,try again\n");
			}
			
		  break;

		case BG_AT: 			//通信测试
			if(Send_AT_Commend(AT,"OK\r\n",1000)== SUCCESS)
			{
			  BG77.AT_step = BG_ATE0;
				printf("bg77-mcu communication success\n");
			}
			else
			{
			  BG77.AT_step = BG_RESET;
				printf("bg77-mcu communication fail,please check usart1\n");
			}
		  break;
			
		case BG_ATE0: 		//关闭回显
			if(Send_AT_Commend(ATE0,"OK\r\n",1000)== SUCCESS)
			{
			  BG77.AT_step = BG_CGSN;
			}				
		  break;
		case BG_CGSN: 		//获取IMME号
			if(Send_AT_Commend(CGSN,"OK\r\n",1000)== SUCCESS)
			{
				Ptr = strstr((char *)BG77.RxBuf,"\r\n");
				Ptr += 2;
				memcpy(BG77.IMEI,Ptr,15);
			  BG77.AT_step = BG_CPIN;
				printf("IMME:%.*s\n",15,BG77.IMEI);
			}				
		  break;
		case BG_CPIN: 		//检测SIM卡 
			if(Send_AT_Commend(CPIN,"READY",1000)== SUCCESS)
			{
			  BG77.AT_step = BG_CCID;
				printf("SIM card ready\n");
			}
			else
			{
			  printf("***************SIM card error****************\n");
			}
		  break;
		case BG_CCID: //获取卡号 
			if(Send_AT_Commend(CCID,"null",1000)== SUCCESS)
			{
				Ptr = strstr((char *)BG77.RxBuf,"CCID:");
				Ptr += 6;
				memcpy(BG77.ICCID,Ptr,20);
			  BG77.AT_step = BG_QICSGP_SEARCH;
				printf("CCID:%.*s\n",20,BG77.ICCID);
			}				
		  break;
		case BG_QICSGP_SEARCH: //APN 查询
			Send_AT_Commend(QICSGP_SET,"null",1000);delay_ms(1000);
			if(Send_AT_Commend(QICSGP_SEARCH,"null",1000)== SUCCESS )
			{
			  Ptr = strstr((char *)BG77.RxBuf,"QICSGP:");
				len = Index_Strength_At_String(Ptr,'"','"');
				Ptr+=11;
				BG77.AT_step = BG_QICSGP_SET;
				printf("APN:%.*s\n",len,Ptr);
			  			
			}
		  break;
		case BG_QICSGP_SET: 		//APN设置
		  if(Apn_Set() == BG_OK)
			{
			  BG77.AT_step = BG_CSQ;
				len = strlen(BG77.APN);
				printf("APN:%.*s set success \n",len,BG77.APN);
			}
			else
			{
			  printf("****************APN set fail**************\n");
			}
		  break;
		case BG_CSQ:  //查询信号强度		
		  if(Wait_CSQ(60000) == BG_OK)	
			{
			  BG77.AT_step = BG_CREG;
        printf("CSQ:%c%c\n",BG77.Csq[0],BG77.Csq[1]);				
			}
			else
			{
			  printf("**************no csq*****************\n");
			}
		  break;
		case BG_CREG: //注网	
		  if(Net_Register() == BG_OK)
			{
			  BG77.AT_step = BG_QIACT_SET;
				printf("net register success\n");
			}
			else
			{
			  USART1_Send_String("AT+QCFG=\"nwscanseq\",030201,1");delay_ms(50);  //快速搜网
				USART1_Send_String("AT+QCFG=\"nwscanmode\",3,1");delay_ms(50);
				USART1_Send_String("AT+QCFG=\"iotopmode\",1,1");delay_ms(50);
				USART1_Send_String("AT+QENG=\"servingcell\"");delay_ms(50);
				printf("******************net register failed and try again*****************\n");
				delay_ms(5000);
			}
		  break;
		case BG_QIACT_SET: //设置上下文
			if(Send_AT_Commend(QIACT_SET,"OK",1000)== SUCCESS)
			{
			  BG77.AT_step = BG_QIACT_SEARCH;
				printf("Activate a PDP Context success\n");
			}
			else
			{
			  printf("*************Activate a PDP Context failed***************\n");
			}
		  break;
		case BG_QIACT_SEARCH: //查看IP 地址
			if(Send_AT_Commend(QIACT_SEARCH,"null",1000)== SUCCESS)
			{
				Ptr = strstr((char *)BG77.RxBuf,"QIACT:");
				Ptr += 14;
				memcpy(BG77.IP,Ptr,13);
			  BG77.AT_step = BG_UDP_OPEN;
				printf("IP:%.*s\n",12,BG77.IP);
			}				
		  break;
		case BG_UDP_OPEN: 		//UDP 连接
			if(Send_AT_Commend(UDP_OPEN,"CONNECT",1000)== SUCCESS)
			{
				BG77.rev_end = 0;
				printf("UDP connecct success\n");
				printf("start Transparent transmission mode\n");
			  BG77.AT_step = BG_UDP_TRANSMISSION;
			}				
		  break;
		case BG_UDP_TRANSMISSION: //UDP 透传 
			BG77.rev_start = 1;
			//数据发送  
		  USART1_Send_String((char *)send);delay_ms(2000);
			printf("Tx:%s\n",send);
		  //数据接收
		  if(BG77.rev_end == 1)
			{
				BG77.rev_end = 0;
			  if(strstr(BG77.RxBuf,"+++")) //退出透传
				{
				  USART1_Send_String("+++");
					BG77.rev_start = 0;
					printf("end Transparent transmission mode\n");
					BG77.AT_step = BG_UDP_CLOSE;
				}
				else //UDP 数据处理
				{
				  printf("Rx:%s\n",BG77.RxBuf);
				}
			}
      		
		  break;
		case BG_UDP_CLOSE:			//UDP 关闭
			if(Send_AT_Commend(UDP_CLOSE,"OK",1000)== SUCCESS)
			{
			  BG77.AT_step = BG_IDLE;
				printf("UDP close \n");
			}
			break;
		case BG_IDLE:
			 printf("nothing\n");
		 break;
		case BG_POWER_DOWN:
			
		 break;
			

	}
}

/* 功能:向BG77 发送AT指令	  阻塞等待接收
 * cmd： 要发送的AT指令
 * rev： bg77 回复的内容有没有期待中的子串，例如OK
 * TimeOutMs： 最大等待回复时间
 */						
ErrorStatus Send_AT_Commend( char  *cmd,char  *rev,u16 TimeOutMs)
{
	u16 ret;
	uint32_t startTime = 0; // 获取当前系统时间
	uint32_t elapsedTime;
	u16 i;
	
	BG77.rev_start = 0;
	BG77.rev_end = 0;
	memset(BG77.RxBuf,0,BG77.RxCnt);   //清除上次发送AT接收到的缓冲
	BG77.RxCnt = 0;
	
	USART1_Send_String(cmd);		//发送指令
	BG77.rev_start = 1;  //开始接收，接收在串口完成
  while(1)
	{
		if(BG77.rev_end == 1)   //串口接收到了一帧数据 
		{
		  if(strstr((char *)BG77.RxBuf,rev) || !strcmp(rev,"null")) //收到的数据中是否有期待的 例如期待OK
			{
				return SUCCESS;
			}
			else
			{
			  return ERROR;							//回复错误退出 
			}
		}
		delay_ms(1);
		i++;
		if(i>=TimeOutMs)
		{
		  return TIMEOUT;
		}  
	}
	
}

//ascll 转 hex
uint8_t char_2_hex(char src)
{
    uint8_t desc;

    if((src >= '0') && (src <= '9'))
        desc = src - 0x30;
    else if((src >= 'a') && (src <= 'f'))
        desc = src - 0x57;
    else if((src >= 'A') && (src <= 'F'))
        desc = src - 0x37;

    return desc;
}

// 计算两个字符在字符串中的距离差
u8 Index_Strength_At_String(char * str,char data1,char data2)
{
	u8 i=0,j=0;
  while(str[i++] != data1);
	j = i;
	while(str[j++] != data2);
	return j-i-1; 
}

// 等待CSQ信号
BG_STATE_T Wait_CSQ(u16 TimeOut)
{
	u16 i = 0;
	u8 csq_value,temp1,temp2;
	char *Ptr;
	while(i<=TimeOut)
	{	
	  if(Send_AT_Commend(CSQ,"OK\r\n",1000)== SUCCESS)
		{
			Ptr = strstr((char *)BG77.RxBuf,"CSQ:");
			Ptr += 5;
			memcpy(BG77.Csq,Ptr,2);
			
			temp1 = char_2_hex(BG77.Csq[0]);
			temp2 = char_2_hex(BG77.Csq[1]);
			temp1 *=10;
			temp1 = temp1 + temp2;
			csq_value = temp1;
			if(csq_value > 0 && csq_value <= 31)  //0-31   
			{
			  BG77.AT_step = BG_CREG;
				return BG_OK;
			}
			else
			{
			  i += 2000;
				printf("waiting csq\n");
				delay_ms(2000);
			}
						
		}	
	}
  return CSQ_ERROR;
}
//注网
BG_STATE_T Net_Register(void)
{
	char *Ptr;
	u8 temp1,temp2;
  if(Send_AT_Commend(CREG,"OK",1000)== SUCCESS)
	{
	  Ptr = strstr((char *)BG77.RxBuf,"CREG:") + 6;
		temp1 = *Ptr;
		Ptr += 2;
		temp2 = *Ptr;
		if((temp1 == 0x30 && temp2 == 0x30) || (temp1 == 0x30 && temp2 == 0x31))
		{
		  return BG_OK;
		}
		else
		{
		  return CREG_ERROR; //注册失败
		}
	}
}
// 设置APN 定长
BG_STATE_T Apn_Set(void)
{
  char *Ptr;
	u8 len;
	char send_buf[50] = "AT+QICSGP=1,1,\"CMNNET\",\"\",\"\",1\r\n";
	Ptr = send_buf;
	Ptr+=15;
	len = strlen(BG77.APN);
	memcpy(Ptr,BG77.APN,len);
	
	if(Send_AT_Commend(send_buf,"OK",1000)== SUCCESS)
	{	 
    return BG_OK;		
	}
	return QICSGP_ERROR;
}
// 设置APN //不定长
BG_STATE_T Apn_Set2(void)
{
	char buf[15] = "AT+QICSGP=1,1,\"";
	char buf2[10] = "CMNNET";
	char buf3[11] = "\",\"\",\"\",1\r\n";
	char send_buf[50] = {0};
	char *Ptr = send_buf;
	u8 len;
	Ptr = send_buf;
	len = strlen(BG77.APN);
  memset(buf2,0,sizeof(buf2));
	memcpy(buf2,BG77.APN,len);
	
	memcpy(Ptr,buf,15);
	Ptr += 15;
	
	memcpy(Ptr,buf2,len);
	Ptr += len;
	
	memcpy(Ptr,buf3,len);
	if(Send_AT_Commend(send_buf,"OK",1000)== SUCCESS)
	{	 
    return BG_OK;		
	}
	return QICSGP_ERROR;
}



BG_STATE_T Get_IP(void)
{
  char *Ptr;
	char IP[6] = {0};
}


BG_STATE_T BG_Start(void)
{
	BG77.rev_start = 1;
	BG77_GPIO_Init();
	GPIO_ResetBits(NAD_RST_PORT,NAD_RST_PIN);	
	BG77_Power_On();
	
	if((strstr((char *)BG77.RxBuf,"APP RDY")))
	{
		printf("bg77 start success\n");
	  return BG_OK;
	}
	else
	{
	  return START_ERROR;
	}
	
	 
}

// GPIO初始化
void BG77_GPIO_Init(void)
{
	GPIO_InitType GPIO_InitStructure;
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA|RCC_APB2_PERIPH_GPIOB, ENABLE);
	GPIO_InitStructure.Pin        = NAD_PWR_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitPeripheral(NAD_PWR_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.Pin        = NAD_WRITE_DISBLE_PIN;
	GPIO_InitPeripheral(NAD_WRITE_DISBLE_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.Pin        = NAD_RST_PIN;
	GPIO_InitPeripheral(NAD_RST_PORT, &GPIO_InitStructure);		
	
	GPIO_InitStructure.Pin        = NAD_NET_STATUS_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitPeripheral(NAD_NET_STATUS_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.Pin        = NAD_STATE_PIN;
  GPIO_InitPeripheral(NAD_STATE_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.Pin        = NAD_AP_READY_PIN;
	GPIO_InitPeripheral(NAD_AP_READY_PORT, &GPIO_InitStructure);
	
	GPIO_ResetBits(NAD_PWR_PORT,NAD_PWR_PIN);
}


//bg77 电源启动
void BG77_Power_On(void)
{
	delay_ms(50);
	GPIO_ResetBits(NAD_RST_PORT,NAD_RST_PIN);	
	GPIO_SetBits(NAD_PWR_PORT,NAD_PWR_PIN);
	delay_ms(700);
	GPIO_ResetBits(NAD_PWR_PORT,NAD_PWR_PIN);	
	delay_ms(4000);
	
//	GPIO_SetBits(NAD_PWR_PORT,NAD_PWR_PIN);	
//	delay_ms(1000);
//	GPIO_ResetBits(NAD_PWR_PORT,NAD_PWR_PIN);
//	delay_ms(700);
//	GPIO_SetBits(NAD_PWR_PORT,NAD_PWR_PIN);	
//	delay_ms(3000);
}

// 复位
void BG77_Power_Reset(void)
{
  GPIO_SetBits(NAD_PWR_PORT,NAD_PWR_PIN);
	delay_ms(700);
	GPIO_ResetBits(NAD_RST_PORT,NAD_RST_PIN);	
	//delay_ms(3000);
}

//读网络状态
u8 Read_Net_State(void)
{
  return GPIO_ReadInputDataBit(NAD_NET_STATUS_PORT,NAD_NET_STATUS_PIN);
}

// 读状态
u8 Read_State(void)
{
  return GPIO_ReadInputDataBit(NAD_STATE_PORT,NAD_STATE_PIN);
}

// 读app状态
u8 Read_App_Ready(void)
{
  return GPIO_ReadInputDataBit(NAD_AP_READY_PORT,NAD_AP_READY_PIN);
}



// 在缓存中找指定字符串，成功返回1，失败返回0
u8 Find(u8 *a)
{ 
	if( strstr((char*)BG77.RxBuf,(char*)a)!=NULL ) return 1;
	else return 0;
}

