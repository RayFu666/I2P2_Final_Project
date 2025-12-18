#include "MonsterGunslayer.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include <allegro5/bitmap_draw.h>
#include <cmath>
#include "../ally/Ally.h"
#include "GunslayerBullet.h"

static constexpr int CELL_W = 75;
static constexpr int CELL_H = 75;
static constexpr int COLS = 6;

// 把「第 idx 格」換算成 (sx, sy)，依你的有效格規則
static void uv_run(int idx, int& sx, int& sy) {
    // run: 0..7 (上排0..5，下排0..1)
    if (idx < 6) { sx = idx * CELL_W; sy = 0; }
    else { sx = (idx - 6) * CELL_W; sy = CELL_H; }
}

static void uv_shoot(int idx, int& sx, int& sy) {
    // shoot: 0..6 (上排0..5，下排0)
    if (idx < 6) { sx = idx * CELL_W; sy = 0; }
    else { sx = 0;            sy = CELL_H; }
}

static void uv_die(int idx, int& sx, int& sy) {
    // die: 0..14 (上排0..5, 中排6..11, 下排12..14)
    if (idx < 6) { sx = idx * CELL_W;      sy = 0; }
    else if (idx < 12) { sx = (idx - 6) * CELL_W; sy = CELL_H; }
    else { sx = (idx - 12) * CELL_W;      sy = 2 * CELL_H; }
}

void MonsterGunslayer::update() {
    DataCenter* DC = DataCenter::get_instance();

    // ---- 進入死亡動畫（只進一次）----
    if (!dying && HP <= 0) {
        dying = true;
        shooting = false;
        die_frame = 0;
        anim_counter = 0;
        // 這裡不要立刻 die_done，讓動畫播完
        return;
    }

    // ---- 死亡動畫播放 ----
    if (dying) {
        anim_counter++;
        if (anim_counter >= anim_freq) {
            anim_counter = 0;
            die_frame++;
            if (die_frame >= 15) {  // 0..14 共15格
                die_done = true;    // 播完就移除
            }
        }
        return;
    }

    // ---- 找跨三線 ally（最近者）----
    Ally* best = nullptr;
    double best_dist = 1e18;

    double cx = shape->center_x();
    double cy = shape->center_y();

    for (Ally* a : DC->allies) {
        if (!a || a->is_dead()) continue;
        double dx = a->center_x() - cx;
        double dy = a->center_y() - cy;
        double dist = std::sqrt(dx * dx + dy * dy);
        if (dist <= shoot_range && dist < best_dist) {
            best = a;
            best_dist = dist;
        }
    }

    shooting = (best != nullptr);

    // ---- 若在射擊狀態：站著不走，但 Monster 的「打塔近戰」仍要保留 ----
    // 最簡單做法：射擊時暫時把 v 改 0，讓 Monster::update() 跑 state/打塔，但不位移。
    int saved_v = v;
    if (shooting) v = 0;
    Monster::update();
    v = saved_v;

    // ---- 射子彈（有目標才射）----
    if (shooting) {
        if (shoot_cooldown > 0) shoot_cooldown--;
        else {
            Point spawn{ cx, cy };
            DC->enemy_bullets.emplace_back(
                new GunslayerBullet(
                    spawn,
                    best,
                    "./assets/image/GunslayerBullet.png", // 你要用的子彈圖
                    bullet_speed,
                    atk,
                    bullet_fly_dist
                )
            );
            shoot_cooldown = shoot_freq;
        }
    }
    else {
        shoot_cooldown = 0; // 沒目標就重置也行（看你喜好）
    }

    // ---- 動畫 frame 推進 ----
    anim_counter++;
    if (anim_counter >= anim_freq) {
        anim_counter = 0;
        if (shooting) {
            shoot_frame = (shoot_frame + 1) % 7; // 0..6
        }
        else {
            run_frame = (run_frame + 1) % 8;     // 0..7
        }
    }
}

void MonsterGunslayer::draw() {
    if (die_done) return;

    ImageCenter* IC = ImageCenter::get_instance();
    DataCenter* DC = DataCenter::get_instance();

    ALLEGRO_BITMAP* sheet = nullptr;
    int sx = 0, sy = 0;

    if (dying) {
        sheet = IC->get("./assets/image/monster/Gunslayer/gunslayer_die.png");
        if (!sheet) return;
        int idx = std::min(die_frame, 14);
        uv_die(idx, sx, sy);
    }
    else if (shooting) {
        sheet = IC->get("./assets/image/monster/Gunslayer/gunslayer_shoot.png");
        if (!sheet) return;
        uv_shoot(shoot_frame % 7, sx, sy);
    }
    else {
        sheet = IC->get("./assets/image/monster/Gunslayer/gunslayer_run.png");
        if (!sheet) return;
        uv_run(run_frame % 8, sx, sy);
    }

    float cam_x = DC->camerax;
    float cam_y = DC->cameray;

    double cx = shape->center_x();
    double cy = shape->center_y();

    float draw_x = static_cast<float>(cx - cam_x - CELL_W / 2.0);
    float draw_y = static_cast<float>(cy - cam_y - CELL_H / 2.0);

    al_draw_bitmap_region(sheet, sx, sy, CELL_W, CELL_H, draw_x, draw_y, 0);
}
