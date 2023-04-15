#include "headers.h"

 int shmid2;
  key_t key_sh;
/* Modify this file as needed*/
int remainingtime;
int time = -1;
 void run();
 void* target;
// void stop();
int main(int agrc, char *argv[])
{
   

    key_sh = ftok("tempfile", 's');
    shmid2 = shmget(key_sh, 4096, IPC_CREAT | 0644);
    initClk();
    remainingtime = atoi(argv[1]);
    //  printf("\nkill process\n");

      //kill (getpid(),SIGSTOP);
      printf("\nruntime %d\n", remainingtime);
    // signal(SIGCONT, run);
    // Signal(SIGSTOP, stop);
    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
   time = getClk();
  int * shared = (int *) shmat(shmid2, (void *)0, 0);
    while (remainingtime > 0) // remaining time should be shared memory as scheduler updates it with every clk and process should keep track of it :)
    {
        start:
     //   printf("\n the print the clk at start %d",getClk());
        while (time== getClk())
        continue;
        target=&&start;
         remainingtime--;
       // sleep(1);
      //  printf("\nremain time: %d process id %d time %d\n",remainingtime,getpid(),time);
          printf("\nremain time: %d process id %d time %d\n",remainingtime,getpid(),getClk());
           
            (*shared)=remainingtime;
            time=getClk();
         
        // remainingtime = ??;
    }
    printf("\n process of id %d finished runing\n",getpid());
    // send signal finish to the ppid as it tereminated to be removerd from queue

    printf("\nthe clk %d\n", getClk());
    destroyClk(false);
    kill (getppid(),SIGUSR1);
    
    return 0;
}
void run()
{
    goto *target;
}/*
void stop()
{
    time = -1;
    // remainingtime=-(getClk()-time);
}*/
