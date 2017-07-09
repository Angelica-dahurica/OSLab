#ifndef MANAGE_H
#define MANAGE_H

#include <stdio.h>

#pragma pack (1)	//设置对齐系数为1字节

//软盘的引导扇区即第0个扇区中的一个数据结构BPB
//共25字节
struct BPB {
	unsigned short BPB_BytsPerSec;	//offset=11 len=2 content=每扇区字节数
	unsigned char BPB_SecPerClus;	//offset=13 len=1 content=每簇扇区数
	unsigned short BPB_RsvdSecCnt;	//offset=14 len=2 content=Boot记录占用多少扇区
	unsigned char BPB_NumFATs;		//offset=16 len=1 content=共有多少FAT表
	unsigned short BPB_RootEntCnt;	//offset=17 len=2 content=根目录文件数最大值
	unsigned short BPB_TotSec16;	//offset=19 len=2 content=扇区总数
	unsigned char BPB_Media;		//offset=21 len=1 content=介质描述符
	unsigned short BPB_FATSz16;		//offset=22 len=2 content=每FAT扇区数
	unsigned short BPB_SecPerTrk;	//offset=24 len=2 content=每磁道扇区数
	unsigned short BPB_NumHeads;	//offset=26 len=2 content=磁头数（面数）
	unsigned int BPB_HiddSec;		//offset=28 len=4 content=隐藏扇区数
	unsigned int BPB_TotSec32;		//offset=32 len=4 content=如果BPB_TotSec16是0，由这个值记录扇区数
};

//根目录区（第19扇区开始）的目录条目
//共32字节
struct root_directory_entry {
	char DIR_Name[11];			//offset=0 len=0xB content=文件名8字节，扩展名3字节
	unsigned char DIR_Attr;		//offset=0xB len=1 content=文件属性
	char reserve[10];			//offset=0xC len=10 content=保留位
	unsigned short DIR_WrtTime;	//offset=0x16 len=2 content=最后一次写入时间
	unsigned short DIR_WrtDate;	//offset=0x18 len=2 content=最后一次写入日期
	unsigned short DIR_FstClus;	//offset=0x1A len=2 content=此条目对应的开始簇号
	unsigned int DIR_FileSize;	//offset=0x1C len=4 content=文件大小
};

#pragma pack ()		//取消自定义对齐

/*
**初始化BPB
*/
void init_BPB_content(struct BPB* bpb_pointer);

/*
**打印目录及文件名
*/
void print_file_name(FILE* file,int offset,struct root_directory_entry* rde_pointer,char* father_name);
void print_child_name(FILE* file,char* directory,int clus);

/*
**打印内容
*/
void print_content(FILE* file,int clus);

/*
**获取fat表项的值
*/
int get_fat_entry(FILE* file,int clus);

/*
**查找文件
*/
void find_file(FILE* file,int offset);

/*
**通过路径找文件
*/
void check_path(int floor,FILE* file,struct root_directory_entry* rde_pointer, int base);

/*
**获取扇区数
*/
int get_clus_byte(int clus);

/*
**打印树
*/
void print_tree(FILE* file,char* path,int offset);

/*
*打印最终结果
*/
void print_result(char (*result)[150]);

/*
**打印
*/
void my_print(int type,int len,char* content);

void print_result_child(char **result);

#endif