#include "headers.h"
void highestPriorityFirst();
void signalFinish(int segnum);
void startProcess();
float calculateWaitSD();
void createprocess();
void schedularchoose();
void srtn();
void rr();
struct PCB *currk;
struct PCB currProc, lastProc, temp;
int msgid;
key_t key;
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
int quantum;
int noProcess;
int time;
int totalWTA;
int totalWT;
int counter;
int *waitingTimeArr;
int totalUsedTime;
int srtn_run;
int shmid2;
key_t key_sh;
int *shared;
struct BuddyNode **arr;
struct BuddyNode *root;
int lastDQ = 0;
struct msgbuff
{
    long mtype;
    struct PCB recvpcd;
};
int main(int argc, char *argv[])
{
    insert(&holeList, 0, 1023, -1);
    root = newBuddyNode(1024, 1, 0, 1023);
    printf("I'm here for root processNum = %d size = %d start = %d end = %d\n",
           root->processNumber, root->size, root->start, root->ending);
    key_sh = ftok("tempfile", 's');
    shmid2 = shmget(key_sh, 4096, IPC_CREAT | 0666);
    shared = (int *)shmat(shmid2, (void *)0, 0);
    startQuantum = 10000000;
    choice = argv[1][0];
    quantum = atoi(argv[2]);
    noProcess = atoi(argv[3]);
    memChoice = argv[4][0];
    key = ftok("tempfile", 'a');
    msgid = msgget(key, 0666 | IPC_CREAT);
    signal(SIGUSR1, signalFinish);
    printf("argv[0] = %s,argv[1] = %s,argv[2] = %s,argv[3] = %s,argv[4] = %s,", argv[0], argv[1], argv[2], argv[3], argv[4]);
    printf("\nChoice: %c\n", choice);
    printf("\nq : %d\n", quantum);
    printf("\nNo of process: %d\n", noProcess);
    arr = malloc((noProcess + 1) * sizeof(struct BuddyNode *));
    waitingTimeArr = malloc((noProcess + 1) * sizeof(int));
    setpqueue(&pq);
    setpqueue(&unallocatedProcess);
    struct msgbuff recvmess;
    // Process_generator should send the processes in msgQ
    initClk();
    if (msgid == -1)
        printf("\nError in creating msgQ\n");
    time = -1;
    file1 = fopen("scheduler.log", "w");
    memFile = fopen("memory.log", "w");
    while (noProcess)
    {
        if (time != getClk())
        {

            time = getClk();
            printf("\nIn scheduler current time is : %d\n", getClk());
            while (msgrcv(msgid, &recvmess, sizeof(recvmess.recvpcd), 0, IPC_NOWAIT) != -1)
            {

                temp = recvmess.recvpcd;
                printf("\nwe are creating processs \n");

                createprocess(temp);
            }

            schedularchoose();
            printf("\nNo of processes now is : %d\n", noProcess);
        }
    }
    int temp = getClk();
    file2 = fopen("scheduler.perf", "w");
    if (file2 == NULL)
        printf("fsddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd");
    fprintf(file2, "CPU utillization = %.2f %%", ((float)totalUsedTime) / getClk() * 100);
    fprintf(file2, "\nAvg WTA = %.2f\n", ((float)totalWTA) /atoi(argv[3]));
    fprintf(file2, "Avg Waiting = %.2f\n", ((float)totalWT) / counter);
    fprintf(file2, "Std WTA = %.2f\n", calculateWaitSD());
    fclose(file2);
    fclose(file1);
    fclose(memFile);
    shmctl(shmid2, IPC_RMID, (struct shmid_ds *)0);
    msgctl(msgid, IPC_RMID, (struct msqid_ds *)0);
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
    else
    {
        struct BuddyNode *theNode = findBuddyNode(root, processTemp.memsize);

        if (theNode == NULL)
        {
            printf("There's no space :(\n");
            enqueue(&unallocatedProcess, processTemp, processTemp.memsize);
            return;
        }
        theNode->processNumber = processTemp.id;
        arr[processTemp.id] = theNode;
        printf("At time %d allocated %d bytes for process %d from %d to %d\n", getClk(),
               processTemp.memsize, processTemp.id, theNode->start, theNode->ending);
        fprintf(memFile, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(),
                processTemp.memsize, processTemp.id, theNode->start, theNode->ending);
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
    if (pq.count == 0 || noProcess == 0)
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
    printf("At time %d process %d started arr %d total %d remain %d wait %d \n",
           getClk(), currProc.id, currProc.ArrTime, currProc.ArrTime, currProc.RemainingTime, currProc.WaitTime);

    fprintf(file1, "At time %d process %d started arr %d total %d remain %d wait %d \n",
            getClk(), currProc.id, currProc.ArrTime, currProc.ArrTime, currProc.RemainingTime, currProc.WaitTime);
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
        pq.head->pcb.RemainingTime = 0;
        if (choice == 'r')
        {
            currProc = pq.head->pcb;
            currk->state = Stopped;
        }
    }

    if (choice != 's')
        currProc.state = Stopped;
    lastProc = currProc;
    totalUsedTime += lastProc.RunTime;
    lastProc.state = Stopped;
    lastProc.TA = getClk() - lastProc.ArrTime + 1;
    lastProc.WaitTime = lastProc.TA - lastProc.RunTime;
    lastProc.WTA = lastProc.TA / lastProc.RunTime;
    totalWTA += lastProc.WTA;
    totalWT += lastProc.WaitTime;
    lastProc.endTime = getClk();
    lastProc.RemainingTime = 0;
    waitingTimeArr[counter++] = lastProc.WaitTime;
    printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",
           getClk(), lastProc.id, lastProc.ArrTime, lastProc.endTime - lastProc.startTime, lastProc.RemainingTime,
           lastProc.WaitTime, lastProc.TA, lastProc.WTA, lastProc.startTime);
    fprintf(file1, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",
            getClk(), lastProc.id, lastProc.ArrTime, lastProc.endTime - lastProc.startTime, lastProc.RemainingTime,
            lastProc.WaitTime, lastProc.TA, lastProc.WTA, lastProc.startTime);
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
        int prev = -1;
        while (!isEmpty(&unallocatedProcess))
        {
            if (prev == unallocatedProcess.head->pcb.id)
                break;
            struct PCB newPCB = dequeue(&unallocatedProcess);
            createprocess(newPCB);
            if (isEmpty(&unallocatedProcess))
                break;
            prev = unallocatedProcess.head->pcb.id;
        }
    }
    else
    {
        printf("data : processNum = %d start = %d end = %d size = %d\n", arr[lastProc.id]->processNumber,
               arr[lastProc.id]->start, arr[lastProc.id]->ending, arr[lastProc.id]->size);
        freeBuddyNode(arr[lastProc.id]);
        printf("At time %d freed %d bytes for process %d from %d to %d\n", getClk(),
               lastProc.memsize, lastProc.id, arr[lastProc.id]->start, arr[lastProc.id]->ending);
        fprintf(memFile, "At time %d freed %d bytes for process %d from %d to %d\n", getClk(),
                lastProc.memsize, lastProc.id, arr[lastProc.id]->start, arr[lastProc.id]->ending);
        int prev = -1;
        while (!isEmpty(&unallocatedProcess) && noProcess != 0)
        {
            if (prev == unallocatedProcess.head->pcb.id)
                break;
            struct PCB newPCB = dequeue(&unallocatedProcess);
            createprocess(newPCB);
            if (isEmpty(&unallocatedProcess))
                break;
            prev = unallocatedProcess.head->pcb.id;
        }
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
        srtn(); // function call
        break;
    }
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
            currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime);
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
        else 
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
        currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime);
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
        currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime);
        fprintf(file1, "At time %d process %d started arr %d total %d remain %d wait %d \n",
                getClk(), currk->id, currk->ArrTime, currk->RunTime, currk->RemainingTime, currk->WaitTime);
        printf("\nForking new process .. \n");
        int pid = fork();
        if (pid == 0)
        {
            char qr[5];
            sprintf(qr, "%d", currk->RunTime); // Convert integer to string
            execl("process.out", "", qr, NULL);

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
void srtn()
{
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
            currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime);
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
                currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime);
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
        currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime);
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
        }
        currk = &pq.head->pcb;
        if (currk->state == Waiting)
        {
            currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime);
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
                currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime);
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
