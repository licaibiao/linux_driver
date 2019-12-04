/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: GPT_DiskPart.cpp
*BlogAddr: caibiao-lee.blog.csdn.net
*Description:磁盘分区类成员的实现，其集中了磁盘分区的所有功能
*Date:     2019-10-12
*Author:   Caibiao Lee
*Version:  V1.0
*Others:
*History:
***********************************************************/
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/mount.h>  
#include <linux/fd.h> 
#include <linux/fs.h>
#include <linux/hdreg.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include "GPT_DiskPart.h"

#define SIGNATURE  "EFI PART"

const unsigned char s8ArrBootCode[] = {
0x33,0xC0,0x8E,0xD0,0xBC,0x00,0x7C,0x8E,0xC0,0x8E,0xD8,0xBE,\
0x00,0x7C,0xBF,0x00,0x06,0xB9,0x00,0x02,0xFC,0xF3,0xA4,0x50,0x68,0x1C,0x06,\
0xCB,0xFB,0xB9,0x04,0x00,0xBD,0xBE,0x07,0x80,0x7E,0x00,0x00,0x7C,0x0B,0x0F,\
0x85,0x0E,0x01,0x83,0xC5,0x10,0xE2,0xF1,0xCD,0x18,0x88,0x56,0x00,0x55,0xC6,\
0x46,0x11,0x05,0xC6,0x46,0x10,0x00,0xB4,0x41,0xBB,0xAA,0x55,0xCD,0x13,0x5D,\
0x72,0x0F,0x81,0xFB,0x55,0xAA,0x75,0x09,0xF7,0xC1,0x01,0x00,0x74,0x03,0xFE,\
0x46,0x10,0x66,0x60,0x80,0x7E,0x10,0x00,0x74,0x26,0x66,0x68,0x00,0x00,0x00,\
0x00,0x66,0xFF,0x76,0x08,0x68,0x00,0x00,0x68,0x00,0x7C,0x68,0x01,0x00,0x68,\
0x10,0x00,0xB4,0x42,0x8A,0x56,0x00,0x8B,0xF4,0xCD,0x13,0x9F,0x83,0xC4,0x10,\
0x9E,0xEB,0x14,0xB8,0x01,0x02,0xBB,0x00,0x7C,0x8A,0x56,0x00,0x8A,0x76,0x01,\
0x8A,0x4E,0x02,0x8A,0x6E,0x03,0xCD,0x13,0x66,0x61,0x73,0x1C,0xFE,0x4E,0x11,\
0x75,0x0C,0x80,0x7E,0x00,0x80,0x0F,0x84,0x8A,0x00,0xB2,0x80,0xEB,0x84,0x55,\
0x32,0xE4,0x8A,0x56,0x00,0xCD,0x13,0x5D,0xEB,0x9E,0x81,0x3E,0xFE,0x7D,0x55,\
0xAA,0x75,0x6E,0xFF,0x76,0x00,0xE8,0x8D,0x00,0x75,0x17,0xFA,0xB0,0xD1,0xE6,\
0x64,0xE8,0x83,0x00,0xB0,0xDF,0xE6,0x60,0xE8,0x7C,0x00,0xB0,0xFF,0xE6,0x64,\
0xE8,0x75,0x00,0xFB,0xB8,0x00,0xBB,0xCD,0x1A,0x66,0x23,0xC0,0x75,0x3B,0x66,\
0x81,0xFB,0x54,0x43,0x50,0x41,0x75,0x32,0x81,0xF9,0x02,0x01,0x72,0x2C,0x66,\
0x68,0x07,0xBB,0x00,0x00,0x66,0x68,0x00,0x02,0x00,0x00,0x66,0x68,0x08,0x00,\
0x00,0x00,0x66,0x53,0x66,0x53,0x66,0x55,0x66,0x68,0x00,0x00,0x00,0x00,0x66,\
0x68,0x00,0x7C,0x00,0x00,0x66,0x61,0x68,0x00,0x00,0x07,0xCD,0x1A,0x5A,0x32,\
0xF6,0xEA,0x00,0x7C,0x00,0x00,0xCD,0x18,0xA0,0xB7,0x07,0xEB,0x08,0xA0,0xB6,\
0x07,0xEB,0x03,0xA0,0xB5,0x07,0x32,0xE4,0x05,0x00,0x07,0x8B,0xF0,0xAC,0x3C,\
0x00,0x74,0x09,0xBB,0x07,0x00,0xB4,0x0E,0xCD,0x10,0xEB,0xF2,0xF4,0xEB,0xFD,\
0x2B,0xC9,0xE4,0x64,0xEB,0x00,0x24,0x02,0xE0,0xF8,0x24,0x02,0xC3,0x49,0x6E,\
0x76,0x61,0x6C,0x69,0x64,0x20,0x70,0x61,0x72,0x74,0x69,0x74,0x69,0x6F,0x6E,\
0x20,0x74,0x61,0x62,0x6C,0x65,0x00,0x45,0x72,0x72,0x6F,0x72,0x20,0x6C,0x6F,\
0x61,0x64,0x69,0x6E,0x67,0x20,0x6F,0x70,0x65,0x72,0x61,0x74,0x69,0x6E,0x67,\
0x20,0x73,0x79,0x73,0x74,0x65,0x6D,0x00,0x4D,0x69,0x73,0x73,0x69,0x6E,0x67,\
0x20,0x6F,0x70,0x65,0x72,0x61,0x74,0x69,0x6E,0x67,0x20,0x73,0x79,0x73,0x74,\
0x65,0x6D,0x00,0x00,0x00,0x63,0x7B,0x9A,0xD7,0x26,0x7E,0x7F,0x00,0x00
};




