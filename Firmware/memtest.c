#include "based.h"
#include "memtest.h"
#include "language.h"
#include "wh1602.h"
#include "dram.h"

//----------------------------------------------------------------------------------------------------
// DRAM test
//----------------------------------------------------------------------------------------------------
void MemoryTest(void)
{
	uint8_t last_p = 0xff;
	WH1602_SetTextProgmemUpLine(Text_Memory_Test);

	/*
	for (uint16_t b = 0; b <= 255; b++)
	{
		uint8_t progress = (uint8_t)(100UL * (int32_t)b / 255UL);
		if (progress != last_p)
		{
			sprintf_P(String, Text_Memory_Test_Process, progress);
			WH1602_SetTextDownLine(String);
			last_p = progress;
		}
		
		// Fill RAM with values
		for (uint32_t addr = 0; addr < 131072UL; addr++)
		{
			uint8_t byte = (b + addr) & 0xff;
			DRAM_WriteByte(addr, byte);
			DRAM_Refresh();
		}
		
		// Test values in RAM
		for (uint32_t addr = 0; addr < 131072UL; addr++)
		{
			uint8_t byte = (b + addr) & 0xff;
			uint8_t byte_r = DRAM_ReadByte(addr);
			DRAM_Refresh();
			if (byte != byte_r)
			{
				WH1602_SetTextProgmemUpLine(Text_Memory_Test_Error);
				sprintf_P(String, Text_Memory_Test_BadByte, (unsigned int)addr, byte, byte_r);
				WH1602_SetTextDownLine(String);
				_delay_ms(5000);
				return;
			}
		}
	}
	*/
	
	
	for (uint32_t addr = 0; addr < 131072UL; addr++)
	{
		uint8_t progress = (uint8_t)(100UL * addr / 131071UL);
		if (progress != last_p)
		{
			sprintf_P(String, Text_Memory_Test_Process, progress);
			WH1602_SetTextDownLine(String);
			last_p = progress;
		}
		uint8_t byte = 0x01;
		for (uint8_t n = 0; n < 8; n++, byte <<= 1)
		{
			DRAM_WriteByte(addr, byte);
			DRAM_Refresh();
			uint8_t byte_r = DRAM_ReadByte(addr);
			if (byte != byte_r)
			{
				WH1602_SetTextProgmemUpLine(Text_Memory_Test_Error);
				sprintf_P(String, Text_Memory_Test_BadByte, (unsigned int)addr, byte, byte_r);
				WH1602_SetTextDownLine(String);
				_delay_ms(5000);
				return;
			}
		}
	}
	/**/

	WH1602_SetTextProgmemUpLine(Text_Memory_Test_OK);
	WH1602_SetTextDownLine("");
	_delay_ms(3000);
}
