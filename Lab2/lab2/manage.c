#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include "manage.h"

extern unsigned short BPB_BytsPerSec;
extern unsigned char BPB_SecPerClus;
extern unsigned short BPB_RsvdSecCnt;
extern unsigned char BPB_NumFATs;
extern unsigned short BPB_RootEntCnt;
extern unsigned short BPB_TotSec16;
extern unsigned char BPB_Media;
extern unsigned short BPB_FATSz16;
extern unsigned short BPB_SecPerTrk;
extern unsigned short BPB_NumHeads;
extern unsigned int BPB_HiddSec;
extern unsigned int BPB_TotSec32;

int path_len = 0;
int tag = 0;
int tag2 = 0;
int res_len = 0;
int directories;
int files;
char path_arr[50][150];
char **result;
char ***tang;

void init_BPB_content(struct BPB* bpb_pointer) {
	BPB_BytsPerSec = bpb_pointer->BPB_BytsPerSec;
	BPB_SecPerClus = bpb_pointer->BPB_SecPerClus;
	BPB_RsvdSecCnt = bpb_pointer->BPB_RsvdSecCnt;
	BPB_NumFATs = bpb_pointer->BPB_NumFATs;
	BPB_RootEntCnt = bpb_pointer->BPB_RootEntCnt;
	BPB_TotSec16 = bpb_pointer->BPB_TotSec16;
	BPB_Media = bpb_pointer->BPB_Media;
	BPB_FATSz16 = bpb_pointer->BPB_FATSz16;
	BPB_SecPerTrk = bpb_pointer->BPB_SecPerTrk;
	BPB_NumHeads = bpb_pointer->BPB_NumHeads;
	BPB_HiddSec = bpb_pointer->BPB_HiddSec;
	BPB_TotSec32 = bpb_pointer->BPB_TotSec32;

	BPB_TotSec16 == 0 ? (BPB_TotSec16 = BPB_TotSec32) : (BPB_TotSec16 = BPB_TotSec16);
}

void print_file_name(FILE* file,int offset,struct root_directory_entry* rde_pointer,char* father_name) {
	char file_name[150];

	//依次处理根目录中的各个条目
	for (int i = 0; i < BPB_BytsPerSec/32; i++) {
		fseek(file,offset,SEEK_SET);	//指向根目录区的开端
		fread(rde_pointer,1,32,file);	//读取一个条目到根目录条目
		offset += 32;	//指向下一条目
		
		//文件夹下没有文件，打印父目录
		if (rde_pointer->DIR_Name[0] == '\0'){
			if(i == 2){
				int len = strlen(father_name);	//长度中不包含'\0'
				char tp[150] = "";
				strncat(tp,father_name,len-1);	//tp最后的'\0'覆盖掉，字符追加完成后，再追加'\0'；-1是减去'/'
				int index = 0;
				char father[strlen(tp)+1];
				for(index = 0; index < strlen(tp); index++){
					father[index]=tolower(tp[index]);
				}
				father[index] = '\0';

				if(tag == 1) {
					strcpy(result[res_len],father);
					res_len++;
				} else {
					my_print(0,strlen(father),father);
					my_print(0,1,"\n");
				}
				break;
			}else{
				continue;
			}
		}	

		//文件名不是字母或数字、空格,不打印
		int boolean = 0;
		for(int j = 0; j < 11; j++) {
			if (!(((rde_pointer->DIR_Name[j] >= 48)&&(rde_pointer->DIR_Name[j] <= 57)) ||
				((rde_pointer->DIR_Name[j] >= 65)&&(rde_pointer->DIR_Name[j] <= 90)) ||
					((rde_pointer->DIR_Name[j] >= 97)&&(rde_pointer->DIR_Name[j] <= 122)) ||
						(rde_pointer->DIR_Name[j] == ' '))) {
				boolean = 1;
				break;
			}
		}
		if (boolean == 1) continue;

		if ((rde_pointer->DIR_Attr) == 0x20 ) {
			//0x20表示这是一个文件
			int pt = 0;
			
			//文件名8字节，不足8位，以空格补齐，扩展名3字节
			for(int k = 0; k < 8; k++){
				if (rde_pointer->DIR_Name[k] == ' ')	break;
				file_name[pt] = rde_pointer->DIR_Name[k];
				pt++;
			}

			file_name[pt] = '.';
			pt++;
			
			for(int k = 8; k < 11; k++){
				file_name[pt] = rde_pointer->DIR_Name[k];
				pt++;
			}
			
			file_name[pt] = '\0';	//到此为止，把文件名提取出来放到了file_name里

			//打印父目录名
			char low_father_name[strlen(father_name)+1];
			int m;
			for(m = 0; m < strlen(father_name); m++){
				low_father_name[m]=tolower(father_name[m]);
			}
			low_father_name[m] = '\0';
			if(tag == 0) my_print(0,strlen(low_father_name),low_father_name);
			
			//打印文件名
			char low_file_name[strlen(file_name)+1];
			for(m = 0; m < strlen(file_name); m++){
				low_file_name[m] = tolower(file_name[m]);
			}
			low_file_name[m] = '\0';
			if(tag == 0) my_print(1,strlen(low_file_name),low_file_name);
			if(tag == 0) my_print(1,1,"\n");

			if(tag == 1 && strcmp(low_father_name,"\0")!=0 && strcmp(low_file_name,"\0")!=0) {
				strcpy(result[res_len],low_father_name);
				strcat(result[res_len],low_file_name);
				res_len++;
			}

		} else {
			//0x10表示这是一个文件目录
			int pt = 0;
			for (int k = 0; k < 11; k++) {
				if (rde_pointer->DIR_Name[k] == ' ')	break;
				file_name[pt] = rde_pointer->DIR_Name[k];
				pt++;
			}
			file_name[pt] = '\0';
			//到此为止，把目录名提取出来放到了file_name
			
			char new_father_name[150];
			stpcpy(new_father_name,father_name);
			strcat(new_father_name,file_name);
			char* help="/";
			strcat(new_father_name,help);
			
			//打印目录及子文件
			print_child_name(file,new_father_name,rde_pointer->DIR_FstClus);
		}
	}
}

