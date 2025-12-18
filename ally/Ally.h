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
enum class Allytype {
    BLACK_DUDE,
    VIKING_HAMMERMAN
};



class Monster;

class Ally : public Object {
public:
    Ally(const Point& p, int lane_id, Allytype type);
    bool is_dead() const { return HP <= 0 || state == AllyState::DIE; }
    virtual ~Ally() = default;

    virtual void update();
    virtual void draw();

    virtual bool can_remove() const {
        return (state == AllyState::DIE && die_animation_cnt <= 0);
    }
    int lane() const { return lane_id; }

public:
    int HP;
    double center_x() const { return shape->center_x(); }
    double center_y() const { return shape->center_y(); }

protected:
    AllyState state = AllyState::WALK;
    double v;
    int lane_id;

    ALLEGRO_BITMAP* walk_sheet;
    int frame;
    int frame_count;
    int frame_switch_counter;
    int frame_switch_freq;

    Monster* target = nullptr;
    int atk = 2;
    int attack_cooldown = 0;
    int attack_freq = 30;
    double attack_range = 40;
    double lane_tolerance = 20;

    //add
    int die_animation_total=180;
    int die_animation_cnt=0;
    float die_alpha=1.0f;
    float die_scale=1.0f;
};


namespace AllyLaneSetting {
    constexpr int lane_count = 3;
    int nearest_lane_id(double y);
    double lane_y_by_id(int id);
}


#endif
