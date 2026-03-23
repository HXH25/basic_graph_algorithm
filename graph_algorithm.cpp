#include <iostream>
#include <vector>
#include "graph_algorithm.h"
#include "matrix.h"
#include <climits>
#include <cmath>

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

std::vector<int> dijkstra(int start, std::vector<std::vector<int>> graph)
{
    int n = graph.size();
    std::vector<int> weights(n, INT_MAX);
    std::vector<bool> visited(n, false);
    int curr = start;

    visited[curr] = true;
    weights[curr] = 0;

    // 循环 n-1 次，因为起始节点已经处理过了
    for(int count = 0; count < n - 1; count++)
    {
        // 更新当前节点的所有邻居的距离
        for(int i = 0; i < n; i++)
        {
            // 检查是否有边从 curr 到 i，且边的权重为正，且当前节点已访问，且通过 curr 到 i 的距离更短
            if(!visited[i] && graph[curr][i] > 0 && weights[curr] != INT_MAX && 
               weights[curr] + graph[curr][i] < weights[i])
            {
                weights[i] = weights[curr] + graph[curr][i];
            }
        }

        // 选择未访问的节点中距离最小的作为新的当前节点
        int min_weight = INT_MAX;
        int new_curr = -1;
        for(int i = 0; i < n; i++)
        {
            if(!visited[i] && weights[i] < min_weight)
            {
                min_weight = weights[i];
                new_curr = i;
            }
        }

        // 如果没有找到新的节点，说明图不连通，退出循环
        if(new_curr == -1)
            break;

        // 更新当前节点并标记为已访问
        curr = new_curr;
        visited[curr] = true;
    }

    return weights;
}