#ifndef ALLY_H_INCLUDED
#define ALLY_H_INCLUDED

#include "../Object.h"
#include "../shapes/Rectangle.h"
#include <allegro5/bitmap.h>
#include <vector>

enum class AllyState {
    WALK,
    ATTACK,
    DIE
};

class Monster;

class Ally : public Object {
public:
    Ally(const Point& p, int lane_id);

    void update();
    void draw();

    bool is_dead() const { return HP <= 0 || state == AllyState::DIE; }

    int lane() const { return lane_id; }

public:
    int HP;

private:
    AllyState state = AllyState::WALK;

    double v;          // 移動速度 (px/sec)
    int lane_id;       // 第幾條線 0/1/2

    ALLEGRO_BITMAP* walk_sheet;
    int frame;
    int frame_count;
    int frame_switch_counter;
    int frame_switch_freq;

    Monster* target = nullptr;
    int atk = 2;             // 攻擊力
    int attack_cooldown = 0;
    int attack_freq = 30;
    double attack_range = 40;  // 可攻擊距離（x 差）
    double lane_tolerance = 25; // 同一條線允許的 y 誤差
};

// Ally.h 最下面 namespace 之外
namespace AllyLaneSetting {
    constexpr int lane_count = 3;
    int nearest_lane_id(double y);
    double lane_y_by_id(int id);
}


#endif
