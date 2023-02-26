#include <bits/stdc++.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <iostream>
#include <sstream>

using namespace std;

int main(int argc, char const *argv[])
{
    int consumer_process_count = atoi(argv[1]);
    string file_name = string(argv[1]);
    bool optimized = (argc > 2);

    key_t unique_key = ftok("./shared_memory_file", 15);
    srand(time(0));

    if (unique_key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    char *str;

    for (int _ = 0;; _++)
    {
        int shmid = shmget(unique_key, 0, 0666);
        if (shmid == -1)
        {
            perror("shmget");
            exit(EXIT_FAILURE);
        }

        str = (char *)shmat(shmid, (void *)0, 0);

        stringstream ss;

        ss << str;
        int node_count = 0, node1, node2;
        string word;

        while (!ss.eof())
        {
            ss >> word;
            if (stringstream(word) >> node1)
                node_count = max(node_count, node1);
            word = "";
        }

        vector<vector<int>> adj_list(++node_count);

        stringstream ss2;
        ss2 << str;

        node1 = -1;
        while (!ss2.eof())
        {
            ss2 >> word;
            if (node1 == -1)
            {
                stringstream(word) >> node1;
                word = "";
            }
            else
            {
                stringstream(word) >> node2;
                adj_list[node1].push_back(node2);
                adj_list[node2].push_back(node1);
                word = "";
                node1 = -1;
            }
        }

        int node_set_start = (node_count / 10) * consumer_process_count;
        int node_set_end = (node_count / 10) * (consumer_process_count + 1);

        FILE *fp = fopen(("./" + file_name + ".txt").c_str(), "w");

        for (int i = node_set_start; i < node_set_end; i++)
        {
            int dist[node_count];
            int par[node_count];

            for (int j = 0; j < node_count; j++)
            {
                dist[j] = INT_MAX;
                par[j] = -1;
            }

            set<pair<int, int>> s;
            s.insert({0, i});
            dist[i] = 0;

            while (!s.empty())
            {
                int nn = s.begin()->second;
                s.erase(s.begin());

                for (auto &j : adj_list[nn])
                {
                    if (dist[j] > dist[nn] + 1)
                    {
                        par[j] = nn;
                        s.erase({dist[j], j});
                        dist[j] = dist[nn] + 1;
                        s.insert({dist[j], j});
                    }
                }
            }

            for (int j = 0; j < node_count; j++)
            {
                if (i == j || dist[j] == INT_MAX)
                    continue;

                string path = "";
                int temp = j;
                vector<int> path_nodes;
                while (temp != i)
                {
                    path_nodes.push_back(temp);
                    temp = par[temp];
                }
                path_nodes.push_back(i);

                reverse(path_nodes.begin(), path_nodes.end());

                for (auto &k : path_nodes)
                    path += to_string(k) + " ";

                fprintf(fp, "%s\n", path.c_str());
            }
        }
        fflush(fp);
        fclose(fp);
        if (consumer_process_count == 1)
            printf("cons-check-1\n");

        sleep(30);
        if (_ == 4)
            break;
    }

    shmdt(str);

    return 0;
}