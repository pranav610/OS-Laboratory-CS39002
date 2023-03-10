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
// todo remove dupicates (like nodes 1 and 2 may have same neighbour so it will be pushed twice)
extern queue<int> q2;
extern pthread_mutex_t lock1, lock2;
// todo add valid names to cond11, cond12, cond21, cond22
extern pthread_cond_t cond11, cond12, cond21, cond22;
extern pthread_mutex_t lock_node[N_NODES];
extern int MAX_DEGREE;
extern int MAX_QUEUE1_SIZE, MAX_QUEUE2_SIZE;
extern FILE *fp;
extern vector<bool> is_present;

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

            log += "UserSimulator\tNode Selected - " + to_string(node_id) + "\n";

            int degree = adj_list[node_id].size();
            log += "UserSimulator\tDegree - " + to_string(degree) + "\n";

            if (!degree)
                degree = 1;
            else
                degree = 10 * (log2((double)degree) + 1);
            log += "UserSimulator\tActions Generated - " + to_string(degree) + "\n";
            log += "UserSimulator\tActions - ";

            for (int i = 0; i < degree; i++)
            {
                int action_type = rand() % 3 + 1;

                if (action_type == 1)
                {
                    nodes[node_id].countActions[0]++;
                    log += to_string(nodes[node_id].countActions[0]) + "th " + "Post, ";
                }
                else if (action_type == 2)
                {
                    nodes[node_id].countActions[1]++;
                    log += to_string(nodes[node_id].countActions[1]) + "th " + "Comment, ";
                }
                else
                {
                    nodes[node_id].countActions[2]++;
                    log += to_string(nodes[node_id].countActions[2]) + "th " + "Like, ";
                }

                Action action(node_id, nodes[node_id].wall_queue.size(), action_type);
                nodes[node_id].wall_queue.push(action);

                pthread_mutex_lock(&lock1);
                while ((int)q1.size() == MAX_QUEUE1_SIZE)
                    pthread_cond_wait(&cond12, &lock1);
                q1.push(action);
                pthread_cond_signal(&cond11);
                pthread_mutex_unlock(&lock1);
            }
            log += "\n\n";
            cout << log;
            fprintf(fp, "%s", log.c_str());
        }
        fflush(fp);
        sleep(2 * 60);
    }
}

void *pushUpdate(void *arg)
{
    // functionalities: 1) whenever a new action is added to q1, it is pushed to the feed queue of all the nodes in its adjacency list
    while (1)
    {
        pthread_mutex_lock(&lock1);
        while (q1.empty())
            pthread_cond_wait(&cond11, &lock1);
        Action action = q1.front();
        q1.pop();
        pthread_cond_signal(&cond12);
        pthread_mutex_unlock(&lock1);

        for (auto i : adj_list[action.get_user_id()])
        {
            // this thread is a producer for q2
            pthread_mutex_lock(&lock2);
            while ((int)q2.size() == MAX_QUEUE2_SIZE)
                pthread_cond_wait(&cond22, &lock2);
            if (!is_present[i])
                q2.push(i), is_present[i] = true;

            pthread_mutex_lock(&lock_node[i]);
            nodes[i].feed_queue.push(action);
            pthread_mutex_unlock(&lock_node[i]);

            pthread_cond_signal(&cond21);
            pthread_mutex_unlock(&lock2);

            string type = "";
            if (action.get_action_type() == 1)
                type = "Post   ";
            else if (action.get_action_type() == 2)
                type = "Comment   ";
            else
                type = "Like   ";

            // add to sns.log file and print to terminal message in the format: pushUpdate: I pushed action number <action_id> of type <type> posted by user <user_id> at time <timestamp> to the feed queue of node <node_id>
            string msg = "pushUpdate\tI pushed action number " + to_string(action.get_action_id()) + " of type " + type + " posted by user " + to_string(action.get_user_id()) + " at time " + to_string(action.get_timestamp()) + " to the feed queue of node " + to_string(i) + "\n";
            cout << msg;
            fprintf(fp, "%s", msg.c_str());
            fflush(fp);
        }
    }
}

void *readPost(void *arg)
{
    while (1)
    {
        /*Check for any node i whose feed queue has been updated*/
        // this thread is a consumer for q2
        pthread_mutex_lock(&lock2);
        while (q2.empty())
            pthread_cond_wait(&cond21, &lock2);
        int i = q2.front();
        q2.pop();
        is_present[i] = false;
        pthread_cond_signal(&cond22);
        pthread_mutex_unlock(&lock2);

        pthread_mutex_lock(&lock_node[i]);
        queue<Action> temp_queue(nodes[i].feed_queue);
        nodes[i].feed_queue = queue<Action>();
        pthread_mutex_unlock(&lock_node[i]);

        string log = "";
        /*Get the chronological order*/
        bool chronological_order = nodes[i].get_chronological_order();
        if (chronological_order)
        {
            vector<Action> temp_vector;
            /*Now store the elements in temp vector in increasing order of time*/
            while (temp_queue.size())
            {
                Action action = temp_queue.front();
                temp_queue.pop();
                temp_vector.push_back(action);
            }
            /*Now sort the temp vector on incresing order of time*/
            sort(temp_vector.begin(), temp_vector.end(), [](Action a, Action b)
                 { return a.get_timestamp() < b.get_timestamp(); });

            for (int j = 0; j < (int)temp_vector.size(); j++)
            {
                string type = "";
                if (temp_vector[j].get_action_type() == 1)
                    type = "Post";
                else if (temp_vector[j].get_action_type() == 2)
                    type = "Comment";
                else
                    type = "Like";

                log = "readPost\tI read   action number " + to_string(temp_vector[j].get_action_id()) + " of type " + type + " posted by user " + to_string(temp_vector[j].get_user_id()) + " at time " + to_string(temp_vector[j].get_timestamp()) + "\n";

                cout << log;
                fflush(stdout);
                fprintf(fp, "%s", log.c_str());
                fflush(fp);
            }
        }
        else // Here Priority is set
        {
            vector<pair<int, Action>> temp_vector;
            /*Now store the elements in temp vector on the basis of number of common nodes*/
            while (temp_queue.size())
            {
                Action action = temp_queue.front();
                temp_queue.pop();
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
                string type = "";
                if (temp_vector[j].second.get_action_type() == 1)
                    type = "Post";
                else if (temp_vector[j].second.get_action_type() == 2)
                    type = "Comment";
                else
                    type = "Like";

                log = "readPost\tI read   action number " + to_string(temp_vector[j].second.get_action_id()) + " of type " + type + " posted by user " + to_string(temp_vector[j].second.get_user_id()) + " at time " + to_string(temp_vector[j].second.get_timestamp()) + "\n";

                cout << log;
                fflush(stdout);
                fprintf(fp, "%s", log.c_str());
                fflush(fp);
            }
        }
    }
}
