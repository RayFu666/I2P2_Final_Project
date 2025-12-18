#ifndef GUNSLAYER_BULLET_H_INCLUDED
#define GUNSLAYER_BULLET_H_INCLUDED

#include "../Object.h"
#include "../shapes/Circle.h"
#include "../shapes/Point.h"
#include <allegro5/bitmap.h>
#include <string>

class Ally;

class GunslayerBullet : public Object {
public:
    GunslayerBullet(const Point& p, Ally* target,
        const std::string& path,
        double v, int dmg, double fly_dist);

    void update();
    void draw();

    bool can_remove() const { return removed; }

    Ally* get_target() const { return target; }
    void clear_target() { target = nullptr; }

private:
    Ally* target = nullptr;
    double vx = 0.0;
    double vy = 0.0;
    double fly_dist = 0.0;
    int dmg = 0;
    ALLEGRO_BITMAP* bitmap = nullptr;
    bool removed = false;
    
};

#endif
