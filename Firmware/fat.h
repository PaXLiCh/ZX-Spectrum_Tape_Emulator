//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//�������� � FAT
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//BOOT-������ � ��������� BPB
#define BS_jmpBoot			0
#define BS_OEMName			3
#define BPB_BytsPerSec		11
#define BPB_SecPerClus		13
#define BPB_ResvdSecCnt		14
#define BPB_NumFATs			16
#define BPB_RootEntCnt		17
#define BPB_TotSec16		19
#define BPB_Media			21
#define BPB_FATSz16			22
#define BPB_SecPerTrk		24
#define BPB_NumHeads		26
#define BPB_HiddSec			28
#define	BPB_TotSec32		32
#define BS_DrvNum			36
#define BS_Reserved1		37
#define BS_BootSig			38
#define BS_VolID			39
#define BS_VolLab			43
#define BS_FilSysType		54
#define BPB_FATSz32			36
#define BPB_ExtFlags		40
#define BPB_FSVer			42
#define BPB_RootClus		44
#define BPB_FSInfo			48
#define BPB_BkBootSec		50
#define BPB_Reserved		52

//��� �������� �������
#define FAT12 0
#define FAT16 1
#define FAT32 2

//----------------------------------------------------------------------------------------------------
//���������� ����������
//----------------------------------------------------------------------------------------------------
unsigned char Sector[512];//������ ��� �������z
unsigned long LastReadSector=0xffffffff;//��������� ��������� ������
unsigned long FATOffset=0;//�������� FAT

unsigned long CurrentRootFileAddr=0;//������� ����� ����� � �������� ����������
unsigned long EndRootFileAddr=0;//��������� ����� ����� � �������� ����������

unsigned long RootDirSectors;//���������� ��������, ������� �������� ����������� 
unsigned long FATSz;//������ ������� FAT
unsigned long DataSec;//���������� �������� � ������� ������ �����
unsigned long CountofClusters;//���������� ��������� ��� ������ (������� ���������� � ������ 2! ��� ����������, � �� ����� ���������� ��������)
unsigned long FirstDataSector;//������ ������ ������
unsigned long FirstRootDirSecNum;//������ �������� ���������� (��� FAT16 - ��� ������ � ��������� �������, ��� FAT32 - ��� ���� � ������� ������ � ��������� BPB_RootClus)
unsigned long ClusterSize;//������ �������� � ������

unsigned char FATType=FAT12;//��� �������� �������

const char Text_FAT_Type[] PROGMEM =  "��� �. �������  \0";
const char Text_FAT32[] PROGMEM =     "FAT32- ������!  \0";
const char Text_FAT16[] PROGMEM =     "FAT16- ��.      \0";
const char Text_FAT12[] PROGMEM =     "FAT12- ������!  \0";

