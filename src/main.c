#include <math.h>
#include <stdio.h>
#include <stdlib.h>


int maina(int argc, char *argv[])
{
    printf("Hey");
}


// Emulator doesn't compile if these aren't here
__attribute__((unused)) void vMainQueueSendPassed(void)
{
    /* This is just an example implementation of the "queue send" trace hook. */
}

__attribute__((unused)) void vApplicationIdleHook(void)
{

}
