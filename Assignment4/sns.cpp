#include <bits/stdc++.h>
#include "threads.hpp"
using namespace std;

/*Your functions here*/

int main()
{
    /* Load the static graph to the memory */
    int num_nodes;
    
    ifstream fin("musae_git_edges.csv");
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
    fin.close();

    vector<vector<int>> adj_list(num_nodes + 1);
    fin.open("musae_git_edges.csv");
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




    return 0;
}