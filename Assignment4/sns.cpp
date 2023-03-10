#include <bits/stdc++.h>
#include "threads.hpp"
#include "data_structures.hpp"
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#define N_THR_READ_POST 2
#define N_THR_PUSH_UPDATE 2
#define N_NODES 380000
#define MAX_QUEUE_SIZE 1000
using namespace std;

/* Shared graph and node data structre between threads */
vector<vector<int>> adj_list;
vector<Node> nodes;

queue<Action> q1;
queue<int> q2;
vector<bool> is_present(N_NODES, false);
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond11 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond12 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond21 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond22 = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_node[N_NODES];
int MAX_DEGREE = 0;
int MAX_QUEUE1_SIZE = 0;
int MAX_QUEUE2_SIZE = 0;
FILE *fp;
int main()
{
    // open file for logging
    fp = fopen("sns.log", "a");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    /*Intialize lock and cond array*/
    for (int i = 0; i < N_NODES; i++)
        pthread_mutex_init(&lock_node[i], NULL);

    /* Seed the random number generator */
    srand(time(NULL));

    /* Load the static graph to the memory */
    int num_nodes = 0;

    ifstream fin("musae_git_edges_small.csv");
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

    num_nodes++;
    fin.close();

    adj_list.clear();

    adj_list.resize(num_nodes);

    fin.open("musae_git_edges_small.csv");

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

    for(auto adj: adj_list)
        MAX_DEGREE = max(MAX_DEGREE, (int)adj.size());
    
    MAX_QUEUE1_SIZE = 10*(1+log2(MAX_DEGREE)) * 100;
    MAX_QUEUE2_SIZE = N_NODES;
    
    /* Initialize the node data structure */
    // nodes.resize(num_nodes);
    for (int i = 0; i < num_nodes; i++)
    {
        nodes.push_back(Node(i));
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

    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
    pthread_cond_destroy(&cond11);
    pthread_cond_destroy(&cond12);
    pthread_cond_destroy(&cond21);
    pthread_cond_destroy(&cond22);
    for (int i = 0; i < N_NODES; i++)
        pthread_mutex_destroy(&lock_node[i]);

    fclose(fp);
    return 0;
}