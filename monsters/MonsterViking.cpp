#include "MonsterViking.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include <allegro5/bitmap_draw.h>
#include <allegro5/allegro_primitives.h>
#include "../shapes/Rectangle.h"

void MonsterViking::update() {
    // 進入死亡流程（只進一次）
    if (!dying && HP <= 0) {
        dying = true;
        die_cnt = 0;
        bitmap_img_id = 0; // 從死亡動畫第0幀開始
        return;
    }

    // 死亡動畫：只播下排 0~4 一次
    if (dying) {
        // 這裡用 die_total 當成「整段死亡動畫要幾個 frame」
        // 5 格 => 每格停留 die_total/5 frame（至少 1）
        int per = std::max(1, die_total / 5);
        int frame = std::min(4, die_cnt / per); // 0..4
        bitmap_img_id = frame;                  // 讓 draw() 取到正確格子
        die_cnt++;
        return;
    }

    // 沒死就照原本 monster 行為（走路/攻擊/被打動畫切換）
    Monster::update();
}

bool MonsterViking::can_remove() const {
    if (!dying) return false;
    return die_cnt >= die_total;
}


void MonsterViking::draw() {
    if (is_dead() && HP <= 0) {
        if (/* 若你之後有想用 MonsterState::DIE 判斷 */ false) {
            return;
        }
    }

    ImageCenter* IC = ImageCenter::get_instance();
    ALLEGRO_BITMAP* sheet = IC->get("./assets/image/monster/Viking/MonsterViking_new.png");
    if (!sheet) return;

    const int frame_w = 50;
    const int frame_h = 65;
    const int cols = 5;

    int f = bitmap_img_id;    
    int col = f % cols;       
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
        sheet,
        sx, sy,
        frame_w, frame_h,
        draw_x, draw_y,
        0
    );

    if (auto* rect = dynamic_cast<Rectangle*>(shape.get())) {
        float left = static_cast<float>(rect->x1 - cam_x);
        float top = static_cast<float>(rect->y1 - cam_y);
        float right = static_cast<float>(rect->x2 - cam_x);
        float bottom = static_cast<float>(rect->y2 - cam_y);
        al_draw_rectangle(left, top, right, bottom, al_map_rgb(255, 0, 0), 2.0f);
    }


}
