#include "OTA.h"



//写入0x12345678到flash时，下次重启会升级APP
void Write_OTA_Flag(u32 data)
{
  FLASH_Unlock();
	FLASH_EraseOnePage(0x0803F800 );  //flash  最后一块扇区
	FLASH_ProgramWord( 0x0803F800 , data);
	FLASH_Lock(); /* 上锁 */
}

u32 Read_OTA_Flag(void)
{
  return *(volatile uint32_t *)0x0803F800 ;
}





