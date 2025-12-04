#include "raylib.h"
#include "migong.h"
#include "lujing.h"
#include <iostream>
#include <vector>
#include <string>

int main(void) {
    // 初始化窗口
     int pingmuKuan = 1350;
     int pingmuGao = 1350;

    InitWindow(pingmuKuan, pingmuGao, "migonggame");
    SetTargetFPS(60);

    // 存储图片信息
    std::vector<std::pair<std::string, Texture2D>> loadedImages;

    // 检查并加载图片
    std::string imageFiles[] = { "wall.png", "grass.png", "lava.png",
                                "floor.png", "start.png", "end.png" };

    for (const auto& filename : imageFiles) {
        if (FileExists(filename.c_str())) {
            Image img = LoadImage(filename.c_str());
            Texture2D texture = LoadTextureFromImage(img);
            loadedImages.push_back({ filename, texture });
            UnloadImage(img);
        }
    }

    // 初始化迷宫
    MiGong migong;

    // 加载地图文件
    if (!migong.JiaZaiDiTu("migong.txt")) {
        std::cout << "加载地图文件失败" << std::endl;
        CloseWindow();
        return -1;
    }

    // 加载图片
    if (!migong.JiaZaiTuPian()) {
        std::cout << "加载图片失败" << std::endl;
        CloseWindow();
        return -1;
    }

    // 计算绘制偏移量
    migong.JisuanPianyi(pingmuKuan, pingmuGao);

    int mapAreaHeight = pingmuGao - 140; // 迷宫区域高度
    migong.JisuanPianyi(pingmuKuan, mapAreaHeight);

    // 路径搜索
    LujingSousuo lujing(migong.GetDitu(), migong.GetQidian(), migong.GetZhongdian());
    Path dfsPath = lujing.dfs();
    Path bfsPath = lujing.bfs();

    bool showDFS = true;
    bool showBFS = true;




    // 主循环
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ONE)) showDFS = !showDFS;
        if (IsKeyPressed(KEY_TWO)) showBFS = !showBFS;
        // 绘制
        BeginDrawing();
        {
            // 清除背景
            ClearBackground(Color{ 50, 50, 50, 255 });

            // 绘制迷宫
            migong.HuiZhi();

            // 绘制路径
            float offsetX = migong.GetPianyiX();
            float offsetY = migong.GetPianyiY();

            if (showDFS) {
                for (const auto& pos : dfsPath) {
                    int x = pos.second * 48 + offsetX;
                    int y = pos.first * 48 + offsetY;
                    DrawRectangle(x + 12, y + 12, 24, 24, ColorAlpha(BLUE, 0.6f));
                }
            }

            if (showBFS) {
                for (const auto& pos : bfsPath) {
                    int x = pos.second * 48 + offsetX;
                    int y = pos.first * 48 + offsetY;
                    DrawRectangle(x + 12, y + 12, 24, 24, ColorAlpha(GREEN, 0.6f));
                }
            }

            // 绘制UI
            DrawRectangle(10, 95, 280, 130, ColorAlpha(BLACK, 0.7f));
            DrawText("Maze Path Finder", 20, 100, 22, YELLOW);
            DrawText(showDFS ? "1: DFS ON" : "1: DFS OFF", 20, 135, 16, BLUE);
            DrawText(showBFS ? "2: BFS ON" : "2: BFS OFF", 20, 160, 16, GREEN);
            DrawText(TextFormat("DFS: %d steps", dfsPath.size()), 20, 185, 16, BLUE);
            DrawText(TextFormat("BFS: %d steps", bfsPath.size()), 150, 185, 16, GREEN);

            // 绘制信息面板
            DrawRectangle(10, 10, 400, 80, ColorAlpha(BLACK, 0.7f));
            DrawText("migonggame", 20, 15, 24, YELLOW);
            DrawText(migong.HuoQuDiTuXinxi().c_str(), 20, 50, 18, WHITE);

            // 绘制控制说明
            DrawRectangle(pingmuKuan - 210, 10, 200, 70, ColorAlpha(BLACK, 0.7f));
            DrawText("Control instruction:", pingmuKuan - 200, 15, 20, YELLOW);
            DrawText("ESC - Exit the game", pingmuKuan - 200, 45, 16, WHITE);

            // 在底部显示已加载的图片
            int bottomAreaY = mapAreaHeight; // 图片区域起始Y坐标

            DrawRectangle(0, bottomAreaY, pingmuKuan, 140, ColorAlpha(BLACK, 0.7f));


            // 标题
            DrawText("Loaded Images:", 20, bottomAreaY + 10, 20, WHITE);

            // 显示图片缩略图
            int startX = 20;
            int imageY = bottomAreaY + 40;

            for (size_t i = 0; i < loadedImages.size(); i++) {
                int x = startX + i * 110; // 每个图片间隔110像素

                // 绘制图片（48x48）
                DrawTexture(loadedImages[i].second, x, imageY, WHITE);

                // 绘制文件名
                DrawText(loadedImages[i].first.c_str(), x, imageY + 55, 10, LIGHTGRAY);

                // 简单边框
                DrawRectangleLines(x, imageY, 48, 48, GRAY);
            }

            // 绘制帧率
            DrawFPS(pingmuKuan - 90, pingmuGao - 30);
        }
        EndDrawing();
    }

    // 清理资源
    CloseWindow();

    std::cout << "Game over" << std::endl;
    return 0;
}