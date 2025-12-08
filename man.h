#ifndef MAN_H
#define MAN_H

#include <raylib.h>
#include"migong.h"
class Man {
public:

    Vector2 position;  // 人物的位置
    float speed;       // 人物移动速度
    Texture2D texture; //人物图片
    Rectangle frameRect;   // 当前帧矩形
    int  numberx=0 ;        // 当前帧编号（0~2）
    int  numbery=0 ;        // 当前行（方向）
    float frameTimer = 0;  // 帧计时器
    float character_width;//角色图三行四列
    float character_height;
    float timer = 0.0f;//用来记录每两个关键帧之间的时间
    float frametime = 0.15f;  // 每帧 0.15 秒
    float invincibleTime = 0.0f; // 无敌计时器
    bool isInvincible = false;   // 是否处于无敌
    float hurtTextTimer = 0.0f;  //  加这里，受伤提示计时器
    int blood = 3;
    Man(float x, float y, float s = 100.0f);
    MiGong* migongptr = nullptr;//接收迷宫指针
    void setMigong(MiGong* m);
   
    // 碰撞检测相关变量
    float collisionOffset = 8.0f;  // 碰撞边界偏移（让碰撞框比人物小一点）
    ~Man() ;
    void Update();     // 处理键盘并移动人物
    void Draw();       // 画出人物
 
};

#endif

