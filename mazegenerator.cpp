#include "mazegenerator.h"
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <set>

MazeGenerator::MazeGenerator(int r, int c) : rows(r), cols(c), gen(rd()) {
    maze.resize(rows, std::vector<int>(cols, 1)); // 初始化为全墙
}

bool MazeGenerator::isValid(int x, int y) {
    return x >= 0 && x < rows && y >= 0 && y < cols;
}

void MazeGenerator::recursiveBacktracking(int x, int y, std::vector<std::vector<bool>>& visited) {
    visited[x][y] = true;
    maze[x][y] = 0; // 设置为通道

    // 四个方向
    int dirs[4][2] = { {-2, 0}, {2, 0}, {0, -2}, {0, 2} };

    // 随机打乱方向
    std::vector<int> indices = { 0, 1, 2, 3 };
    std::shuffle(indices.begin(), indices.end(), gen);

    for (int idx : indices) {
        int nx = x + dirs[idx][0];
        int ny = y + dirs[idx][1];

        if (isValid(nx, ny) && !visited[nx][ny]) {
            // 打通中间的墙
            maze[x + dirs[idx][0] / 2][y + dirs[idx][1] / 2] = 0;
            recursiveBacktracking(nx, ny, visited);
        }
    }
}

void MazeGenerator::primAlgorithm() {
    // 初始化全墙
    maze = std::vector<std::vector<int>>(rows, std::vector<int>(cols, 1));

    // 从随机点开始
    std::uniform_int_distribution<> disX(1, rows - 2);
    std::uniform_int_distribution<> disY(1, cols - 2);

    int startX = disX(gen);
    int startY = disY(gen);

    // 确保是奇数坐标（为了更好的迷宫结构）
    if (startX % 2 == 0) startX--;
    if (startY % 2 == 0) startY--;

    maze[startX][startY] = 0;

    // 边界墙列表
    std::vector<std::tuple<int, int, int, int>> walls; // (墙x, 墙y, 邻接单元格1x, 邻接单元格1y)

    // 添加初始墙
    if (startX > 1) walls.push_back({ startX - 1, startY, startX - 2, startY });
    if (startX < rows - 2) walls.push_back({ startX + 1, startY, startX + 2, startY });
    if (startY > 1) walls.push_back({ startX, startY - 1, startX, startY - 2 });
    if (startY < cols - 2) walls.push_back({ startX, startY + 1, startX, startY + 2 });

    while (!walls.empty()) {
        // 随机选择一堵墙
        std::uniform_int_distribution<> dis(0, walls.size() - 1);
        int idx = dis(gen);

        auto [wallX, wallY, cellX, cellY] = walls[idx];
        walls.erase(walls.begin() + idx);

        // 如果邻接单元格是墙，则打通
        if (maze[cellX][cellY] == 1) {
            maze[wallX][wallY] = 0;
            maze[cellX][cellY] = 0;

            // 添加新的墙
            int dirs[4][2] = { {-2, 0}, {2, 0}, {0, -2}, {0, 2} };
            for (auto& dir : dirs) {
                int nx = cellX + dir[0];
                int ny = cellY + dir[1];
                int wx = cellX + dir[0] / 2;
                int wy = cellY + dir[1] / 2;

                if (isValid(nx, ny) && maze[nx][ny] == 1) {
                    walls.push_back({ wx, wy, nx, ny });
                }
            }
        }
    }
}

void MazeGenerator::kruskalAlgorithm() {
    // 初始化全墙，每个单元格都是独立的集合
    maze = std::vector<std::vector<int>>(rows, std::vector<int>(cols, 1));

    // 为每个单元格创建并查集
    std::vector<int> parent(rows * cols);
    for (int i = 0; i < rows * cols; i++) {
        parent[i] = i;
    }

    // 查找函数
    auto find = [&](int x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
        };

    // 合并函数
    auto unite = [&](int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        if (rootX != rootY) {
            parent[rootY] = rootX;
            return true;
        }
        return false;
        };

    // 收集所有可能的墙（只考虑奇数坐标的单元格）
    std::vector<std::tuple<int, int, int, int>> walls;

    for (int x = 1; x < rows - 1; x += 2) {
        for (int y = 1; y < cols - 1; y += 2) {
            maze[x][y] = 0; // 先设置为通道

            if (x > 1) walls.push_back({ x - 1, y, x, y });
            if (y > 1) walls.push_back({ x, y - 1, x, y });
        }
    }

    // 随机打乱墙
    std::shuffle(walls.begin(), walls.end(), gen);

    // 处理每堵墙
    for (auto [wallX, wallY, cellX, cellY] : walls) {
        int cell1 = (wallX)*cols + wallY;
        int cell2 = cellX * cols + cellY;

        // 如果两个单元格属于不同的集合，打通墙
        if (unite(cell1, cell2)) {
            maze[wallX][wallY] = 0;
        }
    }
}

