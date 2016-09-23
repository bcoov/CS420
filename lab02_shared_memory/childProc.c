#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ipcEx.h"


int main(int argc, char** argv)
{
    printf("CHILD: Received %d arguments\n", argc);
    char data_string[] = "Hello Shared Memory!\n";

    int shm_id = atoi(argv[1]);
    printf("CHILD: Attempting to access segment ID %d...\n", shm_id);

    struct ipc_struct * shared_memory = (struct ipc_struct *) shmat(shm_id, NULL, 0);
    printf("CHILD: Parent requested that I store my data %d times\n",
           shared_memory->repeat_val);

    for (int i = 0; i < shared_memory->repeat_val; ++i) {
        sprintf(shared_memory->data, data_string);
    }
    printf("CHILD: Done copying data, exiting\n");

    shmdt((void *) shared_memory);

    return 0;
}



