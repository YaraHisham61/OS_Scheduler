#include "headers.h"
#include "PriorityQueue.h"

void roundRobin(int q);
void switching(struct PCB *nextProc);
void highestPriorityFirst();
void signalFinish(int segnum);
void startProcess();
struct PCB currProc, lastProc;
int msgid;
PriorityQueue pq;
int startQuantum = 0;
char choice;
int quantum;
int noProcess;
int ID;
int time = 0;
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
    choice = argv[1][0];
    quantum = atoi(argv[2]);
    noProcess = atoi(argv[3]);
    key = ftok("tempfile", 'a');
    signal(SIGCHLD, signalFinish);
    printf("\nChoice: %c\n", choice);
    printf("\nq : %d\n", quantum);
    printf("\nNo of process: %d\n", noProcess);
    setpqueue(&pq);
    // Process_generator should send the processes in msgQ
    initClk();
    //currProc.id = -1;
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1)
        printf("\nError in creating msgQ\n");
    while (noProcess)
    {
        if (time == getClk())
        {
            time++;
             //printf("\nIn Scheduler current time is : %d\n", time++);
            if (msgrcv(msgid, &currProc, sizeof(currProc), 0, IPC_NOWAIT) != -1)
            {
                printf("\nHey from msgQ\n");
                //if (currProc.id == -1)
                   // break;

                currProc.startTime = time;
                currProc.endTime = time + currProc.RunTime;
                enqueue(&pq, currProc, currProc.Priority);
                //startProcess();
            }
            else 
            printf("\n Error in recv\n");
            if (processFinished == true && !isEmpty(&pq))
            {
                processFinished = false;
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
                    currProc.PID = ID;
            }
            printf("\nNo of processes now is : %d\n",noProcess);
        }

    }
    printf("CPU utillization = %.2f %%", ((float)totalUsedTime) / time * 100);
    printf("\nAvg WTA = %.2f\n", ((float)totalUsedTime) / time * 100);
    printf("Avg Waiting = %.2f\n", ((float)totalWT) / counter);
    printf("Std WTA = %.2f\n", calculateWaitSD());

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
    totalWTA += lastProc.WTA;
    totalWT += lastProc.WaitTime;
    lastProc.endTime = time;
    lastProc.RemainingTime = 0;
    waitingTimeArr[counter++] = lastProc.WaitTime;
    printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",
           time, lastProc.id, lastProc.ArrTime, lastProc.endTime - lastProc.startTime, lastProc.RemainingTime,
           lastProc.WaitTime, lastProc.TA, lastProc.WTA);
    totalUsedTime += lastProc.RunTime;
    noProcess--;
    //startProcess();
    // save the endtime of the current process
}
void highestPriorityFirst()
{
    int currTime = getClk();
    printf("At time %d process %d started arr %d total %d remain %d wait %d\n",
           time, currProc.id, currProc.ArrTime, currProc.RemainingTime,
           currProc.startTime - currProc.ArrTime, currProc.WaitTime);
    /*while (getClk() < currTime + currProc.RemainingTime)
    {
        sleep(1);
    };*/
}

void startProcess()
{
    if (pq.count == 0)
        return;
    currProc = dequeue(&pq);
    char qr[5];
    int pid = fork();
    if (pid == 0)
    {

        startQuantum = time; // getClk
        currProc.state = Running;
        currProc.startTime = time;           // getClk
        sprintf(qr, "%d", currProc.RunTime); // Convert integer to string
        execl("process.out", "", &qr, NULL);
    }
    else
        currProc.PID = pid;
}

void roundRobin(int q)
{
    while (!isEmpty(&pq))
    {
        if (getClk() - startQuantum >= q)
        {
            currProc.state = Waiting;
            kill(currProc.PID, SIGSTOP);
        }
    }
}

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