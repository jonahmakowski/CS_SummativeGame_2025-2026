#include "structs.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <stdio.h>

// Globals
#define FPS 30

#define SCREEN_WIDTH 1800
#define SCREEN_HEIGHT 1000

#define SCREEN_MIN_WIDTH 1800
#define SCREEN_MIN_HEIGHT 1000

#define INITIAL_MONEY 500 // Default 500

#define DISCARD_COST 5

#define FULLSCREEN false 
#define CANRESIZE true // May cause issues, not tested much

#define PANEL_ROUNDING 10.0f

#define PROJECTILE_SPEED 30.0f

#define TILE_SIZE 128

#define MAP_DIRECTORY "maps/"

#define UPPER_CARD_Y get_display_height()-500

#define HELP_POPUP_TIME 5.0f // seconds

extern bool moved;

extern int player_health;
extern int player_coins;
extern bool draw_range_circles;

extern ALLEGRO_DISPLAY *display;
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_TIMER *timer;

extern Vector2i mouse_pos;
extern Camera camera;

extern Tower active_towers[1000];
extern Enemy active_enemies[1000];
extern Projectile active_projectiles[1000];
extern HouseSpawn active_housespawn[1000];

extern int active_towers_count;
extern int active_enemies_count;
extern int active_projectiles_count;
extern int active_housespawn_count;

extern ALLEGRO_BITMAP* grass_tile_0;
extern ALLEGRO_BITMAP* grass_tile_1;
extern ALLEGRO_BITMAP* grass_tile_2;

extern ALLEGRO_BITMAP* path_tile_0;
extern ALLEGRO_BITMAP* path_tile_1;
extern ALLEGRO_BITMAP* path_tile_2;
extern ALLEGRO_BITMAP* path_tile_3;
extern ALLEGRO_BITMAP* path_tile_4;
extern ALLEGRO_BITMAP* path_tile_5;
extern ALLEGRO_BITMAP* path_tile_6;
extern ALLEGRO_BITMAP* path_tile_7;
extern ALLEGRO_BITMAP* path_tile_8;
extern ALLEGRO_BITMAP* path_tile_9;

extern ALLEGRO_BITMAP* tower_spot_tile;
extern ALLEGRO_BITMAP* enemy_spawn_tile;
extern ALLEGRO_BITMAP* enemy_goal_tile;

extern ALLEGRO_BITMAP* wall_tile;
extern ALLEGRO_BITMAP* cornerl1_wall_tile;
extern ALLEGRO_BITMAP* cornerl2_wall_tile;
extern ALLEGRO_BITMAP* cornerr1_wall_tile;
extern ALLEGRO_BITMAP* cornerr2_wall_tile;

extern ALLEGRO_BITMAP* housespawn_image;

extern Map active_map;

extern Panel buttons[1000];

extern PossibleMap possible_maps[100];
extern int possible_maps_count;

extern GameState game_state;

extern TowerType avalible_deck[100];
extern int avalible_deck_count;
extern Tower current_hand[5];
extern int current_hand_count;

extern Tower to_place;

extern bool show_ui;
extern bool ui_force_hidden;

extern bool show_card_menu;
extern Tower *card_menu_tower;

extern Upgrade upgrades[1000];
extern int upgrades_count;

extern Panel tooltip_panel;
extern Panel tooltip_text_panels[20];
extern int tooltip_text_panels_count;

// Keybinds

extern Keybind move_up;
extern Keybind move_down;
extern Keybind move_left;
extern Keybind move_right;
extern Keybind range_circle_toggle;
extern Keybind kill_keybind;
extern Keybind next_wave;
extern Keybind toggle_ui;
extern Keybind help_menu;
