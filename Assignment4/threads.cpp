#include <bits/stdc++.h>
#include "threads.hpp"
#include "data_structures.hpp"
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

// Lock1, Cond1, q1 => For userSimulator - pushUpdate communication
// Lock2, Cond2, q2 => For pushUpdate - readPost communication
// Action: 1 => Post, 2 => Comment, 3 => Like
// Sns Log format - UserSimulator :tab: Data Name - Value, more tabs for indentation

void *userSimulator()
{
    while (1)
    {
        set<int> nodes_picked;
        int num_nodes = nodes_picked.size();
        while (num_nodes < 100)
        {
            int node_id = rand() % nodes.size() + 1;
            if (nodes_picked.find(node_id) == nodes_picked.end())
            {
                nodes_picked.insert(node_id);
                num_nodes++;
            }
        }

        for (auto node_id : nodes_picked)
        {
            char *log = new char[10000];
            char *temp = new char[100];

            sprintf(temp, "UserSimulator\tNode Selected - %d\n", node_id);
            strcat(log, temp);

            int degree = adj_list[node_id].size();

            sprintf(temp, "UserSimulator\tDegree - %d\n", degree);
            strcat(log, temp);
            if (!degree)
            {
                printf("%s\n", log);
                free(temp);
                free(log);
                continue;
            }

            sprintf(temp, "UserSimulator\tLog(Degree) - %d\n", degree);
            strcat(log, temp);
            sprintf(temp, "UserSimulator\tActions - %d", degree);
            strcat(log, temp);

            degree = log2((double)degree);
            for (int i = 0; i < degree; i++)
            {
                int action_type = rand() % 3 + 1;
                if (action_type == 1)
                    sprintf(temp, "Post ");
                else if (action_type == 2)
                    sprintf(temp, "Comment ");
                else
                    sprintf(temp, "Like ");
                
                strcat(log, temp);

                Action action(node_id, nodes[node_id].wall_queue.size(), action_type);
                nodes[node_id].wall_queue.push(action);

                pthread_mutex_lock(&lock1);

                q1.push(action);

                pthread_cond_signal(&cond1);

                pthread_mutex_unlock(&lock1);
            }

            cout<<log<<endl;
            free(log);
            free(temp);
        }
        sleep(2 * 60);
    }
}

void *readPost(void *arg)
{
    // TODO
}

void *pushUpdate(void *arg)
{
    // TODO
}