#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    pid = fork();

    if(pid < 0 )
    {
        fprintf(stderr, "Fork Failed");
        exit(-1);
    }
    else if(pid == 0)
    {
        execlp("/bin/ls", "ls", NULL);
        printf("I'm Child Complete\n");
    }
    else
    {
        wait(NULL);
        printf("Child Complete");
        exit(0);
    }
}