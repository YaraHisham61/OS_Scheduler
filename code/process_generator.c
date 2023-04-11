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

    int pid = fork();
    if (pid == 0)
    {
        execl("clk.out", "", NULL); // clk
    }

    pid = fork();
    if (pid == 0)
    {
        execl("sc.out", "", &choice, &qr, NULL); // scheduler
    }
    initClk();
    // To get time use this
    int x = 0;
    while (1)
    {
       
        if (x == getClk())
         printf("\nIn process generator current time is : %d\n", x++);
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
        enqueue(&pq, temp, temp.ArrTime);
    }
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
    // TODO Clears all resources in case of interruption
}
