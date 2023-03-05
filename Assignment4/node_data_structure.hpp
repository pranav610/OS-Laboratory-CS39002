#ifndef NODE_DATA_STRUCTURE_HPP
#define NODE_DATA_STRUCTURE_HPP

#include <queue>
using namespace std;

class{
    public:
    int node_id;
    queue<int> wall_queue;
    queue<int> feed_queue;
};

#endif NODE_DATA_STRUCTURE_HPP