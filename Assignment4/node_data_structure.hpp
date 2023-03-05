#ifndef NODE_DATA_STRUCTURE_HPP
#define NODE_DATA_STRUCTURE_HPP

#include <queue>
using namespace std;

class Node{
    public:
    int node_id;
    queue<int> wall_queue;
    queue<int> feed_queue;
    bool cronological_order;

    Node();
    Node(int);
};

#endif