//----------------------------------------------------------------------------------------------------
//��������� �������
//----------------------------------------------------------------------------------------------------
void FAT_Init(void);//������������� FAT
unsigned long GetByte(unsigned long offset);//������� ����
unsigned long GetShort(unsigned long offset);//������� ��� �����
unsigned long GetLong(unsigned long offset);//������� 4 �����
bool FAT_BeginFileSearch(void);//������ ����� ����� � �������
bool FAT_PrevFileSearch(void);//��������� � ����������� ����� � ��������
bool FAT_NextFileSearch(void);//���������� ����� ����� � ��������
bool FAT_GetFileSearch(char *filename,unsigned long *FirstCluster,unsigned long *Size);//�������� ��������� �������� ���������� ����� � ��������
bool FAT_WriteBlock(unsigned short *BlockSize,unsigned short Block);//�������� � ��� ���� �����
//----------------------------------------------------------------------------------------------------
//������������� FAT
//----------------------------------------------------------------------------------------------------
void FAT_Init(void)
{
 WH1602_SetTextUpLine("");
 WH1602_SetTextDownLine("");

 LastReadSector=0xffffffffUL;
 //���� FAT
 FATOffset=0;
 for(unsigned long fo=0;fo<33554432UL;fo++)
 {
  unsigned char b=GetByte(fo); 
  if (b==233 || b==235)
  {
   b=GetByte(fo+511UL);
   if (b==170)   
   {
    b=GetByte(fo+510UL);
    if (b==85)
	{
     FATOffset=fo;
     break;
	}
   } 
  }
 }
 LastReadSector=0xffffffffUL;
 //���������� ���������� ��������, ������� �������� ����������� 
 RootDirSectors=(unsigned long)(ceil((GetShort(BPB_RootEntCnt)*32UL+(GetShort(BPB_BytsPerSec)-1UL))/GetShort(BPB_BytsPerSec)));
 //���������� ������ ������� FAT
 FATSz=GetShort(BPB_FATSz16);//������ ����� ������� FAT � ��������
 if (FATSz==0) FATSz=GetLong(BPB_FATSz32);
 //���������� ���������� �������� � ������� ������ �����
 unsigned long TotSec=GetShort(BPB_TotSec16);//����� ���������� �������� �� �����
 if (TotSec==0) TotSec=GetLong(BPB_TotSec32);
 DataSec=TotSec-(GetShort(BPB_ResvdSecCnt)+GetByte(BPB_NumFATs)*FATSz+RootDirSectors);
 //���������� ���������� ��������� ��� ������ (������� ���������� � ������ 2! ��� ����������, � �� ����� ���������� ��������)
 CountofClusters=(unsigned long)floor(DataSec/GetByte(BPB_SecPerClus));
 //���������� ������ ������ ������
 FirstDataSector=GetShort(BPB_ResvdSecCnt)+(GetByte(BPB_NumFATs)*FATSz)+RootDirSectors;
 //��������� ��� �������� �������

 FATType=FAT12;
 WH1602_SetTextProgmemUpLine(Text_FAT_Type);
 if (CountofClusters<4085UL)
 {
  WH1602_SetTextProgmemDownLine(Text_FAT12);
  _delay_ms(5000);
  FATType=FAT12;
 }
 else
 {
  if (CountofClusters<65525UL)
  {
   WH1602_SetTextProgmemDownLine(Text_FAT16);  
   _delay_ms(2000);
   FATType=FAT16;
  }
  else
  {
   WH1602_SetTextProgmemDownLine(Text_FAT32);  
   _delay_ms(5000);
   FATType=FAT32;
  }
 }
 if (FATType==FAT12) return;//�� ������������
 //���������� ������ �������� ���������� (��� FAT16 - ��� ������ � ��������� �������, ��� FAT32 - ��� ���� � ������� ������ � ��������� BPB_RootClus)
 FirstRootDirSecNum=GetShort(BPB_ResvdSecCnt)+(GetByte(BPB_NumFATs)*FATSz);
 ClusterSize=GetByte(BPB_SecPerClus)*GetShort(BPB_BytsPerSec);//������ �������� � ������ 
}
//----------------------------------------------------------------------------------------------------
//������ ����� ����� � �������
//----------------------------------------------------------------------------------------------------
bool FAT_BeginFileSearch(void)
{
 //������ �������� ����������
 CurrentRootFileAddr=FirstRootDirSecNum*GetShort(BPB_BytsPerSec);
 EndRootFileAddr=CurrentRootFileAddr+RootDirSectors*GetShort(BPB_BytsPerSec);
 unsigned long FirstCluster;//������ ������� �����
 unsigned long Size;//������ �����
 //��������� � ������� ������� ��� �����
 while(1)
 {
  if (FAT_GetFileSearch(NULL,&FirstCluster,&Size)==false)
  {
   if (FAT_NextFileSearch()==false) return(false);
  } 
  else return(true);
 }
 return(false);
}
//----------------------------------------------------------------------------------------------------
//������� � ����������� ����� � ��������
//----------------------------------------------------------------------------------------------------
bool FAT_PrevFileSearch(void)
{
 while(1)
 {
  CurrentRootFileAddr-=32UL;//������������ �� ������ �����
  if (CurrentRootFileAddr<FirstRootDirSecNum*GetShort(BPB_BytsPerSec)) 
  {
   FAT_BeginFileSearch();//��������� � ������� �����
   return(false);//����� �� ������� ����������
  }
  //����������� ��� �����
  unsigned char n;
  bool res=true;
  for(n=0;n<11;n++)
  {
   unsigned char b=GetByte(CurrentRootFileAddr+(unsigned long)(n));
   if (n==0)
   {
    if (b==0x20 || b==0xE5)
	{
     res=false;
     break;	
	}
   }
   if (b<0x20 || (b>='a' && b<='z'))
   {
    res=false;
    break;
   }
  }
  //������� ����������
  if (res==true)
  {
   unsigned char a=GetByte(CurrentRootFileAddr+10UL);
   unsigned char b=GetByte(CurrentRootFileAddr+9UL);
   unsigned char c=GetByte(CurrentRootFileAddr+8UL);
   if (!(a=='P' && b=='A' && c=='T')) continue;//�������� ����������
   return(true);
  }
 }
 return(false);
}
//----------------------------------------------------------------------------------------------------
//������� � ���������� ����� � ��������
//----------------------------------------------------------------------------------------------------
bool FAT_NextFileSearch(void)
{
 unsigned long fa=CurrentRootFileAddr;
 while(1)
 {
  CurrentRootFileAddr+=32UL;//��������� � ��������� ������  
  if (CurrentRootFileAddr>=EndRootFileAddr)
  {
   CurrentRootFileAddr=fa;//������������ � ����������� �����
   return(false);//����� �� ������� ����������
  }
  unsigned char n;
  bool res=true;
  for(n=0;n<11;n++)
  {
   unsigned char b=GetByte(CurrentRootFileAddr+(unsigned long)(n));
   if (n==0)
   {
    if (b==0x20 || b==0xE5)
	{
     res=false;
     break;	
	}
   }
   if (b<0x20 || (b>='a' && b<='z'))
   {
    res=false;
    break;
   }
  }
  if (res==true)
  {
   unsigned char a=GetByte(CurrentRootFileAddr+10UL);
   unsigned char b=GetByte(CurrentRootFileAddr+9UL);
   unsigned char c=GetByte(CurrentRootFileAddr+8UL);
   if (!(a=='P' && b=='A' && c=='T')) continue;//�������� ����������
   return(true);
  }
 }
 return(false);
}
//----------------------------------------------------------------------------------------------------
//�������� ��������� �������� ���������� ����� � ��������
//----------------------------------------------------------------------------------------------------
bool FAT_GetFileSearch(char *filename,unsigned long *FirstCluster,unsigned long *Size)
{
 unsigned char n;
 bool res=true;
 for(n=0;n<11;n++)
 {
  unsigned char b=GetByte(CurrentRootFileAddr+(unsigned long)(n));
  if (n==0)
  {
   if (b==0x20 || b==0xE5)
   {
    res=false;
    break;	
   }
  }
  if (b<0x20 || (b>='a' && b<='z'))
  {
   res=false;
   break;
  }
  if (filename!=NULL)
  {
   if (n<8) filename[n]=b;
       else filename[n+1]=b;
  }
 }
 if (res==true)
 {
  unsigned char a=GetByte(CurrentRootFileAddr+10UL);
  unsigned char b=GetByte(CurrentRootFileAddr+9UL);
  unsigned char c=GetByte(CurrentRootFileAddr+8UL);
  if (!(a=='P' && b=='A' && c=='T')) return(false);//�������� ����������
  //������ ������� �����
  *FirstCluster=(GetShort(CurrentRootFileAddr+20UL)<<16)|GetShort(CurrentRootFileAddr+26UL);
  //����� ������ ����� � ������
  *Size=GetLong(CurrentRootFileAddr+28UL);
  if (filename!=NULL)
  {
   filename[8]='.';
   filename[12]=0;
   //������ ������� ��� �����
   unsigned long lcrfa=CurrentRootFileAddr;
   unsigned char long_name_length=0;
   unsigned long begin=FirstRootDirSecNum*GetShort(BPB_BytsPerSec);
   while(lcrfa>begin)
   {
    lcrfa-=32UL;
    unsigned char attr=GetByte(lcrfa+11UL);
    if (attr&0xf)//��� ������� ���
    {
     //�������� ������ ���
     unsigned char name_index=GetByte(lcrfa);
     for(n=0;n<10 && long_name_length<=16;n+=2,long_name_length++) filename[long_name_length]=GetByte(lcrfa+n+1UL);
     for(n=0;n<12 && long_name_length<=16;n+=2,long_name_length++) filename[long_name_length]=GetByte(lcrfa+n+14UL);
	 for(n=0;n<4 && long_name_length<=16;n+=2,long_name_length++) filename[long_name_length]=GetByte(lcrfa+n+28UL);
	 if (long_name_length>16) break;
     if (name_index&0x40) break;//��������� ����� �����
    }
    else break;//��� �� ������� ���
   }
   if (long_name_length>16) long_name_length=16;
   if (long_name_length>0) filename[long_name_length]=0;	
  }
  return(true);
 }
 return(false); 
}
//----------------------------------------------------------------------------------------------------
//�������� � ��� ���� �����
//----------------------------------------------------------------------------------------------------
bool FAT_WriteBlock(unsigned short *BlockSize,unsigned short Block)
{
 unsigned long CurrentCluster;
 unsigned long Size;
 unsigned long SecPerClus=GetByte(BPB_SecPerClus); 
 unsigned long BytsPerSec=GetShort(BPB_BytsPerSec);
 unsigned long ResvdSecCnt=GetShort(BPB_ResvdSecCnt);
 
 unsigned long i=0;//����� ������������ ����� �����
 unsigned short dram_addr=0;//����� � ������������ ������
 unsigned short current_block=0;//������� ����� �����
 unsigned short block_size=0;//������ �����
 *BlockSize=0;
 if (FAT_GetFileSearch(string,&CurrentCluster,&Size)==false) return(false);
 unsigned char mode=0;              
 while(i<Size)
 {
  DRAM_Refresh();//���������� ����������� ������ 
  //��������� ������
  unsigned long length=ClusterSize;
  if (length+i>=Size) length=Size-i;
  //�������� ������ ������ ��������
  unsigned long FirstSectorofCluster=((CurrentCluster-2UL)*SecPerClus)+FirstDataSector; 
  unsigned long addr=FirstSectorofCluster*BytsPerSec;
  for(unsigned long m=0;m<length;m++,i++)
  {
   DRAM_Refresh();//���������� ����������� ������
   unsigned char b=GetByte(addr+m);
   if (mode==0)//������ �������� ����� �����
   {
    block_size=b;
	mode=1;
	continue;
   }
   if (mode==1)//������ �������� ����� �����
   {
    block_size|=((unsigned short)b)<<8;
	mode=2;
	dram_addr=0;
	continue;
   }
   if (mode==2)//������ ������
   {
    if (current_block==Block) DRAM_WriteByte(dram_addr,b);//��� ��������� ����
	dram_addr++;
	if (dram_addr>=block_size)//���� ��������
	{
	 if (current_block==Block)//���������� ��������� ����
	 {
	  *BlockSize=block_size;  
	  return(true);
	 }
	 //��������� � ���������� �����
	 block_size=0;
	 current_block++;
	 mode=0;
	}
   } 
  }
  //��������� � ���������� �������� �����
  unsigned long FATOffset=0;//�������� �� ������� FAT � ������ (� FAT32 ��� 4-� �������, � � FAT16 - �����������)
  if (FATType==FAT16) FATOffset=CurrentCluster*2UL;
  unsigned long NextClusterAddr=ResvdSecCnt*BytsPerSec+FATOffset;//����� ���������� ��������
  //��������� ����� ���������� �������� �����
  unsigned long NextCluster=0;
  if (FATType==FAT16) NextCluster=GetShort(NextClusterAddr);
  if (NextCluster==0) break;//�������������� �������
  if (NextCluster>=CountofClusters+2UL) break;//����� ������ ����������� ���������� ������ �������� - ����� ����� ��� ����
  CurrentCluster=NextCluster;
 }
 //����� �����
 return(false);
}
//----------------------------------------------------------------------------------------------------
//������� ����
//----------------------------------------------------------------------------------------------------
unsigned long GetByte(unsigned long offset)
{
 offset+=FATOffset;
 unsigned long s=offset>>9UL;//����� �� 512
 if (s!=LastReadSector)
 {
  LastReadSector=s;
  SD_ReadBlock(offset&0xfffffe00UL,Sector);
  //������ �� ���������, �� ����� ������ ������� �� ������ - ���� ��������, ���� ���
 }
 return(Sector[offset&0x1FFUL]);
}
//----------------------------------------------------------------------------------------------------
//������� ��� �����
//----------------------------------------------------------------------------------------------------
unsigned long GetShort(unsigned long offset)
{
 unsigned long v=GetByte(offset+1UL);
 v<<=8UL;
 v|=GetByte(offset);
 return(v);
}
//----------------------------------------------------------------------------------------------------
//������� 4 �����
//----------------------------------------------------------------------------------------------------
unsigned long GetLong(unsigned long offset)
{
 unsigned long v=GetByte(offset+3UL);
 v<<=8UL;
 v|=GetByte(offset+2UL);
 v<<=8UL;
 v|=GetByte(offset+1UL);
 v<<=8UL;
 v|=GetByte(offset);
 return(v);
}