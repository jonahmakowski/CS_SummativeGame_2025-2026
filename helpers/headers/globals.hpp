#include "structs.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

// Globals

#define MAX_WIDTH 800
#define MAX_HEIGHT 800
#define FPS 30

#define SCREEN_WIDTH 2160
#define SCREEN_HEIGHT 1440
#define FULLSCREEN false // May cause issues, not tested much

#define PANEL_ROUNDING 10.0f

#define PROJECTILE_SPEED 20.0f

#define TILE_SIZE 128

#define MAP_DIRECTORY "maps/"

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

extern int active_towers_count;
extern int active_enemies_count;
extern int active_projectiles_count;

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

// Keybinds

extern Keybind move_up;
extern Keybind move_down;
extern Keybind move_left;
extern Keybind move_right;
extern Keybind range_circle_toggle;
extern Keybind kill_keybind;
