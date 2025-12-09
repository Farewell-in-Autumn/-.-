#pragma once
#ifndef LUJING_H
#define LUJING_H

#include <vector>
#include <utility>
#include <queue>
#include <stack>

// 简化类型定义
using Pos = std::pair<int, int>;
using Path = std::vector<Pos>;
// 节点状态结构体
struct NodeState {
    int x;
    int y;
    int usedLava;  // 0: 未使用熔岩，1: 已使用熔岩
    int cost;

    NodeState(int x, int y, int usedLava, int cost = 0)
        : x(x), y(y), usedLava(usedLava), cost(cost) {
    }

    // 用于优先队列比较
    bool operator>(const NodeState& other) const {
        return cost > other.cost;
    }
};

class LujingSousuo {
private:
    std::vector<std::vector<int>> maze;
    int rows, cols;
    Pos start, end;

    // 获取邻居位置
    std::vector<Pos> getNeighbors(int x, int y);
    // 获取带状态的邻居
    std::vector<NodeState> getNeighborsWithState(const NodeState& node);

    // 检查位置是否有效
    bool isValid(int x, int y);
    // 获取地块的移动成本
    int getMoveCost(int tileType, int usedLava);

public:
    LujingSousuo(const std::vector<std::vector<int>>& m, Pos s, Pos e);

    // 深度优先搜索
    Path dfs();

    // 广度优先搜索  
    Path bfs();
    // Dijkstra算法 - 允许踩一次熔岩地板的最短路径
    Path dijkstraWithOneLava(int& totalCost);

    // A*算法 - 允许踩一次熔岩地板
    Path aStarWithOneLava(int& totalCost);
};

#endif // LUJING_H