unsigned int const g_u8arrcrc32tab[] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
	0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
	0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
	0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
	0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
	0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
	0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
	0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
	0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
	0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
	0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
	0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
	0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
	0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
	0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
	0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
	0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
	0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
	0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
	0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
	0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
	0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
	0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
	0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
	0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
	0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
	0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
	0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
	0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
	0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
	0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
	0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
	0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
	0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
	0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
	0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
	0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
	0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
	0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
	0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
	0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
	0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
	0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
	0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
	0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
	0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
	0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
	0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
	0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
	0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
	0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
	0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL 
};

static unsigned int GPT_CRC32(unsigned char *pu8buf,unsigned int u32size)
{
	unsigned int i, l_u32crc;
    
	l_u32crc = 0xFFFFFFFF;

	for (i = 0; i < u32size; i++)
	{
		l_u32crc = g_u8arrcrc32tab[(l_u32crc ^ pu8buf[i]) & 0xff] ^ (l_u32crc >> 8);
	}
    
	return l_u32crc^0xFFFFFFFF;
}

GPTDiskPart::GPTDiskPart()
{
	m_u32SecPerTrac = SECTOR_BYTE_NUM;	
	m_u32HeadNum = DISK_HEAD_NUM;
	m_u32SecSize = SECTOR_BYTE_NUM;
	m_u64SecCount = 0;
}
GPTDiskPart::~GPTDiskPart()
{

}

GPTDiskPart *GPTDiskPart::GPTDiskPartInstance()
{
	static GPTDiskPart *l_clsGptDP = NULL;
	if(NULL == l_clsGptDP)
	{
		l_clsGptDP = new GPTDiskPart();
	}
	return l_clsGptDP;
}

int GPTDiskPart::GPTDP_Access(char *const ps8Path)
{
	if(NULL == ps8Path)
	{
		GPTP_LOG("[%s,%d] Para Point is NULL error\n",__FILE__,__LINE__);
		return PARA_ERROR;
	}
	return access(ps8Path,F_OK);
}

int GPTDiskPart::GPTDP_Open(char *const ps8Path,int s32Flag)
{
	if(NULL == ps8Path)
	{
		GPTP_LOG("[%s,%d] Para Point is NULL error\n",__FILE__,__LINE__);
		return PARA_ERROR;
	}
	return open(ps8Path,s32Flag);	
}

int GPTDiskPart::GPTDP_Close(int s32Fd)
{
	
	close(s32Fd);
	return 0;
}

int GPTDiskPart::GPTDP_Ioctl(int s32Fd,int s32Cmd,void *pArg)
{
	if(s32Fd < 0 || NULL == pArg)
	{
		GPTP_LOG("[%s,%d] Para Point is NULL error\n",__FILE__,__LINE__);
		return PARA_ERROR;
	}
   return  ioctl(s32Fd,s32Cmd,pArg); /*用法很简单*/
}

