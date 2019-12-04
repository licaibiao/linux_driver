/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: gpt_struct.h
*BlogAddr: caibiao-lee.blog.csdn.net
*Description: GPT分别表和分区头信息结构体
*Date:     2019-10-12
*Author:   Caibiao Lee
*Version:  V1.0
*Others:
*History:
***********************************************************/
#ifndef __HST_PART_STRUCT_H__
#define __HST_PART_STRUCT_H__

#define  GPT_HEAD_RESERVED   420

#pragma pack(1)

/**MBR 主分区表**/
typedef struct Part_info
{
	unsigned char   u8PartSelfFlag;        /**引导指示符 **/
	unsigned char   u8PartStartHead;       /**开始磁头**/
	unsigned char   u8PartStartSec:6;      /**开始扇区**/
	unsigned char   u8PartStartCylLow:2;   /**开始柱面**/
	unsigned char   u8PartStartCylHig8;
	unsigned char   u8PartFlag;            /**系统ID**/
	unsigned char   u8PartStopHead;        /**结束磁头**/
	unsigned char   u8PartStopSec:6;       /**结束扇区**/  
	unsigned char   u8PartStopCylLow:2;    /**结束柱面**/
	unsigned char   u8PartStopCylHig8;
	unsigned int    u32StartSec;            /**相对扇区数*/
	unsigned int    u32SizeSec;             /**总扇区数**/
}PART_INFO_S;

/**主扇区引导**/
typedef struct MBR_info
{
	unsigned char u8arrBoot[446];   /**硬盘主引导记录**/
	PART_INFO_S   starrPartInfo[4]; /**MBR 分区表**/ 
	unsigned char s8ArrTag[2];      /**结束标志(有效标志) 55 AA**/
}MBR_INFO_S;

/**GPT 分区表头**/
typedef struct GPT_Head_Info
{
	unsigned char      u8arrSignature[8];       /**签名 固定为:EFI PART**/
	unsigned char      u8arrVersion[4];         /**版本号**/
	unsigned int       u32HeadLen;              /**GPT头的总长度**/
	unsigned int       u32GPTHeadCRC32;         /**GPT头CRC32校验和**/
	unsigned int       u32Reserved;             /**保留，必须是00**/
	unsigned long long u64GPTHeadPositSec;;     /**GPT头所在的扇区号**/
	unsigned long long u64GPTHeadBackupPositSec;/**GPT头备份所在的扇区**/
	unsigned long long u64GPTDataStartSec;      /**GPT分区区域开始扇区**/
	unsigned long long u64GPTDataEndSec;        /**GPT分区区域结束扇区**/  
	unsigned char      u8arrDiskGUID[16];       /**硬盘GUID**/ 
	unsigned long long u64GPTStartSec;          /**GPT分区表开始扇区号，一般是2**/
	unsigned int       u32MaxNumPartTable;      /**最多容纳分区表的数量,一般为128**/
	unsigned int       u32TableItemLen;         /**每个分区表项字节数，一般为128**/
	unsigned int       u32GPTCRC32;             /**分区表CRC校验和**/
	unsigned char      u8arrReserved[GPT_HEAD_RESERVED]; /**保留，一般填 00**/
}GPT_HEAD_INFO_S;

/**GPT 分区表**/
typedef struct GPT_Info
{
	unsigned char    u8arrPartType[16];   /**分区类型**/
	unsigned char    u8arrGUID[16];       /**分区GUID**/
	unsigned long long   u64PartStartSec; /**分区开始扇区**/
	unsigned long long   u64PartStopSec;  /**分区结束扇区**/
	unsigned long long   u64PartAttrFlag; /**分区标签**/
	char    s8arrPartName[72];            /**分区名字**/
}GPT_INFO_S;

#pragma pack()

#endif

