#include "man.h"

Man::Man(float x, float y, float s) {
    position = { x, y };
    speed = s;

    // 加载图片（翻转）
    Image img = LoadImage("C:/Users/余林隆/Desktop/壁纸/5AEC4FEE7B17CE987CA78BF7AE121722.png");
    ImageResize(&img, 140,180 );
    texture = LoadTextureFromImage(img);
    UnloadImage(img);
    if (texture.id == 0) {
        TraceLog(LOG_ERROR, "Texture load failed!");
    }
    // 初始化行走帧尺寸
    character_width = texture.width / 3;
    character_height = texture.height / 4;


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
    }

    // 8. 更新帧矩形
    frameRect = {
        numberx * character_width,
        numbery * character_height,
        character_width,
        character_height
    };
}
void Man::Draw() {
    DrawTextureRec(texture, frameRect, position, WHITE);
    // 受伤提示文字
    if (hurtTextTimer > 0) {
        DrawText("-1 HP", position.x, position.y - 20, 20, RED);
    }
}
void Man::setMigong(MiGong* m) {
    migongptr = m;
}
Man::~Man() {
    UnloadTexture(texture);
}
