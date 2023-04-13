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
bool processFinished = true;
FILE *file;
PriorityQueue pq;
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
bool first;
int main(int argc, char *argv[])
{
    first = true;
    file = fopen("scheduler.log", "w");
    choice = argv[1][0];
    quantum = atoi(argv[2]);
    noProcess = atoi(argv[3]);
    key = ftok("tempfile", 'a');
    msgid = msgget(key, 0666 | IPC_CREAT);
    signal(SIGCHLD, signalFinish);
    printf("\nChoice: %c\n", choice);
    printf("\nq : %d\n", quantum);
    printf("\nNo of process: %d\n", noProcess);
    setpqueue(&pq);
   
    // Process_generator should send the processes in msgQ
    initClk();
    // currProc.id = -1;
    if (msgid == -1)
        printf("\nError in creating msgQ\n");
    time = getClk();
    while (noProcess)
    {
        if (time == getClk())
        {
            printf("\nIn scheduler current time is : %d\n",time);

            time++;
            // printf("\nIn Scheduler current time is : %d\n", time++);
            if (msgrcv(msgid, &temp, sizeof(temp), 0, IPC_NOWAIT) == -1)
            { printf("\n no Error in recv\n");
            printf("temp %d ", temp.RunTime);
            continue;
            }
                
        
                printf("\nwe are creating processs\n");

                createprocess();
               if(first)
               {
                startProcess();
                first=false;
               }
               
            
            schedularchoose();
            /*if (processFinished == true && !isEmpty(&pq))
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
             }*/
            printf("\nNo of processes now is : %d\n", noProcess);
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
    return 0;
}
void createprocess()
{
    printf("\nHey from msgQ\n");
    switch (choice)
    {
    case 'r':
        enqueue(&pq, temp, 1000);
        break;
    case 'p':
        enqueue(&pq, temp, temp.Priority);

        break;
    case 's':
        enqueue(&pq, temp, temp.RemainingTime);

        break;

    default:
        break;
    }
}

void startProcess() // to run process
{
     printf("\nstart process: count %d\n", pq.count);
    if (pq.count == 0)
        return;
    currProc = dequeue(&pq);
     printf("\nstart process:2\n");
    if (currProc.state == NotStarted)
    {
        char qr[5];
         printf("\nstart process:3\n");
        int pid = fork();
        if (pid == 0)
        {

            startQuantum = time; // getClk
            currProc.state = Running;
            currProc.startTime = time;   
             printf("\nstart process: quantum %d  state %d starttime %d \n",startQuantum,currProc.state,time);        // getClk
            sprintf(qr, "%d", currProc.RunTime); // Convert integer to string
            execl("process.out", "", &qr, NULL);
           printf("\nstart process:5\n");

        }
        else
        {
             printf("\nstart process:9\n");
            currProc.PID = pid;
        }
         printf("\nstart process:110\n");
    }
    else
    {
        startQuantum = time; // getClk
        currProc.state = Running;
         printf("\nstart process:111\n");
    }
}
void signalFinish(int segnum)
{
    lastProc = currProc;
    /////////////////////
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
    startProcess();
    //  save the endtime of the current process
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
void schedularchoose()
{
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

    if (getClk() - startQuantum >= q)
    {
        if (!isEmpty(&pq))
        {
            currProc.RemainingTime -= quantum;
            currProc.state = Waiting;
            kill(currProc.PID, SIGSTOP);
            enqueue(&pq, currProc, 1000);
            startProcess();
        }
        else
        {
            startQuantum = getClk();
            currProc.RemainingTime -= quantum;
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