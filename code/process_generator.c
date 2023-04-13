#include "headers.h"
#include <stdio.h>
#include "PriorityQueue.h"
#include <string.h>
#include <stdlib.h>

void clearResources(int);
void readFile();
void schedulingChoose();
struct PriorityQueue pq;

int quantum;
char choice;
char qr[5];
char countProcess[5];
int msgid;
key_t messageQueueKey;
int main(int argc, char *argv[])
{

    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock
    setpqueue(&pq);

    readFile();
    print_priority_queue(&pq);
    schedulingChoose();
    sprintf(countProcess, "%d", pq.count); // Convert integer to string
    int pid = fork();
    if (pid == 0)
    {
        execl("scheduler.out", "", &choice, &qr, &countProcess, NULL); // clk
    }

    pid = fork();
    if (pid == 0)
    {
        execl("clk.out", "", NULL); // scheduler
    }
    initClk();
    // To get time use this
    int x = 0;
    messageQueueKey = ftok("tempfile", 'a');
    msgid = msgget(messageQueueKey, 0666 | IPC_CREAT);
    if (msgid == -1)
        printf("\nError in creating msgQ\n");
    while (1)
    {

        if (x == getClk())
        {
            x++;
            if(!isEmpty(&pq))
            {
            struct PCB temp = peek(&pq);
            /*if (temp.id == -1)
            {
                dequeue(&pq);
                msgsnd(messageQueueKey, &temp, sizeof(&temp), !IPC_NOWAIT);
                break;
            }*/
            while (!isEmpty(&pq)&& temp.ArrTime <= x )
            {
                
              temp=dequeue(&pq);
                if (msgsnd(msgid, &temp, sizeof(temp), !IPC_NOWAIT) == -1)
                    printf("\n Error in sending\n");

                   printf("\nSending id : %d\n", temp.id); 
                 if(!isEmpty(&pq)  ) 
                  temp = peek(&pq);
                else 
                break;
                // free(&temp);
            }
            }
            //printf("\nIn process generator current time is : %d\n", x);
        }

        // TODO Generation Main Loop
        // 5. Create a data structure for processes and provide it with its parameters.
        // 6. Send the information to the scheduler at the appropriate time.
        // 7. Clear clock resources
    }
   
    destroyClk(true);
    
}

void readFile()
{

    char *path = "processes.txt";
    FILE *filePtr = fopen(path, "r");
    fscanf(filePtr, "%*[^\n]\n");

    struct PCB temp;
    while (!feof(filePtr))
    {
        //    struct PCB temp;
        int id;      // process id
        int ArrTime; // time of arrival
        int RunTime; // running time (in state of running it must be devremented)
        int Priority;

        fscanf(filePtr, "%d", &id);
        fscanf(filePtr, "%d", &ArrTime);
        fscanf(filePtr, "%d", &RunTime);
        fscanf(filePtr, "%d", &Priority);
        setPCB(&temp, id, ArrTime, RunTime, Priority);
        temp.state=NotStarted;
        enqueue(&pq, temp, temp.ArrTime);
    }
    /* temp.id = -1;
     enqueue(&pq, temp, temp.ArrTime);*/
}
void schedulingChoose()
{
    printf("Choose the scheduling algorithm\n");
    printf("p: for highest priority first\n");
    printf("s: for shortest remaining time next\n");
    printf("r: for round robin\n");
    scanf("%s", &choice);
    quantum = -1;
    if (choice == 'r')
    {

        printf("\nplease enter the quantum value\n");
        scanf("%d", &quantum);
    }
    sprintf(qr, "%d", quantum); // Convert integer to string
}

void clearResources(int signum)
{
    msgctl(messageQueueKey, IPC_RMID, (struct msqid_ds *)0);
    exit(-1);
    // TODO Clears all resources in case of interruption
}
