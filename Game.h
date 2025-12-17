#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <allegro5/allegro.h>
#include "UI.h"

/**
 * @brief Main class that runs the whole game.
 * @details All game procedures must be processed through this class.
 */
//add
enum class TutorialStage{
    NONE,
    INTRO,
	MOVE,
    SELECT_ALLY,
    PLACE_ALLY,
    WAIT_KILL_ONE,
	DESTROY_TOWER,
    FINISHED
};
class Game
{
public:
	void execute();
public:
	Game(bool testMode = false);
	~Game();
	void game_init();
	bool game_update();
	void game_draw();
private:
	/**
	 * @brief States of the game process in game_update.
	 * @see Game::game_update()
	 */
	enum class STATE {
		START, // -> LEVEL
		LEVEL, // -> PAUSE, END
		PAUSE, // -> LEVEL
		END,
		//add
		WIN,
		LOSE
	};
	STATE state;
	ALLEGRO_EVENT event;
	ALLEGRO_BITMAP *game_icon;
	ALLEGRO_BITMAP *background;
private:
	ALLEGRO_DISPLAY *display;
	ALLEGRO_TIMER *timer;
	ALLEGRO_EVENT_QUEUE *event_queue;
	UI *ui;

	//add
	int cur_level=1;
//add
private:
	TutorialStage tutorial_stage=TutorialStage::NONE;
    void update_tutorial();
    void draw_tutorial();
};

#endif
