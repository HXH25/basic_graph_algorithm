#include <iostream>
#include <vector>
#include "graph_algorithm.h"

adjacency_matrix warshall(adjacency_matrix graph)//warshall算法:Pjk ← Pjk ∨ （Pji ∧ Pik)
{
    for(int i = 0; i < graph.size(); i++)
    {
        for(int j = 0; j < graph.size(); j++)
        {
            for(int k = 0; k < graph[j].size(); k++)
            {
                graph[j][k] = graph[j][k]|(graph[j][i]&graph[i][k]);
            }
        }
    }

    return graph;
}

adjacency_matrix warshall(adjacency_matrix graph, int i)//重载warshall,支持一步一步模拟
{
    for(int j = 0; j < graph.size(); j++)
    {
        for(int k = 0; k < graph[j].size(); k++)
        {
            graph[j][k] = graph[j][k]|(graph[j][i]&graph[i][k]);
        }
    }

    return graph;
}


std::vector<std::vector<int>>result = {};
void hamilton_one_solution(adjacency_matrix& graph, std::vector<bool>& visited, std::vector<int>curr_route, int curr_position)
{
    if(curr_route.size() == graph.size())
    {
        result.push_back(curr_route);
        return;
    }

    for(int i = 0; i < graph[curr_position].size(); i++)
    {
        if(graph[curr_position][i] != 1) continue;

        if(!visited[i])
        {
            std::vector<bool>new_visited = visited;
            new_visited[i] = true;
            std::vector<int>new_curr_route = curr_route;
            new_curr_route.push_back(i);
            int new_curr_position = i;
            hamilton_one_solution(graph,new_visited,new_curr_route,new_curr_position);
        }

    }

    return;
}
std::vector<std::vector<int>> hamilton(adjacency_matrix graph)
{
    std::vector<bool>visited(graph.size(),false);
    int curr_position = 0;
    std::vector<int>curr_route = {};
    hamilton_one_solution(graph,visited,curr_route,curr_position);

    std::vector<std::vector<int>> return_result = result;
    result = {};
    return return_result;
}