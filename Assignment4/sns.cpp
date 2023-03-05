#include <bits/stdc++.h>
#include "threads.hpp"
#include "node_data_structure.hpp"
#define N_THR_READ_POST 10
#define N_THR_PUSH_UPDATE 25
using namespace std;

/* Shared graph and node data structre between threads */
vector<vector<int>> adj_list;
vector<Node> nodes;

int main()
{
    /* Seed the random number generator */
    srand(time(NULL));

    /* Load the static graph to the memory */
    int num_nodes;

    ifstream fin("musae_git_edges.csv");
    string line;
    getline(fin, line);
    while (getline(fin, line))
    {
        stringstream ss(line);
        string token;
        vector<int> row;
        while (getline(ss, token, ','))
        {
            row.push_back(stoi(token));
        }
        num_nodes = max(num_nodes, max(row[0], row[1]));
    }
    fin.close();

    adj_list.resize(num_nodes);
    fin.open("musae_git_edges.csv");
    getline(fin, line);
    while (getline(fin, line))
    {
        stringstream ss(line);
        string token;
        vector<int> row;
        while (getline(ss, token, ','))
        {
            row.push_back(stoi(token));
        }
        adj_list[row[0]].push_back(row[1]);
        adj_list[row[1]].push_back(row[0]);
    }
    fin.close();

    /* Initialize the node data structure */
    nodes.resize(num_nodes);
    for (int i = 0; i < num_nodes; i++)
    {
        nodes[i] = Node(i);
    }

    /* Creating threads */
    pthread_t userSimulatorThread;
    vector<pthread_t> readPostThreads(N_THR_READ_POST);
    vector<pthread_t> pushUpdateThreads(N_THR_PUSH_UPDATE);

    /* Creating attributes for the threads */
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&userSimulatorThread, &attr, userSimulator, NULL);
    for (auto &thread : readPostThreads)
        pthread_create(&thread, &attr, readPost, NULL);
    for (auto &thread : pushUpdateThreads)
        pthread_create(&thread, &attr, pushUpdate, NULL);

    /* Wait for the threads to exit */
    pthread_join(userSimulatorThread, NULL);
    for (auto &thread : readPostThreads)
        pthread_join(thread, NULL);
    for (auto &thread : pushUpdateThreads)
        pthread_join(thread, NULL);

    return 0;
}