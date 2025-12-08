#include"master.h"
#include"migong.h"
#include <random>

std::random_device rd;
std::mt19937 gen(rd());            // 随机数引擎
std::uniform_int_distribution<> dis(0, 4);


MASTER::MASTER(float x, float y, float s) {
    position = { x, y };
    speed = s;

    // 加载图片（翻转）
    Image img = LoadImage("slime.png");
    ImageResize(&img, 144, 192);
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


void MASTER::Update() {

    float dt = GetFrameTime();
    float oldX = position.x;
    float oldY = position.y;


    // 两个独立计时器
    movetimer += dt;
    frameTimer += dt;

    bool moving = false;

    // ---- 状态机：每隔 movetime 秒随机换方向 ----
    if (movetimer >= movetime) {
        move = dis(gen);
        movetimer = 0;
    }

    // ---- 移动逻辑 ----
    switch (move) {
    case 0: moving = false; break;
    case 1: position.y -= speed * dt; numbery = 3; moving = true; break;
    case 2: position.y += speed * dt; numbery = 0; moving = true; break;
    case 3: position.x -= speed * dt; numbery = 1; moving = true; break;
    case 4: position.x += speed * dt; numbery = 2; moving = true; break;
    }

    // ---- 边界限制：撞墙即时换方向 ----
    float w = (float)GetScreenWidth();
    float h = (float)GetScreenHeight();

    if (position.x < 0) { position.x = 0; move = dis(gen); movetimer = 0; }
    if (position.y < 0) { position.y = 0; move = dis(gen); movetimer = 0; }
    if (position.x > w - character_width) { position.x = w - character_width; move = dis(gen); movetimer = 0; }
    if (position.y > h - character_height) { position.y = h - character_height; move = dis(gen); movetimer = 0; }
    // ========== 【新增】迷宫墙壁碰撞检测 ==========
    bool hitWall = false;
    if (migongptr != nullptr && moving) {
        const auto& map = migongptr->GetDitu();
        float offsetX = migongptr->GetPianyiX();
        float offsetY = migongptr->GetPianyiY();

        // 计算怪物预测位置的中心点（用于检测）
        float centerX = position.x + character_width / 2.0f;
        float centerY = position.y + character_height / 2.0f;

        // 根据移动方向，将检测点稍微“前移”，实现预判碰撞
        float checkAhead = 20.0f; // 预判距离，可调整
        if (move == 1) centerY -= checkAhead;      // 上
        else if (move == 2) centerY += checkAhead; // 下
        else if (move == 3) centerX -= checkAhead; // 左
        else if (move == 4) centerX += checkAhead; // 右

        // 将屏幕坐标转换为地图格子坐标
        int gridX = static_cast<int>((centerX - offsetX) / 48);
        int gridY = static_cast<int>((centerY - offsetY) / 48);

        // 检查格子是否有效且为墙 (1)
        if (gridY >= 0 && gridY < map.size() && gridX >= 0 && gridX < map[gridY].size()) {
            if (map[gridY][gridX] == 1) {
                hitWall = true;
            }
        }

        // 如果撞墙，则回退位置并立即改变方向
        if (hitWall) {
            position.x = oldX;
            position.y = oldY;
            move = dis(gen);    // 随机一个新方向
            movetimer = 0;      // 重置方向计时器，保证立即生效
        }
    }
    // ========== 碰撞检测结束 ==========

   
    // ---- 动画帧控制（不影响 moveTimer）----
    if (moving) {
        if (frameTimer >= frametime) {
            numberx = (numberx + 1) % 3;
            frameTimer = 0;
        }
    }
    else {
        numberx = 1;   // 静止站立帧
    }
  

   
    // ---- 更新帧矩形 ----
    frameRect = { numberx * character_width, numbery * character_height, character_width, character_height };

}
void MASTER::setMigong(MiGong* m) {
    migongptr = m;
}
void MASTER::Draw() {
    DrawTextureRec(texture, frameRect, position, WHITE);
}
MASTER::~MASTER() {
    UnloadTexture(texture);
}
