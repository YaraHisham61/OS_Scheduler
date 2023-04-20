#include "headers.h"

void roundRobin(int q);
void highestPriorityFirst();
void signalFinish(int segnum);
void startProcess();
float calculateWaitSD();
void createprocess();
void schedularchoose();
void strn();
void rr();
struct PCB *currk;
void shortest_remaining_time_next();
struct PCB currProc, lastProc, temp;
int msgid;
key_t key;
bool processFinished = true;
FILE *file;
PriorityQueue pq;
int startQuantum = 0;
char choice;
int quantum, currQuant = 0;
int noProcess;
int ID;
int time;
int utillization;
int totalWTA;
int totalWT;
int counter;
int waitingTimeArr[100] = {-1};
int totalUsedTime;
bool first;
bool switcher;
bool slot;
int strn_run;
int shmid2;
key_t key_sh;
int *shared;
int lastDQ=0;
struct msgbuff
{
    long mtype;
    struct PCB recvpcd;
};
int main(int argc, char *argv[])
{
    key_sh = ftok("tempfile", 's');
    shmid2 = shmget(key_sh, 4096, IPC_CREAT | 0666);
    shared = (int *)shmat(shmid2, (void *)0, 0);
    switcher = false;
    startQuantum = 10000000;
    first = true;
    file = fopen("scheduler.log", "w");
    choice = argv[1][0];
    quantum = atoi(argv[2]);
    noProcess = atoi(argv[3]);
    key = ftok("tempfile", 'a');
    msgid = msgget(key, 0666 | IPC_CREAT);
    signal(SIGUSR1, signalFinish);
    printf("\nChoice: %c\n", choice);
    printf("\nq : %d\n", quantum);
    printf("\nNo of process: %d\n", noProcess);
    setpqueue(&pq);
    struct msgbuff recvmess;
    // Process_generator should send the processes in msgQ
    initClk();
    // currProc.id = -1;
    if (msgid == -1)
        printf("\nError in creating msgQ\n");
    time = -1;
    while (noProcess)
    {
        if (time != getClk())
        {

            time = getClk();
            printf("\nIn scheduler current time is : %d\n", getClk());
            /*if (choice == 'r' && (getClk() - startQuantum) >= (quantum))
            {
                kill(currProc.PID, SIGSTOP);
            }*/

            // printf("\nIn Scheduler current time is : %d\n", time++);
            if (msgrcv(msgid, &recvmess, sizeof(recvmess.recvpcd), 0, IPC_NOWAIT) == -1)
            {
                // printf("\nError in recv\n");
                // printf("temp %d ", temp.RunTime);
            }
            else
            {

                temp = recvmess.recvpcd;
                printf("\nwe are creating processs \n");

                createprocess();
                /*if (first && choice != 's')
                {
                    printf("\nfork start\n");
                    startProcess();
                    first = false;
                    strn_run = getClk();
                }*/
            }

            schedularchoose();
            printf("\nNo of processes now is : %d\n", noProcess);
        }
    }

    printf("\nthe clk %d\n", getClk());
    printf("CPU utillization = %.2f %%", ((float)totalUsedTime) / getClk() * 100);
    printf("\nAvg WTA = %.2f\n", ((float)totalUsedTime) / getClk() * 100);
    printf("Avg Waiting = %.2f\n", ((float)totalWT) / counter);
    printf("Std WTA = %.2f\n", calculateWaitSD());

    // while(1){
    // if(x == getClk()) continue;
    /// currProc->remainTime-=1;
    //}
    shmctl(shmid2, IPC_RMID, (struct shmid_ds *)0);
    msgctl(msgid, IPC_RMID, (struct msqid_ds *)0);

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
    if (pq.count == 0)
    {
        printf("\n Empty Q .. returning\n");
        return;
    }
    currProc = dequeue(&pq);
    printf("\nGonna run process %d at %d using %c algo\n", currProc.id, getClk(), choice);
    startQuantum = getClk(); // getClk

    if (currProc.state == NotStarted)
    {
        printf("\nForking new process ... \n");
        currProc.state = Running;
        char qr[5];
        currProc.startTime = getClk();
        int pid = fork();
        if (pid == 0)
        {
            printf("\nStart process: quantum %d  state %d starttime %d \n", startQuantum, currProc.state, getClk()); // getClk
            sprintf(qr, "%d", currProc.RunTime);                                                                     // Convert integer to string
            execl("process.out", "", &qr, NULL);
        }
        else
        {
            currProc.PID = pid;
        }
    }
    else
    {
        kill(currProc.PID, SIGCONT);
        printf("\nstart process %d :not forking\n", currProc.id);
        // startQuantum--;
        printf("\nStart Quantum is %d for process %d\n", startQuantum, currProc.id);
    }
}

