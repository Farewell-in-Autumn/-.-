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

class LujingSousuo {
private:
    std::vector<std::vector<int>> maze;
    int rows, cols;
    Pos start, end;

    // 获取邻居位置
    std::vector<Pos> getNeighbors(int x, int y);

    // 检查位置是否有效
    bool isValid(int x, int y);

public:
    LujingSousuo(const std::vector<std::vector<int>>& m, Pos s, Pos e);

    // 深度优先搜索
    Path dfs();

    // 广度优先搜索  
    Path bfs();
};

#endif // LUJING_H