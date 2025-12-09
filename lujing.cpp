#include "lujing.h"
#include <vector>
#include <utility>
#include <queue>
#include <stack>
#include <limits>
#include <cmath>
#include <unordered_set>
#include <tuple>
LujingSousuo::LujingSousuo(const std::vector<std::vector<int>>& m, Pos s, Pos e)
    : maze(m), rows(m.size()), cols(m[0].size()), start(s), end(e) {
}

bool LujingSousuo::isValid(int x, int y) {
    return x >= 0 && x < rows && y >= 0 && y < cols && maze[x][y] != 1;
}

std::vector<Pos> LujingSousuo::getNeighbors(int x, int y) {
    std::vector<Pos> neighbors;
    int dirs[4][2] = { {-1,0}, {1,0}, {0,-1}, {0,1} };

    for (auto& d : dirs) {
        int nx = x + d[0];
        int ny = y + d[1];
        if (isValid(nx, ny)) {
            neighbors.push_back({ nx, ny });
        }
    }
    return neighbors;
}

Path LujingSousuo::dfs() {
    std::stack<std::pair<Pos, Path>> stk;
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

    stk.push({ start, {start} });
    visited[start.first][start.second] = true;

    while (!stk.empty()) {
        auto [pos, path] = stk.top();
        stk.pop();

        int x = pos.first, y = pos.second;

        if (pos == end) {
            return path;
        }

        for (auto& nb : getNeighbors(x, y)) {
            int nx = nb.first, ny = nb.second;
            if (!visited[nx][ny]) {
                visited[nx][ny] = true;
                Path newPath = path;
                newPath.push_back(nb);
                stk.push({ nb, newPath });
            }
        }
    }
    return {};
}

Path LujingSousuo::bfs() {
    std::queue<std::pair<Pos, Path>> q;
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

    q.push({ start, {start} });
    visited[start.first][start.second] = true;

    while (!q.empty()) {
        auto [pos, path] = q.front();
        q.pop();

        int x = pos.first, y = pos.second;

        if (pos == end) {
            return path;
        }

        for (auto& nb : getNeighbors(x, y)) {
            int nx = nb.first, ny = nb.second;
            if (!visited[nx][ny]) {
                visited[nx][ny] = true;
                Path newPath = path;
                newPath.push_back(nb);
                q.push({ nb, newPath });
            }
        }
    }
    return {};
}
int LujingSousuo::getMoveCost(int tileType, int usedLava) {
    switch (tileType) {
    case 0:  // 普通地块
    case -1: // 起点
    case -2: // 终点
        return 1;
    case 2:  // 草地
        return 3;
    case 3:  // 熔岩
        if (usedLava == 0) {
            return 0;  // 第一次踩熔岩免费
        }
        else {
            return 1000;  // 第二次踩熔岩高成本
        }
    default:
        return 1;
    }
}
std::vector<NodeState> LujingSousuo::getNeighborsWithState(const NodeState& node) {
    std::vector<NodeState> neighbors;
    int dirs[4][2] = { {-1,0}, {1,0}, {0,-1}, {0,1} };

    for (auto& d : dirs) {
        int nx = node.x + d[0];
        int ny = node.y + d[1];

        if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && maze[nx][ny] != 1) {
            int tileType = maze[nx][ny];

            // 如果是熔岩且已经使用过熔岩机会，跳过
            if (tileType == 3 && node.usedLava == 1) {
                continue;
            }

            int newUsedLava = node.usedLava;
            if (tileType == 3 && node.usedLava == 0) {
                newUsedLava = 1;  // 标记已使用熔岩机会
            }

            int moveCost = getMoveCost(tileType, node.usedLava);
            neighbors.push_back(NodeState(nx, ny, newUsedLava, node.cost + moveCost));
        }
    }

    return neighbors;
}Path LujingSousuo::dijkstraWithOneLava(int& totalCost) {
    // 三维距离数组：dist[x][y][usedLava]
    std::vector<std::vector<std::vector<int>>> dist(
        rows, std::vector<std::vector<int>>(cols, std::vector<int>(2, std::numeric_limits<int>::max())));

    // 三维前驱数组
    std::vector<std::vector<std::vector<NodeState>>> prev(
        rows, std::vector<std::vector<NodeState>>(cols, std::vector<NodeState>(2, NodeState(-1, -1, -1))));

    // 优先队列（最小堆）
    std::priority_queue<NodeState, std::vector<NodeState>, std::greater<NodeState>> pq;

    // 初始化起点
    dist[start.first][start.second][0] = 0;
    pq.push(NodeState(start.first, start.second, 0, 0));

    while (!pq.empty()) {
        NodeState current = pq.top();
        pq.pop();

        int x = current.x;
        int y = current.y;
        int usedLava = current.usedLava;
        int currentCost = current.cost;

        // 如果当前距离大于记录的距离，跳过
        if (currentCost > dist[x][y][usedLava]) {
            continue;
        }

        // 到达终点
        if (x == end.first && y == end.second) {
            totalCost = currentCost;

            // 重建路径
            Path path;
            NodeState curr = current;

            while (curr.x != -1 && curr.y != -1) {
                path.push_back({ curr.x, curr.y });
                curr = prev[curr.x][curr.y][curr.usedLava];
            }

            std::reverse(path.begin(), path.end());
            return path;
        }

        // 探索邻居
        for (auto& neighbor : getNeighborsWithState(current)) {
            int nx = neighbor.x;
            int ny = neighbor.y;
            int nUsedLava = neighbor.usedLava;
            int newCost = neighbor.cost;

            if (newCost < dist[nx][ny][nUsedLava]) {
                dist[nx][ny][nUsedLava] = newCost;
                prev[nx][ny][nUsedLava] = NodeState(x, y, usedLava, currentCost);
                pq.push(neighbor);
            }
        }
    }

    totalCost = -1;
    return {};
}

