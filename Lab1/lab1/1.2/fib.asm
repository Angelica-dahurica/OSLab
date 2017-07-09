section .data
	promote_input:		db 'The input list is : '
	input_length:		equ $ - promote_input
	promote_output:		db 'The result is as follow : '
	output_length:		equ $ - promote_output
	
	color_red:      	db  1Bh, '[31;1m', 0
	color_blue:     	db  1Bh, '[34;1m', 0
	color_len:      	equ $ - color_blue
	color_count: 		db 0

	number_count: 		db 0h ; count input number

	newline:			db 0Ah

section .bss
	input: 		resb 50 ; input string
	array: 		resb 10 ; number array
	pointer: 	resb 4
	done: 		resb 1 ; 1 if not more number,0 if more number
	output: 	resb 20

section .text
	global main:

main:
	mov eax, 4
	mov ebx, 1
	mov ecx, promote_input
	mov edx, input_length
	int 80h

	mov eax, 3
	mov ebx, 0
	mov ecx, input
	mov edx, 50
	int 80h

	mov eax, input
	mov dword[pointer], eax ; init to input
	
	call string_to_num
	
	mov eax, 1
	mov ebx, 0
	int 80h

string_to_num:
	push ebx
	push ecx ; count register
	push edx ; data register

	next_number:
	mov edx, 0h
	call get_length

	mov ebx, dword[pointer] ; string
	sub ebx, 1 ; point to space
	sub ebx, eax ; begin of data, [eax]=len

	outer:
		cmp eax, 0
		je outer_done

		mov ecx, eax ; i = len
		push eax
		mov eax, 0
		mov al, byte[ebx]
		sub eax, 48

		; while i>0 
		jmp inner_c ; -> i<len
		inner: ; *10^i
			push edx
			mov edx, 10
			mul edx
			pop edx
		inner_c:
			loop inner ; i--
		; end while
		
		 add edx, eax
		 pop eax
		 sub eax, 1 ; len--
		 inc ebx
		 jmp outer

	outer_done:
		mov eax, 0h
		mov al, byte[number_count]
		mov byte[array+eax], dl
		inc al
		mov byte[number_count], al ; number++
		
		cmp byte[done], 0
		je next_number

		pop edx
		pop ecx
		pop ebx

		call fibonacci

		ret

get_length:
	push ebx
	mov eax, dword[pointer]
	mov ebx, eax

	next_char:
		cmp byte[eax], 0Ah
		je case1
		cmp byte[eax], 20h
		je case0
		inc eax
		jmp next_char
	case0:
		mov byte[done], 0
		jmp return_get_length
	case1:	
		mov byte[done], 1

	return_get_length:
		inc eax
		mov dword[pointer], eax ; begin of new string
		sub eax, 1 ; end of old string
		sub eax, ebx ; [eax]=len
		pop ebx
		ret

num_to_string:
	push eax
	push ebx ; ebx is used to place 10
	push ecx ; ecx is used to count
	push edx ; edx is used to div
	mov ebx, 10
	mov ecx, 0

	begin:
		mov edx, 0
		div ebx
		add edx, 48
		push edx ; in stack in oppo order
		inc ecx

		cmp eax,0
		je end
		jmp begin

	end:
	mov eax, ecx ; move length to eax
	mov ebx, 0 ; offset is in ebx

	out_stack:
		pop edx ; out stack in order
		mov byte[output+ebx], dl
		inc ebx
		loop out_stack

	mov byte[output+eax], 0Ah ; append newline
	inc eax
	call print
	
	pop edx
	pop ecx
	pop ebx
	pop eax
	ret

print:
	push eax
	push ebx
	push ecx
	push edx

	push eax
	mov edx, 0
	mov eax, 0
	mov al, byte[color_count]
	mov ecx, 2
	div ecx

	cmp edx,0
	mov ecx,color_red
	je end_color
	mov ecx,color_blue
	
	end_color:
		inc edx
		mov byte[color_count], dl
		mov eax, 4
		mov ebx, 1
		mov edx, color_len
		int 80h
		pop eax
	
	mov edx, eax
	mov eax, 4
	mov ebx, 1
	mov ecx, output
	int 80h
	
	pop edx
	pop ecx
	pop ebx
	pop eax
	ret

fibonacci:
	push eax
	push ebx
	push ecx
	push edx

	mov eax, 4
	mov ebx, 1
	mov ecx, promote_output
	mov edx, output_length
	int 80h

	mov eax, 4
	mov ebx, 1
	mov ecx, newline
	mov edx, 1
	int 80h

	pop edx
	pop ecx
	pop ebx
	pop eax
    
    push eax ; the 1st number
	push ebx ; the 2st number
	push ecx ; number
	push edx ; cache and now No.
	mov ecx, 0
	mov cl, byte[number_count] ; [ecx]=number

	mov eax, 1 ; f(0)=1
	mov ebx, 0
	mov edx, 0

	cal:
		push ebx ; ebx is used to calculate address
		mov ebx, 0
		mov bl, byte[number_count] ; [ebx]=number

		sub ebx, ecx

		cmp byte[array+ebx], dl
		pop ebx
		jne recursion ; is not zero, to calculate
		sub ecx, 1 ; is zero, i = n, number--

		push eax
		call num_to_string
		pop eax

	recursion:	
		inc ecx ; adress++
		push edx
		mov edx, eax
		add eax, ebx ; f(n) = f(n-1)+f(n-2)
		mov ebx, edx ; [edx or eax]=f(n-1)this time,=[ebx]=f(n-2)next time
		pop edx
		inc edx ; i++
		loop cal ; number--

	pop ebx
	pop ecx
	pop edx
	pop eax
	ret