#include "migong.h"
#include <cmath>
#include <sstream>


// 修改构造函数，显式初始化所有 Texture2D 成员变量
MiGong::MiGong() : 
    hangshu(0),
    lieshu(0),
    qidianWeizhi({ 0, 0 }),
    zhongdianWeizhi({ 0, 0 }),
    huizhiPianyi({ 0, 0 }),
    qiangTupian({}),
    caodiTupian({}),
    rongyanTupian({}),
    putongTupian({}),
    qidianTupian({}),
    zhongdianTupian({})
{
    // 构造函数体为空
}


MiGong::~MiGong() {
    ShiFangTuPian();
}

bool MiGong::JiaZaiDiTu(const std::string& wenjianming) {
    std::ifstream wenjian(wenjianming);
    if (!wenjian.is_open()) {
        return false;
    }

    // 读取地图尺寸
    wenjian >> hangshu >> lieshu;

    if (hangshu <= 0 || lieshu <= 0) {
        return false;
    }

    // 初始化地图数组
    ditu.resize(hangshu, std::vector<int>(lieshu));

    // 读取地图数据
    for (int i = 0; i < hangshu; ++i) {
        for (int j = 0; j < lieshu; ++j) {
            wenjian >> ditu[i][j];

            // 记录起点和终点位置
            if (ditu[i][j] == QIDIAN) {
                qidianWeizhi.x = j;
                qidianWeizhi.y = i;
            }
            else if (ditu[i][j] == ZHONGDIAN) {
                zhongdianWeizhi.x = j;
                zhongdianWeizhi.y = i;
            }
        }
    }

    wenjian.close();
    return true;
}

bool MiGong::JiaZaiTuPian() {
    // 1. 加载墙图片
    if (FileExists("wall.png")) {
        Image qiangTuxiang = LoadImage("wall.png");
        qiangTupian = LoadTextureFromImage(qiangTuxiang);
        SetTextureFilter(qiangTupian, TEXTURE_FILTER_POINT);
        UnloadImage(qiangTuxiang);
        }

        // 2. 加载草地图片
        if (FileExists("grass.png")) {
            Image caodiTuxiang = LoadImage("grass.png");
            caodiTupian = LoadTextureFromImage(caodiTuxiang);
            SetTextureFilter(caodiTupian, TEXTURE_FILTER_POINT);
            UnloadImage(caodiTuxiang);
        }

        // 3. 加载熔岩图片
        if (FileExists("lava.png")) {
            Image rongyanTuxiang = LoadImage("lava.png");
            rongyanTupian = LoadTextureFromImage(rongyanTuxiang);
            SetTextureFilter(rongyanTupian, TEXTURE_FILTER_POINT);
            UnloadImage(rongyanTuxiang);
        }

        // 4. 加载普通地面图片
        if (FileExists("floor.png")) {
            Image putongTuxiang = LoadImage("floor.png");
            putongTupian = LoadTextureFromImage(putongTuxiang);
            SetTextureFilter(putongTupian, TEXTURE_FILTER_POINT);
            UnloadImage(putongTuxiang);
        }

        // 5. 加载起点图片
        if (FileExists("start.png")) {
            Image qidianTuxiang = LoadImage("start.png");
            qidianTupian = LoadTextureFromImage(qidianTuxiang);
            SetTextureFilter(qidianTupian, TEXTURE_FILTER_POINT);
            UnloadImage(qidianTuxiang);
        }

        // 6. 加载终点图片
        if (FileExists("end.png")) {
            Image zhongdianTuxiang = LoadImage("end.png");
            zhongdianTupian = LoadTextureFromImage(zhongdianTuxiang);
            SetTextureFilter(zhongdianTupian, TEXTURE_FILTER_POINT);
            UnloadImage(zhongdianTuxiang);
        }

        return true;
    };

void MiGong::ShiFangTuPian() {
    UnloadTexture(qiangTupian);
    UnloadTexture(caodiTupian);
    UnloadTexture(rongyanTupian);
    UnloadTexture(putongTupian);
    UnloadTexture(qidianTupian);
    UnloadTexture(zhongdianTupian);
}