bool MazeGenerator::isReachable(int startX, int startY, int endX, int endY) {
    if (maze[startX][startY] == 1 || maze[endX][endY] == 1) {
        return false;
    }

    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::queue<std::pair<int, int>> q;

    q.push({ startX, startY });
    visited[startX][startY] = true;

    int dirs[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();

        if (x == endX && y == endY) {
            return true;
        }

        for (auto& dir : dirs) {
            int nx = x + dir[0];
            int ny = y + dir[1];

            if (isValid(nx, ny) && !visited[nx][ny] && maze[nx][ny] != 1) {
                visited[nx][ny] = true;
                q.push({ nx, ny });
            }
        }
    }

    return false;
}

std::vector<std::pair<int, int>> MazeGenerator::getAvailableFloorTiles() {
    std::vector<std::pair<int, int>> tiles;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (maze[i][j] == 0) { // 普通地板
                tiles.push_back({ i, j });
            }
        }
    }

    return tiles;
}

std::vector<std::pair<int, int>> MazeGenerator::getAllWallPositions() {
    std::vector<std::pair<int, int>> walls;

    for (int i = 1; i < rows - 1; i++) {
        for (int j = 1; j < cols - 1; j++) {
            if (maze[i][j] == 1) {
                walls.push_back({ i, j });
            }
        }
    }

    return walls;
}

void MazeGenerator::addSpecialTiles(int grassCount, int lavaCount) {
    auto floorTiles = getAvailableFloorTiles();

    if (floorTiles.empty()) return;

    // 随机打乱地板位置
    std::shuffle(floorTiles.begin(), floorTiles.end(), gen);

    // 添加草地
    int grassAdded = 0;
    for (const auto& tile : floorTiles) {
        if (grassAdded >= grassCount) break;

        int x = tile.first;
        int y = tile.second;

        // 确保不是起点或终点附近
        if (abs(maze[x][y]) != 1 && abs(maze[x][y]) != 2) {
            maze[x][y] = 2;
            grassAdded++;
        }
    }

    // 重新获取可用的地板（排除已添加的草地）
    floorTiles = getAvailableFloorTiles();
    std::shuffle(floorTiles.begin(), floorTiles.end(), gen);

    // 添加熔岩
    int lavaAdded = 0;
    for (const auto& tile : floorTiles) {
        if (lavaAdded >= lavaCount) break;

        int x = tile.first;
        int y = tile.second;

        // 确保不是起点或终点附近
        if (abs(maze[x][y]) != 1 && abs(maze[x][y]) != 2) {
            maze[x][y] = 3;
            lavaAdded++;
        }
    }
}

void MazeGenerator::generate(int algorithm, int grassCount, int lavaCount) {
    // 确保尺寸为奇数以便生成更好的迷宫
    if (rows % 2 == 0) rows--;
    if (cols % 2 == 0) cols--;

    // 重置迷宫
    maze = std::vector<std::vector<int>>(rows, std::vector<int>(cols, 1));

    // 选择算法生成迷宫
    switch (algorithm) {
    case 0: // 递归回溯法
    {
        std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
        // 从随机起点开始
        std::uniform_int_distribution<> disX(1, rows - 2);
        std::uniform_int_distribution<> disY(1, cols - 2);
        int startX = disX(gen);
        int startY = disY(gen);
        // 确保是奇数
        if (startX % 2 == 0) startX--;
        if (startY % 2 == 0) startY--;
        recursiveBacktracking(startX, startY, visited);
        break;
    }
    case 1: // Prim算法
        primAlgorithm();
        break;
    case 2: // Kruskal算法
        kruskalAlgorithm();
        break;
    default:
        primAlgorithm();
        break;
    }

    // 生成起点和终点
    generateStartAndEnd();

    // 确保起点和终点可达
    int attempts = 0;
    std::pair<int, int> start = getStart();
    std::pair<int, int> end = getEnd();

    while (!isReachable(start.first, start.second, end.first, end.second) && attempts < 10) {
        generateStartAndEnd();
        start = getStart();
        end = getEnd();
        attempts++;
    }

    // 如果仍然不可达，进行清理
    if (!isReachable(start.first, start.second, end.first, end.second)) {
        cleanupMaze();
    }

    // 添加环路和分支增加趣味性
    addLoopsAndBranches(8);

    // 添加特殊地块
    addSpecialTiles(grassCount, lavaCount);

    // 最终确保可达性
    if (!isReachable(start.first, start.second, end.first, end.second)) {
        // 如果还是不可达，重新打通一条路径
        std::vector<std::pair<int, int>> path;
        std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
        std::queue<std::pair<std::pair<int, int>, std::vector<std::pair<int, int>>>>  q;

        q.push({ start, {start} });
        visited[start.first][start.second] = true;

        int dirs[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

        while (!q.empty()) {
            auto [pos, currentPath] = q.front();
            q.pop();

            if (pos == end) {
                path = currentPath;
                break;
            }

            for (auto& dir : dirs) {
                int nx = pos.first + dir[0];
                int ny = pos.second + dir[1];

                if (isValid(nx, ny) && !visited[nx][ny]) {
                    visited[nx][ny] = true;
                    std::vector<std::pair<int, int>> newPath = currentPath;
                    newPath.push_back({ nx, ny });
                    q.push({ {nx, ny}, newPath });
                }
            }
        }

        // 打通路径
        for (const auto& pos : path) {
            maze[pos.first][pos.second] = 0;
        }
    }
}

std::vector<std::vector<int>> MazeGenerator::getMaze() const {
    return maze;
}

std::pair<int, int> MazeGenerator::getStart() const {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (maze[i][j] == -1) {
                return { i, j };
            }
        }
    }
    return { 1, 1 }; // 默认起点
}

