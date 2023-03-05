#include "node_data_structure.hpp"

#include <random>

Node::Node(): node_id(-1), cronological_order(rand() % 2)
{
}

Node::Node(int node_id): node_id(node_id), cronological_order(rand() % 2)
{
}