int GPTDiskPart::GPTDP_Select(int s32Fd,unsigned int u32TimeOut)
{
	struct timeval l_stTimeOut = {0};
	int        l_s32Ret = 0;
	fd_set         l_stReadFdSet;
	fd_set         l_stWriteFdSet;

	l_stTimeOut.tv_sec = u32TimeOut; /*超时3秒*/

	FD_ZERO(&l_stReadFdSet);
	FD_ZERO(&l_stWriteFdSet);
	FD_SET(s32Fd,&l_stReadFdSet);
	FD_SET(s32Fd,&l_stWriteFdSet);

	l_s32Ret = select(s32Fd + 1,&l_stReadFdSet,&l_stWriteFdSet,NULL,&l_stTimeOut);
	if(l_s32Ret <= 0)
	{
		GPTP_LOG("[%s,%d] select error\n",__FILE__,__LINE__);	
		return SELECT_ERROR;
	}
	if(FD_ISSET(s32Fd,&l_stReadFdSet) && FD_ISSET(s32Fd,&l_stWriteFdSet))
	{
		return 0;	
	}
	return SELECT_ERROR;
}

int GPTDiskPart::GPTDP_Read(int s32Fd,unsigned char  *pu8DateBuf,int s32Size)
{
	if(NULL == pu8DateBuf || s32Size < 0 || s32Fd < 0)
	{
		GPTP_LOG("[%s,%d] Para Point is NULL error\n",__FILE__,__LINE__);
		return PARA_ERROR;
	}
	return read(s32Fd,pu8DateBuf,s32Size);	
	
}

int GPTDiskPart::GPTDP_Write(int s32Fd,unsigned char *pu8DateBuf,int s32Size)
{
	if(NULL == pu8DateBuf || s32Size < 0 || s32Fd < 0)
	{
		GPTP_LOG("[%s,%d] Para Point is NULL error\n",__FILE__,__LINE__);
		return PARA_ERROR;
	}
	return write(s32Fd,pu8DateBuf,s32Size);	

}

long long GPTDiskPart::GPTDP_Lseek64(int s32Fd,long long s64Offset,int s32Whence)
{
	if(s32Fd < 0)
	{
		GPTP_LOG("[%s,%d] Para Point is NULL error\n",__FILE__,__LINE__);
		return PARA_ERROR;
	}
	GPTP_LOG("[%s,%d] ====lseek64==%lld=\n",__FILE__,__LINE__,s64Offset);
	return lseek64(s32Fd,s64Offset,s32Whence);	
}	


int GPTDiskPart::GPTDP_WriteMBRHandle(int s32Fd,MBR_INFO_S *pstMBRInfo)
{
	PART_INFO_S    l_starrPartInfo[4] = {0};
	int            l_s32Ret = 0;
	long long      l_s64Ret = 0;

	if(s32Fd < 0 || NULL == pstMBRInfo)
	{
		GPTP_LOG("[%s,%d] Para Point is NULL error\n",__FILE__,__LINE__);
		return PARA_ERROR;
	}
	
	l_starrPartInfo[0].u8PartSelfFlag = 0;
	l_starrPartInfo[0].u8PartStartHead = 0;
	l_starrPartInfo[0].u8PartStartSec = 2;
	l_starrPartInfo[0].u8PartStartCylLow = 0;
	l_starrPartInfo[0].u8PartStartCylHig8 = 0;
	l_starrPartInfo[0].u8PartFlag = 0xEE;
	l_starrPartInfo[0].u8PartStopHead = 0xFF;
	l_starrPartInfo[0].u8PartStopSec = 0x3F;
	l_starrPartInfo[0].u8PartStopCylLow = 0x3;
	l_starrPartInfo[0].u8PartStopCylHig8 = 0xFF;
	l_starrPartInfo[0].u32StartSec = GPT_HEAD_SECNO;
	l_starrPartInfo[0].u32SizeSec = 0xFFFFFFFF;

	bzero(pstMBRInfo,sizeof(MBR_INFO_S));
	pstMBRInfo->s8ArrTag[0] = 0x55;
	pstMBRInfo->s8ArrTag[1] = 0xAA;

    memcpy(pstMBRInfo->u8arrBoot ,s8ArrBootCode,sizeof(s8ArrBootCode));
	memcpy(pstMBRInfo->starrPartInfo,l_starrPartInfo,sizeof(PART_INFO_S) * 4);
	GPTP_LOG("[%s,%d] 开始偏移到0扇区写MBR\n",__FILE__,__LINE__);
	l_s32Ret = GPTDP_Lseek64(s32Fd,0,SEEK_SET);
	if(0 != l_s64Ret)
	{
		GPTP_LOG("[%s,%d] %d is lseek64 error\n",__FILE__,__LINE__,s32Fd);
		return LSEEK_ERROR;	
	}

	l_s32Ret = GPTDP_Write(s32Fd,(unsigned char *)pstMBRInfo,sizeof(MBR_INFO_S));
	if(sizeof(MBR_INFO_S) != l_s32Ret)
	{
		GPTP_LOG("[%s,%d] %d is read error\n",__FILE__,__LINE__,s32Fd);
		return WRITE_ERROR;	
	}
    
	return 0;
}

