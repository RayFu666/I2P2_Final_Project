#include "Game.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "data/OperationCenter.h"
#include "data/SoundCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include "Player.h"
#include "Level.h"

#include "Hero.h"
#include "ally/Ally.h"

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include <vector>
#include <cstring>

// fixed settings
constexpr char game_icon_img_path[] = "./assets/image/game_icon.png";
constexpr char game_start_sound_path[] = "./assets/sound/growl.wav";
constexpr char background_img_path[] = "./assets/image/StartBackground.jpg";
constexpr char background_sound_path[] = "./assets/sound/BackgroundMusic.ogg";

// add
const int MAP_WIDTH = 600;

/**
 * @brief Game entry.
 * @details The function processes all allegro events and update the event state to a generic data storage (i.e. DataCenter).
 * For timer event, the game_update and game_draw function will be called if and only if the current is timer.
 */
void Game::execute() {
    DataCenter* DC = DataCenter::get_instance();
    // main game loop
    bool run = true;
    while (run) {
        // process all events here
        al_wait_for_event(event_queue, &event);
        switch (event.type) {
        case ALLEGRO_EVENT_TIMER: {
            run &= game_update();
            game_draw();
            break;
        } case ALLEGRO_EVENT_DISPLAY_CLOSE: { // stop game
            run = false;
            break;
        } case ALLEGRO_EVENT_KEY_DOWN: {
            DC->key_state[event.keyboard.keycode] = true;
            break;
        } case ALLEGRO_EVENT_KEY_UP: {
            DC->key_state[event.keyboard.keycode] = false;
            break;
        } case ALLEGRO_EVENT_MOUSE_AXES: {
            DC->mouse.x = event.mouse.x;
            DC->mouse.y = event.mouse.y;
            break;
        } case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
            DC->mouse_state[event.mouse.button] = true;
            break;
        } case ALLEGRO_EVENT_MOUSE_BUTTON_UP: {
            DC->mouse_state[event.mouse.button] = false;
            break;
        } default: break;
        }
    }
}

/**
 * @brief Initialize all allegro addons and the game body.
 * @details Only one timer is created since a game and all its data should be processed synchronously.
 */
Game::Game(bool testMode) {
    DataCenter* DC = DataCenter::get_instance();
    GAME_ASSERT(al_init(), "failed to initialize allegro.");

    // initialize allegro addons
    bool addon_init = true;
    addon_init &= al_init_primitives_addon();
    addon_init &= al_init_font_addon();
    addon_init &= al_init_ttf_addon();
    addon_init &= al_init_image_addon();
    addon_init &= al_init_acodec_addon();
    GAME_ASSERT(addon_init, "failed to initialize allegro addons.");

    if (testMode) {
        timer = nullptr;
        event_queue = nullptr;
        display = nullptr;
        debug_log("Game initialized in test mode.\n");
        return;
    }

    // initialize events
    bool event_init = true;
    event_init &= al_install_keyboard();
    event_init &= al_install_mouse();
    event_init &= al_install_audio();
    GAME_ASSERT(event_init, "failed to initialize allegro events.");

    // initialize game body
    GAME_ASSERT(
        timer = al_create_timer(1.0 / DC->FPS),
        "failed to create timer.");
    GAME_ASSERT(
        event_queue = al_create_event_queue(),
        "failed to create event queue.");
    GAME_ASSERT(
        display = al_create_display(DC->window_width, DC->window_height),
        "failed to create display.");

    debug_log("Game initialized.\n");
    game_init();
}

/**
 * @brief Initialize all auxiliary resources.
 */
void Game::game_init() {
    DataCenter* DC = DataCenter::get_instance();
    SoundCenter* SC = SoundCenter::get_instance();
    ImageCenter* IC = ImageCenter::get_instance();
    FontCenter* FC = FontCenter::get_instance();
    // set window icon
    game_icon = IC->get(game_icon_img_path);
    al_set_display_icon(display, game_icon);

    // register events to event_queue
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    // init sound setting
    SC->init();

    // init font setting
    FC->init();

    ui = new UI();
    ui->init();

    DC->level->init();

    DC->hero->init();

    // game start
    background = IC->get(background_img_path);
    debug_log("Game state: change to START\n");
    state = STATE::START;
    al_start_timer(timer);
}

