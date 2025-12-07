#ifndef HERO_H_INCLUDED
#define HERO_H_INCLUDED
#include "Object.h"
// #include "../shapes/Rectangle.h"
// #include "../shapes/Point.h"
#include <string>
#include <map>

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
private:
    HeroState state = HeroState::FRONT;
    double speed = 5;
    std::map<HeroState, std::string> gifPath;
};


#endif
