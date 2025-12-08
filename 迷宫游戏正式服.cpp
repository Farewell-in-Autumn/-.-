#include <raylib.h>
#include "migong.h"
#include "lujing.h"
#include "man.h"
#include "master.h"
#include <iostream>
#include <vector>
#include <string>


int main(void) {
    // 设置窗口尺寸（使用你的窗口尺寸）
    int pingmuKuan = 1200;  // 你可以根据需要修改为 1000
    int pingmuGao = 1000;   // 你可以根据需要修改为 500

    InitWindow(pingmuKuan, pingmuGao, "Maze Game");
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
   
    if (!migong.JiaZaiDiTu("migong.txt")) {
        std::cout << "加载地图文件失败" << std::endl;
        CloseWindow();
        return -1;
    }
    if (!migong.JiaZaiTuPian()) {
        std::cout << "加载图片失败" << std::endl;
        CloseWindow();
        return -1;
    }
    migong.JisuanPianyi(pingmuKuan, pingmuGao);
   
    // 路径搜索
    LujingSousuo lujing(migong.GetDitu(), migong.GetQidian(), migong.GetZhongdian());
    Path dfsPath = lujing.dfs();
    Path bfsPath = lujing.bfs();

    bool showDFS = true;
    bool showBFS = true;

    // 创建人物与怪物
    Pos star = migong.GetQidian();
    int playerX = star.second * 48 + migong.GetPianyiX();
    int playerY = star.first * 48 + migong.GetPianyiY();
    Man player(playerX, playerY); // 人物起始位置
    MASTER master(300, 300);
    //传入地图指针
    player.setMigong(&migong);
    master.setMigong(&migong);
    bool gameStarted = false;
    // 加载开始界面的图片
    Image startScreen = LoadImage("C:/Users/余林隆/Desktop/壁纸/生成迷宫游戏开始界面.png");
    ImageResize(&startScreen, pingmuKuan, pingmuGao);  // 确保尺寸与窗口一致
    Texture2D start = LoadTextureFromImage(startScreen);
    UnloadImage(startScreen);

    // 主循环
    while (!WindowShouldClose()) {
        if (!gameStarted) {
            if (IsKeyPressed(KEY_SPACE)) {
                gameStarted = true;
                UnloadTexture(start);
            }
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(start, 0, 0, WHITE);  // 绘制开始界面
            EndDrawing();
            continue;
        }

        // 更新主角与怪物
        player.Update();
        master.Update();
        if (IsKeyPressed(KEY_ONE)) showDFS = !showDFS;
        if (IsKeyPressed(KEY_TWO)) showBFS = !showBFS;

        // 检测主角与怪物的碰撞
        Rectangle playerRect = { player.position.x, player.position.y, player.character_width, player.character_height };
        Rectangle masterRect = { master.position.x, master.position.y, master.character_width, master.character_height };

        if (CheckCollisionRecs(playerRect, masterRect)) {
            if (!player.isInvincible) {
                player.blood -= 1;            // 扣血
                player.hurtTextTimer = 0.5f;   // 设置提示显示 0.5 秒
                player.isInvincible = true;   // 开启无敌
                player.invincibleTime = 2.0f; // 两秒无敌
            }
        }

        // 绘制迷宫和路径
        BeginDrawing();
        ClearBackground(Color{ 50, 50, 50, 255 });

        migong.HuiZhi();  // 绘制迷宫

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

        // 绘制控制说明
        DrawRectangle(pingmuKuan - 210, 10, 200, 70, ColorAlpha(BLACK, 0.7f));
        DrawText("Control instruction:", pingmuKuan - 200, 15, 20, YELLOW);
        DrawText("ESC - Exit the game", pingmuKuan - 200, 45, 16, WHITE);

        // 绘制主角与怪物
        player.Draw();
        master.Draw();

        // 游戏结束显示
        if (player.blood <= 0) {
            DrawText("YOU DIED", 400, 230, 40, RED);
        }

        // 绘制帧率
        DrawFPS(pingmuKuan - 90, pingmuGao - 30);

        EndDrawing();
    }

    // 清理资源
    CloseWindow();
    std::cout << "Game over" << std::endl;
    return 0;
}