/**
 * @brief The function processes all data update.
 * @details The behavior of the whole game body is determined by its state.
 * @return Whether the game should keep running (true) or reaches the termination criteria (false).
 * @see Game::STATE
 */
bool Game::game_update() {
    DataCenter* DC = DataCenter::get_instance();
    OperationCenter* OC = OperationCenter::get_instance();
    SoundCenter* SC = SoundCenter::get_instance();
    static ALLEGRO_SAMPLE_INSTANCE* background = nullptr;

    switch (state) {
    case STATE::START: {
        static bool is_played = false;
        static ALLEGRO_SAMPLE_INSTANCE* instance = nullptr;

        if (!is_played) {
            instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
            al_set_sample_instance_gain(instance, 0.1f);
            //DC->level->load_level(5);
            is_played = true;
        }
        // add
        int selected = -1;
        if (DC->key_state[ALLEGRO_KEY_1] && !DC->prev_key_state[ALLEGRO_KEY_1]) selected = 1;
        if (DC->key_state[ALLEGRO_KEY_2] && !DC->prev_key_state[ALLEGRO_KEY_2]) selected = 2;
        if (DC->key_state[ALLEGRO_KEY_3] && !DC->prev_key_state[ALLEGRO_KEY_3]) selected = 3;
        if (DC->key_state[ALLEGRO_KEY_4] && !DC->prev_key_state[ALLEGRO_KEY_4]) selected = 4;
        if (DC->key_state[ALLEGRO_KEY_5] && !DC->prev_key_state[ALLEGRO_KEY_5]) selected = 5;
        if (selected != -1) {
            cur_level = selected;
            DC->clear_game();
            DC->level->init();
            DC->level->load_level(cur_level);
            DC->hero->init();
            DC->player->rst();

            debug_log("<Game> state: change to LEVEL\n");
            state = STATE::LEVEL;
        }
        break;
    }
    case STATE::LEVEL: {
        static bool BGM_played = false;
        if (!BGM_played) {
            background = SC->play(background_sound_path, ALLEGRO_PLAYMODE_LOOP);
            al_set_sample_instance_gain(background, 0.1f);
            BGM_played = true;
        }

        if (DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) {
            SC->toggle_playing(background);
            debug_log("<Game> state: change to PAUSE\n");
            state = STATE::PAUSE;
        }

        if (DC->level->remain_monsters() == 0 && DC->monsters.size() == 0) {
            debug_log("<Game> state: change to WIN\n");
            state = STATE::WIN;
        }
        if (DC->player->HP <= 0) {
            debug_log("<Game> state: change to LOSE\n");
            state = STATE::LOSE;
        }
        break;
    }
    case STATE::PAUSE: {
        if (DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) {
            SC->toggle_playing(background);
            debug_log("<Game> state: change to LEVEL\n");
            state = STATE::LEVEL;
        }
        break;
    }
                     // add
    case STATE::WIN: {
        if (DC->key_state[ALLEGRO_KEY_R] && !DC->prev_key_state[ALLEGRO_KEY_R]) {
            DC->clear_game();
            DC->level->init();
            DC->level->load_level(cur_level);
            DC->hero->init();
            DC->player->rst();
            state = STATE::LEVEL;
        }
        if (DC->key_state[ALLEGRO_KEY_ENTER] && !DC->prev_key_state[ALLEGRO_KEY_ENTER]) {
            state = STATE::START;
        }
        if (DC->key_state[ALLEGRO_KEY_ESCAPE] && !DC->prev_key_state[ALLEGRO_KEY_ESCAPE]) {
            state = STATE::END;
        }
        break;
    }
    case STATE::LOSE: {
        if (DC->key_state[ALLEGRO_KEY_R] && !DC->prev_key_state[ALLEGRO_KEY_R]) {
            DC->clear_game();
            DC->level->init();
            DC->level->load_level(cur_level);
            DC->hero->init();
            DC->player->rst();
            state = STATE::LEVEL;
        }
        if (DC->key_state[ALLEGRO_KEY_ENTER] && !DC->prev_key_state[ALLEGRO_KEY_ENTER]) {
            state = STATE::START;
        }
        if (DC->key_state[ALLEGRO_KEY_ESCAPE] && !DC->prev_key_state[ALLEGRO_KEY_ESCAPE]) {
            state = STATE::END;
        }
        break;
    }
    case STATE::END: {
        return false;
    }
    }

    // If the game is not paused, we should progress update.
    if (state == STATE::LEVEL) {
        DC->player->update();
        SC->update();
        ui->update();

        DC->hero->update();
        const int ALLY_COST = 50;

        if (state == STATE::LEVEL &&
            DC->mouse_state[1] &&
            !DC->prev_mouse_state[1] &&
            DC->mouse.x < MAP_WIDTH &&
            DC->ally_sel &&
            DC->ally_preview != -1
            ) {
            if (DC->player->coin >= ALLY_COST) {
                float cam_x = DC->camerax;
                double world_x = DC->mouse.x + cam_x;
                if (world_x < 0) world_x = 0;
                if (world_x > DC->game_field_length) world_x = DC->game_field_length;

                int lane_id = DC->ally_preview;
                double spawn_y = AllyLaneSetting::lane_y_by_id(lane_id);
                Point spawn_pos{ world_x, spawn_y };

                DC->allies.emplace_back(new Ally(spawn_pos, lane_id));

                DC->player->coin -= ALLY_COST;
                DC->ally_preview = -1;
                DC->ally_sel = false;
                DC->ally_type = -1;
            }
            else {
                debug_log("Not enough coin to summon Ally. coin = %d\n", DC->player->coin);
            }
        }

        if (state == STATE::LEVEL &&
            DC->mouse_state[1] && !DC->prev_mouse_state[1] &&
            DC->mouse.x >= MAP_WIDTH) {

            DC->ally_sel = true;
            DC->ally_type = 0;
        }

        if (DC->ally_sel && DC->mouse.x < MAP_WIDTH) {
            int lane_id = AllyLaneSetting::nearest_lane_id(DC->mouse.y);
            DC->ally_preview = lane_id;
        }
        else {
            DC->ally_preview = -1;
        }

        for (Ally* a : DC->allies) {
            a->update();
        }

        auto& allies = DC->allies;
        for (auto it = allies.begin(); it != allies.end(); ) {
            Ally* a = *it;
            if (a->can_remove()) {
                delete a;
                it = allies.erase(it);
            }
            else {
                ++it;
            }
        }

        if (state != STATE::START) {
            DC->level->update();
            OC->update();
        }
    }

    double hero_x = DC->hero->center_x();
    float camx = static_cast<float>(hero_x - MAP_WIDTH / 2.0f);
    float maxcam = static_cast<float>(DC->game_field_length - MAP_WIDTH);
    if (maxcam < 0) {
        DC->camerax = 0.0f;
    }
    else {
        if (camx < 0) camx = 0;
        if (maxcam < camx) camx = maxcam;
        DC->camerax = camx;
    }

    memcpy(DC->prev_key_state, DC->key_state, sizeof(DC->key_state));
    memcpy(DC->prev_mouse_state, DC->mouse_state, sizeof(DC->mouse_state));
    return true;
}