void print_child_name(FILE* file,char* directory,int clus) {
	int offset = get_clus_byte(clus);
	struct root_directory_entry rde;
	struct root_directory_entry* rde_pointer = &rde;
	print_file_name(file,offset,rde_pointer,directory);
}

void print_content(FILE* file,int clus) {
	int offset = get_clus_byte(clus);
	char* content = (char*)malloc(512);
	fseek(file,offset,SEEK_SET);
	fread(content,1,512,file);
	my_print(1,strlen(content),content);
}

int get_fat_entry(FILE* file,int clus) {
	int fatPos = BPB_RsvdSecCnt*BPB_BytsPerSec+clus*3/2;	//FAT表中每个簇号占用12个位，也就是1.5个字节，所以把簇号扩大1.5倍
	
	unsigned short content;
	unsigned short* content_pointer = &content;
	fseek(file,fatPos,SEEK_SET);
	fread(content_pointer,1,2,file);	//读两个字节

	int type;
	clus%2 == 0 ? type = 0 : (type = 1);

	//存储为小尾顺序
	//如果是偶数，直接读取;如果是奇数，右移4位，不要的那0.5个字节就移出去了
	if(type == 0)
		return content&0x0fff;
	else
		return (content>>4)&0x0fff;
}

void find_file(FILE* file,int offset) {
	while(1) {
		char* input_prompt = "Input command (Enter q to exit):\n";
		my_print(2,strlen(input_prompt),input_prompt);

		char path[150] = "";
		scanf("%s",path);
		if(strcmp(path,"q") == 0)	break;

		if(strcmp(path,"count") == 0) {
			scanf("%s",path);
			print_tree(file,path,(BPB_FATSz16*BPB_NumFATs+BPB_RsvdSecCnt)*BPB_BytsPerSec);
		} else {
			char* delim = "/";
			char* res = strtok(path,delim);

			while(res != NULL) {
				strcpy(path_arr[path_len],res);
				for(int i = 0; i < strlen(path_arr[path_len]); i++){
					path_arr[path_len][i] = toupper(path_arr[path_len][i]);
				}
				path_len++;
				res = strtok(NULL, delim);
			}

			struct root_directory_entry rde;
			struct root_directory_entry* rde_pointer = &rde;
			check_path(0,file,rde_pointer, offset);
			path_len = 0;
		}
	}
}

