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
      kill (getpid(),SIGSTOP);
      printf("\ncontinue process\n");
    // Signal(SIGCONT, run);
    // Signal(SIGSTOP, stop);
    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
   time = getClk();

    while (remainingtime > 0) // remaining time should be shared memory as scheduler updates it with every clk and process should keep track of it :)
    {
        while (time== getClk())
        continue;
        
            remainingtime--;
            time=getClk();
        
        // remainingtime = ??;
    }
    // send signal finish to the ppid as it tereminated to be removerd from queue
    kill(getppid(), SIGUSR1);
    destroyClk(false);

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