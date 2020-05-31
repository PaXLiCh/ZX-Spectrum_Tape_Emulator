#ifndef LANGUAGE_H_
#define LANGUAGE_H_

#define LANGUAGE_EN

extern char String[25]; //16x2 LCD chars

#ifdef LANGUAGE_RU

// Startup
static const char Text_Start_Title[] PROGMEM           = "ZX Tape emulator\0";
static const char Text_Start_Version[] PROGMEM         = "������ 0.00     ";


// SD card info
static const char Text_SD_No_SPI_Up[] PROGMEM          = "����� ������ �� \0";
static const char Text_SD_No_SPI_Down[] PROGMEM        = "������������ SPI\0";
static const char Text_SD_No_Response[] PROGMEM        = "����� ������!   \0";
static const char Text_SD_Size_Error_Up[] PROGMEM      = "����� SD-�����  \0";
static const char Text_SD_Size_Error_Down[] PROGMEM    = "�� ���������!   \0";
static const char Text_SD_Size[] PROGMEM               = "����� SD-�����  \0";


// FS info
static const char Text_FS_Type[] PROGMEM               = "��� �. �������  \0";
static const char Text_FS_FAT32[] PROGMEM              = "FAT32- ������!  \0";
static const char Text_FS_FAT16[] PROGMEM              = "FAT16- ��.      \0";
static const char Text_FS_FAT12[] PROGMEM              = "FAT12- ������!  \0";
static const char Text_FS_NoFAT[] PROGMEM              = " FAT  �� �������\0";
static const char Text_FS_MBR_Found[] PROGMEM          = "   ������ MBR   \0";


// Main menu
static const char Text_Main_Menu_Select[] PROGMEM      = "    ��������    \0";
static const char Text_Main_Menu_Play_Speed1[] PROGMEM = "> ���������� x1 <";
static const char Text_Main_Menu_Play_Speed2[] PROGMEM = "> ���������� x2 <";
static const char Text_Main_Menu_Play_Speed4[] PROGMEM = "> ���������� x4 <";
static const char Text_Main_Menu_Memory_Test[] PROGMEM = ">  ���� ������  <";


// Tape menu
static const char Text_Tape_Menu_No_Image[] PROGMEM    = "��� ������ tap !\0";
static const char Text_Tape_Menu_File[] PROGMEM        = "[%02u:%05u] ����"; // level, index
static const char Text_Tape_Menu_Dir[] PROGMEM         = "[%02u:%05u] �����"; // level, index


// Tape play
static const char Text_Tape_Play_Block_Pause[] PROGMEM = "����:%u [0]"; // block
static const char Text_Tape_Play_Block[] PROGMEM       = "����:%u [%u]"; // block, counter


// Memory test
static const char Text_Memory_Test[] PROGMEM           = "   ���� ������  \0";
static const char Text_Memory_Test_Error[] PROGMEM     = " ������ ������ !\0";
static const char Text_Memory_Test_OK[] PROGMEM        = "������ �������� \0";

static const char Text_Memory_Test_Process[] PROGMEM   = "���������:%i %%"; // progress
static const char Text_Memory_Test_BadByte[] PROGMEM   = "%05x = [%02x , %02x]"; // (unsigned int)address, byte test, byte read


#endif // LANGUAGE_RU

#ifdef LANGUAGE_EN

// Startup
static const char Text_Start_Title[] PROGMEM           = "ZX Tape emulator\0";
static const char Text_Start_Version[] PROGMEM         = "version 0.00     ";


// SD card info
static const char Text_SD_No_SPI_Up[] PROGMEM          = "SD card not     \0";
static const char Text_SD_No_SPI_Down[] PROGMEM        = "support SPI     \0";
static const char Text_SD_No_Response[] PROGMEM        = "SD didnt respond\0";
static const char Text_SD_Size_Error_Up[] PROGMEM      = "SD card size    \0";
static const char Text_SD_Size_Error_Down[] PROGMEM    = "not detected!   \0";
static const char Text_SD_Size[] PROGMEM               = "SD card size    \0";


// FS info
static const char Text_FS_Type[] PROGMEM               = "File system type\0";
static const char Text_FS_FAT32[] PROGMEM              = "FAT32 - BAD!    \0";
static const char Text_FS_FAT16[] PROGMEM              = "FAT16 - OK      \0";
static const char Text_FS_FAT12[] PROGMEM              = "FAT12 - BAD!    \0";
static const char Text_FS_NoFAT[] PROGMEM              = " FAT not found  \0";
static const char Text_FS_MBR_Found[] PROGMEM          = "   MBR found    \0";


// Main menu
static const char Text_Main_Menu_Select[] PROGMEM      = "     Select     \0";
static const char Text_Main_Menu_Play_Speed1[] PROGMEM = "> Play tape x1 <";
static const char Text_Main_Menu_Play_Speed2[] PROGMEM = "> Play tape x2 <";
static const char Text_Main_Menu_Play_Speed4[] PROGMEM = "> Play tape x4 <";
static const char Text_Main_Menu_Memory_Test[] PROGMEM = "> Memory  test <";


// Tape menu
static const char Text_Tape_Menu_No_Image[] PROGMEM    = "No files to play\0";
static const char Text_Tape_Menu_File[] PROGMEM        = "[%02u:%05u] File"; // level, index
static const char Text_Tape_Menu_Dir[] PROGMEM         = "[%02u:%05u] Dir"; // level, index


// Tape play
static const char Text_Tape_Play_Block_Pause[] PROGMEM = "Block:%u [0]"; // block
static const char Text_Tape_Play_Block[] PROGMEM       = "Block:%u [%u]"; // block, counter


// Memory test
static const char Text_Memory_Test[] PROGMEM           = "   Memory test  \0";
static const char Text_Memory_Test_Error[] PROGMEM     = "  Test failed!  \0";
static const char Text_Memory_Test_OK[] PROGMEM        = " Memory test OK \0";

static const char Text_Memory_Test_Process[] PROGMEM   = "Tested: %i %%"; // progress
static const char Text_Memory_Test_BadByte[] PROGMEM   = "%05x = [%02x , %02x]"; // (unsigned int)address, byte test, byte read


#endif // LANGUAGE_EN

#endif /* LANGUAGE_H_ */