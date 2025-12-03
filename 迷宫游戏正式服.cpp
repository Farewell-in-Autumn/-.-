#include <raylib.h>
#include "man.h"
#include"master.h"
int main() {
    
    InitWindow(1000, 500, "Maze Game");
    // 加载开始界面的图片
    Image startScreen = LoadImage("C:/Users/余林隆/Desktop/壁纸/生成迷宫游戏开始界面.png");
    ImageResize(&startScreen,1000, 500);
    Texture2D start= LoadTextureFromImage(startScreen);
    UnloadImage(startScreen);
    bool gameStarted = false;
    //创建主角
    Man player(100, 100);  // 人物起始位置（100,100）
    //创建怪物
    MASTER master(300, 300);
    while (!WindowShouldClose()) {
        // ----------- 开始界面（显示图片） ------------
        if (!gameStarted) {

            if (IsKeyPressed(KEY_SPACE)) {
                gameStarted = true;
                UnloadTexture(start);
            }
            BeginDrawing();
            ClearBackground(RAYWHITE);
            // 绘制整张图片
            DrawTexture(start, 0, 0, WHITE);
            EndDrawing();
            continue;
        }
        player.Update();//实时检测键盘输入
        master.Update();//怪物的行走逻辑
        // ----------- 检测主角撞到怪物 --------------
        Rectangle playerRect = {
            player.position.x,
            player.position.y,
            player.character_width,
            player.character_height
        };

        Rectangle masterRect = {
            master.position.x,
            master.position.y,
            master.character_width,
            master.character_height
        };

        if (CheckCollisionRecs(playerRect, masterRect)) {
            // 如果当前不是无敌状态
            if (!player.isInvincible) {
                player.blood -= 1;            // 扣血
                player.hurtTextTimer = 0.5f;   //  设置提示显示 0.5 秒
                player.isInvincible = true;   // 开启无敌
                player.invincibleTime = 2.0f; // 两秒无敌
            }
        }
        BeginDrawing();//开始绘制
        ClearBackground(RAYWHITE);//绘制背景

        player.Draw();//绘制人物
        master.Draw();//绘制怪物
        if (player.blood <= 0) {
            DrawText("YOU DIED", 400, 230, 40, RED);
        }
        DrawText("Maze Game Demo", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