void check_path(int floor,FILE* file,struct root_directory_entry* rde_pointer, int offset) {
	char file_name[150];

	//依次处理根目录中的各个条目
	int hasPrint = 0;
	for (int i = 0; i < BPB_BytsPerSec/32; i++) {
		fseek(file,offset,SEEK_SET);
		fread(rde_pointer,1,32,file);	//读取一个条目到根目录条目
		offset += 32;	//offset指向下一个条目

		if (rde_pointer->DIR_Name[0] == '\0'){
			continue;
		}

		//文件名不是字母或数字、空格,不输出
		int boolean = 0;
		for (int j = 0; j < 11; j++) {
			if (!(((rde_pointer->DIR_Name[j] >= 48)&&(rde_pointer->DIR_Name[j] <= 57)) ||
				((rde_pointer->DIR_Name[j] >= 65)&&(rde_pointer->DIR_Name[j] <= 90)) ||
					((rde_pointer->DIR_Name[j] >= 97)&&(rde_pointer->DIR_Name[j] <= 122)) ||
						(rde_pointer->DIR_Name[j] == ' '))) {
				boolean = 1;
				break;
			}
		}
		if (boolean == 1) continue;

		int k;
		if ((rde_pointer->DIR_Attr) == 0x20 ) {
			//0x20表示这是一个文件
			int pt = 0;
			
			//文件名8字节，不足8位，以空格补齐，扩展名3字节
			for(int k = 0; k < 8; k++){
				if (rde_pointer->DIR_Name[k] == ' ')	break;
				file_name[pt] = rde_pointer->DIR_Name[k];
				pt++;
			}

			file_name[pt] = '.';
			pt++;
			
			for(int k = 8; k < 11; k++){
				file_name[pt] = rde_pointer->DIR_Name[k];
				pt++;
			}
			
			file_name[pt] = '\0';	//到此为止，把文件名提取出来放到了file_name里

			//检验当前读出的文件是否是要输出的文件
			int value = get_fat_entry(file,rde_pointer->DIR_FstClus);
			if(strcmp(path_arr[floor],file_name) == 0 && tag == 0){
				//打印文件内容
				int clus = rde_pointer->DIR_FstClus;
				if(value >= 0xFF8){		//值大于或等于0xFF8，表示当前簇已经是本文件的最后一个簇
					print_content(file,clus);
				}else{
					while(value < 0xFF8){
						if (value == 0xFF7) {
							my_print(2,strlen("坏簇\n"),"坏簇\n");
							break;
						}
						print_content(file,clus);
						clus = value;
						value = get_fat_entry(file,clus);
					}
					print_content(file,clus);
				}
				hasPrint = 1;
			} else if(strcmp(path_arr[floor],file_name) == 0 && tag == 1){
				hasPrint = 1;
			}
		} else {
			//0x10表示这是一个文件目录
			int pt = 0;
			for (int k = 0; k < 11; k++) {
				if (rde_pointer->DIR_Name[k] == ' ')	break;
				file_name[pt] = rde_pointer->DIR_Name[k];
				pt++;
			}
			file_name[pt] = '\0';
			//到此为止，把目录名提取出来放到了file_name
			
			if(strcmp(path_arr[floor],file_name) == 0){
				int base = get_clus_byte(rde_pointer->DIR_FstClus);

				if(floor == (path_len-1)){
					struct root_directory_entry rde;
					struct root_directory_entry* rde_pointer = &rde;
				
					char father_name[150] = "";
					char* help="/";
					for(int i = 0; i <= floor; i++){
						strcat(father_name,path_arr[i]);
						strcat(father_name,help);
					}
					
					print_file_name(file,base,rde_pointer,father_name);
				} else {
					struct root_directory_entry rde;
					struct root_directory_entry* rde_pointer = &rde;
					
					check_path(++floor,file,rde_pointer,base);
				}
				
				hasPrint = 1;
			}
		}
	}
			
	if(floor==(path_len-1) && hasPrint==0) {
		char prompt[] = "unknown path\n";
		my_print(2,strlen(prompt),prompt);
	}
}

