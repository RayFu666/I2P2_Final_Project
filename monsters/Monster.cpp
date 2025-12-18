#include "Monster.h"
#include "MonsterWolf.h"
#include "MonsterCaveMan.h"
#include "MonsterWolfKnight.h"
#include "MonsterDemonNinja.h"
#include "MonsterViking.h" 
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../Level.h"
#include "../shapes/Point.h"
#include "../shapes/Rectangle.h"
#include "../Utils.h"
#include <allegro5/allegro_primitives.h>
#include "../ally/Ally.h"
#include "MonsterGunslayer.h"


#include "../BaseTower.h"
#include "../Player.h"
using namespace std;

// fixed settings
enum class Dir {
    UP, DOWN, LEFT, RIGHT
};
namespace MonsterSetting {
    static constexpr char monster_imgs_root_path[static_cast<int>(MonsterType::MONSTERTYPE_MAX)][40] = {
        "./assets/image/monster/Wolf",
        "./assets/image/monster/CaveMan",
        "./assets/image/monster/WolfKnight",
        "./assets/image/monster/DemonNinja",
        "./assets/image/monster/Viking",
        "./assets/image/monster/Gunslayer"
    };
    static constexpr char dir_path_prefix[][10] = {
        "UP", "DOWN", "LEFT", "RIGHT"
    };
}

/**
 * @brief Create a Monster* instance by the type.
 * @param type the type of a monster.
 * @param path walk path of the monster. The path should be represented in road grid format.
 * @return The curresponding Monster* instance.
 * @see Level::grid_to_region(const Point &grid) const
 */
Monster* Monster::create_monster(MonsterType type, const vector<Point>& path) {
    switch (type) {
    case MonsterType::WOLF: {
        return new MonsterWolf{ path };
    }
    case MonsterType::CAVEMAN: {
        return new MonsterCaveMan{ path };
    }
    case MonsterType::WOLFKNIGHT: {
        return new MonsterWolfKnight{ path };
    }
    case MonsterType::DEMONNIJIA: {
        return new MonsterDemonNinja{ path };
    }
    case MonsterType::VIKING:               // ★ 新增 case
        return new MonsterViking{ path };
    case MonsterType::GUNSLAYER: {
        return new MonsterGunslayer{ path };
    }
    case MonsterType::MONSTERTYPE_MAX: {}
    }
    GAME_ASSERT(false, "monster type error.");
}

/**
 * @brief Given velocity of x and y direction, determine which direction the monster should face.
 */
Dir convert_dir(const Point& v) {
    if (v.y < 0 && abs(v.y) >= abs(v.x))
        return Dir::UP;
    if (v.y > 0 && abs(v.y) >= abs(v.x))
        return Dir::DOWN;
    if (v.x < 0 && abs(v.x) >= abs(v.y))
        return Dir::LEFT;
    if (v.x > 0 && abs(v.x) >= abs(v.y))
        return Dir::RIGHT;
    return Dir::RIGHT;
}

Monster::Monster(const vector<Point>& path, MonsterType type) {
    DataCenter* DC = DataCenter::get_instance();

    shape.reset(new Rectangle{ 0, 0, 0, 0 });
    this->type = type;
    dir = Dir::RIGHT;
    state = MonsterState::WALK;
    bitmap_img_id = 0;
    bitmap_switch_counter = 0;
    for (const Point& p : path)
        this->path.push(p);
    if (!path.empty()) {
        const Point& grid = this->path.front();
        const Rectangle& region = DC->level->grid_to_region(grid);
        // Temporarily set the bounding box to the center (no area) since we haven't got the hit box of the monster.
        shape.reset(new Rectangle{ region.center_x(), region.center_y(), region.center_x(), region.center_y() });
        this->path.pop();
    }
}

void Monster::take_damage(int dmg) {
    HP -= dmg;
}

bool Monster::is_dead() const {
    return HP <= 0 || state == MonsterState::DIE;
}

/**
 * @details This update function updates the following things in order:
 * @details * Move pose of the current facing direction (bitmap_img_id).
 * @details * Current position (center of the hit box). The position is moved based on the center of the hit box (Rectangle). If the center of this monster reaches the center of the first point of path, the function will proceed to the next point of path.
 * @details * Update the real bounding box by the center of the hit box calculated as above.
 */


