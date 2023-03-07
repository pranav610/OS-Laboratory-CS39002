#include <bits/stdc++.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <chrono>

using namespace std;

#define CONSUMER_COUNT 10
#define CONS_SLEEP 30
unordered_map<int, vector<int>> source_dist, source_par;

set<int> nodes_assigned;

int consumer_process_count;

string file_name;

int new_node_count;

void djkstra(vector<vector<int>> &adj_list)
{
    // auto start = chrono::high_resolution_clock::now();
    FILE *fp = fopen(("./" + file_name + ".txt").c_str(), "w");
    int node_count = adj_list.size();
    for (auto &i : nodes_assigned)
    {
        vector<int> &dist = source_dist[i];
        vector<int> &par = source_par[i];
        dist.resize(node_count);
        par.resize(node_count);

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
}

void djkstra_optimized(vector<vector<int>> &adj_list)
{
    FILE *fp = fopen(("./" + file_name + ".txt").c_str(), "w");
    int node_count = adj_list.size();

    for (int i = node_count - new_node_count; i < node_count; i++)
    {
        vector<int> dist, par;

        dist.resize(node_count);
        par.resize(node_count);

        for (int j = 0; j < node_count; j++)
        {
            dist[j] = INT_MAX;
            par[j] = -1;
        }

        queue<int> q;
        q.push(i);
        dist[i] = 0;

        while (!q.empty())
        {
            int nn = q.front();
            q.pop();

            for (auto &j : adj_list[nn])
            {
                if (dist[j] > dist[nn] + 1)
                {
                    par[j] = nn;
                    dist[j] = dist[nn] + 1;
                    q.push(j);
                }
            }
        }

        if (i % 10 == consumer_process_count)
            source_dist[i] = dist, source_par[i] = par;

        for (auto &node_assigned : nodes_assigned)
        {
            source_dist[node_assigned].resize(node_count, INT_MAX);
            source_par[node_assigned].resize(node_count, -1);
            for (int j = 0; j < node_count; j++)
            {
                if (source_dist[node_assigned][j] > dist[node_assigned] + dist[j])
                {
                    source_dist[node_assigned][j] = dist[node_assigned] + dist[j];
                    source_par[node_assigned][j] = par[j];
                }
            }

            if (i == node_assigned)
                continue;

            int nn = node_assigned, parent = par[nn];
            while (parent != i)
            {
                nn = parent;
                parent = par[nn];
            }
            source_par[node_assigned][i] = nn;
        }
    }
    for (auto &i : nodes_assigned)
    {
        for (int j = 0; j < node_count; j++)
        {
            vector<int> &dist = source_dist[i];
            vector<int> &par = source_par[i];

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
}

int main(int argc, char const *argv[])
{
    consumer_process_count = atoi(argv[1]);
    file_name = string(argv[1]);
    bool optimized = (argc > 2);

    key_t unique_key = ftok("./shared_memory_file", 15);
    srand(time(0));

    if (unique_key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    char *str;
    int prev_node_count = 0;
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
        new_node_count = node_count - prev_node_count;
        bool is_subsequent_iteration = !nodes_assigned.empty();
        int i = (nodes_assigned.empty() ? consumer_process_count : *nodes_assigned.rbegin());
        while (i < node_count)
        {
            nodes_assigned.insert(i);
            i += CONSUMER_COUNT;
        }

        if (is_subsequent_iteration)
        {
            if (optimized)
            {
                djkstra_optimized(adj_list);
            }
            else
            {
                djkstra(adj_list);
            }
        }
        else
        {
            djkstra(adj_list);
        }

        prev_node_count = node_count;
        sleep(CONS_SLEEP);
    }

    shmdt(str);

    return 0;
}