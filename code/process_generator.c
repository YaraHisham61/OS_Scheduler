#include "headers.h"

void clearResources(int);
void readFile();
void schedulingChoose();
struct PriorityQueue pq;

int quantum;
char choice;
char qr[5];
char countProcess[5];
int msgid;
char *path;
key_t messageQueueKey;
int scheduling;
struct msgbuff
{
    long mtype;
    struct PCB sendpcd;
};
//
// file name - -sch - scheduler number - -q - quantum - -mem - memoryNumber

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    char memChoice;
    path = argv[1];
    setpqueue(&pq);
    readFile();
    print_priority_queue(&pq);
        quantum = -1;
    choice = argv[3][0];
    if (argc>6)
    {
        quantum = atoi(argv[5]);
        memChoice = argv[7][0];
        if(memChoice=='1'){
            memChoice = 'f';
        }else
            memChoice = 'b';
    }else{
        memChoice = argv[5][0];
        if(memChoice=='1'){
            memChoice = 'f';
        }else
            memChoice = 'b';
    }

    schedulingChoose();
    sprintf(countProcess, "%d", pq.count); // Convert integer to string
    
    int pid = fork();

    if (pid == 0)
    {
            printf("\nmemChoice = %c choice = %c\n" ,memChoice, choice);
        execl("scheduler.out", "", &choice, &qr, &countProcess, &memChoice, NULL); // clk
    }

    pid = fork();
    if (pid == 0)
    {
        execl("clk.out", "", NULL); // scheduler
    }
    initClk();
    // To get time use this
    int x = -1;
    messageQueueKey = ftok("tempfile", 'a');
    msgid = msgget(messageQueueKey, 0666 | IPC_CREAT);
    if (msgid == -1)
        printf("\nError in creating msgQ\n");
    struct msgbuff sendmess;
    while (1)
    {

        if (x != getClk())
        {

            x = getClk();
            if (!isEmpty(&pq))
            {
                struct PCB temp = peek(&pq);

                while (!isEmpty(&pq) && temp.ArrTime <= getClk())
                {
                    sendmess.mtype = 1;

                    temp = dequeue(&pq);
                    sendmess.sendpcd = temp;
                    if (msgsnd(msgid, &sendmess, sizeof(sendmess.sendpcd), !IPC_NOWAIT) == -1)
                        printf("\n Error in sending\n");

                    printf("\nSending id : %d\n", temp.id);
                    if (!isEmpty(&pq))
                        temp = peek(&pq);
                    else
                        break;
                }
            }
        }
        // printf("\nIn process generator current time is : %d\n", x);

        // TODO Generation Main Loop
        // 5. Create a data structure for processes and provide it with its parameters.
        // 6. Send the information to the scheduler at the appropriate time.
        // 7. Clear clock resources
    }

    destroyClk(true);
}

void readFile()
{

    FILE *filePtr = fopen(path, "r");
    if (filePtr == NULL)
    {
        printf("The file not found\n");
        exit(0);
    }
    fscanf(filePtr, "%*[^\n]\n");
    printf("I read %s\n", path);
    struct PCB temp;
    while (!feof(filePtr))
    {
        //    struct PCB temp;
        int id;      // process id
        int ArrTime; // time of arrival
        int RunTime; // running time (in state of running it must be devremented)
        int Priority;
        int memsize;

        fscanf(filePtr, "%d", &id);
        fscanf(filePtr, "%d", &ArrTime);
        fscanf(filePtr, "%d", &RunTime);
        fscanf(filePtr, "%d", &Priority);
        fscanf(filePtr, "%d", &memsize);
        setPCB(&temp, id, ArrTime, RunTime, Priority, memsize);
        // setPCB(&temp, id, ArrTime, RunTime, Priority);
        temp.state = NotStarted;
        enqueue(&pq, temp, temp.ArrTime);
    }
    /* temp.id = -1;
     enqueue(&pq, temp, temp.ArrTime);*/
}
void schedulingChoose()
{

    switch (choice)
    {
    case '1':
        choice = 'p';
        break;
    case '2':
        choice = 's';
        break;
    case '3':
        choice = 'r';
        break;

    default:
        break;
    }
    sprintf(qr, "%d", quantum); // Convert integer to string
    printf("\nqr = %s\n", qr);
    printf("\nquantum = %d\n", quantum);
    // memoryChoose();
}

void clearResources(int signum)
{
    msgctl(messageQueueKey, IPC_RMID, (struct msqid_ds *)0);
    raise(SIGINT);
    exit(0);
    // TODO Clears all resources in case of interruption
}