void Monster::update_walk_state() {
    DataCenter* DC = DataCenter::get_instance();
    ImageCenter* IC = ImageCenter::get_instance();

    double movement = v / DC->FPS;

    while (!path.empty() && movement > 0) {
        const Point& grid = this->path.front();
        const Rectangle& region = DC->level->grid_to_region(grid);
        const Point next_goal{ region.center_x(), region.center_y() };

        double d = Point::dist(
            Point{ shape->center_x(), shape->center_y() },
            next_goal
        );

        Dir tmpdir = dir;
        if (d < 1e-9) d = 1e-9;

        if (d < movement) {
            movement -= d;
            tmpdir = convert_dir(
                Point{ next_goal.x - shape->center_x(),
                       next_goal.y - shape->center_y() }
            );
            shape->update_center_x(next_goal.x);
            shape->update_center_y(next_goal.y);
            path.pop();
        }
        else {
            double dx = (next_goal.x - shape->center_x()) / d * movement;
            double dy = (next_goal.y - shape->center_y()) / d * movement;
            tmpdir = convert_dir(Point{ dx, dy });
            shape->update_center_x(shape->center_x() + dx);
            shape->update_center_y(shape->center_y() + dy);
            movement = 0;
        }
        dir = tmpdir;
    }

    // ====== 更新 hitbox（Viking 特判，其餘照原本圖片抓）======
    double cx = shape->center_x();
    double cy = shape->center_y();

    if (type == MonsterType::VIKING) {
        const int w = 70;
        const int h = 70;
        shape.reset(new Rectangle{
            (cx - w / 2.), (cy - h / 2.),
            (cx - w / 2. + w), (cy - h / 2. + h)
            });
    }
    else if (type == MonsterType::GUNSLAYER) {
        // ★ Gunslayer 每格 75x75（你 MonsterGunslayer.cpp 用的 CELL_W/H）
        const int w = 75, h = 75;
        shape.reset(new Rectangle{ cx - w / 2., cy - h / 2., cx - w / 2. + w, cy - h / 2. + h });
    }
    else {
        char buffer[50];
        sprintf(
            buffer, "%s/%s_%d.png",
            MonsterSetting::monster_imgs_root_path[static_cast<int>(type)],
            MonsterSetting::dir_path_prefix[static_cast<int>(dir)],
            bitmap_img_ids[static_cast<int>(dir)][bitmap_img_id]
        );
        ALLEGRO_BITMAP* bitmap = IC->get(buffer);
        const int w = (int)al_get_bitmap_width(bitmap) * 0.8;
        const int h = (int)al_get_bitmap_height(bitmap) * 0.8;
        shape.reset(new Rectangle{ cx - w / 2., cy - h / 2., cx - w / 2. + w, cy - h / 2. + h });
    }

    // 重新抓一次（hitbox reset 後 center 可能一樣，但保險）
    cx = shape->center_x();
    cy = shape->center_y();

    // ====== 距離塔判斷（所有怪都要能打塔）======
    Rectangle* rect = dynamic_cast<Rectangle*>(shape.get());
    double base_left = (DC->right_base ? DC->right_base->left() : (double)DC->game_field_length);
    double dist_to_base = rect ? (base_left - rect->x2) : 1e9;

    // ★ Gunslayer：不在 Monster 裡處理打 ally（遠程射擊交給 MonsterGunslayer）
    if (type == MonsterType::GUNSLAYER) {
        if (dist_to_base <= attack_range) {
            if (dist_to_base < 0) dist_to_base = 0;
            target_ally = nullptr;
            state = MonsterState::ATTACK;   // 只用來打塔
            attack_cooldown = 0;
        }
        return; // ★ 很重要：直接結束，避免下面去找 ally
    }

    // ====== 非 Gunslayer：照舊找「同 lane」最近 ally，進入 ATTACK 近戰 ======
    Ally* best = nullptr;
    double best_dx = 1e9;

    for (Ally* a : DC->allies) {
        if (!a || a->is_dead()) continue;

        double ax = a->shape->center_x();
        double ay = a->shape->center_y();

        if (std::abs(ay - cy) > lane_tolerance) continue; // 同 lane
        if (ax <= cx) continue;                           // 只打前方

        double dx_front = ax - cx;
        if (dx_front <= attack_range && dx_front < best_dx) {
            best_dx = dx_front;
            best = a;
        }
    }

    if (best) {
        target_ally = best;
        state = MonsterState::ATTACK;
        attack_cooldown = 0;
    }
    else if (dist_to_base <= attack_range) {
        if (dist_to_base < 0) dist_to_base = 0;
        target_ally = nullptr;
        state = MonsterState::ATTACK;
        attack_cooldown = 0;
    }
}



