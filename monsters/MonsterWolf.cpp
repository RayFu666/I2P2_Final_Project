#include "MonsterWolf.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include <allegro5/bitmap_draw.h>
#include <algorithm>

static constexpr int CELL = 64;
static constexpr int COLS = 16;

void MonsterWolf::update() {
    Monster::update();

    if (state == MonsterState::DIE) {
        anim_cnt++;
        if (anim_cnt >= anim_freq) {
            anim_cnt = 0;
            die_frame++;
            if (die_frame >= 8) {
                die_done = true;
                die_frame = 7;
            }
        }
        return;
    }

    anim_cnt++;
    if (anim_cnt >= anim_freq) {
        anim_cnt = 0;
        if (state == MonsterState::ATTACK) attack_frame = (attack_frame + 1) % 8;
        else walk_frame = (walk_frame + 1) % 8;
    }
}

bool MonsterWolf::can_remove() const {
    return die_done;
}

void MonsterWolf::draw() {
    if (die_done) return;

    ImageCenter* IC = ImageCenter::get_instance();
    DataCenter* DC = DataCenter::get_instance();

    ALLEGRO_BITMAP* sheet = IC->get("./assets/image/monster/Wolf/coconut_new.png");
    if (!sheet) return;

    int sx = 0, sy = 0;

    if (state == MonsterState::DIE) {
        sx = std::min(die_frame, 7) * CELL;
        sy = 1 * CELL;
    }
    else if (state == MonsterState::ATTACK) {
        sx = (8 + (attack_frame % 8)) * CELL;
        sy = 0;
    }
    else {
        sx = (walk_frame % 8) * CELL;
        sy = 0;
    }

    float cam_x = DC->camerax;
    float cam_y = DC->cameray;

    float draw_x = static_cast<float>(shape->center_x() - cam_x - CELL / 2.0);
    float draw_y = static_cast<float>(shape->center_y() - cam_y - CELL / 2.0);

    al_draw_bitmap_region(sheet, sx, sy, CELL, CELL, draw_x, draw_y, 0);
}
