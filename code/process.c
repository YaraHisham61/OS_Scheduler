#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    while (remainingtime > 0) //remaining time should be shared memory as scheduler updates it with every clk and process should keep track of it :)
    {
        // remainingtime = ??;
    }
    
    destroyClk(false);
    
    return 0;
}
