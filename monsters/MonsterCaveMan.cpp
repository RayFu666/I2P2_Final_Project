#include "MonsterCaveMan.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include <allegro5/bitmap_draw.h>
#include <algorithm>

static constexpr int CELL = 64;   // 1024/16 = 64
static constexpr int COLS = 16;

void MonsterCaveMan::update() {
    // 先讓 Monster 跑原本走路/攻擊/打塔邏輯（狀態 state 也會更新）
    Monster::update();

    // 進入 DIE 後播死亡動畫，播完才 remove
    if (state == MonsterState::DIE) {
        anim_cnt++;
        if (anim_cnt >= anim_freq) {
            anim_cnt = 0;
            die_frame++;
            if (die_frame >= 8) {        // 下排左半 8 格
                die_done = true;
                die_frame = 7;           // 卡最後一格也行
            }
        }
        return;
    }

    // WALK / ATTACK 動畫
    anim_cnt++;
    if (anim_cnt >= anim_freq) {
        anim_cnt = 0;
        if (state == MonsterState::ATTACK) {
            attack_frame = (attack_frame + 1) % 8;
        }
        else {
            walk_frame = (walk_frame + 1) % 8;
        }
    }
}

bool MonsterCaveMan::can_remove() const {
    // 你希望「播完死亡動畫才消失」
    return die_done;
}

void MonsterCaveMan::draw() {
    if (die_done) return;

    ImageCenter* IC = ImageCenter::get_instance();
    DataCenter* DC = DataCenter::get_instance();

    // ✅ 請確認你把檔案放在這個路徑（或你改成你實際放置的位置）
    ALLEGRO_BITMAP* sheet = IC->get("./assets/image/monster/CaveMan/Slime_new.png");
    if (!sheet) return;

    int sx = 0, sy = 0;

    if (state == MonsterState::DIE) {
        // 下排左半：0..7
        sx = std::min(die_frame, 7) * CELL;
        sy = 1 * CELL;
    }
    else if (state == MonsterState::ATTACK) {
        // 上排右半：8..15
        sx = (8 + (attack_frame % 8)) * CELL;
        sy = 0;
    }
    else {
        // 上排左半：0..7
        sx = (walk_frame % 8) * CELL;
        sy = 0;
    }

    float cam_x = DC->camerax;
    float cam_y = DC->cameray;

    float draw_x = static_cast<float>(shape->center_x() - cam_x - CELL / 2.0);
    float draw_y = static_cast<float>(shape->center_y() - cam_y - CELL / 2.0);

    al_draw_bitmap_region(sheet, sx, sy, CELL, CELL, draw_x, draw_y, 0);
}
