#include <bits/stdc++.h>
#include "threads.hpp"
#include "data_structures.hpp"
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>

// Lock1, Cond1, q1 => For userSimulator - pushUpdate communication
// Lock2, Cond2, q2 => For pushUpdate - readPost communication
// Action: 1 => Post, 2 => Comment, 3 => Like
// Sns Log format - UserSimulator :tab: Data Name - Value, more tabs for indentation
#define N_NODES 380000

extern vector<vector<int>> adj_list;
extern vector<Node> nodes;
extern queue<Action> q1;
extern queue<int> q2;
extern pthread_mutex_t lock1, lock2;
extern pthread_cond_t cond1, cond2;
extern pthread_mutex_t lock_node[N_NODES];

void *userSimulator(void *arg)
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
            string log = "";

            // sprintf(temp, "UserSimulator\tNode Selected - %d\n", node_id);
            // strcat(log, temp);
            log += "UserSimulator\tNode Selected - " + to_string(node_id) + "\n";

            int degree = adj_list[node_id].size();

            // sprintf(temp, "UserSimulator\tDegree - %d\n", degree);
            // strcat(log, temp);
            log += "UserSimulator\tDegree - " + to_string(degree) + "\n";

            if (!degree)
            {
                cout<<log<<endl;
                continue;
            }

            degree = log2((double)degree);
            // sprintf(temp, "UserSimulator\tLog(Degree) - %d\n", degree);
            // strcat(log, temp);
            // sprintf(temp, "UserSimulator\tActions - ");
            // strcat(log, temp);
            log += "UserSimulator\tLog(Degree) - " + to_string(degree) + "\n";
            log += "UserSimulator\tActions - ";

            for (int i = 0; i < degree; i++)
            {
                int action_type = rand() % 3 + 1;
                if (action_type == 1)
                    log += "Post ";
                else if (action_type == 2)
                    log += "Comment ";
                else
                    log += "Like ";

                Action action(node_id, nodes[node_id].wall_queue.size(), action_type);
                nodes[node_id].wall_queue.push(action);

                pthread_mutex_lock(&lock1);

                q1.push(action);

                pthread_cond_signal(&cond1);

                pthread_mutex_unlock(&lock1);
            }
            
            cout << log << endl;    
            cout<<endl;
        }
        sleep(2 * 60);
    }
}

void *readPost(void* arg)
{

    while (1)
    {
        /*Check for any node i whose feed queue has been updated*/
        pthread_mutex_lock(&lock2);
        pthread_cond_wait(&cond2, &lock2);
        int i = q2.front();
        q2.pop();
        pthread_mutex_unlock(&lock2);

        pthread_mutex_lock(&lock_node[i]);
        string log="";
        /*Get the chronological order*/
        bool chronological_order = nodes[i].get_chronological_order();
        if (chronological_order)
        {
            vector<Action> temp_vector;
            /*Now store the elements in temp vector in increasing order of time*/
            while (nodes[i].feed_queue.size())
            {
                Action action = nodes[i].feed_queue.front();
                nodes[i].feed_queue.pop();
                temp_vector.push_back(action);
            }
            /*Now sort the temp vector on incresing order of time*/
            sort(temp_vector.begin(), temp_vector.end(), [](Action a, Action b)
                 { return a.get_timestamp() < b.get_timestamp(); });

            for (int j = 0; j < (int)temp_vector.size(); j++)
            {
                string type="";
                if (temp_vector[j].get_action_type() == 1)
                    type = "Post";
                else if (temp_vector[j].get_action_type() == 2)
                    type = "Comment";
                else
                    type = "Like";

                // sprintf(temp, "I read action number %d of type %s posted by user %d at time %s", temp_vector[j].get_action_id(), type, temp_vector[j].get_user_id(), temp_vector[j].get_timestamp());
                // strcat(log, temp);

                log += "I read action number " + to_string(temp_vector[j].get_action_id()) + " of type " + type + " posted by user " + to_string(temp_vector[j].get_user_id()) + " at time " + to_string(temp_vector[j].get_timestamp()) + "\n";
                /*Append it to sns.log file*/
                // FILE *fp = fopen("sns.log", "a");
                // fprintf(fp, "%s", log);
                // fclose(fp);
                // free(log);
            }
        }
        else // Here Priority is set
        {
            vector<pair<int, Action>> temp_vector;
            /*Now store the elements in temp vector on the basis of number of common nodes*/
            while (nodes[i].feed_queue.size())
            {
                Action action = nodes[i].feed_queue.front();
                nodes[i].feed_queue.pop();
                int num_common_nodes = 0;
                for (auto v : adj_list[i])
                {
                    if (find(adj_list[action.get_user_id()].begin(), adj_list[action.get_user_id()].end(), v) != adj_list[action.get_user_id()].end())
                        num_common_nodes++;
                }
                temp_vector.push_back({num_common_nodes, action});
            }
            /*Now sort the temp vector on decreasing order of number of common nodes*/
            sort(temp_vector.begin(), temp_vector.end(), [](pair<int, Action> a, pair<int, Action> b)
                 { return a.first > b.first; });

            for (int j = 0; j < (int)temp_vector.size(); j++)
            {
                string type="";
                if (temp_vector[j].second.get_action_type() == 1)
                    type = "Post";
                else if (temp_vector[j].second.get_action_type() == 2)
                    type = "Comment";
                else
                    type = "Like";
                // sprintf(temp, "I read action number %d of type %s posted by user %d at time %s", temp_vector[j].second.get_action_id(), type, temp_vector[j].second.get_user_id(), temp_vector[j].second.get_timestamp());
                // strcat(log, temp);

                log += "I read action number " + to_string(temp_vector[j].second.get_action_id()) + " of type " + type + " posted by user " + to_string(temp_vector[j].second.get_user_id()) + " at time " + to_string(temp_vector[j].second.get_timestamp()) + "\n";
                /*Append it to sns.log file*/
                // FILE *fp = fopen("sns.log", "a");
                // fprintf(fp, "%s", log);
                // fclose(fp);
            }
        }

        pthread_mutex_unlock(&lock_node[i]);
    }
}

void *pushUpdate(void *arg)
{
   return NULL;
}