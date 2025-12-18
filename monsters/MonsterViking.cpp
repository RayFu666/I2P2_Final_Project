#include "MonsterViking.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include <allegro5/bitmap_draw.h>
#include <allegro5/allegro_primitives.h>
#include "../shapes/Rectangle.h"

void MonsterViking::update() {
    Monster::update();
}

bool MonsterViking::can_remove() const {
    return is_dead();
}

void MonsterViking::draw() {
    if (is_dead()) return;

    ImageCenter* IC = ImageCenter::get_instance();
    ALLEGRO_BITMAP* sheet = IC->get("./assets/image/monster/Viking/MonsterVikingStrongMan.png");
    if (!sheet) return;

    const int frame_w = 70;
    const int frame_h = 70;
    const int cols = 5;

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

}
