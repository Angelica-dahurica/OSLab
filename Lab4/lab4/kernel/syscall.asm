
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"

INT_VECTOR_SYS_CALL equ 0x90
_NR_get_ticks       equ 0 ; 要跟 global.c 中 sys_call_table 的定义相对应！
_NR_show_str	    equ 1
_NR_sleep			equ 2
_NR_P				equ 3
_NR_V				equ 4

; 导出符号
global	get_ticks
global  show_str
global  sleep
global  P
global  V

bits 32
[section .text]

; ====================================================================
;                              get_ticks
; ====================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                          void show_str(char* str, int color);
; ====================================================================
show_str:
		mov		eax, _NR_show_str
		mov		ebx, [esp + 4]
		int		INT_VECTOR_SYS_CALL
		ret

; ====================================================================
;                          void sleep(int second);
; ====================================================================
sleep:
		mov 	eax, _NR_sleep
		mov 	ebx, [esp + 4]
		int 	INT_VECTOR_SYS_CALL
		ret

; ====================================================================
;                          void P(int sem);
; ====================================================================
P:
		mov		eax, _NR_P
		mov 	ebx, [esp + 4]
		int		INT_VECTOR_SYS_CALL
		ret

; ====================================================================
;                          void V(int sem);
; ====================================================================
V:
		mov		eax, _NR_V
		mov		ebx, [esp + 4]
		int		INT_VECTOR_SYS_CALL
		ret