int GPTDiskPart::GPTDP_WriteGPTHead(int s32Fd,GPT_HEAD_INFO_S *pstGPTHeadInfo)
{
	unsigned int l_u32Len = 0;
	unsigned int l_u32Size = 0;
	int          l_s32Ret = 0;
 	long long    l_s64offset = 0;
 	long long    l_s64Ret = 0;
	unsigned char l_u8arrBuf[17] = {
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,\
        0x11,0x12,0x13,0x14,0x15,0x16};

	if(s32Fd < 0 || NULL == pstGPTHeadInfo)
	{
		GPTP_LOG("[%s,%d] Para Point is NULL error\n",__FILE__,__LINE__);
		return PARA_ERROR;
	}

	l_u32Size = sizeof(GPT_HEAD_INFO_S);

	bzero(pstGPTHeadInfo,l_u32Size); 


	/*GPT头签名*/
	memcpy(pstGPTHeadInfo->u8arrSignature,"EFI PART",8);
	
	/*版本号*/
	pstGPTHeadInfo->u8arrVersion[0] = 0x00;
	pstGPTHeadInfo->u8arrVersion[1] = 0x00;
	pstGPTHeadInfo->u8arrVersion[2] = 0x01;
	pstGPTHeadInfo->u8arrVersion[3] = 0x00;

	/*GPT头的总字节数*/
	pstGPTHeadInfo->u32HeadLen = sizeof(GPT_HEAD_INFO_S) - GPT_HEAD_RESERVED;

	/*GPT头的CRC32校验和*/
	pstGPTHeadInfo->u32GPTHeadCRC32 = 0;

	/*预留*/
	pstGPTHeadInfo->u32Reserved = 0;

	/*GPT头所在扇区号*/
	pstGPTHeadInfo->u64GPTHeadPositSec = GPT_HEAD_SECNO;

	/*GPT头备份所在的扇区号*/
	pstGPTHeadInfo->u64GPTHeadBackupPositSec = GPT_HEAD_BACKUP_SECNO(m_u64SecCount);

	/*分区区域开始的扇区号*/
	pstGPTHeadInfo->u64GPTDataStartSec = GPT_DATA_START_SECNO;

	/*GPT分区区域结束的扇区号*/
	pstGPTHeadInfo->u64GPTDataEndSec = GPT_DATA_END_SECNO(m_u64SecCount);
    
    /*这个结构体是关键*/
	 struct hd_driveid l_stHD;  

	 l_s32Ret = GPTDP_Ioctl(s32Fd,HDIO_GET_IDENTITY,&l_stHD);
	 if(l_s32Ret < 0) 
	 {
		perror("error:");
		GPTP_LOG("[%s,%d]ioctl error\n",__FILE__,__LINE__);
		memcpy(pstGPTHeadInfo->u8arrDiskGUID,l_u8arrBuf,16);
	 }
	 else
     {
		GPTP_LOG("Serial No = %s\n",l_stHD.serial_no);
	 	l_u32Len = sizeof(l_stHD.serial_no);
	 	l_u32Len = l_u32Len > 16 ? 16 : l_u32Len;
	 	memcpy(pstGPTHeadInfo->u8arrDiskGUID,l_stHD.serial_no,l_u32Len);
	 }


	/*GPT分区表开始扇区号*/
	pstGPTHeadInfo->u64GPTStartSec = GPT_TABLE_SECNO;

	/*最多容纳分区表的数量*/
	pstGPTHeadInfo->u32MaxNumPartTable = GPT_MAX_PART_NUM;

	/*每个分区表项的字节数*/
	pstGPTHeadInfo->u32TableItemLen  = GPT_PART_ITEMS;

	/*分区表CRC32校验和*/
	pstGPTHeadInfo->u32GPTCRC32 = 0;


    /**写扇区头**/
	l_s64offset = GPT_HEAD_SECNO;
	l_s64offset *= m_u32SecSize;
	GPTP_LOG("[%s,%d] 开始偏移到扇区:%llu写GPT head\n",__FILE__,__LINE__,pstGPTHeadInfo->u64GPTHeadPositSec);
	l_s64Ret = GPTDP_Lseek64(s32Fd,l_s64offset,SEEK_SET);
	if(l_s64Ret != l_s64offset)
	{
		GPTP_LOG("[%s,%d] %d is lseek64 error\n",__FILE__,__LINE__,s32Fd);
		return LSEEK_ERROR;	
	}

	l_u32Size = sizeof(GPT_HEAD_INFO_S);
	l_s32Ret = GPTDP_Write(s32Fd,(unsigned char *)pstGPTHeadInfo,l_u32Size);
	if(l_u32Size != (unsigned int)l_s32Ret)
	{
		GPTP_LOG("[%s,%d] %d is write error\n",__FILE__,__LINE__,s32Fd);
		return WRITE_ERROR;	
	}

    /**写备份扇区头**/
	l_s64offset = GPT_HEAD_BACKUP_SECNO(m_u64SecCount);
	l_s64offset *= m_u32SecSize;
	GPTP_LOG("[%s,%d] 开始偏移到扇区:%llu写备份GPT head\n",__FILE__,__LINE__,pstGPTHeadInfo->u64GPTHeadPositSec);
	l_s64Ret = GPTDP_Lseek64(s32Fd,l_s64offset,SEEK_SET);
	if(l_s64Ret != l_s64offset)
	{
		GPTP_LOG("[%s,%d] %d is lseek64 error\n",__FILE__,__LINE__,s32Fd);
		return LSEEK_ERROR;	
	}

	l_u32Size = sizeof(GPT_HEAD_INFO_S);
	l_s32Ret = GPTDP_Write(s32Fd,(unsigned char *)pstGPTHeadInfo,l_u32Size);
	if(l_u32Size != (unsigned int)l_s32Ret)
	{
		GPTP_LOG("[%s,%d] %d is write error\n",__FILE__,__LINE__,s32Fd);
		return WRITE_ERROR;	
	}
    
	return 0;
}

