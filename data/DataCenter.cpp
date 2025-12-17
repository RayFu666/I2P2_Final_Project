#include "DataCenter.h"
#include <cstring>
#include "../Level.h"
#include "../Player.h"
#include "../monsters/Monster.h"
#include "../towers/Tower.h"
#include "../towers/Bullet.h"

#include "../Hero.h"
#include "../ally/Ally.h"
#include "../BaseTower.h"

// fixed settings
namespace DataSetting {
	constexpr double FPS = 60;
	constexpr int window_width = 800;
	constexpr int window_height = 600;
	//change
	constexpr int game_field_length =1200;
}

DataCenter::DataCenter() {
	this->FPS = DataSetting::FPS;
	this->window_width = DataSetting::window_width;
	this->window_height = DataSetting::window_height;
	this->game_field_length = DataSetting::game_field_length;

	//add
	this->camerax=0.0f;
	this->cameray=0.0f;

	//add
	ally_sel=false;
	ally_preview=-1;
	
	memset(key_state, false, sizeof(key_state));
	memset(prev_key_state, false, sizeof(prev_key_state));
	mouse = Point(0, 0);
	memset(mouse_state, false, sizeof(mouse_state));
	memset(prev_mouse_state, false, sizeof(prev_mouse_state));
	player = new Player();
    level = new Level();

    hero = new Hero();
    // ally = new Ally();
}

DataCenter::~DataCenter() {
    delete player;
    delete level;
    for (Monster*& m : monsters) {
        delete m;
    }
    for (Tower*& t : towers) {
        delete t;
    }
    for (Bullet*& b : towerBullets) {
        delete b;
    }
    for (Ally*& a : allies) delete a;
}
//add
void
DataCenter::clear_game(){
	for(Monster*& m:monsters){
        delete m;
    }
    monsters.clear();

    for(Tower*& t:towers){
        delete t;
    }
    towers.clear();

    for(Bullet*& b:towerBullets){
        delete b;
    }
    towerBullets.clear();

    for(Ally*& a:allies){
        delete a;
    }
    allies.clear();
    if (left_base) { delete left_base; left_base = nullptr; }
    if (right_base) { delete right_base; right_base = nullptr; }

}