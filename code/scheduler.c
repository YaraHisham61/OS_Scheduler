#include "headers.h"
#include "PriorityQueue.h"

void roundRobin(int q);
void highestPriorityFirst();
void signalFinish(int segnum);
void startProcess();
float calculateWaitSD();
void createprocess();
void schedularchoose();
struct PCB currProc, lastProc, temp;
int msgid;
key_t key;
FILE *file,*file2;
PriorityQueue pq, runningQueue;
int startQuantum = 0;
char choice;
int quantum;
int noProcess;
int ID;
int time;
int count;
int utillization;
int totalWTA;
int totalWT;
int counter;
int waitingTimeArr[100] = {-1};
int totalUsedTime;

int main(int argc, char *argv[])
{
    file = fopen("scheduler.log", "w");
    file2 = fopen("scheduler.perf", "w");
    choice = argv[1][0];
    quantum = atoi(argv[2]);
    noProcess = atoi(argv[3]);
    key = 5;
    msgid = msgget(key, 0666 | IPC_CREAT);
    signal(SIGUSR1, signalFinish);
    printf("\nChoice: %c\n", choice);
    printf("\nq : %d\n", quantum);
    printf("\nNo of process: %d\n", noProcess);
    setpqueue(&pq);
    setpqueue(&runningQueue);
    // Process_generator should send the processes in msgQ
    initClk();
    if (msgid == -1)
        printf("\nError in creating msgQ\n");
    time = getClk();
    while (noProcess)
    {
        if (time == getClk())
        {

            // printf("\nIn Scheduler current time is : %d\n", time++);
            while (msgrcv(msgid, &temp, sizeof(temp), 0, IPC_NOWAIT) != -1)
            {
                createprocess();
            }
            schedularchoose();
            time++;
        }
    }
    fprintf(file2,"CPU utillization = %.2f %%", ((float)totalUsedTime) / time * 100);
    fprintf(file2,"\nAvg WTA = %.2f\n", ((float)totalWTA) / counter);
    fprintf(file2,"Avg Waiting = %.2f\n", ((float)totalWT) / counter);
    fprintf(file2,"Std WTA = %.2f\n", calculateWaitSD());

    // TODO implement the scheduler :)
    // upon termination release the clock resources.
    fclose(file);
    fclose(file2);
    destroyClk(true);
    kill(0, SIGUSR1);
}
void createprocess()
{
    printf("\n I recived a message from ID = %d \n",temp.id);
    switch (choice)
    {
    case 'r':
        enqueue(&pq, temp, 1000);
        break;
    case 'p':
        enqueue(&pq, temp, temp.Priority);
        break;
    case 's':
        enqueue(&pq, temp, currProc.RemainingTime);

    default:
        break;
    }
    // char qr[5];
    // int pid = fork();
    // if (pid == 0)
    // {
    //     sprintf(qr, "%d", temp.RunTime); // Convert integer to string
    //     execl("process.out", "", &qr, NULL);
    // }
    // else
    // {
    //     temp.PID = pid;
    //     // kill(pid, SIGSTOP);
    // }
}
void startProcess() // to run process
{
    if (pq.count == 0)
        return;
    currProc = dequeue(&pq);
    startQuantum = time; // getClk
    currProc.state = Running;
    currProc.startTime = time; // getClk
    kill(currProc.PID, SIGCONT);
}
void signalFinish(int segnum)
{
    lastProc = dequeue(&runningQueue);
    /////////////////////

    lastProc.state = Stopped;
    lastProc.TA = time - lastProc.ArrTime;
    lastProc.WTA = lastProc.TA / lastProc.RunTime;
    totalWTA += lastProc.WTA;
    totalWT += lastProc.WaitTime;
    lastProc.endTime = time;
    lastProc.RemainingTime = 0;
    waitingTimeArr[counter++] = lastProc.WaitTime;
    printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",
           getClk(), lastProc.id, lastProc.ArrTime, lastProc.RunTime, lastProc.RemainingTime,
           lastProc.WaitTime, lastProc.TA, lastProc.WTA);
    totalUsedTime += lastProc.RunTime;
    noProcess--;
    // startProcess();
    //   save the endtime of the current process
}
void highestPriorityFirst()
{
    currProc = dequeue(&pq);
    
    currProc.startTime = time;
    currProc.WaitTime = currProc.startTime - currProc.ArrTime;
    currProc.RemainingTime = currProc.RunTime;
    enqueue(&runningQueue, currProc, 0);
    fprintf(file,"At time %d process %d started arr %d total %d remain %d wait %d\n",
           time, currProc.id, currProc.ArrTime, currProc.RunTime,
           currProc.RemainingTime, currProc.WaitTime);
    int pid = fork();
    if (pid == 0)
    {

        while (1)
        {
            if (time == getClk())
            {
                continue;
            }
            currProc.RemainingTime--;
            if (currProc.RemainingTime == 0)
                break;
            time = getClk();
        }
        kill(getppid(), SIGUSR1);
        exit(0);
    }
}
void schedularchoose()
{
    if (isEmpty(&pq) || !isEmpty(&runningQueue))
        return;
    switch (choice)
    {
    case 'r':
        roundRobin(quantum);
        break;
    case 'p':
        highestPriorityFirst();
        break;
    case 's':
        // function call
        break;
    }
}

void roundRobin(int q)
{
    if (!isEmpty(&pq))
    {
        if (getClk() - startQuantum >= q)
        {
            currProc.RemainingTime -= quantum;
            currProc.state = Waiting;
            kill(currProc.PID, SIGSTOP);
            enqueue(&pq, currProc, 1000);
            startProcess();
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