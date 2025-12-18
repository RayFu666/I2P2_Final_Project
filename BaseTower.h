#ifndef BASETOWER_H_INCLUDED
#define BASETOWER_H_INCLUDED

#include <allegro5/bitmap.h>
#include "shapes/Point.h"
#include "shapes/Rectangle.h"
#include <memory>

class Rectangle;

class BaseTower {
public:
    BaseTower(const Point& topLeft, ALLEGRO_BITMAP* bmp, int hp = 100);

    void update() {}
    void draw() const;

    double left()  const;
    double right() const;
    double top()   const;
    double bottom()const;

    bool is_dead() const { return HP <= 0; }
    void take_damage(int dmg) { HP -= dmg; }

    int HP;
private:
    ALLEGRO_BITMAP* bitmap;
    std::unique_ptr<Rectangle> shape;
};

#endif
