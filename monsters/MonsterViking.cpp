#include "MonsterViking.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include <allegro5/bitmap_draw.h>
#include <allegro5/allegro_primitives.h>
#include "../shapes/Rectangle.h"

void MonsterViking::update() {
    // 不做 dying 死亡動畫，直接交給 Monster::update()
    Monster::update();
}

bool MonsterViking::can_remove() const {
    // 死掉就直接移除（你們的清除流程應該會用 can_remove）
    return is_dead();
}

void MonsterViking::draw() {
    // 死掉就不畫（你說死掉直接消失）
    if (is_dead()) return;

    ImageCenter* IC = ImageCenter::get_instance();
    ALLEGRO_BITMAP* sheet = IC->get("./assets/image/monster/Viking/MonsterVikingStrongMan.png");
    if (!sheet) return;

    // 新圖：350x70，單排 => 5 格
    const int frame_w = 70;   // 350 / 5
    const int frame_h = 70;   // 高度就是 70
    const int cols = 5;

    // 保險：避免 bitmap_img_id 超出
    int f = bitmap_img_id % cols;
    int sx = f * frame_w;
    int sy = 0;

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

    // (可選) hitbox debug 框
    // if (auto* rect = dynamic_cast<Rectangle*>(shape.get())) {
    //     float left = static_cast<float>(rect->x1 - cam_x);
    //     float top = static_cast<float>(rect->y1 - cam_y);
    //     float right = static_cast<float>(rect->x2 - cam_x);
    //     float bottom = static_cast<float>(rect->y2 - cam_y);
    //     al_draw_rectangle(left, top, right, bottom, al_map_rgb(255, 0, 0), 2.0f);
    // }
}
