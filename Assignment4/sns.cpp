#include <bits/stdc++.h>
#include "threads.hpp"
#include "data_structures.hpp"
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#define N_THR_READ_POST 10
#define N_THR_PUSH_UPDATE 25
#define MAX_QUEUE_SIZE 1000
using namespace std;

/* Shared graph and node data structre between threads */
vector<vector<int>> adj_list;
vector<Node> nodes;

const int N_NODES = 37705;

queue<Action> q1;
queue<int> q2;
vector<bool> is_present(N_NODES, false);
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t q1_added = PTHREAD_COND_INITIALIZER;
pthread_cond_t q1_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t q2_added = PTHREAD_COND_INITIALIZER;
pthread_cond_t q2_full = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_node[N_NODES];
int MAX_DEGREE = 0;
int MAX_QUEUE1_SIZE = 0;
int MAX_QUEUE2_SIZE = 0;
FILE *fp;
int main()
{
    // open file for logging
    fp = fopen("sns.log", "w");
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
    string file_name = "musae_git_edges.csv";
    ifstream fin(file_name);
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

    fin.open(file_name);

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
    MAX_QUEUE2_SIZE = min(N_NODES,100*MAX_DEGREE);
    
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
    pthread_cond_destroy(&q1_added);
    pthread_cond_destroy(&q1_full);
    pthread_cond_destroy(&q2_added);
    pthread_cond_destroy(&q2_full);
    for (int i = 0; i < N_NODES; i++)
        pthread_mutex_destroy(&lock_node[i]);

    fclose(fp);
    return 0;
}