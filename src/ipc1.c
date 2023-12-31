#include <string.h>

#include "shared.h"

const size_t MEM_SIZE = sizeof(shared);

int main(void)
{
    int fd, error, value;
    shared *addr;
    static sem_t *sem;
    const char MSG[] = "hello world";

    /* Create shared memory object and set its size */

    fd = shm_open(SNAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        errExit("shm_open");

    error = ftruncate(fd, MEM_SIZE);
    if (error == -1)
        errExit("ftruncate");

    /* Map shared memory object */

    addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    sem = &(addr->sem);
    if (addr == MAP_FAILED)
        errExit("mmap");

    /*error = msync(addr, MEM_SIZE, MS_ASYNC);
    error = posix_fadvise(fd, 0, MEM_SIZE, POSIX_FADV_DONTNEED);
    if (error == -1)
        errExit("msync or posix_fadsive");*/

    error = close(fd);
    if (error == -1)
        errExit("close");

    error = sem_init(sem, 2, 0);
    if (error == -1)
        errExit("sem_init");
    
    error = sem_getvalue(sem, &value);
    if (error == -1)
        errExit("sem_getvalue");
    printf("SEM= %d\n", value);

    memset(addr->msg, 0, LENGTH);
    if (strlen(MSG) < LENGTH)
        strncpy(addr->msg, MSG, LENGTH - 1);
    printf("IPC1= %s\n", addr->msg);
    sem_post(sem);
    for (int i = 0; i < 10; i++)
    {
        /*sem_post(sem);*/

        memcpy(&(addr->val), &i, sizeof i);
        printf("IPC1= %d\n", addr->val);

        sleep(1);
        /*sem_wait(sem);*/
    }
    sem_wait(sem);

    error = sem_getvalue(sem, &value);
    if (error == -1)
        errExit("sem_getvalue");
    printf("SEM= %d\n", value);

    /*error = munmap(addr, MEM_SIZE);
    if (error == -1)
        errExit("munmap");*/

    exit(EXIT_SUCCESS);
}
