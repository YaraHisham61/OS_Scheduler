#ifndef pcbn
#define pcbn

typedef short STATE;
#define NotStarted 0
#define Waiting 1
#define Running 2
#define Terminated 3
#define Stopped 4

// defining the PCB struct (Process Control Block)
struct PCB
{
    int id;            // process id
    int ArrTime;       // time of arrival
    int RunTime;       // running time (in state of running it must be devremented)
    int Priority;      // priority (the less the number the higher the priority)
    int WaitTime;      // time spent waiting
    int state;         // ready or waiting or running or terminated
    int TA;            // turnaround time
    int RemainingTime; // RemainingTime time
    int startTime;
    int endTime;
    double WTA; // weighted tur arround time
    int PID;    // pid of the actual created process
    int countq;
    //int memsize;
};

// sets the main prameters of the pcb
//  at the initialinzation the pcb hasn't arrived
void setPCB(struct PCB *pcb, int ID, int ARR, int RUN, int Pr)
{
    pcb->id = ID;
    pcb->ArrTime = ARR;
    pcb->RunTime = RUN;
    pcb->RemainingTime = pcb->RunTime;
    pcb->Priority = Pr;
    pcb->RemainingTime = RUN;
    pcb->state = NotStarted;
    pcb->WaitTime = 0;
    pcb->countq=0;
    //pcb->memsize=size;
}

// sets the value of a pcb by the value of another pcb
void CopyPCB(struct PCB *pcb, struct PCB other)
{
    pcb->id = other.id;
    pcb->ArrTime = other.ArrTime;
    pcb->RunTime = other.RunTime;
    pcb->Priority = other.Priority;
    pcb->WaitTime = other.WaitTime;
    pcb->state = other.state;
    pcb->TA = other.TA;
    pcb->WTA = other.WTA;
    pcb->PID = other.PID;
    pcb->startTime = other.startTime;
    pcb->endTime = other.endTime;
    pcb->RemainingTime = other.RemainingTime;
}

// sets the state
void setState(struct PCB *pcb, STATE s)
{
    pcb->state = s;
}

// sets the wait time
void SetWaitTime(struct PCB *pcb, int wait)
{
    pcb->WaitTime = wait;
}

//
void IncreaseWaitTime(struct PCB *pcb, int NewWait)
{
    pcb->WaitTime = pcb->WaitTime + NewWait;
}

void Start(struct PCB *pcb)
{
    int process = fork();
    if (process == -1)
    {
        printf("error in forking\n");
    }
    else if (process == 0)
    {
        char sendid = pcb->id + '0';
        char *sendingid = &sendid;
        char *argv[] = {"./process.out", sendingid, 0};
        execve(argv[0], &argv[0], NULL);
    }
}
#endif