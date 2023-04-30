#include "headers.h"

// void signalState(int segnum);
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
FILE *file1;
FILE *file2;
FILE *memFile;
PriorityQueue pq;
PriorityQueue unallocatedProcess;
LinkedList holeList;
LinkedList processList;
int startQuantum = 0;
char choice;
char memChoice;
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
int lastDQ = 0;
struct msgbuff
{
    long mtype;
    struct PCB recvpcd;
};
int main(int argc, char *argv[])
{
    insert(&holeList, 0, 1023, -1);
    key_sh = ftok("tempfile", 's');
    shmid2 = shmget(key_sh, 4096, IPC_CREAT | 0666);
    shared = (int *)shmat(shmid2, (void *)0, 0);
    switcher = false;
    startQuantum = 10000000;
    first = true;
    choice = argv[1][0];
    memChoice = argv[4][0];
    quantum = atoi(argv[2]);
    noProcess = atoi(argv[3]);
    key = ftok("tempfile", 'a');
    msgid = msgget(key, 0666 | IPC_CREAT);
    signal(SIGUSR1, signalFinish);
    printf("\nChoice: %c\n", choice);
    printf("\nq : %d\n", quantum);
    printf("\nNo of process: %d\n", noProcess);
    setpqueue(&pq);
    setpqueue(&unallocatedProcess);
    struct msgbuff recvmess;
    // Process_generator should send the processes in msgQ
    initClk();
    // currProc.id = -1;
    if (msgid == -1)
        printf("\nError in creating msgQ\n");
    time = -1;
    file1 = fopen("scheduler.log", "a+");
    memFile = fopen("memory.log", "a+");
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
            while (msgrcv(msgid, &recvmess, sizeof(recvmess.recvpcd), 0, IPC_NOWAIT) != -1)
            {

                temp = recvmess.recvpcd;
                printf("\nwe are creating processs \n");

                createprocess(temp);
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
    int temp = getClk();
    if (choice == 'r')
    {
        while (temp == getClk())
            ;
    }
    file2 = fopen("scheduler.perf", "w");
    if (file2 == NULL)
        printf("fsddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd");
    fprintf(file2, "\nthe clk %d\n", getClk());
    fprintf(file2, "CPU utillization = %.2f %%", ((float)totalUsedTime) / getClk() * 100);
    fprintf(file2, "\nAvg WTA = %.2f\n", ((float)totalUsedTime) / getClk() * 100);
    fprintf(file2, "Avg Waiting = %.2f\n", ((float)totalWT) / counter);
    fprintf(file2, "Std WTA = %.2f\n", calculateWaitSD());
    fclose(file2);
    fclose(file1);
    fclose(memFile);

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
void createprocess(struct PCB processTemp)
{
    printf("\nHey from msgQ\n");
    if (memChoice == 'f')
    {
        LinkedListNode theNode = *holeAllocation(&holeList, processTemp.memsize);
        if (theNode.start == -1)
        {
            enqueue(&unallocatedProcess, processTemp, processTemp.memsize);
            return;
        }
        theNode.procNumber = processTemp.id;
        insert(&processList, theNode.start, theNode.end, theNode.procNumber);

        printf("At time %d allocated %d bytes for process %d from %d to %d\n", getClk(),
               processTemp.memsize, processTemp.id, theNode.start, theNode.end);
        fprintf(memFile, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(),
                processTemp.memsize, processTemp.id, theNode.start, theNode.end);
    }
    switch (choice)
    {
    case 'r':
        enqueue(&pq, processTemp, 1000);
        break;
    case 'p':
        enqueue(&pq, processTemp, processTemp.Priority);

        break;
    case 's':
        enqueue(&pq, processTemp, processTemp.RemainingTime);

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
    while (getClk() == lastDQ)
        ;
    currProc = dequeue(&pq);
    printf("\nGonna run process %d at %d using %c algo state %d\n", currProc.id, getClk(), choice, currProc.state);
    startQuantum = getClk(); // getClk

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

void signalFinish(int segnum)
{
    printf("\nIn signal handler\n");
    if (pq.head != NULL && choice != 'p')
    {
        // currProc = pq.head->pcb;
        pq.head->pcb.RemainingTime = 0;
        if (choice == 'r')
        {
            currProc = pq.head->pcb;
            currk->state = Stopped;
        }
    }
    // currProc = pq.head->pcb;
    // pq.head->pcb.RemainingTime = 0;
    // pq.head->pcb.state = Stopped;
    if (choice != 's')
        currProc.state = Stopped;
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
    fprintf(file1, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",
            getClk(), lastProc.id, lastProc.ArrTime, lastProc.endTime - lastProc.startTime, lastProc.RemainingTime,
            lastProc.WaitTime, lastProc.TA, lastProc.WTA);
    totalUsedTime += lastProc.RunTime;
    noProcess--;
    lastDQ = getClk();
    if (memChoice == 'f')
    {
        LinkedListNode tempNode = *searchProcessNumber(&processList, lastProc.id);
        deleteLinkedListNode(&processList, &tempNode);
        holeFree(&holeList, &tempNode);
        printf("At time %d freed %d bytes for process %d from %d to %d\n", getClk(),
               lastProc.memsize, lastProc.id, tempNode.start, tempNode.end);
        fprintf(memFile, "At time %d freed %d bytes for process %d from %d to %d\n", getClk(),
                lastProc.memsize, lastProc.id, tempNode.start, tempNode.end);
    }

    while (!isEmpty(&unallocatedProcess) && searchBySize(&holeList, peek(&unallocatedProcess).memsize) != NULL)
    {
        struct PCB newPCB = dequeue(&unallocatedProcess);
        createprocess(newPCB);
    }

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
    while (pq.head->pcb.state == Stopped)
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
        if (currk->RemainingTime - *shared /*getClk() - lastDQ */ >= quantum)
        {
            fprintf(file1, "At time %d process %d stopped arr %d total %d remain %d wait %d \n",
                    getClk(), currk->id, currk->ArrTime, currk->RunTime, currk->RemainingTime, currk->WaitTime);
            kill(currk->PID, SIGUSR2);
            kill(currk->PID, SIGSTOP);
            lastDQ = getClk();
            currk->state = Waiting;
            printf("\nStopping process %d\n", currk->id);
            currk->RemainingTime = *shared;
            if (currk->state != Stopped && currk->RemainingTime != 0)
                enqueue(&pq, dequeue(&pq), 1000);
            else if (currk->state != Stopped)
            {
                raise(SIGUSR1);
                dequeue(&pq);
            }
        }
        else // hello
            return;
    }
    while (!isEmpty(&pq) && pq.head->pcb.state == Stopped)
    {
        dequeue(&pq);
    }
    if (isEmpty(&pq))
        return;

    while (lastDQ == getClk())
        ;
    currk = &pq.head->pcb;

    if (currk->state == Waiting)
    {
        fprintf(file1, "At time %d process %d resumed arr %d total %d remain %d wait %d \n",
                getClk(), currk->id, currk->ArrTime, currk->RunTime, currk->RemainingTime, currk->WaitTime);
        printf("\n Continue process .. no forking\n");
        currk->countq = getClk();
        currk->state = Running;
        *shared = currk->RemainingTime;
        kill(currk->PID, SIGCONT);
        return;
    }
    else if (currk->state == NotStarted)
    {
        fprintf(file1, "At time %d process %d started arr %d total %d remain %d wait %d \n",
                getClk(), currk->id, currk->ArrTime, currk->RunTime, currk->RemainingTime, currk->WaitTime);
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
            *shared = currk->RemainingTime;
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
void strn()
{
    switcher = true;
    if (isEmpty(&pq))
        return;
    char qr[5];
    currProc = peek(&pq);
    if (currk == NULL)
    {
        if (lastDQ == getClk())
        {
            printf("\n ana al awal \n");
            return;
        }
        currk = &pq.head->pcb;
        if (currk->state == Waiting)
        {

            fprintf(file1, "At time %d process %d resumed arr %d total %d remain %d wait %d \n",
                    getClk(), currk->id, currk->ArrTime, currk->RunTime, currk->RemainingTime, currk->WaitTime);

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

                fprintf(file1, "At time %d process %d started arr %d total %d remain %d wait %d \n",
                        getClk(), currk->id, currk->ArrTime, currk->RunTime, currk->RemainingTime, currk->WaitTime);

                currk->state = Running;
                currk->PID = pid;
            }
        }
    }
    else if (pq.head->pcb.id != currk->id)
    {
        printf("\n ana al tane \n");
        lastDQ = getClk();
        currk->RemainingTime = (*shared);
        currk->state = Waiting;
        fprintf(file1, "At time %d process %d stopped arr %d total %d remain %d wait %d \n",
                getClk(), currk->id, currk->ArrTime, currk->RunTime, currk->RemainingTime, currk->WaitTime);
        kill(currk->PID, SIGUSR2);
        kill(currk->PID, SIGSTOP);
        if (lastDQ == getClk())
        {
            printf("\n dasdasdasdasdasdasdasdas \n");
            printf("\n rakam %d \n", pq.head->pcb.id);
            currk = NULL;
            return;
            // strn();
        }
        currk = &pq.head->pcb;
        if (currk->state == Waiting)
        {
            fprintf(file1, "At time %d process %d resumed arr %d total %d remain %d wait %d \n",
                    getClk(), currk->id, currk->ArrTime, currk->RunTime, currk->RemainingTime, currk->WaitTime);
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
                fprintf(file1, "At time %d process %d started arr %d total %d remain %d wait %d \n",
                        getClk(), currk->id, currk->ArrTime, currk->RunTime, currk->RemainingTime, currk->WaitTime);
                currk->state = Running;
                printf("\n %d \n", currk->id);
                currk->PID = pid;
            }
        }
    }
    else if ((currk->RemainingTime = (*shared)) == 0)
    {
        printf("\n dequeue process %d \n", currk->id);
        currProc = *currk;
        dequeue(&pq);
        currk = NULL;
    }
}
