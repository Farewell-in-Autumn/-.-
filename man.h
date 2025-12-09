#ifndef MAN_H
#define MAN_H

#include <raylib.h>
#include"migong.h"
#include"mazegenerator.h"
class Man {
public:

    Vector2 position;  // 人物的位置
    float speed;       // 人物移动速度
    float basespeed;
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
     // 新增状态变量
    bool isFinished = false;      // 是否到达终点
    bool isDead = false;          // 是否死亡
    int lavaStepCount = 0;        // 踩到熔岩的次数
    float lavaWarningTimer = 0.0f; // 熔岩警告计时器
    bool showLavaWarning = false;  // 显示熔岩警告
    float winTextTimer = 3.0f;    // 胜利文本显示时间
    bool isLavaInvincible = false;          // 熔岩无敌状态
    float lavaInvincibleTime = 0.0f;        // 熔岩无敌计时器
    float lavaInvincibleDuration = 1.0f;    // 熔岩无敌持续时间（1秒）
   
    // 添加碰撞检测相关
    bool CheckWallCollision(float newX, float newY);  // 碰撞检测函数
    bool CheckTileCollision(int gridX, int gridY);    // 检测单个格子碰撞
    
    // 获取当前位置的地块类型
    int GetCurrentTileType();  // 新增函数
    // 碰撞检测相关变量
    float collisionOffset = 8.0f;  // 碰撞边界偏移（让碰撞框比人物小一点）
    ~Man() ;
    void Update();     // 处理键盘并移动人物
    void Draw();       // 画出人物
 
};

#endif

