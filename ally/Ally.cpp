#include "Ally.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include <allegro5/bitmap_draw.h>
#include "../monsters/Monster.h"
#include "../Utils.h"
#include "../Hero.h"
namespace {
    constexpr double lane_y_table[AllyLaneSetting::lane_count] = {
        220.0, 300.0, 380.0
    };
}

int AllyLaneSetting::nearest_lane_id(double y) {
    int best_id = 0;
    double best_dist = std::abs(y - lane_y_table[0]);
    for (int i = 1; i < lane_count; ++i) {
        double d = std::abs(y - lane_y_table[i]);
        if (d < best_dist) {
            best_dist = d;
            best_id = i;
        }
    }
    return best_id;
}

double AllyLaneSetting::lane_y_by_id(int id) {
    return lane_y_table[id];
}
//change
Ally::Ally(const Point& p, int lane_id)
    :lane_id(lane_id)
{
    //DataCenter* DC = DataCenter::get_instance();
    ImageCenter* IC = ImageCenter::get_instance();
    //add
    
    walk_sheet = IC->get("./assets/image/ally/black_dude.png");
    HP=20;
    v=60;
    atk=2;
    attack_freq=30;
    attack_range=30.0;

    frame_count=6;

    //walk_sheet = IC->get("./assets/image/ally/black_dude.png");

    //HP = 30;
    //v = 60;

    frame = 0;
    //frame_count = 6;
    frame_switch_freq = 10;
    frame_switch_counter = 0;

    state = AllyState::WALK;

    shape.reset(new Rectangle{ p.x, p.y, p.x, p.y });

    //atk = 3;
    //attack_freq = 30;
    attack_cooldown = 0;
    //attack_range = 40.0;
    lane_tolerance = 25.0;

    target = nullptr;
}

void Ally::update() {
    DataCenter* DC = DataCenter::get_instance();

    if (HP <= 0 && state != AllyState::DIE) {
        state = AllyState::DIE;
        target = nullptr;

        //add
        die_animation_cnt=die_animation_total;
        die_alpha=1.0f;
        die_scale=1.0f;
    }

    if (state == AllyState::WALK || state == AllyState::ATTACK) {
        if (frame_switch_counter > 0) {
            --frame_switch_counter;
        }
        else {
            frame = (frame + 1) % frame_count;
            frame_switch_counter = frame_switch_freq;
        }
    }

    double cx = shape->center_x();
    double cy = shape->center_y();

    double speed_bonus=v;
    int atk_bonus=atk;
    Hero *hero=DC->hero;
    if (hero&&hero->skill_active()&&hero->skill_radius(cx, cy)){
        speed_bonus+=hero->skill_speed_bonus();
        atk_bonus+=hero->skill_atk_bonus();
    }
    switch (state) {
    case AllyState::WALK: {
        double dx = speed_bonus / DC->FPS;
        shape->update_center_x(cx - dx);

        shape->update_center_y(AllyLaneSetting::lane_y_by_id(lane_id));

        double new_x = shape->center_x();
        //add
        const double base_x=10.0;

        if (new_x <base_x) {
            shape->update_center_x(base_x);
            new_x=base_x;
        }
        
        Monster* best = nullptr;
        double best_dx = 1e9;

        for (Monster* m : DC->monsters) {
            if (!m || m->is_dead()) continue;

            double mx = m->shape->center_x();
            double my = m->shape->center_y();

            if (std::abs(my - cy) > lane_tolerance) continue;
            if (mx >= cx) continue;

            double dx_front = cx - mx;
            if (dx_front <= attack_range && dx_front < best_dx) {
                best_dx = dx_front;
                best = m;
            }
        }

        if (best) {
            target = best;
            state = AllyState::ATTACK;
            attack_cooldown = 0;
        }else{
            //add
            double distance=cx-base_x;
            if(distance<=attack_range){
                target=nullptr;
                state=AllyState::ATTACK;
                attack_cooldown=0;
            }
        }

        break;
    }


    case AllyState::ATTACK: {
        //add
        const double base_x=10.0;
        cx=shape->center_x();
        cy=shape->center_y();

        // if (!target || target->is_dead()) {
        //     target = nullptr;
        //     state = AllyState::WALK;
        //     break;
        // }
        if(target){
            double tx = target->shape->center_x();
            double ty = target->shape->center_y();

            double dist_x = std::abs(cx - tx);
            double dist_y = std::abs(cy - ty);
            if(target->is_dead()||dist_y >lane_tolerance||dist_x>attack_range){
                target=nullptr;
            }else{
                if(attack_cooldown>0){
                    --attack_cooldown;
                }else{
                    target->take_damage(atk_bonus);
                    HP-=1;
                    attack_cooldown=attack_freq;
                }
                break;
            }
        }
        double distance=cx-base_x;
        if(distance>attack_range){
            state=AllyState::WALK;
            break;
        }
        if(attack_cooldown>0){
            --attack_cooldown;
        }else{
            DC->enemy_base_hp-=atk_bonus;
            if(DC->enemy_base_hp<0)DC->enemy_base_hp=0;

            // debug_log("[EnemyBase] took %d damage, HP = %d\n",
            //           atk, DC->enemy_base_hp);

            attack_cooldown=attack_freq;
        }
        break;
    }



    case AllyState::DIE: {
        if(die_animation_cnt>0){
            die_animation_cnt--;
        }
        float t=1.0f-static_cast<float>(die_animation_cnt)/die_animation_total;
        die_alpha=1.0f-t;
        die_scale=1.0f-0.5f*t;
        break;
    }
    }
}

void Ally::draw() {
    if (!walk_sheet) return;

    int frame_w = 64;
    int frame_h = 64;
    int cols = 2;

    int f = frame % frame_count;
    int col = f % cols;
    int row = f / cols;

    int sx = col * frame_w;
    int sy = row * frame_h;
    //add
    DataCenter *DC=DataCenter::get_instance();
	float cam_x=DC->camerax;
	float cam_y=DC->cameray;

    double cx = shape->center_x();
    double cy = shape->center_y();

    float draw_w=static_cast<float>(frame_w);
    float draw_h=static_cast<float>(frame_h);
    float alpha=1.0f;

    if (state==AllyState::DIE){
        if (die_animation_cnt<=0)return;

        draw_w=static_cast<float>(frame_w*die_scale);
        draw_h=static_cast<float>(frame_h*die_scale);
        alpha=die_alpha;
    }

	float sc_x=static_cast<float>(cx-cam_x-draw_w/2.0);
	float sc_y=static_cast<float>(cy-cam_y-draw_h/2.0);

    ALLEGRO_COLOR tint = al_map_rgba_f(1.0f,1.0f,1.0f,alpha);
    al_draw_tinted_scaled_bitmap(
        walk_sheet,tint,
        sx, sy, frame_w, frame_h,
        sc_x,
        sc_y,
        draw_w,
        draw_h,
        0
    );
}
