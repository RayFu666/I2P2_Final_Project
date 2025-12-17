#include "Hero.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "shapes/Rectangle.h"
#include <allegro5/bitmap_draw.h>

namespace HeroSetting {

    static constexpr char sprite_path[] = "./assets/image/PlayHero_3.png";

    static constexpr int col=8;
    static constexpr int row=4;

    static constexpr int WALK_START=0;
    static constexpr int WALK_FRAMES=7;
}

void Hero::init() {
    ImageCenter* IC = ImageCenter::get_instance();
    hero_sheet=IC->get(HeroSetting::sprite_path);
    //GAME_ASSERT(hero_sheet,"failed to load hero sheet");

    int sheet_w = al_get_bitmap_width(hero_sheet);
    int sheet_h = al_get_bitmap_height(hero_sheet);

    frame_count = HeroSetting::col;
    frame_w = sheet_w / HeroSetting::col;
    frame_h = sheet_h / HeroSetting::row;

    double startx = 250;
    double starty = 100;
    shape.reset(new Rectangle{
        static_cast<int>(startx),
        static_cast<int>(starty),
        static_cast<int>(startx + frame_w),
        static_cast<int>(starty + frame_h)
    });
}

void Hero::draw() {
    if (!hero_sheet) return;

    DataCenter* DC = DataCenter::get_instance();
    float camx = DC->camerax;
    float camy = DC->cameray;

    int row=0;
    if(state==HeroState::LEFT){
        row=2;
    }else{
        row=0;
    }

    int sx=(HeroSetting::WALK_START+frame)*frame_w;
    int sy=row*frame_h;

    double worldx = shape->center_x();
    double worldy = shape->center_y();

    float dx = static_cast<float>(worldx - camx - frame_w / 2);
    float dy = static_cast<float>(worldy - camy - frame_h / 2);

    al_draw_scaled_bitmap(
        hero_sheet,
        sx, sy,
        frame_w, frame_h,
        dx, dy,
        frame_w*1.6,frame_h*1.6,
        0
    );
}

void Hero::update() {
    DataCenter* DC = DataCenter::get_instance();
    bool moved = false;

    if (DC->key_state[ALLEGRO_KEY_W]) {
        shape->update_center_y(shape->center_y() - speed);
        //state = HeroState::BACK;
        moved = true;
    } else if (DC->key_state[ALLEGRO_KEY_A]) {
        shape->update_center_x(shape->center_x() - speed);
        state = HeroState::LEFT;
        moved = true;
    } else if (DC->key_state[ALLEGRO_KEY_S]) {
        shape->update_center_y(shape->center_y() + speed);
        //state = HeroState::FRONT;
        moved = true;
    } else if (DC->key_state[ALLEGRO_KEY_D]) {
        shape->update_center_x(shape->center_x() + speed);
        state = HeroState::RIGHT;
        moved = true;
    }

    if (moved) {
        ++anim_counter;
        if (anim_counter >= anim_speed) {
            anim_counter = 0;
            frame = (frame + 1) % frame_count;
        }
    } else {
        frame = 0;
        anim_counter = 0;
    }
}
double Hero::center_x() const {
    return shape->center_x();
}
double Hero::center_y() const {
    return shape->center_y();
}
