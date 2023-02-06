#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(){
    pid_t b,a = getpid();
    printf("pid: %d\n", a);
    b = fork();
    if(b == 0)
    {
        printf("child pid: %d\n", getpid());
        printf("parent pid: %d\n", getppid());
    }
    else 
        printf("parent pid: %d\n", getpid());
}
