#include <bits/stdc++.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <iostream>
#include <sstream>

using namespace std;

#define RANGE_LOW_NODES 1
#define RANGE_HIGH_NODES 3
#define RANGE_LOW_EDGES 1
#define RANGE_HIGH_EDGES 3

int main()
{
    key_t unique_key = ftok("./shared_memory_file", 15);
    srand(time(0));

    if (unique_key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int shmid = shmget(unique_key, 0, 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    char *str = (char *)shmat(shmid, (void *)0, 0);

    printf("Producer process started\n");

    for (int _ = 0;; _++)
    {
        stringstream ss;

        ss << str;
        int node_count = 0, node;
        string word;

        while (!ss.eof())
        {
            ss >> word;
            if (stringstream(word) >> node)
                node_count = max(node_count, node);
            word = "";
        }

        int arr[++node_count];
        memset(arr, 0, sizeof(arr));

        stringstream ss2;
        ss2 << str;

        while (!ss2.eof())
        {
            ss2 >> word;
            if (stringstream(word) >> node)
                arr[node]++;
            word = "";
        }

        for (int i = 1; i < node_count; i++)
            arr[i] += arr[i - 1];

        int tot = arr[node_count - 1];

        int m = RANGE_LOW_NODES + rand() % (RANGE_HIGH_NODES - RANGE_LOW_NODES + 1);
        string extra_edges = "";

        for (int i = 0; i < m; ++i)
        {
            int k = RANGE_LOW_EDGES + rand() % (RANGE_HIGH_EDGES - RANGE_LOW_EDGES + 1);
            set<int> already_connected;
            for (int j = 0; j < k; ++j)
            {
                int random_node_selection_param = 1 + rand() % tot;
                int connect_to_node = lower_bound(arr, arr + node_count, random_node_selection_param) - arr;

                if (already_connected.find(connect_to_node) != already_connected.end())
                {
                    j--;
                    continue;
                }
                already_connected.insert(connect_to_node);
                string temp = to_string(i + node_count) + " " + to_string(connect_to_node) + "\n";
                extra_edges += temp;
            }
        }

        string new_graph = string(str) + extra_edges;
        shmdt(str);
        shmctl(shmid, IPC_RMID, NULL);
        shmid = shmget(unique_key, new_graph.length() + 5, IPC_CREAT | 0666);
        str = (char *)shmat(shmid, (void *)0, 0);

        memcpy(str, (new_graph).c_str(), new_graph.length());

        printf("prod-check\n");
        sleep(50);
        if (_ == 2)
            break;
    }

    shmdt(str);

    return 0;
}