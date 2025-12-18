#include "VikingMan.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../shapes/Rectangle.h"
#include "../monsters/Monster.h"
#include <allegro5/bitmap_draw.h>
#include <allegro5/allegro_primitives.h>

VikingMan::VikingMan(const Point& p, int lane_id)
    : Ally(p, lane_id, Allytype::VIKING_HAMMERMAN)
{
    ImageCenter* IC = ImageCenter::get_instance();

    walk_sheet = IC->get("./assets/image/ally/VikingMan_new.png");

    HP = 30;
    v = 60;
    atk = 3;

    frame = 0;
    frame_count = 5;
    frame_switch_freq = 10;
    frame_switch_counter = 0;

    state = AllyState::WALK;

    shape.reset(new Rectangle{ p.x, p.y, p.x, p.y });

    attack_freq = 30;
    attack_cooldown = 0;
    attack_range = 40.0;
    lane_tolerance = 25.0;

    dying = false;
    die_cnt = 0;
    die_total = 25;
    
}

void VikingMan::update() {
    DataCenter* DC = DataCenter::get_instance();

    if (!dying && HP <= 0) {
        dying = true;
        state = AllyState::DIE;
        target = nullptr;
        die_cnt = 0;
        frame = 0;
        return;
    }

    if (dying) {
        int per = std::max(1, die_total / 5);
        frame = std::min(4, die_cnt / per);
        die_cnt++;
        return;
    }

    if (frame_switch_counter > 0) --frame_switch_counter;
    else {
        frame = (frame + 1) % 5;
        frame_switch_counter = frame_switch_freq;
    }

    double cx = shape->center_x();
    double cy = shape->center_y();

    switch (state) {
    case AllyState::WALK: {
        double dx = v / DC->FPS;
        shape->update_center_x(cx - dx);

        shape->update_center_y(AllyLaneSetting::lane_y_by_id(lane_id));

        const double base_x = 100.0;
        if (shape->center_x() < base_x) shape->update_center_x(base_x);

        Monster* best = nullptr;
        double best_dx = 1e9;

        for (Monster* m : DC->monsters) {
            if (!m || m->is_dead()) continue;

            double mx = m->shape->center_x();
            double my = m->shape->center_y();

            if (std::abs(my - cy) > lane_tolerance) continue;
            if (mx >= cx) continue;

            double dx_front = cx - mx;
            if (dx_front <= attack_range && dx_front < best_dx) {
                best_dx = dx_front;
                best = m;
            }
        }

        if (best) {
            target = best;
            state = AllyState::ATTACK;
            attack_cooldown = 0;
        }
        else {
            const double base_x = 100.0;
            double distance = cx - base_x;
            if (distance <= attack_range) {
                target = nullptr;
                state = AllyState::ATTACK;
                attack_cooldown = 0;
            }
        }
        break;

    }

    case AllyState::ATTACK: {
        const double base_x = 100.0;

        if (target) {
            bool still_exist = false;
            for (Monster* m : DC->monsters) {
                if (m == target) { still_exist = true; break; }
            }
            if (!still_exist || target->is_dead()) target = nullptr;
        }

        if (target) {
            double tx = target->shape->center_x();
            double ty = target->shape->center_y();
            double dist_x = std::abs(cx - tx);
            double dist_y = std::abs(cy - ty);

            if (dist_y > lane_tolerance || dist_x > attack_range) {
                target = nullptr;
            }
            else {
                if (attack_cooldown > 0) --attack_cooldown;
                else {
                    target->take_damage(atk);
                    attack_cooldown = attack_freq;
                }
                break;
            }
        }

        double distance = cx - base_x;
        if (distance > attack_range) {
            state = AllyState::WALK;
            break;
        }

        if (attack_cooldown > 0) --attack_cooldown;
        else {
            DC->enemy_base_hp -= atk;
            if (DC->enemy_base_hp < 0) DC->enemy_base_hp = 0;
            attack_cooldown = attack_freq;
        }
        break;

        double tx = target->shape->center_x();
        double ty = target->shape->center_y();

        double dist_x = std::abs(cx - tx);
        double dist_y = std::abs(cy - ty);

        if (dist_y > lane_tolerance || dist_x > attack_range) {
            target = nullptr;
            state = AllyState::WALK;
            break;
        }

        if (attack_cooldown > 0) --attack_cooldown;
        else {
            target->take_damage(atk);
            attack_cooldown = attack_freq;
        }
        break;
    }

    case AllyState::DIE:
        break;
    }
}

bool VikingMan::can_remove() const {
    return dying && die_cnt >= die_total;
}

void VikingMan::draw() {
    if (!walk_sheet) return;

    const int frame_w = 50;
    const int frame_h = 65;
    const int cols = 5;

    int col = frame % cols;
    int row = dying ? 1 : 0;
    int sx = col * frame_w;
    int sy = row * frame_h;

    DataCenter* DC = DataCenter::get_instance();
    float cam_x = DC->camerax;
    float cam_y = DC->cameray;

    double cx = shape->center_x();
    double cy = shape->center_y();

    float draw_x = static_cast<float>(cx - cam_x - frame_w / 2.0);
    float draw_y = static_cast<float>(cy - cam_y - frame_h / 2.0);

    al_draw_bitmap_region(
        walk_sheet,
        sx, sy,
        frame_w, frame_h,
        draw_x, draw_y,
        ALLEGRO_FLIP_HORIZONTAL
    );

    if (auto* rect = dynamic_cast<Rectangle*>(shape.get())) {
        float left = static_cast<float>(rect->x1 - cam_x);
        float top = static_cast<float>(rect->y1 - cam_y);
        float right = static_cast<float>(rect->x2 - cam_x);
        float bottom = static_cast<float>(rect->y2 - cam_y);
        al_draw_rectangle(left, top, right, bottom, al_map_rgb(255, 0, 0), 2.0f);
    }
}
