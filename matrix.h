#pragma once
#include <iostream>
#include <vector>

class adjacency_matrix //邻接矩阵
{
    private:

    std::vector<std::vector<int>> data;

    public:
    std::vector<std::vector<int>> ad_to_in();

    //重载[]运算符
    std::vector<int>& operator[](int row);

    adjacency_matrix();
    adjacency_matrix(int size_row, int size_column);
    adjacency_matrix(const std::vector<std::vector<int>>& matrix);

    int size();
    void print();
};

class incidence_matrix//关联矩阵
{
    private:
    std::vector<std::vector<int>> data;

    public:
    // 构造函数
    incidence_matrix();
    incidence_matrix(int size_row, int size_column);
    incidence_matrix(const std::vector<std::vector<int>>& matrix);
    
    // 重载[]运算符
    std::vector<int>& operator[](int row);
    
    std::vector<std::vector<int>> in_to_ad();

    int size();
    void print();
};