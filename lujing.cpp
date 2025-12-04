#include "lujing.h"
#include <vector>
#include <utility>
#include <queue>
#include <stack>

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