/**
 * @brief Draw the whole game and objects.
 */
void Game::game_draw() {
    DataCenter* DC = DataCenter::get_instance();
    OperationCenter* OC = OperationCenter::get_instance();
    FontCenter* FC = FontCenter::get_instance();

    al_clear_to_color(al_map_rgb(100, 100, 100));

    if (state != STATE::END) {
        float camx = DC->camerax;

        // 先允許畫整個畫面
        al_set_clipping_rectangle(0, 0, DC->window_width, DC->window_height);

        // 背景 + 右側灰色選單區
        al_draw_bitmap(background, -camx * 0.3f, 0, 0);
        al_draw_filled_rectangle(
            MAP_WIDTH, 0,
            DC->window_width, DC->window_height,
            al_map_rgb(100, 100, 100)
        );

        // 只讓「地圖世界」畫在左半邊
        al_set_clipping_rectangle(0, 0, MAP_WIDTH, DC->window_height);

        if (state != STATE::START) {
            DC->level->draw();
            DC->hero->draw();

            for (Ally* a : DC->allies) {
                a->draw();
            }

            OC->draw();
        }

        // 還原 clipping，接下來畫 overlay / UI
        al_set_clipping_rectangle(0, 0, DC->window_width, DC->window_height);

        if (state != STATE::START) {
            if (DC->ally_sel && DC->ally_preview != -1) {
                double lane_y = AllyLaneSetting::lane_y_by_id(DC->ally_preview);
                float half_h = 20.0f;

                al_draw_filled_rectangle(
                    0.0f,
                    static_cast<float>(lane_y - half_h),
                    static_cast<float>(MAP_WIDTH),
                    static_cast<float>(lane_y + half_h),
                    al_map_rgba(255, 255, 255, 210)
                );
            }

            ui->draw();
        }
    }

    switch (state) {
    case STATE::START: {
        al_draw_filled_rectangle(
            0, 0, DC->window_width, DC->window_height,
            al_map_rgba(0, 0, 0, 120)
        );
        al_draw_text(
            FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
            DC->window_width / 2.0, 150,
            ALLEGRO_ALIGN_CENTRE, "Select Level"
        );
        al_draw_text(
            FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255),
            DC->window_width / 2.0, 220,
            ALLEGRO_ALIGN_CENTRE, "[1]Level 1"
        );
        al_draw_text(
            FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255),
            DC->window_width / 2.0, 260,
            ALLEGRO_ALIGN_CENTRE, "[2]Level 2"
        );
        al_draw_text(
            FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255),
            DC->window_width / 2.0, 300,
            ALLEGRO_ALIGN_CENTRE, "[3]Level 3"
        );
        al_draw_text(
            FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255),
            DC->window_width / 2.0, 340,
            ALLEGRO_ALIGN_CENTRE, "[4]Level 4"
        );
        al_draw_text(
            FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255),
            DC->window_width / 2.0, 380,
            ALLEGRO_ALIGN_CENTRE, "[5]Level 5"
        );
        break;
    }
    case STATE::LEVEL: {
        break;
    }
    case STATE::PAUSE: {
        al_draw_filled_rectangle(0, 0, DC->window_width, DC->window_height,
            al_map_rgba(50, 50, 50, 64));
        al_draw_text(
            FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
            DC->window_width / 2., DC->window_height / 2.,
            ALLEGRO_ALIGN_CENTRE, "GAME PAUSED");
        break;
    }
    case STATE::WIN: {
        al_draw_filled_rectangle(
            0, 0, DC->window_width, DC->window_height,
            al_map_rgba(0, 0, 0, 150)
        );
        al_draw_text(
            FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 0),
            DC->window_width / 2.0, DC->window_height / 2.0 - 40,
            ALLEGRO_ALIGN_CENTRE, "YOU WIN!"
        );
        // 用你隊友較細分的三行提示
        al_draw_text(
            FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255),
            DC->window_width / 2.0, DC->window_height / 2.0 + 10,
            ALLEGRO_ALIGN_CENTRE, "R:Retry"
        );
        al_draw_text(
            FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255),
            DC->window_width / 2.0, DC->window_height / 2.0 + 60,
            ALLEGRO_ALIGN_CENTRE, "ENTER:Level Select"
        );
        al_draw_text(
            FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255),
            DC->window_width / 2.0, DC->window_height / 2.0 + 110,
            ALLEGRO_ALIGN_CENTRE, "esc:quit"
        );
        break;
    }
    case STATE::LOSE: {
        al_draw_filled_rectangle(
            0, 0, DC->window_width, DC->window_height,
            al_map_rgba(0, 0, 0, 150)
        );
        al_draw_text(
            FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 0),
            DC->window_width / 2.0, DC->window_height / 2.0 - 40,
            ALLEGRO_ALIGN_CENTRE, "YOU LOSE!"
        );
        al_draw_text(
            FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255),
            DC->window_width / 2.0, DC->window_height / 2.0 + 10,
            ALLEGRO_ALIGN_CENTRE, "R:Retry"
        );
        al_draw_text(
            FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255),
            DC->window_width / 2.0, DC->window_height / 2.0 + 60,
            ALLEGRO_ALIGN_CENTRE, "ENTER:Level Select"
        );
        al_draw_text(
            FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255),
            DC->window_width / 2.0, DC->window_height / 2.0 + 110,
            ALLEGRO_ALIGN_CENTRE, "esc:quit"
        );
        break;
    }
    case STATE::END: {
        break;
    }
    }

    al_flip_display();
}

Game::~Game() {
    if (display) al_destroy_display(display);
    if (timer) al_destroy_timer(timer);
    if (event_queue) al_destroy_event_queue(event_queue);
}
