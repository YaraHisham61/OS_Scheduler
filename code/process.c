#include "headers.h"
#include "signal.h"

/* Modify this file as needed*/
int remainingtime;
int time = -1;
// void run();
// void stop();
int main(int agrc, char *argv[])
{
    initClk();
    remainingtime = atoi(argv[1]);
    //  printf("\nkill process\n");

      //kill (getpid(),SIGSTOP);
      printf("\nruntime %d\n", remainingtime);
    // Signal(SIGCONT, run);
    // Signal(SIGSTOP, stop);
    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
   time = getClk();

    while (remainingtime > 0) // remaining time should be shared memory as scheduler updates it with every clk and process should keep track of it :)
    {
        while (time== getClk())
        continue;
        printf("\nremain time: %d process id %d time %d\n",remainingtime,getpid(),time);
            remainingtime--;
            time=getClk();
        
        // remainingtime = ??;
    }
    printf("\n process of id %d finished runing\n",getpid());
    // send signal finish to the ppid as it tereminated to be removerd from queue
    destroyClk(false);
kill (getppid(),SIGUSR1);
    return 0;
}
/*void run()
{
    time = getClk();
}
void stop()
{
    time = -1;
    // remainingtime=-(getClk()-time);
}*/
