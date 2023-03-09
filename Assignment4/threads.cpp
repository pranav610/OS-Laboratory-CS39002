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

void *readPost()
{

    while(1)
    {
        /*Check for any node i whose feed queue has been updated*/
        for(int i = 1; i <= N_NODES; i++)
        {
            pthread_mutex_lock(&lock_nodes[i]);
            char* log = new char[10000];
            char* temp = new char[100];
            /*Get the chronological order*/
            bool chronological_order = nodes[i].get_chronological_order();
            if(chronological_order){
                vector<Action> temp_vector;
                /*Now store the elements in temp vector in increasing order of time*/
                while(nodes[i].feed_queue.size())
                {
                    Action action = nodes[i].feed_queue.front();
                    nodes[i].feed_queue.pop();
                    temp_vector.push_back(action);
                }
                /*Now sort the temp vector on incresing order of time*/
                sort(temp_vector.begin(), temp_vector.end(), [](Action a, Action b){
                    return a.get_timestamp() < b.get_timestamp();
                });

                for(int j=0;j<temp_vector.size();j++)
                {
                    char type[10];
                    if(temp_vector[j].get_action_type() == 1)
                        sprintf(type, "Post");
                    else if(temp_vector[j].get_action_type() == 2)
                        sprintf(type, "Comment");
                    else
                        sprintf(type, "Like");

                    sprintf(temp, "I read action number %d of type %s posted by user %d at time %s" , temp_vector[j].get_action_id(), type, temp_vector[j].get_user_id() , temp_vector[j].get_timestamp());
                    strcat(log, temp);
                    /*Append it to sns.log file*/
                    FILE* fp = fopen("sns.log", "a");
                    fprintf(fp, "%s", log);
                    fclose(fp);
                    free(log);
                }
                    

            }else // Here Priority is set
            {
                vector<pair<int, Action>> temp_vector;
                /*Now store the elements in temp vector on the basis of number of common nodes*/
                while(nodes[i].feed_queue.size())
                {
                    Action action = nodes[i].feed_queue.front();
                    nodes[i].feed_queue.pop();
                    int num_common_nodes = 0;
                    for(auto v : adj_list[i])
                    {
                        if(adj_list[action.get_user_id()].find(v) != adj_list[action.get_user_id()].end())
                            num_common_nodes++;
                    }
                    temp_vector.push_back({num_common_nodes, action});
                }
                /*Now sort the temp vector on decreasing order of number of common nodes*/
                sort(temp_vector.begin(), temp_vector.end(), [](pair<int, Action> a, pair<int, Action> b){
                    return a.first > b.first;
                });

                for(int j=0;j<temp_vector.size();j++)
                {
                    char type[10];
                    if(temp_vector[j].second.get_action_type() == 1)
                        sprintf(type, "Post");
                    else if(temp_vector[j].second.get_action_type() == 2)
                        sprintf(type, "Comment");
                    else
                        sprintf(type, "Like");

                    sprintf(temp, "I read action number %d of type %s posted by user %d at time %s" , temp_vector[j].second.get_action_id(), type, temp_vector[j].second.get_user_id() , temp_vector[j].second.get_timestamp());
                    strcat(log, temp);
                    /*Append it to sns.log file*/
                    FILE* fp = fopen("sns.log", "a");
                    fprintf(fp, "%s", log);
                    fclose(fp);
                    free(log);
                }
            }

            pthread_mutex_unlock(&lock_nodes[i]);
        }
    }
}

void *pushUpdate(void *arg)
{
    // TODO
}