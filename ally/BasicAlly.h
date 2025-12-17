#ifndef BASICALY_H_INCLUDED
#define BASICALY_H_INCLUDED

#include "Ally.h"
#include "../data/ImageCenter.h" 

class BasicAlly : public Ally {
public:
    BasicAlly(const Point& p, int lane_id) : Ally(p, lane_id) {
        ImageCenter* IC = ImageCenter::get_instance();
        walk_sheet = IC->get("./assets/image/ally/black_dude.png");

        HP = 30;
        v = 60;

        frame = 0;
        frame_count = 6;
        frame_switch_freq = 10;
        frame_switch_counter = 0;

        state = AllyState::WALK;

        atk = 3;
        attack_freq = 30;
        attack_cooldown = 0;
        attack_range = 40.0;
        lane_tolerance = 25.0;

        target = nullptr;
    }
};

#endif
