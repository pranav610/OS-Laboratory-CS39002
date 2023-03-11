#include "data_structures.hpp"

#include <random>

Node::Node(int node_id = -1) : node_id(node_id), chronological_order(rand() % 2)
{
    this->countActions[0] = 0;
    this->countActions[1] = 0;
    this->countActions[2] = 0;
}

bool Node::get_chronological_order()
{
    return this->chronological_order;
}

int Node::get_node_id()
{
    return this->node_id;
}

Action::Action(int user_id = -1, int action_id = -1, int action_type = 0) : user_id(user_id), action_id(action_id), action_type(action_type)
{
    this->timestamp = time(NULL);
}

int Action::get_user_id()
{
    return this->user_id;
}

int Action::get_action_id()
{
    return this->action_id;
}

int Action::get_action_type()
{
    return this->action_type;
}

time_t Action::get_timestamp()
{
    return this->timestamp;
}