Path LujingSousuo::aStarWithOneLava(int& totalCost) {
    // 启发函数：曼哈顿距离
    auto heuristic = [&](int x, int y) {
        return abs(x - end.first) + abs(y - end.second);
        };

    // 三维距离数组
    std::vector<std::vector<std::vector<int>>> gScore(
        rows, std::vector<std::vector<int>>(cols, std::vector<int>(2, std::numeric_limits<int>::max())));

    std::vector<std::vector<std::vector<NodeState>>> cameFrom(
        rows, std::vector<std::vector<NodeState>>(cols, std::vector<NodeState>(2, NodeState(-1, -1, -1))));

    // 优先队列：存储(fScore, state)
    using PQElement = std::tuple<int, int, int, int, int>; // fScore, x, y, usedLava, gScore
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> openSet;

    // 初始化起点
    gScore[start.first][start.second][0] = 0;
    int fScoreStart = heuristic(start.first, start.second);
    openSet.push({ fScoreStart, start.first, start.second, 0, 0 });

    while (!openSet.empty()) {
        auto [fScore, x, y, usedLava, currentGScore] = openSet.top();
        openSet.pop();

        // 到达终点
        if (x == end.first && y == end.second) {
            totalCost = currentGScore;

            // 重建路径
            Path path;
            NodeState curr(x, y, usedLava);

            while (curr.x != -1 && curr.y != -1) {
                path.push_back({ curr.x, curr.y });
                curr = cameFrom[curr.x][curr.y][curr.usedLava];
            }

            std::reverse(path.begin(), path.end());
            return path;
        }

        // 如果当前gScore不是最新的，跳过
        if (currentGScore > gScore[x][y][usedLava]) {
            continue;
        }

        // 探索邻居
        NodeState currentState(x, y, usedLava, currentGScore);
        for (auto& neighbor : getNeighborsWithState(currentState)) {
            int nx = neighbor.x;
            int ny = neighbor.y;
            int nUsedLava = neighbor.usedLava;
            int tentativeGScore = neighbor.cost;

            if (tentativeGScore < gScore[nx][ny][nUsedLava]) {
                // 找到更好的路径
                cameFrom[nx][ny][nUsedLava] = currentState;
                gScore[nx][ny][nUsedLava] = tentativeGScore;
                int fScoreNew = tentativeGScore + heuristic(nx, ny);
                openSet.push({ fScoreNew, nx, ny, nUsedLava, tentativeGScore });
            }
        }
    }

    totalCost = -1;
    return {};
}