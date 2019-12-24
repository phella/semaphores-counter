#include <sys/types.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

using namespace std;

int main()
{
    int counter = 0;
    int result = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL);
    if (result == -1)
    {
        perror("error in reversing memory");
        exit(-1);
    }
    int *shmaddr;
    shmaddr = (int *)shmat(result, 0, 0);
    if (shmaddr == (void *)-1)
    {
        perror("Error in attach");
        exit(-1);
    }
    cout << shmaddr << endl;
    *shmaddr = 0;
    int pid = fork();
    for (int i = 0; i < 1000000; i++)
    {
        if (pid == 0)
            *shmaddr += 1;
        else
            *shmaddr -= 1;
    }
    cout << *shmaddr << endl;
    shmdt(shmaddr);

    return 0;
}