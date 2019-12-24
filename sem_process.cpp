#include <sys/types.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/sem.h>

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
    struct seminfo *__buf;
    void *__pad;
};

void down(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1)
    {
        perror("Error in p()");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf v_op;

    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &v_op, 1) == -1)
    {
        perror("Error in v()");
        exit(-1);
    }
}

using namespace std;
int main()
{
    union semun Semun;
    int counter = 0;
    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL);
    if (shmid == -1)
    {
        perror("error in reversing memory");
        exit(-1);
    }
    int *shmaddr;
    shmaddr = (int *)shmat(shmid, 0, 0);
    if (shmaddr == (void *)-1)
    {
        perror("Error in attach");
        exit(-1);
    }
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL);
    if (semid == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }
    // semaphore value should be zero here
    Semun.val = 1;
    if (semctl(semid, 0, SETVAL, Semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }
    *shmaddr = 0;
    int pid = fork();
    for (int i = 0; i < 1000000; i++)
    {
        down(semid);
        if(pid == 0)
            *shmaddr += 1;
        else 
            *shmaddr -= 1;
        up(semid);
    }
    cout << *shmaddr << endl;
    shmdt(shmaddr);
    return 0;
}