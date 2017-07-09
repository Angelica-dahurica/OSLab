
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.asm */
PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);

/* protect.c */
PUBLIC void	init_prot();
PUBLIC u32	seg2phys(u16 seg);

/* klib.c */
PUBLIC void	delay(int time);

/* kernel.asm */
void restart();
void close_int();
void open_int();

/* main.c */
void TestA();

/* i8259.c */
PUBLIC void init_8259A();
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);
PUBLIC void init_clock();
PUBLIC void milli_delay(int milli_sec);

/* keyboard.c */
PUBLIC void init_keyboard();
PUBLIC void keyboard_read(TTY* p_tty);
PUBLIC void keyboard_handler(int irq);

/* tty.c */
PUBLIC void task_tty();
PUBLIC void in_process(TTY* p_tty, u32 key);
PUBLIC void show_result(CONSOLE* p_con);

/* console.c */
PUBLIC void init_screen(TTY* p_tty);
PUBLIC void out_char(CONSOLE* p_con, char ch,int color);
PUBLIC void scroll_screen(CONSOLE* p_con, int direction);
PUBLIC void set_cursor(unsigned int position);
PUBLIC int is_current_console(CONSOLE* p_con);
PUBLIC void select_console(int nr_console);

/* 以下是系统调用相关 */

/* proc.c */
PUBLIC  int     sys_get_ticks();        /* sys_call */

/* syscall.asm */
PUBLIC  void    sys_call();             /* int_handler */
PUBLIC  int     get_ticks();