std::pair<int, int> MazeGenerator::getEnd() const {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (maze[i][j] == -2) {
                return { i, j };
            }
        }
    }
    return { rows - 2, cols - 2 }; // 默认终点
}

void MazeGenerator::generateStartAndEnd() {
    // 清除可能的旧起点和终点
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (maze[i][j] == -1 || maze[i][j] == -2) {
                maze[i][j] = 0;
            }
        }
    }

    auto floorTiles = getAvailableFloorTiles();
    if (floorTiles.size() < 2) return;

    std::shuffle(floorTiles.begin(), floorTiles.end(), gen);

    // 设置起点
    maze[floorTiles[0].first][floorTiles[0].second] = -1;

    // 设置终点（尽量远离起点）
    int maxDistance = 0;
    std::pair<int, int> bestEnd = floorTiles[1];

    for (size_t i = 1; i < std::min(floorTiles.size(), (size_t)20); i++) {
        int dist = abs(floorTiles[i].first - floorTiles[0].first) +
            abs(floorTiles[i].second - floorTiles[0].second);
        if (dist > maxDistance) {
            maxDistance = dist;
            bestEnd = floorTiles[i];
        }
    }

    maze[bestEnd.first][bestEnd.second] = -2;
}

void MazeGenerator::cleanupMaze() {
    // 移除死胡同
    bool changed = true;

    while (changed) {
        changed = false;

        for (int i = 1; i < rows - 1; i++) {
            for (int j = 1; j < cols - 1; j++) {
                if (maze[i][j] == 0) { // 只处理普通通道
                    int wallCount = 0;

                    if (maze[i - 1][j] == 1) wallCount++;
                    if (maze[i + 1][j] == 1) wallCount++;
                    if (maze[i][j - 1] == 1) wallCount++;
                    if (maze[i][j + 1] == 1) wallCount++;

                    // 如果三面是墙，这是一个死胡同，可以移除
                    if (wallCount >= 3 && maze[i][j] != -1 && maze[i][j] != -2) {
                        maze[i][j] = 1;
                        changed = true;
                    }
                }
            }
        }
    }
}

void MazeGenerator::addLoopsAndBranches(int extraPaths) {
    auto walls = getAllWallPositions();
    std::shuffle(walls.begin(), walls.end(), gen);

    int pathsAdded = 0;

    for (const auto& wall : walls) {
        if (pathsAdded >= extraPaths) break;

        int x = wall.first;
        int y = wall.second;

        // 检查这堵墙是否可以打通（不会创建太大的开放空间）
        int passageCount = 0;

        if (x > 0 && maze[x - 1][y] != 1) passageCount++;
        if (x < rows - 1 && maze[x + 1][y] != 1) passageCount++;
        if (y > 0 && maze[x][y - 1] != 1) passageCount++;
        if (y < cols - 1 && maze[x][y + 1] != 1) passageCount++;

        // 如果墙的两边都有通道，打通它创建环路
        if (passageCount >= 2) {
            maze[x][y] = 0;
            pathsAdded++;
        }
    }
}

void MazeGenerator::printMaze() const {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            switch (maze[i][j]) {
            case 1: std::cout << "██"; break;  // 墙
            case 0: std::cout << "  "; break;  // 通道
            case 2: std::cout << "▓▓"; break;  // 草地
            case 3: std::cout << "??"; break;  // 熔岩
            case -1: std::cout << "S "; break; // 起点
            case -2: std::cout << "E "; break; // 终点
            default: std::cout << "??"; break;
            }
        }
        std::cout << std::endl;
    }
}

bool MazeGenerator::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return false;
    }

    // 写入尺寸
    file << rows << " " << cols << std::endl;

    // 写入迷宫数据
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            file << maze[i][j] << " ";
        }
        file << std::endl;
    }

    file.close();
    return true;
}