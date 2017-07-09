#include <stdio.h>
#include "manage.h"

unsigned short BPB_BytsPerSec;
unsigned char BPB_SecPerClus;
unsigned short BPB_RsvdSecCnt;
unsigned char BPB_NumFATs;
unsigned short BPB_RootEntCnt;
unsigned short BPB_TotSec16;
unsigned char BPB_Media;
unsigned short BPB_FATSz16;
unsigned short BPB_SecPerTrk;
unsigned short BPB_NumHeads;
unsigned int BPB_HiddSec;
unsigned int BPB_TotSec32;

void my_print(int type,int len,char* content);

int main() {
	FILE* fat12;
	fat12 = fopen("a.img","rb");	//打开二进制文件

	struct BPB bpb;
	struct BPB* bpb_pointer = &bpb;

	fseek(fat12,11,SEEK_SET);	//移动文件流至fat12中BPB开始的位置
	fread(bpb_pointer,1,25,fat12);		//从文件流中读取数据，每次读1个字节，读25次

	init_BPB_content(bpb_pointer);

	struct root_directory_entry rde;
	struct root_directory_entry* rde_pointer = &rde;

	print_file_name(fat12,(BPB_FATSz16*BPB_NumFATs+BPB_RsvdSecCnt)*BPB_BytsPerSec,rde_pointer,"");	//打印文件名
	find_file(fat12,(BPB_FATSz16*BPB_NumFATs+BPB_RsvdSecCnt)*BPB_BytsPerSec);	//根据输入的文件路径查找文件

	fclose(fat12);
}
