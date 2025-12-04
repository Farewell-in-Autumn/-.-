#include"master.h"
#include <random>

std::random_device rd;
std::mt19937 gen(rd());            // 随机数引擎
std::uniform_int_distribution<> dis(0, 4);


MASTER::MASTER(float x, float y, float s) {
    position = { x, y };
    speed = s;

    // 加载图片（翻转）
    Image img = LoadImage("C:/Users/余林隆/Desktop/壁纸/slime.png");
    ImageResize(&img, 300, 300);
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

void MASTER::Draw() {
    DrawTextureRec(texture, frameRect, position, WHITE);
}
MASTER::~MASTER() {
    UnloadTexture(texture);
}
