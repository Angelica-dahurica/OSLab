
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
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
#include "proto.h"

void print_wait();
void add_ID();
void cut_hair(int);
void get_haircut(int);

int cur_id;

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");

	close_int();
	disp_pos = 0;
	for(int i = 0; i< 80 * 25; i++)
		disp_str(" ");
	disp_pos = 0;
	init_screen(tty_table);
	open_int();

	TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	u16		selector_ldt	= SELECTOR_LDT_FIRST;
	int i;
        u8              privilege;
        u8              rpl;
        int             eflags;
	for (i = 0; i < NR_TASKS+NR_PROCS; i++) {
                if (i < NR_TASKS) {     /* 任务 */
                        p_task    = task_table + i;
                        privilege = PRIVILEGE_TASK;
                        rpl       = RPL_TASK;
                        eflags    = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
                }
                else {                  /* 用户进程 */
                        p_task    = user_proc_table + (i - NR_TASKS);
                        privilege = PRIVILEGE_USER;
                        rpl       = RPL_USER;
                        eflags    = 0x202; /* IF=1, bit 2 is always 1 */
                }

		strcpy(p_proc->p_name, p_task->name);	// name of the process
		p_proc->pid = i;			// pid

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		p_proc->regs.cs	= (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = eflags;

		p_proc->nr_tty = 0;
		p_proc->sleep = 0;
		p_proc->wait = 0;

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	proc_table[0].ticks = proc_table[0].priority = 	6;
	proc_table[1].ticks = proc_table[1].priority =  6;
	proc_table[2].ticks = proc_table[2].priority =  6;
	proc_table[3].ticks = proc_table[3].priority =  6;
	proc_table[4].ticks = proc_table[4].priority =  6;
	proc_table[5].ticks = proc_table[5].priority =  6;
	
	proc_table[1].type = OTHER;
	proc_table[2].type = BARBER;
	proc_table[3].type = CUSTOMER_A;
	proc_table[4].type = CUSTOMER_B;
	proc_table[5].type = CUSTOMER_C;

	k_reenter = 0;
	ticks = 0;

	waiting = 0;
	c_id = 0;
	
	//信号量
	mutex.value=1;
	mutex.head=0;
	mutex.tail=0;
	mutex.count=0;
	mutex.name="m";
	barbers.value=0;
	barbers.head=0;
	barbers.tail=0;
	barbers.count=0;
	barbers.name="b";
	customers.value=0;
	customers.head=0;
	customers.tail=0;
	customers.count=0;
	customers.name="c";

	p_proc_ready = proc_table;

	init_clock();
    init_keyboard();
	restart();

	while(1){
		
	}
}

/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
	while(1){
		close_int();
		disp_pos = 0;
		for(int i = 0; i < 80*25; i++)
			disp_str(" ");
		disp_pos = 0;
		init_screen(tty_table);
		open_int();	
		milli_delay(30000);
	}
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestB()
{
	while(1){
		show_str("Barber is sleeping.\n");
		P(&customers);
			//有顾客吗?若无顾客,理发师睡眠
		P(&mutex);
			//若有顾客时，进入临界区
		waiting--;//等候顾客数少一个
		V(&barbers);//理发师准备为顾客理发
		V(&mutex);//退出临界区
		milli_delay(5000);
		cut_hair(cur_id); 
      		//理发师正在理发(非临界区)
	}
}

/*======================================================================*
                               TestC
 *======================================================================*/
void TestC()
{
	while(1){
		P(&mutex);//进入临界区
		add_ID();
		int count = c_id;
		if(waiting<CHAIRS){
			//有空椅子
			show_str("customer NO.");
			disp_int(count);
			show_str(" sits down and waiting.\n");
			waiting++;//等候顾客数加1
			V(&customers);//唤醒理发师
			V(&mutex);//退出临界区
			P(&barbers);
				//理发师忙，顾客坐下等待
			get_haircut(count);//否则顾客坐下理发
			cur_id = count;
		}else{
			V(&mutex);//人满了,走吧！
		}
		milli_delay(5000);
	}
}


/*======================================================================*
                               TestD
 *======================================================================*/
void TestD()
{
	while(1){
		P(&mutex);//进入临界区
		add_ID();
		int count = c_id;
		if(waiting<CHAIRS){
			//有空椅子
			show_str("customer NO.");
			disp_int(count);
			show_str(" sits down and waiting.\n");
			waiting++;//等候顾客数加1
			V(&customers);//唤醒理发师
			V(&mutex);//退出临界区
			P(&barbers);
				//理发师忙，顾客坐下等待
			get_haircut(count);//否则顾客坐下理发
			cur_id = count;
		}else{
			V(&mutex);//人满了,走吧！
		}
		milli_delay(5000);
	}
}




/*======================================================================*
                               TestE
 *======================================================================*/
void TestE()
{
	while(1){
		P(&mutex);//进入临界区
		add_ID();
		int count = c_id;
		if(waiting<CHAIRS){
			//有空椅子
			show_str("customer NO.");
			disp_int(count);
			show_str(" sits down and waiting.\n");
			waiting++;//等候顾客数加1
			V(&customers);//唤醒理发师
			V(&mutex);//退出临界区
			P(&barbers);
				//理发师忙，顾客坐下等待
			get_haircut(count);//否则顾客坐下理发
			cur_id = count;
		}else{
			V(&mutex);//人满了,走吧！
		}
		milli_delay(5000);
	}
}


void print_wait(){
	show_str("Waiting Num: ");
	disp_int(waiting);
	show_str("\n");
	if(waiting==CHAIRS){
		show_str("No enough chair, customer NO.");
		disp_int(c_id);
		show_str(" leave.\n");
	}
}

void add_ID(){
	c_id++;
	show_str("customer NO.");
	disp_int(c_id);
	show_str(" come.\n");
	print_wait();
}

void cut_hair(int id){
	show_str("The barber finished cutting hair for customer NO.");
	disp_int(id);
	show_str(".\n");
}

void get_haircut(int id){
	show_str("customer NO.");
	disp_int(id);
	show_str(" ");
	show_str("is getting hair cut\n");
}

