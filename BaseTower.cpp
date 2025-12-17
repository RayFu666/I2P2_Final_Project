#include "BaseTower.h"
#include "data/DataCenter.h"
#include "shapes/Rectangle.h"
#include <allegro5/allegro.h>

BaseTower::BaseTower(const Point& topLeft, ALLEGRO_BITMAP* bmp, int hp)
    : HP(hp), bitmap(bmp)
{
    int w = al_get_bitmap_width(bitmap);
    int h = al_get_bitmap_height(bitmap);
    shape.reset(new Rectangle(topLeft.x, topLeft.y, topLeft.x + w, topLeft.y + h));
}

void BaseTower::draw() const {
    DataCenter* DC = DataCenter::get_instance();
    float camx = DC->camerax;
    float camy = DC->cameray;

    float sx = (float)shape->x1 - camx;
    float sy = (float)shape->y1 - camy;
    al_draw_bitmap(bitmap, sx, sy, 0);
}

double BaseTower::left()   const { return shape->x1; }
double BaseTower::right()  const { return shape->x2; }
double BaseTower::top()    const { return shape->y1; }
double BaseTower::bottom() const { return shape->y2; }