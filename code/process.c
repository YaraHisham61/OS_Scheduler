#include "headers.h"

int shmid2;
key_t key_sh;
/* Modify this file as needed*/
int remainingtime;
bool flag;
int time = -1;
void run();
void stop();
int main(int agrc, char *argv[])
{
flag=false;
  key_sh = ftok("tempfile", 's');
  shmid2 = shmget(key_sh, 4096, IPC_CREAT | 0644);
  initClk();
  remainingtime = atoi(argv[1]);
  //  printf("\nkill process\n");

  // kill (getpid(),SIGSTOP);
  printf("\nruntime %d\n", remainingtime);
   signal(SIGUSR1, run);
   signal(SIGUSR2, stop);
  // TODO it needs to get the remaining time from somewhere
  // remainingtime = ??;

  int *shared = (int *)shmat(shmid2, (void *)0, 0);
  while (remainingtime > 0) // remaining time should be shared memory as scheduler updates it with every clk and process should keep track of it :)
  {

    //   printf("\n the print the clk at start %d",getClk());
    if (time != getClk()&& flag==false)
    {
      time = getClk();
      remainingtime--;
      // sleep(1);
      //  printf("\nremain time: %d process id %d time %d\n",remainingtime,getpid(),time);
      printf("\nremain time: %d process id %d time %d\n", remainingtime, getpid(), getClk());

      (*shared) = remainingtime;
    }
    else
    flag =false;
    // remainingtime = ??;
  }
  printf("\n process of id %d finished runing\n", getpid());
  // send signal finish to the ppid as it tereminated to be removerd from queue

  //printf("\nthe clk %d\n", getClk());
  destroyClk(false);
  kill(getppid(), SIGUSR1);

  return 0;
}
void run(){
  flag=false;
}
void stop(){
  flag=true;
}