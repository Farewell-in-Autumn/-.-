#pragma once
#ifndef MAZE_GENERATOR_H
#define MAZE_GENERATOR_H

#include <vector>
#include <random>
#include <stack>
#include <queue>
#include <algorithm>

class MazeGenerator {
private:
    int rows, cols;
    std::vector<std::vector<int>> maze;

    // 随机数生成器
    std::random_device rd;
    std::mt19937 gen;

    // 检查坐标是否在迷宫范围内
    bool isValid(int x, int y);

    // 递归回溯法生成迷宫
    void recursiveBacktracking(int x, int y, std::vector<std::vector<bool>>& visited);

    // Prim算法生成迷宫
    void primAlgorithm();

    // Kruskal算法生成迷宫
    void kruskalAlgorithm();

    // 确保起点和终点可达
    bool isReachable(int startX, int startY, int endX, int endY);

    // 添加特殊地块
    void addSpecialTiles(int grassCount, int lavaCount);

    // 获取所有可访问的普通地块
    std::vector<std::pair<int, int>> getAvailableFloorTiles();

    // 获取所有墙的位置
    std::vector<std::pair<int, int>> getAllWallPositions();

public:
    MazeGenerator(int r, int c);

    // 生成迷宫
    void generate(int algorithm = 0, int grassCount = 10, int lavaCount = 5);

    // 获取生成的迷宫
    std::vector<std::vector<int>> getMaze() const;

    // 获取起点和终点
    std::pair<int, int> getStart() const;
    std::pair<int, int> getEnd() const;

    // 打印迷宫到控制台
    void printMaze() const;

    // 保存迷宫到文件
    bool saveToFile(const std::string& filename) const;

    // 随机生成起点和终点
    void generateStartAndEnd();

    // 清理迷宫（移除死胡同）
    void cleanupMaze();

    // 添加更多的分支和环路
    void addLoopsAndBranches(int extraPaths = 5);
};

#endif // MAZE_GENERATOR_H