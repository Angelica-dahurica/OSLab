
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               tty.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

#define TTY_FIRST	(tty_table)
#define TTY_END		(tty_table + NR_CONSOLES)

PRIVATE void init_tty(TTY* p_tty);
PRIVATE void tty_do_read(TTY* p_tty);
PRIVATE void tty_do_write(TTY* p_tty);
PRIVATE void put_key(TTY* p_tty, u32 key);

PRIVATE void recover(CONSOLE* p_con);

/*======================================================================*
                           task_tty
 *======================================================================*/
PUBLIC void task_tty()
{
	TTY*	p_tty;

	find_index = 0;
	is_show = 0;

	init_keyboard();

	for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
		init_tty(p_tty);
	}
	select_console(0);
	while (1) {
		for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
			tty_do_read(p_tty);
			tty_do_write(p_tty);
		}
	}
}

/*======================================================================*
			   init_tty
 *======================================================================*/
PRIVATE void init_tty(TTY* p_tty)
{
	p_tty->inbuf_count = 0;
	p_tty->p_inbuf_head = p_tty->p_inbuf_tail = p_tty->in_buf;

	init_screen(p_tty);
}

/*======================================================================*
				in_process
 *======================================================================*/
PUBLIC void in_process(TTY* p_tty, u32 key)
{
        char output[2] = {'\0', '\0'};

        if (!(key & FLAG_EXT)) {
			if(!is_show){
				put_key(p_tty, key);
			}
        }
        else {
            int raw_code = key & MASK_RAW;
            switch(raw_code) {
                case ENTER:
					if(!is_show){
						put_key(p_tty, '\n');
					}
					break;
                case BACKSPACE:
					if(!is_show){
						put_key(p_tty, '\b');
					}
					break;
				case TAB:
					if(!is_show){
						put_key(p_tty, ' ');put_key(p_tty, ' ');put_key(p_tty, ' ');put_key(p_tty, ' ');
					}
					break;
				case ESC:
					is_find = !is_find;
					if(!is_find){
						recover(p_tty->p_console);
					}
					break;				
                default:
                    break;
            }
        }
}

/*======================================================================*
			      put_key
*======================================================================*/
PRIVATE void put_key(TTY* p_tty, u32 key)
{
	if (p_tty->inbuf_count < TTY_IN_BYTES) {
		*(p_tty->p_inbuf_head) = key;
		p_tty->p_inbuf_head++;
		if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_head = p_tty->in_buf;
		}
		p_tty->inbuf_count++;
	}
}


/*======================================================================*
			      tty_do_read
 *======================================================================*/
PRIVATE void tty_do_read(TTY* p_tty)
{
	if (is_current_console(p_tty->p_console)) {
		keyboard_read(p_tty);
	}
}


/*======================================================================*
			      tty_do_write
 *======================================================================*/
PRIVATE void tty_do_write(TTY* p_tty)
{
	if (p_tty->inbuf_count) {
		char ch = *(p_tty->p_inbuf_tail);
		p_tty->p_inbuf_tail++;
		if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_tail = p_tty->in_buf;
		}
		p_tty->inbuf_count--;

		out_char(p_tty->p_console, ch, DEFAULT);
	}
}


/*======================================================================*
                  show_result
 *======================================================================*/
PUBLIC void show_result(CONSOLE* p_con)
{
    close_int();
    
    is_show = 1;
    int show = 1;
	int loc = p_con->cursor;
    
    for (int i = 0; i < loc; i++) {
        u8* p_vmem = (u8*)(V_MEM_BASE + i * 2);
        
        if (*p_vmem == find_content[0]) {
            u8* temp = p_vmem;
            show = 1;
            
            for (int j = 0; j < find_index; ++j) {
                if (*temp != find_content[j]) {
                    show = 0;
                    break;
                }
                temp = temp + 2;
            }
            
            if (show) {
                temp--;
                for (int j = 0; j < find_index; ++j) {
                    *temp = GREEN;
                    temp = temp - 2;
                }
            }
        }
    }
    
    open_int();
}

/*======================================================================*
                           recover
 *======================================================================*/
PRIVATE void recover(CONSOLE* p_con)
{
    close_int();
    
	int loc = p_con->cursor;   
    int temp_index = find_index;
    
    int i = 0;
    
    for (i = 0; i < loc; i++) {
        u8* p_vmem = (u8*)(V_MEM_BASE + i * 2);
        p_vmem++;
        *p_vmem = DEFAULT;
    }
    
    i = loc - 1;
    
    while (find_index > 0) {
        u8* p_vmem = (u8*)(V_MEM_BASE + i * 2);
        *p_vmem++ = ' ';
        *p_vmem++ = DEFAULT;
        i--;
        find_index--;
    }
    
    find_index = 0;
    is_show = 0;

    loc = loc - temp_index;
    set_cursor(loc);
    
    open_int();
}

