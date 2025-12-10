#include "Ally.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include <allegro5/bitmap_draw.h>
#include "../monsters/Monster.h"

namespace {
    constexpr double lane_y_table[AllyLaneSetting::lane_count] = {
        250.0, 300.0, 350.0
    };
}

int AllyLaneSetting::nearest_lane_id(double y) {
    int best_id = 0;
    double best_dist = std::abs(y - lane_y_table[0]);
    for (int i = 1; i < lane_count; ++i) {
        double d = std::abs(y - lane_y_table[i]);
        if (d < best_dist) {
            best_dist = d;
            best_id = i;
        }
    }
    return best_id;
}

double AllyLaneSetting::lane_y_by_id(int id) {
    return lane_y_table[id];
}

Ally::Ally(const Point& p, int lane_id)
    : lane_id(lane_id)
{
    //DataCenter* DC = DataCenter::get_instance();
    ImageCenter* IC = ImageCenter::get_instance();

    // 先用跟 CaveMan 一樣的 sprite sheet（之後你可以換）
    walk_sheet = IC->get("./assets/image/ally/black_dude.png");

    HP = 10;   // 隨便先給
    v = 60;   // 每秒 60px

    frame = 0;
    frame_count = 6;        // 3x2 = 6 幀
    frame_switch_freq = 10;
    frame_switch_counter = 0;

    state = AllyState::WALK;

    // 先把中心放在 p（p 是你指定的 x，y 之後會被「貼近 lane」）
    shape.reset(new Rectangle{ p.x, p.y, p.x, p.y });

    atk = 2;
    attack_freq = 30;       // 30 frame 打一次 ≈ 0.5 秒
    attack_cooldown = 0;
    attack_range = 40.0;
    lane_tolerance = 25.0;

    target = nullptr;
}

void Ally::update() {
    DataCenter* DC = DataCenter::get_instance();

    // 1. 死亡判定
    if (HP <= 0 && state != AllyState::DIE) {
        state = AllyState::DIE;
        target = nullptr;
    }

    // 2. 動畫更新（目前 WALK / ATTACK 共用同一張走路圖）
    if (state == AllyState::WALK || state == AllyState::ATTACK) {
        if (frame_switch_counter > 0) {
            --frame_switch_counter;
        }
        else {
            frame = (frame + 1) % frame_count;
            frame_switch_counter = frame_switch_freq;
        }
    }

    double cx = shape->center_x();
    double cy = shape->center_y();

    switch (state) {
    case AllyState::WALK: {
        double dx = v / DC->FPS;
        shape->update_center_x(cx - dx);

        // 把 y 固定在 lane 上，避免飄掉
        shape->update_center_y(AllyLaneSetting::lane_y_by_id(lane_id));

        // ★ 左邊界 clamp：不要讓 Ally 一直走出框外
        double new_x = shape->center_x();
        if (new_x < 50.0) {
            shape->update_center_x(50.0);
        }

        // 然後再來找怪物（這段你已經寫好了）
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

        break;
    }


    case AllyState::ATTACK: {
        // 目標死掉或被移除 → 回 WALK
        if (!target || target->is_dead()) {
            target = nullptr;
            state = AllyState::WALK;
            break;
        }

        double tx = target->shape->center_x();
        double ty = target->shape->center_y();

        double dist_x = std::abs(cx - tx);
        double dist_y = std::abs(cy - ty);

        // 仍然要求在「同一條 lane 附近」，但不用管前面後面，只要沒離太遠就繼續打
        if (dist_y > lane_tolerance || dist_x > attack_range) {
            target = nullptr;
            state = AllyState::WALK;
            break;
        }

        // 冷卻中就只播動畫不扣血
        if (attack_cooldown > 0) {
            --attack_cooldown;
        }
        else {
            target->take_damage(atk);
            // ★ 順便讓 Ally 也扣血（Monster 反擊），先硬寫一個數值，之後再改成 Monster 的攻擊力
            HP -= 1;

            attack_cooldown = attack_freq;
        }

        break;
    }


                          // ☠ DIE：原地不動，等外面把這隻 Ally 從 vector erase
    case AllyState::DIE: {
        // 這裡先不做事，之後你在 Game 或 DataCenter/OperationCenter 那裡
        // 會寫「掃 Allies，把 is_dead() 的 erase 掉」
        break;
    }
    }
}

void Ally::draw() {
    if (!walk_sheet) return;

    int frame_w = 64;
    int frame_h = 64;
    int cols = 2;

    int f = frame % frame_count;
    int col = f % cols;
    int row = f / cols;

    int sx = col * frame_w;
    int sy = row * frame_h;

    double cx = shape->center_x();
    double cy = shape->center_y();

    al_draw_bitmap_region(
        walk_sheet,
        sx, sy, frame_w, frame_h,
        cx - frame_w / 2,
        cy - frame_h / 2,
        0
    );
}
