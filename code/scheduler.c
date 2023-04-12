#include "headers.h"
#include "PriorityQueue.h"

void roundRobin(int q);
void switching(struct PCB *nextProc);
void highestPriorityFirst();
void signalFinish(int segnum);
struct PCB currProc, lastProc;
PriorityQueue pq;
int ID;
int time;
key_t key;
bool processFinished = true;
FILE *file;
int count;
int utillization;
int totalWTA;
int totalWT;
int counter;
int waitingTimeArr[100] = {-1};
int totalUsedTime;
float calculateWaitSD();
int main(int argc, char *argv[])
{
    file = fopen("scheduler.log", "w");
    char choice = argv[1][0];
    key = ftok("tempfile", 'a');
    signal(SIGCHLD, signalFinish);

    // Process_generator should send the processes in msgQ
    initClk();
    while (1)
    {
        if (time == getClk())
            printf("\nIn Scheduler current time is : %d\n", time++);
        if (msgrcv(key, &currProc, sizeof(currProc), 0, IPC_NOWAIT) != -1)
        {
            if (currProc.id == -1)
                break;
            currProc.startTime = time;
            currProc.endTime = time + currProc.RunTime;
            enqueue(&pq, currProc, currProc.Priority);
        }
        if (processFinished == true && !isEmpty(&pq))
        {
            processFinished = false;
            currProc = dequeue(&pq);
            currProc.startTime = time;
            currProc.WaitTime = time - currProc.ArrTime;
            currProc.state = Running;
            lastProc = currProc;
            ID = fork();
            if (ID == 0)
            {
                highestPriorityFirst();
                exit(99);
            }
            else
                currProc.PID =ID;
        }
    }
    fprintf(file, "CPU utillization = %.2f %\n", ((float)totalUsedTime) / time * 100);
    fprintf(file, "Avg WTA = %.2f\n", ((float)totalUsedTime) / time * 100);
    fprintf(file, "Avg Waiting = %.2f\n", ((float)totalWT) / counter);
    fprintf(file, "Std WTA = %.2f\n", calculateWaitSD());

    // while(1){
    // if(x == getClk()) continue;
    /// currProc->remainTime-=1;
    //}

    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    destroyClk(true);
}
void signalFinish(int segnum)
{
    processFinished = true;
    lastProc.state = Stopped;
    lastProc.TA = time - lastProc.startTime;
    lastProc.WTA = lastProc.TA / lastProc.RunTime;
    totalWTA+=lastProc.WTA;
    totalWT+=lastProc.WaitTime;
    lastProc.endTime = time;
    lastProc.RemainingTime = 0;
    waitingTimeArr[counter++] = lastProc.WaitTime;
    fprintf(file, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %d\n",
            time, lastProc.id, lastProc.ArrTime, lastProc.endTime - lastProc.startTime, lastProc.RemainingTime,
            lastProc.WaitTime, lastProc.TA, lastProc.WTA);
    totalUsedTime += lastProc.RunTime;
}
void highestPriorityFirst()
{
    int currTime = getClk();
    fprintf(file, "At time %d process %d started arr %d total %d remain %d wait %d\n",
            time, currProc.id, currProc.ArrTime, currProc.RemainingTime,
            currProc.startTime - currProc.ArrTime, currProc.WaitTime);
    while (getClk() < currTime + currProc.RemainingTime)
    {
        sleep(1);
    };
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
float calculateWaitSD()
{
    float sum = 0.0, mean, SD = 0.0;
    int i;
    for (i = 0; i < counter; ++i)
    {
        sum += waitingTimeArr[i];
    }
    mean = sum / 10;
    for (i = 0; i < counter; ++i)
    {
        SD += pow(waitingTimeArr[i] - mean, 2);
    }
    return sqrt(SD / 10);
}