#include "Hero.h"
#include "data/DataCenter.h"
#include "data/GIFCenter.h"
#include "algif5/algif.h"
#include "shapes/Rectangle.h"

namespace HeroSetting {
    static constexpr char gif_root_path[50] = "./assets/gif/Hero";
    static constexpr char gif_postfix[][10] = {
        "left",
        "right",
        "front",
        "back",
    };
}

void Hero::init() {
    for (size_t type = 0;type < static_cast<size_t>(HeroState::HEROSTATE_MAX);++type) {
        char buffer[50];
        sprintf(buffer, "%s/dragonite_%s.gif",
            HeroSetting::gif_root_path,
            HeroSetting::gif_postfix[static_cast<int>(type)]);
        gifPath[static_cast<HeroState>(type)] = std::string{buffer};
    }
    GIFCenter* GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION* gif = GIFC->get(gifPath[state]);
    //DataCenter* DC = DataCenter::get_instance();

    //add
    double startx=100;
    double starty=100;
    shape.reset(new Rectangle{
        static_cast<int>(startx),
        static_cast<int>(starty),
        static_cast<int>(startx+gif->width),
        static_cast<int>(starty+gif->height)
    });
}

void Hero::draw() {
    GIFCenter* GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION* gif = GIFC->get(gifPath[state]);
    //algif_draw_gif(gif, shape->center_x() - gif->width / 2, shape->center_y() - gif->height / 2, 0);
    //add
    DataCenter* DC = DataCenter::get_instance();
    float camx=DC->camerax;
    float camy=DC->cameray;

    double worldx=shape->center_x();
    double worldy=shape->center_y();

    float scx=static_cast<float>(worldx-camx-gif->width/2);
    float scy=static_cast<float>(worldy-camy-gif->height/2);

    algif_draw_gif(gif,scx,scy,0);
}

void Hero::update() {
    DataCenter* DC = DataCenter::get_instance();
    if (DC->key_state[ALLEGRO_KEY_W]) {
        shape->update_center_y(shape->center_y() - speed);
        state = HeroState::BACK;
    }
    else if (DC->key_state[ALLEGRO_KEY_A]) {
        shape->update_center_x(shape->center_x() - speed);
        state = HeroState::LEFT;
    }
    else if (DC->key_state[ALLEGRO_KEY_S]) {
        shape->update_center_y(shape->center_y() + speed);
        state = HeroState::FRONT;
    }
    else if (DC->key_state[ALLEGRO_KEY_D]) {
        shape->update_center_x(shape->center_x() + speed);
        state = HeroState::RIGHT;
    }
}
//add
double Hero::center_x() const{
    return shape->center_x();
}
double Hero::center_y() const{
    return shape->center_y();
}