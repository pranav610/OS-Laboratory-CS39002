#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#define MINUTES_TO_SLEEP 2
#define CHILD_COUNT 5
#define CHILD_CHILD_COUNT 10

using namespace std;

int main() {
    while (1)
    {
        for(int i=0; i<CHILD_COUNT; i++)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                for(int j=0; j<CHILD_CHILD_COUNT; j++)
                    pid_t pid2 = fork();
                return 0;
            }
        }
    }
    
}