#ifndef MONSTERGUNSLAYER_H_INCLUDED
#define MONSTERGUNSLAYER_H_INCLUDED

#include "Monster.h"

class Ally;

class MonsterGunslayer : public Monster {
public:
    MonsterGunslayer(const std::vector<Point>& path)
        : Monster{ path, MonsterType::GUNSLAYER }
    {
        HP = 25;
        v = 45;
        money = 35;
        atk = 3;

        bitmap_switch_freq = 8;

        bitmap_img_ids.clear();
        for (int d = 0; d < 4; ++d) {
            bitmap_img_ids.emplace_back(std::vector<int>{0, 1, 2, 3, 4}); // 先假設 5 幀
        }
    }

    void update() override;
    void draw() override;
    bool can_remove() const override { return die_done; }

    // ★ Gunslayer 射程（可調）
    double shoot_range = 350.0;

private:
    int shoot_freq = 30;          // 每幾 frame 射一次（可調）
    int shoot_cooldown = 0;

    double bullet_speed = 250.0;  // 子彈速度
    double bullet_fly_dist = 800.0;

    // 動畫控制
    bool shooting = false;
    bool dying = false;
    bool die_done = false;

    int anim_counter = 0;
    int anim_freq = 6;          // 每幾 frame 換一格（可調）

    int run_frame = 0;          // 0..7
    int shoot_frame = 0;        // 0..6
    int die_frame = 0;          // 0..14
};

#endif