void signalFinish(int segnum)
{
    printf("\nIn signal handler\n");
    currProc = pq.head->pcb;
    pq.head->pcb.RemainingTime = 0;
    pq.head->pcb.state = Stopped;

    lastProc = currProc;
    /////////////////////
    processFinished = true;
    lastProc.state = Stopped;
    lastProc.TA = getClk() - lastProc.startTime;
    lastProc.WTA = lastProc.TA / lastProc.RunTime;
    totalWTA += lastProc.WTA;
    totalWT += lastProc.WaitTime;
    lastProc.endTime = getClk();
    lastProc.RemainingTime = 0;
    waitingTimeArr[counter++] = lastProc.WaitTime;
    printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",
           getClk(), lastProc.id, lastProc.ArrTime, lastProc.endTime - lastProc.startTime, lastProc.RemainingTime,
           lastProc.WaitTime, lastProc.TA, lastProc.WTA);
    totalUsedTime += lastProc.RunTime;
    noProcess--;
    lastDQ=getClk();

    /* if (choice == 'r')
        while (lastProc.endTime == getClk())
        {
        }
    if (switcher == false)
    {
        startProcess();
        slot = true;
    }*/
    //  save the endtime of the current process
    signal(SIGUSR1, signalFinish);
}
void highestPriorityFirst()
{
    if (currProc.state != Running)
        startProcess();
}
void schedularchoose()
{
    switch (choice)
    {
    case 'r':
        rr();
        break;
    case 'p':
        highestPriorityFirst();
        break;
    case 's':
        strn(); // function call
        break;
    }
}

