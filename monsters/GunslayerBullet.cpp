#include "GunslayerBullet.h"
#include "../ally/Ally.h"
#include <algorithm>
#include <cmath>
#include <allegro5/allegro.h>
#include <allegro5/bitmap_draw.h>
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"


GunslayerBullet::GunslayerBullet(const Point& p, Ally* target,
    const std::string& img_path, double v, int dmg, double fly_dist)
    : target(target), fly_dist(fly_dist), dmg(dmg)
{
    ImageCenter* IC = ImageCenter::get_instance();
    bitmap = IC->get(img_path);

    double r = 10;
    if (bitmap) {
        r = std::min(al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap)) * 0.4;
    }
    shape.reset(new Circle{ p.x, p.y, r });

    if (target) {
        Point t{ target->shape->center_x(), target->shape->center_y() };
        double d = Point::dist(p, t);
        if (d < 1e-6) d = 1e-6;
        vx = (t.x - p.x) * v / d;
        vy = (t.y - p.y) * v / d;
    }
}

void GunslayerBullet::update() {
    if (removed || fly_dist <= 0) return;

    DataCenter* DC = DataCenter::get_instance();
    double dx = vx / DC->FPS;
    double dy = vy / DC->FPS;

    double movement = Point::dist(Point{ dx, dy }, Point{ 0, 0 });
    if (movement < 1e-9) movement = 1e-9;

    if (fly_dist > movement) {
        shape->update_center_x(shape->center_x() + dx);
        shape->update_center_y(shape->center_y() + dy);
        fly_dist -= movement;
    }
    else {
        shape->update_center_x(shape->center_x() + dx * fly_dist / movement);
        shape->update_center_y(shape->center_y() + dy * fly_dist / movement);
        fly_dist = 0;
    }

    if (!target) return;
    if (target->can_remove() || target->is_dead()) {
        target = nullptr;
        removed=true;
        return;
    }

    double bx = shape->center_x();
    double by = shape->center_y();
    double tx = target->shape->center_x();
    double ty = target->shape->center_y();
    double dist2 = (bx - tx) * (bx - tx) + (by - ty) * (by - ty);

    if (dist2 <= 25.0 * 25.0) {
        target->HP -= dmg;
        removed = true;
    }
}

void GunslayerBullet::draw() {
    if (!bitmap || removed) return;

    DataCenter* DC = DataCenter::get_instance();
    float cam_x = DC->camerax;
    float cam_y = DC->cameray;

    float x = static_cast<float>(shape->center_x() - cam_x - al_get_bitmap_width(bitmap) / 2);
    float y = static_cast<float>(shape->center_y() - cam_y - al_get_bitmap_height(bitmap) / 2);

    al_draw_bitmap(bitmap, x, y, 0);
}
