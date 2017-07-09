global my_print

section .data

	type:				db 0	;define byte
	len:				dd 0	;define dword(4 byte)
	content:			dd "",0	;define dword(4 byte) and end with 0

	color_red:      	db  1Bh, '[31;1m', 0
	color_blue:     	db  1Bh, '[34;1m', 0
	color_default:		db	1Bh, '[37;1m', 0
	color_len:      	equ $ - color_default

section .text

my_print:	
	;栈由高地址向低地址成长
	;函数调用是用入栈的方式传递参数
	;函数参数入栈顺序是从右至左的
	;故在函数处理参数时,esp+4就是最后一个入栈的参数的地址,即最左边一个参数
	;栈指针寄存器(extended stack pointer),其内存放着一个指针,该指针永远指向系统栈最上面一个栈帧的栈顶
	mov eax, [esp+4]	;把以[esp+4]为起始地址的内存中的数据存入eax寄存器
	mov ecx, [esp+8]
	mov edx, [esp+12]

	mov byte[type], al	;低8位，即1字节
	mov dword[len], ecx
	mov dword[content], edx

	mov al, byte[type]
	cmp al, 0h			;输出类型为0
	je red
	cmp al, 1h			;输出类型为1
	je blue

continue:
	mov eax, 4
	mov ebx, 1
	mov ecx, dword[content]
	mov edx, dword[len]
	int 80h

	mov eax,4
	mov ebx,1
	mov ecx,color_default
	mov edx,color_len
	int 80h

	ret

red:
	mov eax,4
	mov ebx,1
	mov ecx,color_red
	mov edx,color_len
	int 80h
	jmp continue
	ret

blue:
	mov eax,4
	mov ebx,1
	mov ecx,color_blue
	mov edx,color_len
	int 80h
	jmp continue
	ret