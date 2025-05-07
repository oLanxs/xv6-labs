// Sleeping locks

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"

void
initsleeplock(struct sleeplock *lk, char *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

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

void
releasesleep(struct sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

int
holdingsleep(struct sleeplock *lk)
{
  int r;
  
  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  return r;
}

// Função wakeup: acorda todos os processos dormindo em chan
void
wakeup(void *chan)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++){
    if(p->state == SLEEPING && p->chan == chan){
      acquire(&p->lock);
      p->state = RUNNABLE;
      release(&p->lock);
    }
  }
}

// Ping pong entre dois processos
void
pingpong(void)
{
  int pid = fork();
  int i;
  char *ping_chan = "ping";
  char *pong_chan = "pong";

  if(pid == 0){
    // Processo filho (pong)
    for(i = 0; i < 5; i++){
      sleep(pong_chan, &tickslock);
      printf("pong\n");
      wakeup(ping_chan);
    }
    exit(0);
  } else {
    // Processo pai (ping)
    for(i = 0; i < 5; i++){
      printf("ping\n");
      wakeup(pong_chan);
      sleep(ping_chan, &tickslock);
    }
    wait(0);
  }
}
