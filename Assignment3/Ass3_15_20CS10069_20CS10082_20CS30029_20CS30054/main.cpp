#include <bits/stdc++.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/shm.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

#define CONSUMER_COUNT 10

int main()
{
    key_t unique_key = ftok("./shared_memory_file", 15);

    if (unique_key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int shmid = shmget(unique_key, 1000, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    char *str = (char *)shmat(shmid, (void *)0, 0);

    string graph = "";
    FILE *fp = fopen("my_graph.txt", "r");

    while (1)
    {
        char c = fgetc(fp);
        if (c == EOF)
            break;

        graph += c;
    }

    memcpy(str, graph.c_str(), graph.length());
    pid_t producer_pid = fork();
    
    if (producer_pid == 0)
    {
        // producer process here
        exit(EXIT_SUCCESS);
    }
    else
    {   
        set<pid_t> consumer_pids;
        for(int i=0; i<CONSUMER_COUNT; ++i)
        {   
            pid_t consumer_pid = fork();
            if (consumer_pid == 0)
            {
                // consumer process here
                exit(EXIT_SUCCESS);
            }
            else
            {
                consumer_pids.insert(consumer_pid);
            }
        }
        for(auto pid: consumer_pids)
            waitpid(pid, NULL, 0);
        waitpid(producer_pid, NULL, 0);
    }

    shmdt(str);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}