void Monster::update_attack_state() {
    DataCenter* DC = DataCenter::get_instance();

    double cx = shape->center_x();
    double cy = shape->center_y();

    if(target_ally){
        bool still_exist = false;
        for (Ally* a : DC->allies) {
            if (a == target_ally) {
                still_exist = true;
                break;
            }
        }

        if (!still_exist||target_ally->is_dead()) {
            target_ally = nullptr;
            state = MonsterState::WALK;
            return;
        }

        double ax = target_ally->shape->center_x();
        double ay = target_ally->shape->center_y();

        double dist_x = std::abs(cx - ax);
        double dist_y = std::abs(cy - ay);

        if (dist_y > lane_tolerance || dist_x > attack_range) {
            target_ally = nullptr;
            state = MonsterState::WALK;
            return;
        }

        if (attack_cooldown > 0) {
            --attack_cooldown;
        }
        else {

            target_ally->HP -= atk;
            attack_cooldown = attack_freq;
        }
        return;
    }
    
    if (!DC->right_base || DC->right_base->is_dead()) {
        if (attack_cooldown > 0) attack_cooldown--;
        else {
            DC->player->HP -= atk;
            attack_cooldown = attack_freq;
        }
        return;
    }

    double base_left = DC->right_base->left()+40.0;
    Rectangle* rect = dynamic_cast<Rectangle*>(shape.get());
    double dist_base = rect ? (base_left - rect->x2) : 1e9;

    if (dist_base > attack_range) {
        state = MonsterState::WALK;
        return;
    }

    if (attack_cooldown > 0) {
        attack_cooldown--;
    }
    else {
        DC->right_base->take_damage(atk);
        //debug_log("[TowerHP] right_base HP=%d\n", DC->right_base->HP);
        attack_cooldown = attack_freq;

        if (DC->right_base->is_dead()) {
            DC->player->HP = 0;
        }
    }

    
}

void Monster::draw() {
    if (type == MonsterType::GUNSLAYER) return;
    if (state == MonsterState::DIE)return;
	ImageCenter *IC = ImageCenter::get_instance();
	
	char buffer[50];
	sprintf(
		buffer, "%s/%s_%d.png",
		MonsterSetting::monster_imgs_root_path[static_cast<int>(type)],
		MonsterSetting::dir_path_prefix[static_cast<int>(dir)],
		bitmap_img_ids[static_cast<int>(dir)][bitmap_img_id]);
	ALLEGRO_BITMAP *bitmap = IC->get(buffer);

	//add
	DataCenter *DC=DataCenter::get_instance();
	float cam_x=DC->camerax;
	float cam_y=DC->cameray;

	float world_x=static_cast<float>(shape->center_x());
	float world_y=static_cast<float>(shape->center_y());

	float sc_x=world_x-cam_x-al_get_bitmap_width(bitmap)/2.0f;
	float sc_y=world_y-cam_y-al_get_bitmap_height(bitmap)/2.0f;
	al_draw_bitmap(
		bitmap,
		sc_x,
		sc_y,
		0
	);
}

 void Monster::update_die_state() {

 }


 void Monster::update() {
     //DataCenter* DC = DataCenter::get_instance();
     //ImageCenter* IC = ImageCenter::get_instance();
     if (HP <= 0 && state != MonsterState::DIE) {
         state = MonsterState::DIE;
     }

     if (bitmap_switch_counter > 0) {
         --bitmap_switch_counter;
     }
     else {
         bitmap_img_id = (bitmap_img_id + 1) % (bitmap_img_ids[static_cast<int>(dir)].size());
         bitmap_switch_counter = bitmap_switch_freq;
     }

     switch (state) {
     case MonsterState::WALK:
         update_walk_state();
         break;
     case MonsterState::ATTACK:
         update_attack_state();
         break;
     case MonsterState::DIE:
         update_die_state();
         break;
     }
 }



