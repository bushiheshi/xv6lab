#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"
uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
//Add a sys_trace() function in kernel/sysproc.c
uint64
sys_trace(void)
{
    int mask;

    // 要检索 trace() 的参数，用 syscall.c 里的函数来检查
    // 例子参考 kernel/sysproc.c
    // 为什么要检查参数？xv6-handout-ch4.4 有说明
    if(argint(0, &mask) < 0)// 检查 a0 里用户传入的参数，保存到变量 mask
		return -1;
    myproc()->tmask = mask;// 保存到 trapframe
    return 0;
}

uint64
sys_sysinfo(void)
{
	uint64 uptr;
	struct sysinfo info;
  	struct proc *p = myproc();

	if(argaddr(0, &uptr) < 0)    return -1;// 获取用户传入的指针
	info.freemem = freenum();
	info.nproc = procnum();
	if(copyout(p->pagetable, uptr, (char*)&info, sizeof(info)) < 0)    return -1;
	return 0;
}
