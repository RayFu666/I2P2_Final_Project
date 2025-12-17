#ifndef HERO_H_INCLUDED
#define HERO_H_INCLUDED
#include "Object.h"
// #include "../shapes/Rectangle.h"
// #include "../shapes/Point.h"
#include <string>
#include <map>
#include <allegro5/bitmap.h>
enum class HeroState {
    LEFT,
    RIGHT,
    FRONT,
    BACK,
    HEROSTATE_MAX
};

class Hero : public Object
{
public:
    void init();
    void update();
    void draw() override;

    //add
    double center_x() const;
    double center_y() const;

    bool skill_active()const{return skill_timer>0;}
    bool skill_radius(double x,double y)const;
    double skill_speed_bonus()const;
    int skill_atk_bonus()const;
private:
    HeroState state = HeroState::FRONT;
    double speed = 5;
    ALLEGRO_BITMAP* hero_sheet=nullptr;

    int frame=0;
    int frame_count=0;
    int frame_w=0;
    int frame_h=0;

    int anim_counter=0;
    int anim_speed=6;

    int skill_timer=0;
};


#endif