void MiGong::HuiZhi() {


    // 绘制所有地块
    for (int i = 0; i < hangshu; ++i) {
        for (int j = 0; j < lieshu; ++j) {
            int x = huizhiPianyi.x + j * 48;
            int y = huizhiPianyi.y + i * 48;

            // 检查地块是否在屏幕外（解决下半部分显示不全问题）
            if (y + 48 < 0 || y > GetScreenHeight() ||
                x + 48 < 0 || x > GetScreenWidth()) {
                continue; // 跳过完全在屏幕外的地块
            }

            // 选择图片
            Texture2D* dangqianTupian = &putongTupian;
            Color yanse = WHITE;

            // 根据地块类型选择图片
            switch (ditu[i][j]) {
            case QIANG:
                dangqianTupian = &qiangTupian;
                break;

            case CAODI:
                dangqianTupian = &caodiTupian;
                // 草地轻微动画效果
                yanse = ColorAlpha(WHITE, 0.9f + 0.1f * sinf(GetTime() * 2));
                break;

            case RONGYAN:
                dangqianTupian = &rongyanTupian;
                // 熔岩动画效果
                yanse = ColorAlpha(WHITE, 0.8f + 0.2f * sinf(GetTime() * 3));
                break;

            case QIDIAN:
                dangqianTupian = &qidianTupian;
                // 起点动画效果
                yanse = ColorAlpha(WHITE, 0.7f + 0.3f * sinf(GetTime() * 4));
                break;

            case ZHONGDIAN:
                dangqianTupian = &zhongdianTupian;
                // 终点动画效果
                yanse = ColorAlpha(WHITE, 0.6f + 0.4f * sinf(GetTime() * 2));
                break;

            default: // PUTONG
                dangqianTupian = &putongTupian;
                break;
            }

            Rectangle destRect = {
                static_cast<float>(x),
                static_cast<float>(y),
                48.0f,
                48.0f
            };

            // 使用 DrawTexturePro 进行精确控制绘制
            DrawTexturePro(
                *dangqianTupian,
                Rectangle{ 0, 0, static_cast<float>(dangqianTupian->width), static_cast<float>(dangqianTupian->height) },
                destRect,
                Vector2{ 0, 0 },
                0.0f,
                yanse
            );

        }
    }
}

void MiGong::JisuanPianyi(int pingmuKuan, int pingmuGao) {
    // 计算地图总尺寸
    int dituKuan = lieshu * 48;
    int dituGao = hangshu * 48;

    // 计算居中偏移量
    huizhiPianyi.x = (pingmuKuan - dituKuan) / 2.0f;
    huizhiPianyi.y = (pingmuGao - dituGao) / 2.0f;

    // 确保偏移量不小于0
    if (huizhiPianyi.x < 0) huizhiPianyi.x = 0;
    if (huizhiPianyi.y < 0) huizhiPianyi.y = 0;
}

std::string MiGong::HuoQuDiTuXinxi() const {
    std::stringstream ss;
    ss << "Map: " << hangshu << "x" << lieshu ;
    ss << " | Start: (" << (int)qidianWeizhi.y << "," << (int)qidianWeizhi.x << ")";
    ss << " | End: (" << (int)zhongdianWeizhi.y << "," << (int)zhongdianWeizhi.x << ")";
    return ss.str();
}
bool MiGong::LoadFromData(const std::vector<std::vector<int>>& mazeData) {
    if (mazeData.empty() || mazeData[0].empty()) {
        return false;
    }

    ditu = mazeData;
    hangshu = ditu.size();        // 替换 rows
    lieshu = ditu[0].size();      // 替换 cols

    // 查找起点和终点
    bool foundStart = false, foundEnd = false;
    for (int i = 0; i < hangshu; i++) {        // 替换 rows  hangshu
        for (int j = 0; j < lieshu; j++) {     // 替换 cols  lieshu
            if (ditu[i][j] == -1) {
                qidianWeizhi = { (float)j, (float)i };  // 替换 qidian  qidianWeizhi
                foundStart = true;
            }
            else if (ditu[i][j] == -2) {
                zhongdianWeizhi = { (float)j, (float)i }; // 替换 zhongdian  zhongdianWeizhi
                foundEnd = true;
            }
        }
    }

    // 如果没有找到起点或终点，设置默认值
    if (!foundStart) {
        qidianWeizhi = { 1, 1 };           // 替换 qidian qidianWeizhi
        if (1 < hangshu && 1 < lieshu) {   // 确保坐标有效
            ditu[1][1] = -1;
        }
    }
    if (!foundEnd) {
        zhongdianWeizhi = { (float)(lieshu - 2), (float)(hangshu - 2) }; // 替换 zhongdian  zhongdianWeizhi
        if ((hangshu - 2) >= 0 && (hangshu - 2) < hangshu &&
            (lieshu - 2) >= 0 && (lieshu - 2) < lieshu) {  // 确保坐标有效
            ditu[hangshu - 2][lieshu - 2] = -2;
        }
    }

    return true;
}