objects	=	main.o my.o manage.o my.o
hello	:	$(objects)
	gcc -o hello $(objects)
main.o	:	main.c
	gcc -c -o main.o main.c
manage.o : manage.c manage.h
	gcc -c -o manage.o manage.c
my.o	:	my_print.asm
		nasm -f elf -o my.o my_print.asm	
.PHONY	:	clean
clean	:
	rm hello $(objects)