int GPTDiskPart::GPTDP_WritePartTable(int s32Fd,unsigned char u8PartCount,GPT_HEAD_INFO_S *pstGPTHeadInfo)
{
    unsigned int        l_u32Size = 0;
    unsigned char       l_u8PartIndex = 0;
    unsigned int        l_u32PartNum  =0;
    unsigned int        l_u32SecperCly = 0;
    int                 l_s32Ret = 0;
    long long           l_s64Ret = 0;
    long long           l_s64offset = 0;
    unsigned long long  l_u64DataSec = 0;
    unsigned long long  l_u64EachDataPartSec = 0;
    unsigned long long  l_u64StartSec = 0;
    unsigned long long  l_u64EndSec = 0;
    unsigned long long  l_u64Stmp = 0;
    GPT_INFO_S	        l_starrGPTInfo[GPT_MAX_PART_NUM] = {0};
    
    unsigned char  l_u8arrMainPartGUID[17] = {0xA2,0xA0,0xD0,0xEB,0xE5,0xB9,0x33,0x44,0x87,0xC0,0x68,0xB6,0xB7,0x26,0x99,0xC7};


	if((s32Fd < 0)||(0==m_u64SecCount)||(0==u8PartCount))
	{
		GPTP_LOG("[%s,%d] input para error\n",__FILE__,__LINE__);
		return PARA_ERROR;
	}
    
	if(0 == m_u32SecPerTrac || 0 == m_u32HeadNum || 0 == m_u32SecSize)
	{
		m_u32SecPerTrac = SECTOR_BYTE_NUM;	
		m_u32HeadNum = DISK_HEAD_NUM;
		m_u32SecSize = SECTOR_BYTE_NUM;
	}

	l_s64offset = GPT_TABLE_SECNO;
	l_s64offset *= m_u32SecSize;
    
	GPTP_LOG("[%s,%d] 开始偏移到扇区:%llu写GPT 表头\n",__FILE__,__LINE__,l_s64offset);
	l_s64Ret = GPTDP_Lseek64(s32Fd,l_s64offset,SEEK_SET);
	if(l_s64Ret != l_s64offset)
	{
		GPTP_LOG("[%s,%d] %d is lseek64 error\n",__FILE__,__LINE__,s32Fd);
		return LSEEK_ERROR;	
	}

    
    l_u64DataSec = GPT_HEAD_BACKUP_SECNO(m_u64SecCount) - GPT_DATA_START_SECNO;
    l_u64EachDataPartSec = l_u64DataSec/u8PartCount;

    /**扇区对齐**/
    l_u32SecperCly  = m_u32SecPerTrac * m_u32HeadNum; 
    if(0!=(l_u64EachDataPartSec%l_u32SecperCly))
    {
        l_u64EachDataPartSec = l_u64EachDataPartSec - (l_u64EachDataPartSec%l_u32SecperCly);
    }

    l_u64StartSec = GPT_DATA_START_SECNO;
    l_u64EndSec   = l_u64StartSec + l_u64EachDataPartSec;
    
    for(int i=0;i<u8PartCount;i++)
    {
        if(l_u64EndSec>=GPT_TABLE_BACKUP_SECNO(m_u64SecCount))
        {
            break;
        }
            
        /**分区类型GUID,使用微软公司的主分区类型**/
        memcpy(l_starrGPTInfo[i].u8arrPartType,l_u8arrMainPartGUID,16);

        /**分区GUID**/
        for(int j=0;j<16;j++)
        {
            l_starrGPTInfo[i].u8arrGUID[j] = (rand()/255) & 0xFF;
        }

        /**开始扇区号**/
        l_starrGPTInfo[i].u64PartStartSec = l_u64StartSec;

        /**结束扇区号，每个分区预留一些空闲空间**/
        l_starrGPTInfo[i].u64PartStopSec = l_u64EndSec - l_u32SecperCly;

        /**扇区属性**/
        l_starrGPTInfo[i].u64PartAttrFlag = 0;

        /**扇区名字**/
        snprintf(l_starrGPTInfo[i].s8arrPartName,sizeof(l_starrGPTInfo[i]),"Disk_Part_%d\n",i);

        l_u64StartSec = l_u64EndSec + 1;
        l_u64EndSec   = l_u64EndSec + l_u64EachDataPartSec;
    }

    /**写分区表**/
    l_s64offset = GPT_TABLE_SECNO;
    GPTP_LOG("[%s,%d] s64offset:%lld\n",__FILE__,__LINE__,l_s64offset);
    l_s64offset *= m_u32SecSize;

	GPTP_LOG("[%s,%d] 开始偏移到扇区开始位置:%lld,%lld写GPT Table\n",__FILE__,__LINE__,l_s64offset/m_u32SecSize,l_s64offset);
	l_s64Ret = GPTDP_Lseek64(s32Fd,l_s64offset,SEEK_SET);
	if(l_s64offset != l_s64Ret)
	{
		GPTP_LOG("[%s,%d] lseek64 offset:%lld error\n",__FILE__,__LINE__,l_s64offset);
		return LSEEK_ERROR;	
	}

	l_s32Ret = GPTDP_Write(s32Fd,(unsigned char *)l_starrGPTInfo,sizeof(l_starrGPTInfo));
	if(sizeof(l_starrGPTInfo) != (unsigned int)l_s32Ret)
	{
		GPTP_LOG("[%s,%d] %d is read error\n",__FILE__,__LINE__,s32Fd);
		return WRITE_ERROR;	
	}

    /*写备份分区表*/
    l_s64offset = GPT_TABLE_BACKUP_SECNO(m_u64SecCount);
    GPTP_LOG("[%s,%d] s64offset:%lld\n",__FILE__,__LINE__,l_s64offset);
    l_s64offset *= m_u32SecSize;

    GPTP_LOG("[%s,%d] 开始偏移到备份扇区开始位置:%lld,%lld写GPT Table\n",__FILE__,__LINE__,l_s64offset/m_u32SecSize,l_s64offset);
    l_s64Ret = GPTDP_Lseek64(s32Fd,l_s64offset,SEEK_SET);
    if(l_s64offset != l_s64Ret)
    {
        GPTP_LOG("[%s,%d] lseek64 offset:%lld error\n",__FILE__,__LINE__,l_s64offset);
        return LSEEK_ERROR;	
    }

    l_s32Ret = GPTDP_Write(s32Fd,(unsigned char *)l_starrGPTInfo,sizeof(l_starrGPTInfo));
    if(sizeof(l_starrGPTInfo) != (unsigned int)l_s32Ret)
    {
        GPTP_LOG("[%s,%d] %d is read error\n",__FILE__,__LINE__,s32Fd);
        return WRITE_ERROR;	
    }


    /**更新GPT头中的CRC校验信息**/
    l_s64offset =GPT_HEAD_SECNO ;
    GPTP_LOG("[%s,%d] s64offset:%lld\n",__FILE__,__LINE__,l_s64offset);
    l_s64offset *= m_u32SecSize;

    GPTP_LOG("[%s,%d] 开始偏移到GPT头开始位置:%lld,%lld写GPT Table\n",__FILE__,__LINE__,l_s64offset/m_u32SecSize,l_s64offset);
    l_s64Ret = GPTDP_Lseek64(s32Fd,l_s64offset,SEEK_SET);
    if(l_s64offset != l_s64Ret)
    {
        GPTP_LOG("[%s,%d] lseek64 offset:%lld error\n",__FILE__,__LINE__,l_s64offset);
        return LSEEK_ERROR;	
    }

    pstGPTHeadInfo->u32GPTCRC32 = GPT_CRC32((unsigned char *)l_starrGPTInfo,sizeof(l_starrGPTInfo));
    pstGPTHeadInfo->u32GPTHeadCRC32 = GPT_CRC32((unsigned char *)pstGPTHeadInfo,pstGPTHeadInfo->u32HeadLen);

    l_s32Ret = GPTDP_Write(s32Fd,(unsigned char *)pstGPTHeadInfo,sizeof(GPT_HEAD_INFO_S));
    if(sizeof(GPT_HEAD_INFO_S) != (unsigned int)l_s32Ret)
    {
        GPTP_LOG("[%s,%d] %d is read error\n",__FILE__,__LINE__,s32Fd);
        return WRITE_ERROR;	
    }

    /**更新备份GPT头中的CRC校验信息**/
    l_s64offset =GPT_HEAD_BACKUP_SECNO(m_u64SecCount) ;
    GPTP_LOG("[%s,%d] s64offset:%lld\n",__FILE__,__LINE__,l_s64offset);
    l_s64offset *= m_u32SecSize;

    GPTP_LOG("[%s,%d] 开始偏移到备份GPT头开始位置:%lld,%lld写GPT Table\n",__FILE__,__LINE__,l_s64offset/m_u32SecSize,l_s64offset);
    l_s64Ret = GPTDP_Lseek64(s32Fd,l_s64offset,SEEK_SET);
    if(l_s64offset != l_s64Ret)
    {
        GPTP_LOG("[%s,%d] lseek64 offset:%lld error\n",__FILE__,__LINE__,l_s64offset);
        return LSEEK_ERROR;	
    }

    /**注意备份分区头的位置信息与正常分区头的位置参数不同**/
	l_u64Stmp = pstGPTHeadInfo->u64GPTHeadBackupPositSec;
	pstGPTHeadInfo->u64GPTHeadBackupPositSec = pstGPTHeadInfo->u64GPTHeadPositSec;
	pstGPTHeadInfo->u64GPTHeadPositSec       = l_u64Stmp;
	pstGPTHeadInfo->u64GPTStartSec           = GPT_TABLE_BACKUP_SECNO(m_u64SecCount);

    /**注意GPT头CRC校验不包括它自己，所以应该将它赋值为0后再计算CRC值**/
    pstGPTHeadInfo->u32GPTHeadCRC32 = 0;
    pstGPTHeadInfo->u32GPTHeadCRC32          = GPT_CRC32((unsigned char *)pstGPTHeadInfo,pstGPTHeadInfo->u32HeadLen);
    
    l_s32Ret = GPTDP_Write(s32Fd,(unsigned char *)pstGPTHeadInfo,sizeof(GPT_HEAD_INFO_S));
    if(sizeof(GPT_HEAD_INFO_S) != (unsigned int)l_s32Ret)
    {
        GPTP_LOG("[%s,%d] write error l_s32Ret = %d , len = %d \n",__FILE__,__LINE__,l_s32Ret,sizeof(GPT_HEAD_INFO_S));
        return WRITE_ERROR;	
    }

    return 0;

}

