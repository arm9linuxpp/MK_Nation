#include "OTA.h"



//д��0x12345678��flashʱ���´�����������APP
void Write_OTA_Flag(u32 data)
{
  FLASH_Unlock();
	FLASH_EraseOnePage(0x0803F800 );  //flash  ���һ������
	FLASH_ProgramWord( 0x0803F800 , data);
	FLASH_Lock(); /* ���� */
}

u32 Read_OTA_Flag(void)
{
  return *(volatile uint32_t *)0x0803F800 ;
}





