#include "headers.h"
#include "PCB.h"

void handler(int sig);
int msgq_id, msgq_id2;
int main()
{
    int l=6;
    signal(SIGINT, handler);
    key_t key_id;
    int rec_val,id=0,arr=0;

    key_id = ftok("tempfile", 'a');               // create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); // create message queue and return id
    key_t key_id2;
    int send_val;
          // create unique ke
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }

    struct PCB message;
    while (l--)
    {
        /* receive all types of messages */
    setPCB(&message,id++,arr++,5,0);
            send_val = msgsnd(msgq_id, &message, sizeof(message), !IPC_NOWAIT);
            if (send_val == -1)
                perror("Errror in send");
        }

    return 0;
}
void handler(int sig)
{

    if (msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0) != 0)
    {
        perror("Error in removing");
        exit(-1);
    }
    exit(0);
}
