#ifndef DATA_STRUCTURES_HPP
#define DATA_STRUCTURES_HPP

#include <queue>
#include<time.h>
using namespace std;

class Action
{
private:
    int user_id;
    int action_id;
    int action_type;
    time_t timestamp;

public:
    Action(int, int, int);

    int get_user_id();
    int get_action_id();
    int get_action_type();
    time_t get_timestamp();
};

class Node
{
private:
    int node_id;
    bool chronological_order;

public:
    queue<Action> wall_queue;
    queue<Action> feed_queue;

    Node(int);

    bool get_chronological_order();
    int get_node_id();
};

#endif