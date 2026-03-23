#include <iostream>
#include <vector>
#include "matrix.h"
#include "graph_algorithm.h"
#include <climits>
#include <cmath>

using namespace std;

int main()
{
    int n = 0;
    cin >> n;
    vector<vector<int>>input_matrix = {};
    for(int i = 0; i < n; ++i)
    {
        vector<int>row = {};
        for(int j = 0; j < n; ++j)
        {
            int input_int = 0;
            cin >> input_int;
            row.push_back(input_int);
        }
        input_matrix.push_back(row);
    }//所有的算法都是基于邻接矩阵的

    //warshall算法使用示例:
    cout << endl;
    cout << "warshall:" << endl;
    adjacency_matrix out_matrix_warshall = warshall(input_matrix);
    out_matrix_warshall.print();
    
    //hamilton算法使用示例(寻找hamilton道路):
    cout << endl;
    cout << "hamilton:" << endl;
    vector<vector<int>> out_matrix_hamilton = hamilton(input_matrix);
    for(int i = 0; i < out_matrix_hamilton.size(); i++)
    {
        for(int j = 0; j < out_matrix_hamilton[i].size(); j++)
        {
            cout << out_matrix_hamilton[i][j] << " ";
        }
        cout << endl;
    }

    //分支定界法使用示例:

    //便宜算法使用示例:

    //Dijkstra算法使用示例:
    cout << endl;
    cout << "Dijkstra:" << endl;
    input_matrix = {};
    for(int i = 0; i < n; ++i)//这里输入的是有权矩阵
    {
        vector<int>row = {};
        for(int j = 0; j < n; ++j)
        {
            int input_int = 0;
            cin >> input_int;
            row.push_back(input_int);
        }
        input_matrix.push_back(row);
    }
    vector<int>out_matrix_dijkstra = dijkstra(0,input_matrix);
    for(int i = 0; i < out_matrix_dijkstra.size(); i++)
    {
        cout << out_matrix_dijkstra[i] << " ";
    }
    cout << endl;

    //Ford算法使用示例:

    //最长路径算法使用示例:

    return 0;
}