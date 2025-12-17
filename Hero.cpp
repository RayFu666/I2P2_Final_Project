#include "Hero.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "shapes/Rectangle.h"
#include <allegro5/bitmap_draw.h>
#include "Utils.h"
#include "Player.h"
namespace HeroSetting {

    static constexpr char sprite_path[] = "./assets/image/PlayHero_3.png";

    static constexpr int col=8;
    static constexpr int row=4;

    static constexpr int WALK_START=0;
    static constexpr int WALK_FRAMES=7;

    static constexpr int SKILL_COST=50;
    static constexpr double SKILL_RADIUS=120.0;
    static constexpr double SKILL_SPEED_BONUS=60;
    static constexpr int SKILL_ATK_BONUS=1;
    static constexpr double SKILL_DURATION=3.0;
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

    if(skill_timer>0){
        skill_timer--;
    }
    if(DC->key_state[ALLEGRO_KEY_Q]&&!DC->prev_key_state[ALLEGRO_KEY_Q]){
        if(skill_timer<=0&&DC->player->coin>=HeroSetting::SKILL_COST){
            DC->player->coin-=HeroSetting::SKILL_COST;

            int duration_frames=
                static_cast<int>(HeroSetting::SKILL_DURATION*DC->FPS);
            skill_timer=duration_frames;
            //debug
            debug_log("[Hero] Skill activated.\n");
        }
    }
}
double Hero::center_x() const {
    return shape->center_x();
}
double Hero::center_y() const {
    return shape->center_y();
}
//add
bool Hero::skill_radius(double x,double y)const{
    if (skill_timer <= 0) return false;
    double dx=center_x()-x;
    double dy=center_y()-y;
    return (dx*dx+dy*dy)<=HeroSetting::SKILL_RADIUS*HeroSetting::SKILL_RADIUS;
}

double Hero::skill_speed_bonus()const{
    return HeroSetting::SKILL_SPEED_BONUS;
}

int Hero::skill_atk_bonus()const{
    return HeroSetting::SKILL_ATK_BONUS;
}