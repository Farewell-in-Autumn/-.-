#include "man.h"

Man::Man(float x, float y, float s) {
    position = { x, y };
    speed = s;

    // 加载图片（翻转）
    Image img = LoadImage("C:/Users/余林隆/Desktop/壁纸/character.png");
    ImageResize(&img, 300,300 );
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
   
    // 关 键 帧 区 域 矩 形
    
   
    float dt = GetFrameTime();
    bool moving = false;//判断是否行走
   
    if (IsKeyDown(KEY_UP)) { 
        position.y -= speed * dt;
        numbery = 3;
        moving = true;
    }
    if (IsKeyDown(KEY_DOWN)) { 
        position.y += speed * dt;
        numbery = 0;
        moving = true;
    }
    if (IsKeyDown(KEY_LEFT)) { 
        position.x -= speed * dt; 
        numbery = 1;
        moving = true;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        position.x += speed * dt;
        numbery =  2;
        moving = true;
    }
    // 边界限制（防止人物走出窗口）
    float w = (float)GetScreenWidth();
    float h = (float)GetScreenHeight();
    if (position.x < 0) position.x = 0;
    if (position.y < 0) position.y = 0;
    if (position.x > w - character_width)  position.x = w - character_width;
    if (position.y > h - character_height) position.y = h - character_height;
    if (moving) {
        timer += dt;
        if (timer >= frametime) {//已经满了，切换下一个动画动作
            timer = 0;
            numberx= (numberx + 1) % 3; // 每行 3 帧
        }
    }
    else {
        numberx = 1; // 停止时显示站立帧
    }
    // -------------------- 主角受伤逻辑 --------------------
    if (isInvincible) {
        invincibleTime -= dt;
        if (invincibleTime <= 0) {
            isInvincible = false;
        }
    }
    //  受伤提示计时器递减
    if (hurtTextTimer > 0) {
        hurtTextTimer -= dt;
    }

    // 怪物位置来自外部，所以这里不检测，检测写在 main
    // 因为 Man 类不知道 MASTER 的存在
    //更新关键帧矩形区域
     frameRect = { numberx* character_width ,numbery* character_height, character_width , character_height };

}

void Man::Draw() {
    DrawTextureRec(texture, frameRect, position, WHITE);
    // 受伤提示文字
    if (hurtTextTimer > 0) {
        DrawText("-1 HP", position.x, position.y - 20, 20, RED);
    }
}
Man::~Man() {
    UnloadTexture(texture);
}