int get_clus_byte(int clus) {
	//BPB_RsvdSecCnt 引导扇区
	//BPB_FATSz16*BPB_NumFATs FAT表扇区
	//(BPB_RootEntCnt*32+BPB_BytsPerSec-1)/BPB_BytsPerSec 根目录扇区
	//(clus-2)*BPB_SecPerClus 数据区开始簇号为2
	return (BPB_RsvdSecCnt+BPB_FATSz16*BPB_NumFATs+(BPB_RootEntCnt*32+BPB_BytsPerSec-1)/BPB_BytsPerSec+(clus-2)*BPB_SecPerClus)*BPB_BytsPerSec;
}

void print_tree(FILE* file,char* path,int offset) {
	tag = 1;

	// 先分配50个指针单元，注意是指针单元
	// 所以每个单元的大小是sizeof(char *)
	result = (char **) malloc(50 * sizeof(char * ));
	// 再分配150个字符单元
	// 上面的50个指针单元指向这150个字符单元首地址
	for(int i = 0; i < 50; i++)
		result[i] = (char * )malloc(150 * sizeof(char ));
	res_len = 0;
	
	char folder[150] = "";
	int i;
	for(i = 0; i < strlen(path); i++) {
		folder[i] = tolower(path[i]);
	}
	folder[i] = '\0';
	char ps[150];
	strcpy(ps,path);
	char* delim = "/";
	char* res = strtok(path,delim);
	while(res != NULL) {
		strcpy(path_arr[path_len],res);
		for(int i = 0; i < strlen(path_arr[path_len]); i++){
			path_arr[path_len][i] = toupper(path_arr[path_len][i]);
		}
		path_len++;
		res = strtok(NULL, delim);
	}

	struct root_directory_entry rde;
	struct root_directory_entry* rde_pointer = &rde;
	check_path(0,file,rde_pointer, offset);
	path_len = 0;

	//到此为止result中存储了输入路径中的所有子目录，下面处理这些子目录
	print_result(result,ps,1);

	for(int i = 0; i < 50; i++)
		free((void *)result[i]);
	free((void *)result);

	tag = 0;
}

void print_result(char **result,char* path,int floor) {
	if(res_len == 0) {
		char prompt[] = "Not a directory!\n";
		my_print(0,strlen(prompt),prompt);
	} else if(res_len == 1 && !(strstr((*result),"."))) {
		if(floor != 1) {
			for(int k = 1; k < floor; k++)
				my_print(2,strlen("    "),"    ");
		}
		my_print(2,strlen(*result),*result);
		my_print(2,strlen(" : ")," : ");
		my_print(2,strlen("0"),"0");
		my_print(2,strlen(" directories,")," directories,");
		my_print(2,strlen("0"),"0");
		my_print(2,strlen(" files\n")," files\n");
	}  else if(res_len == 1 && (strstr((*result),"."))) {
		if(floor != 1) {
			for(int k = 1; k < floor; k++)
				my_print(2,strlen("    "),"    ");
		}
		char* delim = "/";
		char* res = strtok((*result),delim);
		char arr[50][150];
		int len = 0;
		while(res != NULL) {
			strcpy(arr[len],res);
			len++;
			res = strtok(NULL, delim);
		}
		char name[150] = "";
		char* help="/";
		int i;
		for(i = 0; i < len-2; i++){
			strcat(name,arr[i]);
			strcat(name,help);
		}
		strcat(name,arr[i]);
		my_print(2,strlen(name),name);
		my_print(2,strlen(" : ")," : ");
		my_print(2,strlen("0"),"0");
		my_print(2,strlen(" directories,")," directories,");
		my_print(2,strlen("1"),"1");
		my_print(2,strlen(" files\n")," files\n");
	} else {
		if(floor != 1) {
			for(int k = 1; k < floor; k++)
				my_print(2,strlen("    "),"    ");
		}
			int k = 0;
			char path_tmp[150];
			strcpy(path_tmp,path);
			for(int t = 0; t < res_len; t++) {
				if(strstr(result[t],path_tmp)){
					result[t] = strrpl(result[t],path_tmp,"");
					for(int p = 0; p < strlen(result[t])-1; p++) {
						result[t][p] = result[t][p+1];
					}
					result[t][strlen(result[t])-1]='\0';
				}
				if(!strstr(path,"/") && k == 0){
					my_print(2,strlen(path_tmp),path_tmp);
					my_print(2,strlen(" : ")," : ");
					k = 1;
				} else {
					char* delim = "/";
					char* dir = strtok(path,delim);
					if(dir != NULL){
						dir = strtok(NULL, delim);		
						if(k == 0){
							my_print(2,strlen(dir),dir);
							my_print(2,strlen(" : ")," : ");
							k = 1;
						}		
					}
				}
				
			}
	
		print_child_result(result,floor);
	}
}

