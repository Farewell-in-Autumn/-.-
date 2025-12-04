#ifndef MASTER_H
#define MASTER_H

#include <raylib.h>
class MASTER {
public:
    Vector2 position;  // 人物的位置
    float speed;       // 人物移动速度
    Texture2D texture; //人物图片
    Rectangle frameRect;   // 当前帧矩形
    int  numberx = 0;        // 当前帧编号（0~2）
    int  numbery = 0;        // 当前行（方向）
    float frameTimer = 0;  // 帧计时器
    float character_width;//角色图三行四列
    float character_height;
    float timer = 0.0f;//用来记录每两个关键帧之间的时间
    float frametime = 0.15f;  // 每帧 0.15 秒
    float movetime=2.0f;
    float movetimer = 0;
    int move=1;
    
    MASTER(float x, float y, float s = 100.0f);
    ~MASTER();
    void Update();     // 处理键盘并移动人物
    void Draw();       // 画出人物
};

#endif