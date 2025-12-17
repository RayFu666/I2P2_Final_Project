#include "UI.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include <algorithm>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include "shapes/Point.h"
#include "shapes/Rectangle.h"
#include "Player.h"
#include "towers/Tower.h"
#include "Level.h"

// fixed settings
constexpr char love_img_path[] = "./assets/image/love.png";
constexpr int love_img_padding = 5;
constexpr int tower_img_left_padding = 30;
constexpr int tower_img_top_padding = 30;

void
UI::init() {
	//DataCenter *DC = DataCenter::get_instance();
	ImageCenter *IC = ImageCenter::get_instance();
	love = IC->get(love_img_path);

	//add,change
	constexpr int MAP_WIDTH=600;
	int tl_x =MAP_WIDTH+ tower_img_left_padding;
	int tl_y = tower_img_top_padding;
	//int max_height = 0;
	// arrange tower shop

    ally_place.clear();

    int ally_price = 50;

    // BASIC
    {
        ALLEGRO_BITMAP* bitmap = IC->get("./assets/image/ally/black_dude.png");
        ally_place.emplace_back(bitmap, Point{ tl_x, tl_y }, ally_price, AllyType::BASIC);
    }

    // VIKINGMAN (你自己換成 VikingMan 用的 icon；暫時也可先用同一張測試)
    {
        ALLEGRO_BITMAP* bitmap = IC->get("./assets/image/ally/VikingManPress.png");
        ally_place.emplace_back(bitmap, Point{ tl_x, tl_y + 80 }, ally_price, AllyType::VIKINGMAN);
    }

	//for(size_t i = 0; i < (size_t)(TowerType::TOWERTYPE_MAX); ++i) {
		//change
		
		

		//ALLEGRO_BITMAP *bitmap = IC->get(TowerSetting::tower_menu_img_path[i]);
		// int w = al_get_bitmap_width(bitmap);
		// int h = al_get_bitmap_height(bitmap);
		// if(tl_x + w > DC->window_width) {
		// 	//change
		// 	tl_x =MAP_WIDTH + tower_img_left_padding;
		// 	tl_y += max_height + tower_img_top_padding;
		// 	max_height = 0;
		// }
		// tower_items.emplace_back(bitmap, Point{tl_x, tl_y}, TowerSetting::tower_price[i]);
		// tl_x += w + tower_img_left_padding;
		// max_height = std::max(max_height, h);
	//}
	debug_log("<UI> state: change to HALT\n");
	state = STATE::HALT;
	on_item = -1;
}

void
UI::update() {
	DataCenter *DC = DataCenter::get_instance();
	const Point &mouse = DC->mouse;

	switch(state) {
		case STATE::HALT: {
			on_item=-1;
			for(size_t i = 0; i < ally_place.size(); ++i) {
                auto& [bitmap, p, price, type] = ally_place[i];
				//int w = al_get_bitmap_width(bitmap);
				//int h = al_get_bitmap_height(bitmap);
				// hover on a shop tower item
				if(mouse.overlap(Rectangle{p.x, p.y, p.x+48, p.y+48})) {
					on_item = i;
					debug_log("<UI> state: change to HOVER\n");
					state = STATE::HOVER;
					break;
				}
			}
			break;
		}
		 case STATE::HOVER: {
             auto& [bitmap, p, price, type] = ally_place[on_item];
			//int w = al_get_bitmap_width(bitmap);
			//int h = al_get_bitmap_height(bitmap);
			if(!mouse.overlap(Rectangle{p.x, p.y, p.x+48, p.y+48})) {
				on_item = -1;
				debug_log("<UI> state: change to HALT\n");
				state = STATE::HALT;
				break;
			}
			// click mouse left button
			//change
			if(DC->mouse_state[1] && !DC->prev_mouse_state[1]) {
				// no money
				if(price > DC->player->coin) {
					debug_log("<UI> Not enough money to buy ally %d.\n", on_item);
					//break;
				}else{

					DC->ally_sel=true;
                    DC->ally_type = type;

                    debug_log("[UI] Select ally_type = %d\n", (int)DC->ally_type);
                }
				debug_log("<UI> state: change to HALT\n");
				state = STATE::HALT;
			}
			break;
		} 
		default:{
			state=STATE::HALT;
			break;
		}
		//case STATE::SELECT: {
		// 	// click mouse left button: place
		// 	if(DC->mouse_state[1] && !DC->prev_mouse_state[1]) {
		// 		debug_log("<UI> state: change to PLACE\n");
		// 		state = STATE::PLACE;
		// 	}
		// 	// click mouse right button: cancel
		// 	if(DC->mouse_state[2] && !DC->prev_mouse_state[2]) {
		// 		on_item = -1;
		// 		debug_log("<UI> state: change to HALT\n");
		// 		state = STATE::HALT;
		// 	}
		// 	break;
		// } case STATE::PLACE: {
		// 	// check placement legality
		// 	ALLEGRO_BITMAP *bitmap = Tower::get_bitmap(static_cast<TowerType>(on_item));
		// 	int w = al_get_bitmap_width(bitmap);
		// 	int h = al_get_bitmap_height(bitmap);
		// 	const int MAP_WIDTH=600;
		// 	//add,debug,chamge
		// 	if (mouse.x >=MAP_WIDTH) {
		// 		debug_log("<UI> click in UI panel, cancel tower placement.\n");
		// 		on_item=-1;
		// 		state=STATE::HALT;
		// 		break;
		// 	}
		// 	float camx=DC->camerax;
		// 	int world_x=static_cast<int>(mouse.x+camx);
		// 	int world_y=static_cast<int>(mouse.y);


		// 	Rectangle place_region{
		// 		world_x - w / 2,
		// 		world_y - h / 2, 
		// 		world_x + w / 2, 
		// 		world_y + h / 2
		// 	};
		// 	bool place = true;
		// 	// tower cannot be placed on the road
		// 	place &= (!DC->level->is_onroad(place_region));
		// 	// tower cannot intersect with other towers
		// 	for(Tower *tower : DC->towers) {
		// 		place &= (!place_region.overlap(tower->get_region()));
		// 	}
		// 	if(!place) {
		// 		debug_log("<UI> Tower place failed.\n");
		// 	} else {
		// 		DC->towers.emplace_back(
		// 			Tower::create_tower(
		// 				static_cast<TowerType>(on_item),Point{world_x,world_y}
		// 			));
		// 		DC->player->coin -= std::get<2>(ally_place[on_item]);
		// 	}
		// 	debug_log("<UI> state: change to HALT\n");
		// 	state = STATE::HALT;
		// 	break;
		// }
	}
}

