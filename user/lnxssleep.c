#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"

// Inicializa um sleep lock
void
initsleeplock(struct sleeplock *lk, char *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

// Adquire o sleep lock (dorme se já estiver trancado)
void
acquiresleep(struct sleeplock *lk)
{
  acquire(&lk->lk);
  while (lk->locked) {
    sleep(lk, &lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

// Libera o sleep lock e acorda quem estiver esperando
void
releasesleep(struct sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

// Verifica se o processo atual segura o sleep lock
int
holdingsleep(struct sleeplock *lk)
{
  int r;
  
  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  return r;
}

// Função sleep baseada em ticks
extern uint ticks;
extern struct spinlock tickslock;

// Coloca o processo atual para dormir por n ticks
void
sleep_ticks(int n)
{
  uint ticks0;

  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
}
