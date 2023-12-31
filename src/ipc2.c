#include "shared.h"

int main(void)
{
    int fd, error, value;
    shared *addr;
    struct stat sb;
    static sem_t *sem;
    const char MSG[] = "world hello";
    /* Create shared memory object and set its size */

    fd = shm_open(SNAME, O_RDWR, 0);
    if (fd == -1)
        errExit("shm_open");

    error = fstat(fd, &sb);
    if (error == -1)
        errExit("fstat");

    /* Map shared memory object */

    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    sem = &(addr->sem);
    if (addr == MAP_FAILED)
        errExit("mmap");

    /*error = msync(addr, sb.st_size, MS_ASYNC);
    error = posix_fadvise(fd, 0, sb.st_size, POSIX_FADV_DONTNEED);
    if (error == -1)
        errExit("msync or posix_fadsive");*/

    error = close(fd);
    if (error == -1)
        errExit("close");

    error = sem_getvalue(sem, &value);
    if (error == -1)
        errExit("sem_getvalue");

    printf("sem= %d\n", value);

    error = mprotect(addr, sb.st_size, PROT_WRITE);
    if (error == -1)
        errExit("mprotect");

    if (strlen(MSG) < LENGTH)
        strncpy(addr->msg, MSG, LENGTH - 1);
    printf("IPC2= %s\n", addr->msg);
    /*sem_wait(sem);*/
    //for (int i = 0; i < 10; i++)
    while(value > 0)
    {
        /*sem_post(sem);*/

        addr->val *= 2;
        printf("IPC2= %d\n", addr->val);

        sleep(1);
        /*sem_wait(sem);*/
        error = sem_getvalue(sem, &value);
        if (error == -1)
            errExit("sem_getvalue");
    }
    /*sem_post(sem);*/

    sem_destroy(sem);
    error = munmap(addr, sb.st_size);
    if (error == -1)
        errExit("munmap");

    error = shm_unlink(SNAME);
    if (error == -1)
        errExit("shm_unlink");

    exit(EXIT_SUCCESS);
}
