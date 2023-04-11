#include "headers.h"
struct PCB
{
    int finTime;
    int arrTime;
    bool state;
    int runTime;
    int remainTime;
    int turnaroundTime;
};

void roundRobin(int q);
void switching(struct PCB *nextProc);
struct PCB *currProc = NULL;

int main(int argc, char *argv[])
{
    char choice = argv[1][0];
    int qunatum = atoi(argv[2]);
    printf("\n choice:%c\n", choice);
    printf("\nquantum:%d\n", qunatum);
    // Process_generator should send the processes in msgQ
    initClk();
    int x = 0;
    while (1)
    {

        if (x == getClk())
            printf("\nIn Scheduler current time is : %d\n", x++);
    }
    // while(1){
    // if(x == getClk()) continue;
    /// currProc->remainTime-=1;
    //}

    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    destroyClk(true);
}

// void switching (struct PCB * nextProc)
//{
// currProc->state=0;
// currProc=nextProc;
// currProc->state=1;
//  nextProc should be dequeued before entering the function
// currProc (before switching) --> dequeue from PCB then enqueue to Ready queue
// currProc (after switching) --> enqueue to PCB
// }