void
UI::draw() {
	DataCenter *DC = DataCenter::get_instance();
	FontCenter *FC = FontCenter::get_instance();
	//const Point &mouse = DC->mouse;
	// draw HP
	//const int &game_field_length = DC->game_field_length;
	const int &player_HP = DC->player->HP;

	//add
	constexpr int MAP_WIDTH=600;
	const int panel=MAP_WIDTH;
	ALLEGRO_FONT* hp_font=FC->courier_new[FontSize::LARGE];
	//add
	al_draw_textf(
		FC->courier_new[FontSize::MEDIUM],
		al_map_rgb(255,0,0),
		panel + love_img_padding,
		DC->window_height+love_img_padding-al_get_font_line_height(hp_font),
		ALLEGRO_ALIGN_LEFT,
		"HP: %d", player_HP
	);
	// int love_width = al_get_bitmap_width(love);
	// for(int i = 1; i <= player_HP; ++i) {
	// 	al_draw_bitmap(
	// 		love,
	// 		//change
	// 		panel+love_img_padding + (love_width + love_img_padding) * i,
	// 		love_img_padding,
	// 		0
	// 	);
	// }
	// draw coin
	const int &player_coin = DC->player->coin;
	al_draw_textf(
		FC->courier_new[FontSize::MEDIUM],al_map_rgb(0, 0, 0),
		panel+love_img_padding,
		love_img_padding,
		ALLEGRO_ALIGN_LEFT, "coin: %5d", player_coin);
	// draw ally shop items
    // draw ally shop items
    for (size_t i = 0; i < ally_place.size(); ++i) {
        auto& [bitmap, p, price, type] = ally_place[i];

        // icon
        al_draw_scaled_bitmap(bitmap, 0, 0,
            al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap),
            p.x, p.y, 48, 48, 0);

        // border
        al_draw_rectangle(
            p.x - 1, p.y - 1,
            p.x + 48 + 1, p.y + 48 + 1,
            al_map_rgb(0, 0, 0), 1);

        // price
        al_draw_textf(
            FC->courier_new[FontSize::MEDIUM], al_map_rgb(0, 0, 0),
            p.x + 48 / 2, p.y + 48,
            ALLEGRO_ALIGN_CENTRE, "%d", price);

        // selected highlight
        if (DC->ally_sel && type == DC->ally_type) {
            al_draw_rectangle(
                p.x - 3, p.y - 3,
                p.x + 48 + 3, p.y + 48 + 3,
                al_map_rgb(255, 255, 0), 2
            );
        }
    }


	switch(state) {
		static Tower *selected_tower = nullptr;
		case STATE::HALT: {
			// No tower should be selected for HALT state.
			if(selected_tower != nullptr) {
				delete selected_tower;
				selected_tower = nullptr;
			}
			break;
		} case STATE::HOVER: {
            auto& [bitmap, p, price, type] = ally_place[on_item];
			//int w = al_get_bitmap_width(bitmap);
			//int h = al_get_bitmap_height(bitmap);
			// Create a semitransparent mask covered on the hovered tower.
			al_draw_filled_rectangle(p.x, p.y, p.x + 48, p.y + 48, al_map_rgba(50, 50, 50, 64));
			break;
		}
		// case STATE::SELECT: {
		// 	// If a tower is selected, we new a corresponding tower for previewing purpose.
		// 	if(selected_tower == nullptr) {
		// 		selected_tower = Tower::create_tower(static_cast<TowerType>(on_item), mouse);
		// 	} else {
		// 		selected_tower->shape->update_center_x(mouse.x);
		// 		selected_tower->shape->update_center_y(mouse.y);
		// 	}
		// }
		// case STATE::PLACE: {
		// 	// If we select a tower from menu, we need to preview where the tower will be built and its attack range.
		// 	ALLEGRO_BITMAP *bitmap = Tower::get_bitmap(static_cast<TowerType>(on_item));
		// 	al_draw_filled_circle(mouse.x, mouse.y, selected_tower->attack_range(), al_map_rgba(255, 0, 0, 32));
		// 	int w = al_get_bitmap_width(bitmap);
		// 	int h = al_get_bitmap_height(bitmap);
		// 	al_draw_bitmap(bitmap, mouse.x - w / 2, mouse.y - h / 2, 0);
		// 	break;
		// }
	}
}
