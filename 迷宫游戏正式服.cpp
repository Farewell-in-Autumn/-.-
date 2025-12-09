#include <raylib.h>
#include "migong.h"
#include "lujing.h"
#include "man.h"
#include "master.h"
#include <iostream>
#include <vector>
#include <string>
#include"mazegenerator.h"

int main(void) {
    // 添加迷宫生成选项
    bool useRandomMaze = false;  // 默认使用固定迷宫
    int currentAlgorithm = 0;     // 当前使用的算法
    // 设置窗口尺寸（使用你的窗口尺寸）
    int pingmuKuan = 1200;  // 你可以根据需要修改为 1000
    int pingmuGao = 1000;   // 你可以根据需要修改为 500
    bool x = true;
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
    // 新增：允许踩一次熔岩的最短路径
    int dijkstraCost = 0;
    Path dijkstraPath = lujing.dijkstraWithOneLava(dijkstraCost);

    int aStarCost = 0;
    Path aStarPath = lujing.aStarWithOneLava(aStarCost);
    bool showDFS = true;
    bool showBFS = true;
    bool showDijkstra = true;
    bool showAStar = true;
    // 创建人物与怪物
    Pos star = migong.GetQidian();
    int playerX = star.second * 48 + migong.GetPianyiX();
    int playerY = star.first * 48 + migong.GetPianyiY();
    Man player(playerX, playerY); // 人物起始位置
    MASTER master(500, 300);
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
        // 在游戏主循环中添加按键检测
        if (IsKeyPressed(KEY_G)) {
            // 生成新的随机迷宫
            MazeGenerator generator(20, 20);
            generator.generate(currentAlgorithm, 15, 8); // 使用当前算法生成

            // 获取新迷宫
            auto newMaze = generator.getMaze();

            // 重新加载迷宫
            migong.LoadFromData(newMaze);

            // 重新计算路径
            LujingSousuo lujingNew(migong.GetDitu(), migong.GetQidian(), migong.GetZhongdian());
            dfsPath = lujingNew.dfs();
            bfsPath = lujingNew.bfs();

            // 重新计算允许踩熔岩的路径
            dijkstraPath = lujingNew.dijkstraWithOneLava(dijkstraCost);
            aStarPath = lujingNew.aStarWithOneLava(aStarCost);

            // 重置玩家位置
            Pos newStart = migong.GetQidian();
            player.position.x = newStart.second * 48 + migong.GetPianyiX();
            player.position.y = newStart.first * 48 + migong.GetPianyiY();
            player.isFinished = false;
            player.isDead = false;
            player.blood = 3;
            player.lavaStepCount = 0;

            std::cout << "生成新迷宫完成！" << std::endl;
        }

        // 切换生成算法
        if (IsKeyPressed(KEY_F1)) {
            currentAlgorithm = 0;
            std::cout << "切换到递归回溯算法" << std::endl;
        }
        if (IsKeyPressed(KEY_F2)) {
            currentAlgorithm = 1;
            std::cout << "切换到Prim算法" << std::endl;
        }
        if (IsKeyPressed(KEY_F3)) {
            currentAlgorithm = 2;
            std::cout << "切换到Kruskal算法" << std::endl;
        }
        // 如果游戏结束（熔岩死亡），检查是否按R键重新开始
        if (player.isDead && IsKeyPressed(KEY_R)) {
            // 重置玩家状态
            player.blood = 3;
            player.isDead = false;
            player.lavaStepCount = 0;
            player.showLavaWarning = false;
            player.isFinished = false;
            player.isLavaInvincible = false;
            player.lavaInvincibleTime = 0.0f;
            // 重置玩家位置到起点
            Pos star = migong.GetQidian();
            player.position.x = star.second * 48 + migong.GetPianyiX();
            player.position.y = star.first * 48 + migong.GetPianyiY();
        }

        // 如果游戏胜利（到达终点）并且计时结束，可以退出或重置游戏
        if (player.isFinished && player.winTextTimer <= 0) {
            // 可以选择自动重新开始或等待按键
            // 例如：按空格键重新开始
            if (IsKeyPressed(KEY_SPACE)) {
                player.isFinished = false;
                player.winTextTimer = 3.0f;

                // 重置玩家位置到起点
                Pos star = migong.GetQidian();
                player.position.x = star.second * 48 + migong.GetPianyiX();
                player.position.y = star.first * 48 + migong.GetPianyiY();
            }
        }

        // 更新主角与怪物
        player.Update();
        master.Update();
        if (IsKeyPressed(KEY_ONE)) showDFS = !showDFS;
        if (IsKeyPressed(KEY_TWO)) showBFS = !showBFS;
        if (IsKeyPressed(KEY_THREE)) showDijkstra = !showDijkstra;
        if (IsKeyPressed(KEY_FOUR)) showAStar = !showAStar;
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
        if (showDijkstra && !dijkstraPath.empty()) {
            for (const auto& pos : dijkstraPath) {
                int x = pos.second * 48 + offsetX;
                int y = pos.first * 48 + offsetY;
                DrawRectangle(x + 12, y + 12, 24, 24, ColorAlpha(RED, 0.6f));
            }
        }

        if (showAStar && !aStarPath.empty()) {
            for (const auto& pos : aStarPath) {
                int x = pos.second * 48 + offsetX;
                int y = pos.first * 48 + offsetY;
                DrawRectangle(x + 12, y + 12, 24, 24, ColorAlpha(YELLOW, 0.6f));
            }
        }
        // 绘制UI
        
        if (IsKeyDown(KEY_SIX)) x = !x;
        if (x==true) {
            DrawRectangle(10, 95, 400, 200, ColorAlpha(BLACK, 0.7f));
            DrawText("Maze Path Finder", 20, 100, 22, YELLOW);
            DrawText(showDFS ? "1: DFS ON" : "1: DFS OFF", 20, 135, 16, BLUE);
            DrawText(showBFS ? "2: BFS ON" : "2: BFS OFF", 20, 160, 16, GREEN);
            DrawText(TextFormat("DFS: %d steps", dfsPath.size()), 20, 185, 16, BLUE);
            DrawText(TextFormat("BFS: %d steps", bfsPath.size()), 150, 185, 16, GREEN);
            DrawText(showDijkstra ? "3: Dijkstra ON" : "3: Dijkstra OFF", 20, 220, 16, RED);
            DrawText(showAStar ? "4: A* ON" : "4: A* OFF", 20, 245, 16, YELLOW);
            if (!dijkstraPath.empty()) {
                DrawText(TextFormat("Dijkstra: %d steps (cost: %d)", dijkstraPath.size(), dijkstraCost),
                    150, 220, 16, RED);
            }
            if (!aStarPath.empty()) {
                DrawText(TextFormat("A*: %d steps (cost: %d)", aStarPath.size(), aStarCost),
                    150, 245, 16, YELLOW);
            }
        }
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
