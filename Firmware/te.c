//----------------------------------------------------------------------------------------------------
//подключаемые библиотеки
//----------------------------------------------------------------------------------------------------
#include "based.h"


//----------------------------------------------------------------------------------------------------
//глобальные переменные
//----------------------------------------------------------------------------------------------------

uint16_t BlockSize = 0;//размер блока данных в памяти
volatile uint16_t DataCounter = 0;//колиество выданных байт данных
volatile short LeadToneCounter = 0;//время выдачи пилот-тона
volatile uint8_t TapeOutMode = 0;//режим вывода
bool TapeOutVolume = false;//выдаваемый сигнал
volatile uint8_t Speed;//скорость работы

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//макроопределения
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define TAPE_OUT_LEAD 0
#define TAPE_OUT_SYNCHRO_1 1
#define TAPE_OUT_SYNCHRO_2 2
#define TAPE_OUT_DATA 3
#define TAPE_OUT_STOP 4

#define MAX_LEVEL 20

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//настройки кнопок
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define BUTTON_UP_DDR      DDRD
#define BUTTON_UP_PORT     PORTD
#define BUTTON_UP_PIN      PIND
#define BUTTON_UP          3

#define BUTTON_CENTER_DDR  DDRD
#define BUTTON_CENTER_PORT PORTD
#define BUTTON_CENTER_PIN  PIND
#define BUTTON_CENTER      2

#define BUTTON_DOWN_DDR    DDRD
#define BUTTON_DOWN_PORT   PORTD
#define BUTTON_DOWN_PIN    PIND
#define BUTTON_DOWN        1

#define BUTTON_SELECT_DDR  DDRD
#define BUTTON_SELECT_PORT PORTD
#define BUTTON_SELECT_PIN  PIND
#define BUTTON_SELECT      4

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//линии магнитофона
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define TAPE_OUT_DDR  DDRD
#define TAPE_OUT_PORT PORTD
#define TAPE_OUT_PIN  PIND
#define TAPE_OUT      0

//----------------------------------------------------------------------------------------------------
//прототипы функций
//----------------------------------------------------------------------------------------------------
void TapeMenu(void);//меню магнитофона
void MemoryTest(void);//тест памяти
void OutputImage(void);//запуск образа
void WaitAnyKey(void);//ожидание любой клавиши
void InitAVR(void);//инициализация контроллера

//----------------------------------------------------------------------------------------------------
//дополнительные библиотеки
//----------------------------------------------------------------------------------------------------

#include "language.h"
#include "dram.h"
#include "wh1602.h"
#include "sd.h"
#include "fat.h"
#include "memtest.h"

