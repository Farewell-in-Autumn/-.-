#include "man.h"

Man::Man(float x, float y, float s) {
    position = { x, y };
    speed = s;
    basespeed = s;
    // 加载图片（翻转）
    Image img = LoadImage("character.png");
    ImageResize(&img, 140,180 );
    texture = LoadTextureFromImage(img);
    UnloadImage(img);
    if (texture.id == 0) {
        TraceLog(LOG_ERROR, "Texture load failed!");
    }
    // 初始化行走帧尺寸
    character_width = texture.width / 3;
    character_height = texture.height / 4;
    //初始化熔岩相关
    isLavaInvincible = false;
    lavaInvincibleTime = 0.0f;
    lavaInvincibleDuration = 1.0f;
    // 默认人物状态（站立，向下）
    numberx = 1;
    numbery = 0;

    // 初始化帧矩形，否则第一帧会不显示
    frameRect = {
        numberx * character_width,
        numbery * character_height,
        character_width,
        character_height
    };
}


void Man::Update() {
    // 如果已经到达终点或死亡，不处理移动
    if (isFinished || isDead) {
        // 只是更新计时器
        if (isFinished && winTextTimer > 0) {
            winTextTimer -= GetFrameTime();
        }
        if (showLavaWarning && lavaWarningTimer > 0) {
            lavaWarningTimer -= GetFrameTime();
            if (lavaWarningTimer <= 0) {
                showLavaWarning = false;
            }
        }
        return;
    }
    float dt = GetFrameTime();
    bool moving = false;

    // 记录计划移动的距离
    float plannedMoveX = 0, plannedMoveY = 0;

    // 1. 先计算玩家想要移动的方向和距离
    if (IsKeyDown(KEY_UP)) {
        plannedMoveY = -speed * dt;
        numbery = 3;
        moving = true;
    }
    if (IsKeyDown(KEY_DOWN)) {
        plannedMoveY = speed * dt;
        numbery = 0;
        moving = true;
    }
    if (IsKeyDown(KEY_LEFT)) {
        plannedMoveX = -speed * dt;
        numbery = 1;
        moving = true;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        plannedMoveX = speed * dt;
        numbery = 2;
        moving = true;
    }

    // 2. 先尝试X轴移动（如果计划有X轴移动）
    if (plannedMoveX != 0 && migongptr != nullptr) {
        float testX = position.x + plannedMoveX; // 测试移动后的X位置

        // 计算测试位置的碰撞箱
        float left = testX + collisionOffset;
        float right = testX + character_width - collisionOffset;
        float top = position.y + collisionOffset;
        float bottom = position.y + character_height - collisionOffset;

        const auto& map = migongptr->GetDitu();
        float offsetX = migongptr->GetPianyiX();
        float offsetY = migongptr->GetPianyiY();

        // 转换为格子坐标
        int gridLeft = static_cast<int>((left - offsetX) / 48);
        int gridRight = static_cast<int>((right - offsetX) / 48);
        int gridTop = static_cast<int>((top - offsetY) / 48);
        int gridBottom = static_cast<int>((bottom - offsetY) / 48);

        // 确保坐标在有效范围内
        if (gridTop >= 0 && gridBottom < map.size() &&
            gridLeft >= 0 && gridRight < map[0].size()) {

            bool canMove = true;

            // 检查碰撞箱覆盖的所有格子
            for (int y = gridTop; y <= gridBottom; ++y) {
                for (int x = gridLeft; x <= gridRight; ++x) {
                    if (map[y][x] == 1) { // 碰到墙
                        canMove = false;
                        break;
                    }
                }
                if (!canMove) break;
            }

            // 如果可以移动，则更新X位置
            if (canMove) {
                position.x = testX;
            }
            // 如果不能移动，就保持原位置（不做任何事）
        }
    }

    // 3. 再尝试Y轴移动（如果计划有Y轴移动）
    if (plannedMoveY != 0 && migongptr != nullptr) {
        float testY = position.y + plannedMoveY; // 测试移动后的Y位置

        // 计算测试位置的碰撞箱（使用可能已更新的X位置）
        float left = position.x + collisionOffset;
        float right = position.x + character_width - collisionOffset;
        float top = testY + collisionOffset;
        float bottom = testY + character_height - collisionOffset;

        const auto& map = migongptr->GetDitu();
        float offsetX = migongptr->GetPianyiX();
        float offsetY = migongptr->GetPianyiY();

        // 转换为格子坐标
        int gridLeft = static_cast<int>((left - offsetX) / 48);
        int gridRight = static_cast<int>((right - offsetX) / 48);
        int gridTop = static_cast<int>((top - offsetY) / 48);
        int gridBottom = static_cast<int>((bottom - offsetY) / 48);

        // 确保坐标在有效范围内
        if (gridTop >= 0 && gridBottom < map.size() &&
            gridLeft >= 0 && gridRight < map[0].size()) {

            bool canMove = true;

            // 检查碰撞箱覆盖的所有格子
            for (int y = gridTop; y <= gridBottom; ++y) {
                for (int x = gridLeft; x <= gridRight; ++x) {
                    if (map[y][x] == 1) { // 碰到墙
                        canMove = false;
                        break;
                    }
                }
                if (!canMove) break;
            }

            // 如果可以移动，则更新Y位置
            if (canMove) {
                position.y = testY;
            }
            // 如果不能移动，就保持原位置
        }
    }

    // 4. 如果迷宫指针为空，则直接移动（用于测试）
    if (migongptr == nullptr) {
        position.x += plannedMoveX;
        position.y += plannedMoveY;
    }
    // 4. 【新增】检测当前位置的地块类型并处理效果
    int tileType = GetCurrentTileType();

    // 重置速度为基本速度
    speed = basespeed;

    // 根据地块类型处理
    switch (tileType) {
    case -2: // 终点
        isFinished = true;
        winTextTimer = 3.0f; // 显示3秒胜利信息
        break;

    case 2: // 草地，减速三分之一
        speed = basespeed * 0.33f; // 减速到三分之一
        break;

    case 3: // 熔岩
        // 更新熔岩无敌计时器
        if (isLavaInvincible) {
            lavaInvincibleTime -= dt;
            if (lavaInvincibleTime <= 0) {
                isLavaInvincible = false;
            }
        }

        // 如果不在无敌状态，处理熔岩效果
        if (!isLavaInvincible) {
            lavaStepCount++;

            if (lavaStepCount == 1) {
                // 第一次踩到熔岩，显示警告并进入无敌状态
                showLavaWarning = true;
                lavaWarningTimer = 2.0f;
                isLavaInvincible = true;
                lavaInvincibleTime = lavaInvincibleDuration;
            }
            else if (lavaStepCount >= 2) {
                // 第二次踩到熔岩，游戏失败
                isDead = true;
            }
        }
        break;
    default:
        // 其他地块不做特殊处理
        break;
    }

    
    // 5. 窗口边界限制
    float w = (float)GetScreenWidth();
    float h = (float)GetScreenHeight();
    if (position.x < 0) position.x = 0;
    if (position.y < 0) position.y = 0;
    if (position.x > w - character_width) position.x = w - character_width;
    if (position.y > h - character_height) position.y = h - character_height;

    // 6. 动画更新
    if (moving) {
        timer += dt;
        if (timer >= frametime) {
            timer = 0;
            numberx = (numberx + 1) % 3;
        }
    }
    else {
        numberx = 1;
    }

    // 7. 受伤逻辑
    if (isInvincible) {
        invincibleTime -= dt;
        if (invincibleTime <= 0) {
            isInvincible = false;
        }
    }

    if (hurtTextTimer > 0) {
        hurtTextTimer -= dt;
    }// 8. 熔岩警告计时器递减
    if (showLavaWarning && lavaWarningTimer > 0) {
        lavaWarningTimer -= dt;
        if (lavaWarningTimer <= 0) {
            showLavaWarning = false;
        }
    }

    // 8. 更新帧矩形
    frameRect = {
        numberx * character_width,
        numbery * character_height,
        character_width,
        character_height
    };
}
int Man::GetCurrentTileType() {
    if (migongptr == nullptr) return 0; // 默认普通地面

    const auto& map = migongptr->GetDitu();
    float offsetX = migongptr->GetPianyiX();
    float offsetY = migongptr->GetPianyiY();

    // 计算人物中心点
    float centerX = position.x + character_width / 2.0f;
    float centerY = position.y + character_height / 2.0f;

    // 将屏幕坐标转换为地图格子坐标
    int gridX = static_cast<int>((centerX - offsetX) / 48);
    int gridY = static_cast<int>((centerY - offsetY) / 48);

    // 检查格子坐标是否有效
    if (gridY >= 0 && gridY < map.size() && gridX >= 0 && gridX < map[0].size()) {
        return map[gridY][gridX];
    }

    return 0; // 默认普通地面
}
void Man::Draw() {
    DrawTextureRec(texture, frameRect, position, WHITE);
    // 受伤提示文字
    if (hurtTextTimer > 0) {
        DrawText("-1 HP", position.x, position.y - 20, 20, RED);
    } // 熔岩警告
    if (showLavaWarning && lavaWarningTimer > 0) {
        const char* warningText = "WARNING: You stepped on lava! Step again to die!";
        int textWidth = MeasureText(warningText, 30);
        int screenWidth = GetScreenWidth();
        DrawText(warningText, (screenWidth - textWidth) / 2, 50, 30, ORANGE);
    }// 胜利显示
    if (isFinished && winTextTimer > 0) {
        const char* winText = "VICTORY! You reached the destination!";
        int textWidth = MeasureText(winText, 50);
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        // 半透明背景
        DrawRectangle(0, screenHeight / 2 - 60, screenWidth, 120, Fade(BLACK, 0.7f));

        // 胜利文字
        DrawText(winText, (screenWidth - textWidth) / 2, screenHeight / 2 - 25, 50, GREEN);

        // 倒计时提示
        std::string countdownText = "Game ends in " + std::to_string((int)ceil(winTextTimer)) + " seconds";
        int countdownWidth = MeasureText(countdownText.c_str(), 20);
        DrawText(countdownText.c_str(), (screenWidth - countdownWidth) / 2, screenHeight / 2 + 35, 20, YELLOW);
    }

    // 游戏失败显示（熔岩）
    if (isDead) {
        const char* loseText = "GAME OVER! You were burned by lava!";
        int textWidth = MeasureText(loseText, 50);
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        // 半透明背景
        DrawRectangle(0, screenHeight / 2 - 60, screenWidth, 120, Fade(BLACK, 0.7f));

        // 失败文字
        DrawText(loseText, (screenWidth - textWidth) / 2, screenHeight / 2 - 25, 50, RED);

        // 提示重新开始
        const char* restartText = "Press R to restart the game";
        int restartWidth = MeasureText(restartText, 30);
        DrawText(restartText, (screenWidth - restartWidth) / 2, screenHeight / 2 + 35, 30, YELLOW);
    }
}

void Man::setMigong(MiGong* m) {
    migongptr = m;
}
Man::~Man() {
    UnloadTexture(texture);
}