void roundRobin(int q)
{
    /*   if (slot == true)
       {
           kill(currProc.PID, SIGCONT);
           slot = false;
       }*/
    /*if (isEmpty(&pq))
        return;
    if ((currProc.RemainingTime - *shared) >= (q-1) && switcher == false)
    {
        int timer = getClk();
        switcher = true;
        slot = true;
        printf("\n clk %d and quant %d the quantum difference is %d\n", getClk(), startQuantum, getClk() - startQuantum);

        lastProc = currProc;
        currProc.RemainingTime = *shared;
        if (!isEmpty(&pq))
        {

            currProc.state = Waiting;
            kill(currProc.PID, SIGSTOP);
            if (pq.head->pcb.state != NotStarted)
                while (timer == getClk())
                {
                }
            startProcess();
            if (lastProc.state != Stopped && lastProc.RemainingTime != 0)
                enqueue(&pq, lastProc, 1000);
            printf("\nactual switch\n");
            //   print_priority_queue(&pq);
        }
        else
        {

            startQuantum = getClk();
            // currProc.RemainingTime -= quantum;
            printf("\nvirtual switch\n");
        }
        switcher = false;
    }*/
}
void rr()
{
    if (isEmpty(&pq))
        return;
    if (pq.head->pcb.state == Stopped )
    {
        dequeue(&pq);

    }
    if (isEmpty(&pq))
        return;

    currk = &pq.head->pcb;
    printf("\nhead id: %d \n", currk->id);
    if (pq.head->pcb.state == Running)
    {
        printf("\n the diff is : %d for process %d \n", currk->RemainingTime - *shared, currk->id);
        if (/*currk->RemainingTime - *shared*/getClk()-lastDQ >= quantum)
        {
            kill(currk->PID, SIGUSR2);
            kill(currk->PID, SIGSTOP);
            lastDQ=getClk();  
            currk->state = Waiting;
            printf("\nStopping process %d\n", currk->id);
            currk->RemainingTime = *shared;
            enqueue(&pq, dequeue(&pq), 1000);
            
        }
        else // hello
            return;

    }
    
    while(lastDQ==getClk());
    currk = &pq.head->pcb;

    if (currk->state == Waiting)
    {
        printf("\n Continue process .. no forking\n");
        currk->countq = getClk();
        currk->state = Running;
        kill(currk->PID, SIGCONT);
        return;
    }
    else if (currk->state == NotStarted)
    {
        printf("\nForking new process .. \n");
        int pid = fork();
        if (pid == 0)
        {
            char qr[5];
            sprintf(qr, "%d", currk->RunTime); // Convert integer to string
            execl("process.out", "", qr, NULL);
            /// exit(0);
        }
        else
        {
            currk->state = Running;
            currk->PID = pid;
            currk->countq = getClk();
        }
    }
}
// run i

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
void shortest_remaining_time_next()
{

    if (slot == true)
    {
        kill(currProc.PID, SIGCONT);
        slot = false;
    }
    if (isEmpty(&pq))
        return;
    switcher = true;
    struct PCB srtn = peek(&pq);
    currProc.RemainingTime = (*shared);
    if (currProc.RemainingTime > srtn.RemainingTime || currProc.state == Stopped)
    {
        slot = true;
        //  currProc.RemainingTime -= (getClk() - strn_run)+1;
        currProc.state = Waiting;
        kill(currProc.PID, SIGSTOP);
        printf("|\n remaining time :%d", currProc.RemainingTime);
        if (currProc.state != Stopped && currProc.RemainingTime != 0)
            enqueue(&pq, currProc, currProc.RemainingTime);
        startProcess();
        kill(currProc.PID, SIGCONT);
        strn_run = getClk();
        printf("\n srtn switch\n");
    }
    switcher = false;
}
void strn()
{
    switcher = true;
    if (isEmpty(&pq))
        return;
    char qr[5];
    currProc = peek(&pq);
    printf("\n process \n");
    if (currk == NULL)
    {
        // currProc.state = Running;
        printf("\n process one run now \n");
        currk = &pq.head->pcb;
        // currk->RemainingTime = (*shared);
        if (currk->state == Stopped)
        {
            currk->state = Running;
            kill(currk->PID, SIGCONT);
        }
        else
        {

            int pid = fork();
            if (pid == 0)
            {
                sprintf(qr, "%d", currk->RunTime); // Convert integer to string
                execl("process.out", "", qr, NULL);
                exit(0);
            }
            else
            {
                currk->state = Running;
                currk->PID = pid;
            }
        }
    }
    else if (pq.head->pcb.id != currk->id)
    {
        printf("\n process process run now \n");
        currk->RemainingTime = (*shared);
        currk->state = Stopped;
        kill(currk->PID, SIGSTOP);
        currk = &pq.head->pcb;
        // currk->RemainingTime = (*shared);
        if (currk->state == Stopped)
        {
            currk->state = Running;
            kill(currk->PID, SIGCONT);
        }
        else
        {
            int pid = fork();
            if (pid == 0)
            {
                sprintf(qr, "%d", currk->RunTime); // Convert integer to string
                execl("process.out", "", qr, NULL);
                exit(0);
            }
            else
            {
                currk->state = Running;
                printf("\n %d \n", currk->id);
                currk->PID = pid;
            }
        }
    }
    else if ((currk->RemainingTime = (*shared)) == 0)
    {

        // currk->RemainingTime = 100000000;
        printf("\n dequeue process %d \n", currk->id);
        dequeue(&pq);
        currk = NULL;
    }
}