//----------------------------------------------------------------------------------------------------
// MAIN PROGRAM
//----------------------------------------------------------------------------------------------------
int main(void)
{
	InitAVR();
	WH1602_Init();

	// Display startup text until complete initialization
	WH1602_SetTextProgmemUpLine(Text_Start_Title);
	WH1602_SetTextProgmemDownLine(Text_Start_Version);

	DRAM_Init();
	SD_Init();
	FAT_Init();

	// Main menu
	uint8_t select_item = 0;
	while(1)
	{
		WH1602_SetTextProgmemUpLine(Text_Main_Menu_Select);
		if (select_item==0) strcpy_P(String, Text_Main_Menu_Play_Speed1);
		if (select_item==1) strcpy_P(String, Text_Main_Menu_Play_Speed2);
		if (select_item==2) strcpy_P(String, Text_Main_Menu_Play_Speed4);
		if (select_item==3) strcpy_P(String, Text_Main_Menu_Memory_Test);
		WH1602_SetTextDownLine(String);
		_delay_ms(500);

		//ждём нажатий кнопок
		while(1)
		{
			if (BUTTON_UP_PIN&(1<<BUTTON_UP))
			{
				if (select_item==0) select_item=3;
				else select_item--;
				break;
			}
			if (BUTTON_DOWN_PIN&(1<<BUTTON_DOWN))
			{
				if (select_item==3) select_item=0;
				else select_item++;
				break;
			}
			if (BUTTON_SELECT_PIN&(1<<BUTTON_SELECT))
			{
				if (select_item==0)
				{
					Speed=0;
					TapeMenu();
				}
				if (select_item==1)
				{
					Speed=1;
					TapeMenu();
				}
				if (select_item==2)
				{
					Speed=2;
					TapeMenu();
				}
				if (select_item==3)
				{
					MemoryTest();
				}
				break;
			}
		}
	}
	return(0);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//общие функции
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------------------------------------------------------------------------------------------------
//меню магнитофона
//----------------------------------------------------------------------------------------------------
void TapeMenu(void)
{
	uint8_t n;

	// Get the first file on SD card
	if (FAT_BeginFileSearch() == false)
	{
		// No files to play
		WH1602_SetTextProgmemUpLine(Text_Tape_Menu_No_Image);
		WH1602_SetTextDownLine("");
		_delay_ms(2000);
		return;
	}

	int8_t hidden; // hidden file
	int8_t system; // system file
	int8_t directory;// directory
	uint32_t FirstCluster;// first cluster of the file
	uint32_t Size;// file size
	uint16_t index=1;// file index
	static uint16_t level_index[MAX_LEVEL];//MAX_LEVEL уровней вложенности
	uint8_t level = 0;
	level_index[0] = index;

	while(1)
	{
		//выводим данные с SD-карты
		//читаем имя файла
		if (FAT_GetFileSearch(String, &FirstCluster, &Size, &directory, &hidden, &system) == true)
		{
			WH1602_SetTextDownLine(String);
		}
		if (directory == false)
		{
			sprintf_P(String, Text_Tape_Menu_File, level, index);
		}
		else
		{
			sprintf_P(String, Text_Tape_Menu_Dir, level, index);
		}
		WH1602_SetTextUpLine(String);
		_delay_ms(200);
		
		// Listen buttons
		while(1)
		{
			// Button 'Up' -> Previous file
			if (BUTTON_UP_PIN&(1<<BUTTON_UP))
			{
				uint8_t i = 1;
				if (BUTTON_CENTER_PIN&(1<<BUTTON_CENTER))
				{
					i = 10;
				}
				for (n = 0; n < i; n++)
				{
					if (FAT_PrevFileSearch() == true)
					{
						index--;
					}
					else
					{
						break;
					}
				}
				break;
			}
			// Button 'Down' -> Next file
			if (BUTTON_DOWN_PIN&(1<<BUTTON_DOWN))
			{
				uint8_t i=1;
				if (BUTTON_CENTER_PIN&(1<<BUTTON_CENTER))
				{
					i = 10;
				}
				for (n = 0; n < i; n++)
				{
					if (FAT_NextFileSearch() == true)
					{
						index++;
					}
					else break;
				}
				break;
			}
			// Button 'Select' -> Play file / open directory
			if (BUTTON_SELECT_PIN&(1<<BUTTON_SELECT))
			{
				if (directory == 0)
				{
					// для файла - запускаем на выполнение
					OutputImage();
				}
				else
				{
					if (level < MAX_LEVEL)
					{
						//запоминаем достигнутый уровень
						level_index[level] = index;
					}
					if (directory < 0)//если мы вышли на уровень вверх
					{
						if (level > 0)
						{
							level--;
						}
					}
					else
					{
						level++;
						if (level < MAX_LEVEL)
						{
							level_index[level] = 1;
						}
					}
					// заходим в директорию
					FAT_EnterDirectory(FirstCluster);
					index = 1;
					if (level < MAX_LEVEL)
					{
						for (uint16_t s = 1; s < level_index[level]; s++)
						{
							if (FAT_NextFileSearch() == true)
							{
								index++;
							}
							else break;
						}
					}
				}
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------
// Play file
//----------------------------------------------------------------------------------------------------
void OutputImage(void)
{
	_delay_ms(500);
	
	// Repeat for each block of image
	uint16_t block=0;
	while(1)
	{
		sprintf_P(String, Text_Tape_Play_Block, 0, 0);
		WH1602_SetTextUpLine(String);
		if (FAT_WriteBlock(&BlockSize, block) == false)
		{
			// No more blocks in file
			break;
		}

		//выводим номер блока файла
		sprintf_P(String, Text_Tape_Play_Block, block + 1, BlockSize);
		WH1602_SetTextUpLine(String);

		//запускаем таймер и регенерируем память
		TCNT0 = 0;//начальное значение таймера
		LeadToneCounter = 6000 << Speed;
		TapeOutMode = TAPE_OUT_LEAD;
		TapeOutVolume = false;
		DataCounter = 0;
		uint16_t dl = 0;
		sei();
		while(1)
		{
			cli();
			DRAM_Refresh();
			if (TapeOutMode==TAPE_OUT_STOP)
			{
				sprintf_P(String, Text_Tape_Play_Block_Pause, block + 1);
				WH1602_SetTextUpLine(String);
				uint16_t new_block=block+1;
				//формируем паузу
				int delay=200;
				if (BlockSize>0x13) delay=500;//передавался файл
				for(uint16_t n=0;n<delay;n++)
				{
					_delay_ms(10);
					if (BUTTON_SELECT_PIN&(1<<BUTTON_SELECT))//выход
					{
						TAPE_OUT_PORT&=0xff^(1<<TAPE_OUT);
						return;
					}
					if (BUTTON_CENTER_PIN&(1<<BUTTON_CENTER))//пауза
					{
						_delay_ms(200);
						while(1)
						{
							if (BUTTON_CENTER_PIN&(1<<BUTTON_CENTER)) break;
						}
						_delay_ms(200);
					}
					if (BUTTON_UP_PIN&(1<<BUTTON_UP))//на блок вперёд
					{
						_delay_ms(200);
						new_block = block + 1;
						break;
					}
					if (BUTTON_DOWN_PIN&(1<<BUTTON_DOWN))//на блок назад
					{
						_delay_ms(200);
						if (block>0) new_block = block - 1;
						break;
					}
				}
				block=new_block;
				break;
			}
			uint16_t dc=BlockSize-DataCounter;
			uint16_t tm=TapeOutMode;
			sei();
			if (tm==TAPE_OUT_DATA)
			{
				if (dl==30000)
				{
					sprintf_P(String, Text_Tape_Play_Block, block + 1, dc);
					WH1602_SetTextUpLine(String);
					dl=0;
				}
				else
				{
					dl++;
				}
			}
			_delay_us(10);
			if (BUTTON_SELECT_PIN&(1<<BUTTON_SELECT))//выход
			{
				cli();
				TAPE_OUT_PORT&=0xff^(1<<TAPE_OUT);
				return;
			}
			if (BUTTON_CENTER_PIN&(1<<BUTTON_CENTER))//пауза
			{
				cli();
				_delay_ms(200);
				while(1)
				{
					if (BUTTON_CENTER_PIN&(1<<BUTTON_CENTER)) break;
				}
				sei();
				_delay_ms(200);
			}
			
			if (BUTTON_UP_PIN&(1<<BUTTON_UP))// To next block
			{
				_delay_ms(200);
				block++;
				break;
			}
			if (BUTTON_DOWN_PIN&(1<<BUTTON_DOWN))// To previous block
			{
				_delay_ms(200);
				if (block > 0) block--;
				break;
			}
		}
		cli();
	}
	TAPE_OUT_PORT&=0xff^(1<<TAPE_OUT);
}

//----------------------------------------------------------------------------------------------------
//ожидание любой клавиши
//----------------------------------------------------------------------------------------------------
void WaitAnyKey(void)
{
	_delay_ms(200);
	while(1)
	{
		if (BUTTON_UP_PIN&(1<<BUTTON_UP)) break;
		if (BUTTON_DOWN_PIN&(1<<BUTTON_DOWN)) break;
		if (BUTTON_CENTER_PIN&(1<<BUTTON_CENTER)) break;
		if (BUTTON_SELECT_PIN&(1<<BUTTON_SELECT)) break;
	}
}

//----------------------------------------------------------------------------------------------------
//инициализация контроллера
//----------------------------------------------------------------------------------------------------
void InitAVR(void)
{
	cli();
	
	//настраиваем порты
	DDRA=0;
	DDRB=0;
	DDRD=0;
	DDRC=0;
	
	BUTTON_UP_DDR&=0xff^(1<<BUTTON_UP);
	BUTTON_DOWN_DDR&=0xff^(1<<BUTTON_DOWN);
	BUTTON_CENTER_DDR&=0xff^(1<<BUTTON_CENTER);
	BUTTON_SELECT_DDR&=0xff^(1<<BUTTON_SELECT);

	TAPE_OUT_DDR|=(1<<TAPE_OUT);
	
	//задаём состояние портов
	PORTA=0xff;
	PORTB=0xff;
	PORTD=0xff;
	PORTC=0xff;
	
	//настраиваем таймер T0
	TCCR0=((0<<CS02)|(1<<CS01)|(1<<CS00));//выбран режим деления тактовых импульсов на 64
	TCNT0=0;//начальное значение таймера
	TIMSK=(1<<TOIE0);//прерывание по переполнению таймера (таймер T0 восьмибитный и считает на увеличение до 0xff)
	
	TAPE_OUT_PORT &= 0xff ^ (1 << TAPE_OUT);
}

//----------------------------------------------------------------------------------------------------
//обработчик вектора прерывания таймера T0 (8-ми разрядный таймер) по переполнению
//----------------------------------------------------------------------------------------------------
ISR(TIMER0_OVF_vect)
{
	static uint8_t byte=0;//выдаваемый байт
	static uint8_t index=0;//номер выдаваемого бита
	static uint16_t addr=0;//текущий адрес
	TCNT0=0;
	if (TapeOutMode==TAPE_OUT_STOP)
	{
		TAPE_OUT_PORT&=0xff^(1<<TAPE_OUT);
		return;
	}
	if (TapeOutVolume==true)
	{
		TAPE_OUT_PORT|=1<<TAPE_OUT;
		TapeOutVolume=false;
	}
	else
	{
		TAPE_OUT_PORT&=0xff^(1<<TAPE_OUT);
		TapeOutVolume=true;
	}
	
	// Play lead tone
	if (TapeOutMode==TAPE_OUT_LEAD)
	{
		TCNT0=255-(142>>Speed);//начальное значение таймера
		if (LeadToneCounter>0) LeadToneCounter--;
		else
		{
			TapeOutMode=TAPE_OUT_SYNCHRO_1;
			return;
		}
	}
	//выводим синхросигнал 1
	if (TapeOutMode==TAPE_OUT_SYNCHRO_1)
	{
		TCNT0=255-(43>>Speed);//начальное значение таймера
		TapeOutMode=TAPE_OUT_SYNCHRO_2;
		return;
	}
	//выводим синхросигнал 2
	if (TapeOutMode==TAPE_OUT_SYNCHRO_2)
	{
		TCNT0=255-(48>>Speed);//начальное значение таймера
		TapeOutMode=TAPE_OUT_DATA;
		index=16;
		byte=0;
		addr=0;
		return;
	}
	
	// Play image data from DRAM
	if (TapeOutMode==TAPE_OUT_DATA)
	{
		if (index>=16)
		{
			if (addr>=BlockSize)
			{
				TapeOutMode=TAPE_OUT_STOP;
				DataCounter=0;
				return;
			}
			index=0;
			byte=DRAM_ReadByte(addr);
			addr++;
			DataCounter=addr;
		}
		//выдаём бит
		if (byte&128) TCNT0=255-(112>>Speed);//начальное значение таймера
		else TCNT0=255-(56>>Speed);//начальное значение таймера
		if ((index%2)==1) byte<<=1;
		index++;
		return;
	}
}