void print_child_result(char **result,int floor) {
	tang = (char ***)malloc(50*sizeof(char**));
	for(int i = 0; i < 50; i++)
		tang[i] = (char ** )malloc(50 * sizeof(char* ));
	for(int i = 0; i < 50; i++)
		for(int j = 0; j < 50; j++)
			tang[i][j] = (char * )malloc(150 * sizeof(char ));

	int count[50];
	int count_len = 0;

	directories = 0;
	files = 0;
	char tmp[150];

	int dir_len_one = 0;
	int dir_len_two = 0;

	for(int i = 0; i < res_len; i++) {
		char* directory = (char *) malloc(150 * sizeof(char));	//第i行
		for(int j = 0; j < strlen(result[i]); j++) {
			directory[j] = result[i][j];
		}

		char directory_copy[150];
		strcpy(directory_copy,directory);
		char* delim = "/";
		char* res = strtok(directory,delim);
		if(res != NULL) {
			
			if(!strstr(res,".")) {
				if(i == 0) {
					directories++;
					strcpy(tang[dir_len_one][dir_len_two],directory_copy);
					strcpy(tmp,res);
					dir_len_two++;
					count[count_len] = dir_len_two;
				}else if(strcmp(tmp,res) == 0) {
					strcpy(tang[dir_len_one][dir_len_two],directory_copy);
					dir_len_two++;
					count[count_len] = dir_len_two;
				}else if(strcmp(tmp,res) != 0) {
					directories++;
					dir_len_two = 0;
					dir_len_one++;
					strcpy(tang[dir_len_one][dir_len_two],directory_copy);
					count_len++;
					dir_len_two++;
					count[count_len] = dir_len_two;
					strcpy(tmp,res);
				}
			} else {
				files++;
			}
		} else {
			files++;
		}

		for(int j = 0; j < strlen(result[i]); j++) {
			directory[j] = ' ';
		}
		free(directory);
	}

	char str = directories + 48;
	char *s = (char *)malloc(sizeof(char));
	s = &str;
	my_print(2,1,s);
	my_print(2,strlen(" directories,")," directories,");

	str = files + 48;
	s = &str;
	my_print(2,1,s);
	my_print(2,strlen(" files\n")," files\n");

	QAQ(dir_len_one,tang,floor,count);
}

char* strrpl(char *s, const char *s1, const char *s2) {
	char *ptr;
	if (ptr = strstr(s, s1)){
		memmove(ptr + strlen(s2) , ptr + strlen(s1), strlen(ptr) - strlen(s1) + 1);
		memcpy(ptr, &s2[0], strlen(s2));
	}
	return s;
}

void QAQ(int dir_len_one,char*** tang,int floor,int count[]) {

	for(int i = 0; i <= dir_len_one; i++) {
		res_len = count[i];
		char* ttt;
		char* delim = "/";
		char tttt[50];
		strcpy(tttt,tang[i][0]);
		ttt = strtok(tttt,delim);
		if(ttt != NULL)
			print_result(tang[i],ttt,floor+1);
		else{
			print_result(tang[i],tang[i][0],floor+1);
		}
	}
}
