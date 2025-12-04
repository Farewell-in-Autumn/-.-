#ifndef MIGONG_H
#define MIGONG_H

#include "raylib.h"
#include <vector>
#include <string>
#include <fstream>

using Pos = std::pair<int, int>;

// 地块类型
enum DiKuaiLeiXing {
    QIDIAN = -1,    // 起点
    ZHONGDIAN = -2, // 终点
    PUTONG = 0,     // 普通地面
    QIANG = 1,      // 墙
    CAODI = 2,      // 草地
    RONGYAN = 3     // 熔岩
};

class MiGong {
private:
    std::vector<std::vector<int>> ditu;  // 地图数据
    int hangshu;                         // 行数
    int lieshu;                          // 列数
    Vector2 qidianWeizhi;                // 起点位置
    Vector2 zhongdianWeizhi;             // 终点位置

    // 地块图片
    Texture2D qiangTupian;
    Texture2D caodiTupian;
    Texture2D rongyanTupian;
    Texture2D putongTupian;
    Texture2D qidianTupian;
    Texture2D zhongdianTupian;

    // 绘制偏移量
    Vector2 huizhiPianyi;

public:
    MiGong();
    ~MiGong();

    // 加载迷宫文件
    bool JiaZaiDiTu(const std::string& wenjianming);

    // 加载图片
    bool JiaZaiTuPian();

    // 释放图片资源
    void ShiFangTuPian();

    // 绘制地图
    void HuiZhi();

    // 计算绘制偏移量（使地图居中）
    void JisuanPianyi(int pingmuKuan, int pingmuGao);

    // 获取地图信息
    std::string HuoQuDiTuXinxi() const;

    // 获取起点位置
    Pos GetQidian() const {
        return { static_cast<int>(qidianWeizhi.y),
              static_cast<int>(qidianWeizhi.x) };
    }

    // 获取终点位置  
    Pos GetZhongdian() const {
        return { static_cast<int>(zhongdianWeizhi.y),
              static_cast<int>(zhongdianWeizhi.x) };
    }

    // 获取迷宫数据引用
    const std::vector<std::vector<int>>& GetDitu() const { return ditu; }

    // 获取绘制偏移量（需要添加成员变量访问）
    float GetPianyiX() const { return huizhiPianyi.x; }
    float GetPianyiY() const { return huizhiPianyi.y; }


    
};



#endif // MIGONG_H