int GPTDiskPart::GPTDP_CreateGPTDiskPart(char *const ps8Path, unsigned char u8PartCount)
{
	int             l_s32Ret = 0;	
	int             l_s32Fd = 0;
	MBR_INFO_S      l_stMBRInfo = {0};
	GPT_HEAD_INFO_S l_stGPTHeadInfo = {0};
	GPT_INFO_S      l_starrGPTInfo[GPT_PART_ITEMS] = {0};
	
	if(NULL == ps8Path)
	{
		GPTP_LOG("[%s,%d] Para Point is NULL error\n",__FILE__,__LINE__);
		return PARA_ERROR;
	}
    
	if(0 != GPTDP_Access(ps8Path))
	{
		GPTP_LOG("[%s,%d] Dev noexit  error\n",__FILE__,__LINE__);
		return FILE_NO_ERROR;
	}
    
	GPTP_LOG("[%s,%d] 创建磁盘%s\n",__FILE__,__LINE__,ps8Path);
	l_s32Fd = GPTDP_Open(ps8Path,O_RDWR);
	if(l_s32Fd < 0)
	{
		GPTP_LOG("[%s,%d]  open  error\n",__FILE__,__LINE__);
		return FILE_OP_ERROR;	
	}

	/**这里获取的是磁盘的大小，单位为byte**/
	l_s32Ret = GPTDP_Ioctl(l_s32Fd,BLKGETSIZE64,&m_u64SecCount); 
	if(-1 == l_s32Ret)
	{
		GPTP_LOG("[%s,%d],l_s32Ret=%d\n",__FILE__,__LINE__,l_s32Ret);
		perror("error");
		close(l_s32Fd);
		return IOCTL_ERROR;
	}
    
	if(0 != m_u32SecSize)
	{
		m_u64SecCount = m_u64SecCount/m_u32SecSize;
	}

	m_u32SecPerTrac = SECTORS_PER_TRACK;	
	m_u32HeadNum    = DISK_HEAD_NUM;
	m_u32SecSize    = SECTOR_BYTE_NUM;

    GPTP_LOG("[%s,%d] %u,%u,%u,\n",__FILE__,__LINE__,m_u32HeadNum,m_u32SecPerTrac,m_u32SecSize);
    GPTP_LOG("[%s,%d] 磁盘总扇区数:%llu\n",__FILE__,__LINE__,m_u64SecCount);
    l_s32Ret = GPTDP_WriteMBRHandle(l_s32Fd,&l_stMBRInfo);
    if(0 != l_s32Ret)
    {
        GPTP_LOG("[%s,%d]  MBR Write error\n",__FILE__,__LINE__);
        close(l_s32Fd);
        return l_s32Ret;	
    }
    
    l_s32Ret = GPTDP_WriteGPTHead(l_s32Fd,&l_stGPTHeadInfo);
    if(0 != l_s32Ret)
    {
        GPTP_LOG("[%s,%d]  GPT Head Handle error\n",__FILE__,__LINE__);
        close(l_s32Fd);
        return l_s32Ret;	
    }
    
	GPTP_LOG("[%s,%d] %llu\n",__FILE__,__LINE__,l_stGPTHeadInfo.u64GPTHeadBackupPositSec);
	l_s32Ret = GPTDP_WritePartTable(l_s32Fd,u8PartCount,&l_stGPTHeadInfo);
	if(0 != l_s32Ret)
	{
		GPTP_LOG("[%s,%d]  GPT Table Handle error\n",__FILE__,__LINE__);
        
        close(l_s32Fd);
		return l_s32Ret;	
    }

    if(l_s32Fd>0)
    {
        close(l_s32Fd);
        l_s32Fd = -1;
    }
}


