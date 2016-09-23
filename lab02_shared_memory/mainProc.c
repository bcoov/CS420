#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "ipcEx.h"


int main(int argc, char** argv)
{

    /* Print out usage statement when no value is specified */
    if (argc != 2) {
        printf("You must specify an integer value as an argument\n");
        return 1;
    }

    int shmem = shmget(IPC_PRIVATE, sizeof(struct ipc_struct), S_IRUSR | S_IWUSR);
    printf("PARENT: Created shared memory with a segment ID of %d\n", shmem);

    struct ipc_struct * shared_memory = (struct ipc_struct*) shmat(shmem, NULL, 0);

    shared_memory->repeat_val = atoi(argv[1]);

    pid_t pid = fork();

    // Child process
    if (pid == 0) {
        execlp("./childProc", (char *) shmem, (char *) NULL);
    } else {
        wait(NULL);

        printf(shared_memory->data);

        shmdt(shared_memory);
        shmctl(shmem, IPC_RMID, NULL);
    }

    return 0;
}

