#include "headers/helpers.hpp"

bool moved = false;

int player_health = 100;
int player_coins = 1000;
bool draw_range_circles = false;

ALLEGRO_DISPLAY *display = nullptr;
ALLEGRO_EVENT_QUEUE *event_queue = nullptr;
ALLEGRO_TIMER *timer = nullptr;

Font default_font;
Vector2i mouse_pos = {0, 0};
Camera camera = {{0, 0}, {0, 0}};

Tower active_towers[1000];
Enemy active_enemies[1000];
Projectile active_projectiles[1000];
HouseSpawn active_housespawn[1000];

int active_towers_count = 0;
int active_enemies_count = 0;
int active_projectiles_count = 0;
int active_housespawn_count = 0;

ALLEGRO_BITMAP* grass_tile_0;
ALLEGRO_BITMAP* grass_tile_1;
ALLEGRO_BITMAP* grass_tile_2;

ALLEGRO_BITMAP* path_tile_0;
ALLEGRO_BITMAP* path_tile_1;
ALLEGRO_BITMAP* path_tile_2;
ALLEGRO_BITMAP* path_tile_3;
ALLEGRO_BITMAP* path_tile_4;
ALLEGRO_BITMAP* path_tile_5;
ALLEGRO_BITMAP* path_tile_6;
ALLEGRO_BITMAP* path_tile_7;
ALLEGRO_BITMAP* path_tile_8;
ALLEGRO_BITMAP* path_tile_9;

ALLEGRO_BITMAP* tower_spot_tile;
ALLEGRO_BITMAP* enemy_spawn_tile;
ALLEGRO_BITMAP* enemy_goal_tile;

ALLEGRO_BITMAP* housespawn_image;

Map active_map;

Panel buttons[1000];

PossibleMap possible_maps[100];
int possible_maps_count = 0;

GameState game_state = MAIN_MENU;

TowerType avalible_deck[100] = {TowerType::SNOWBALL_THROWER, TowerType::WATER_BALLOON, TowerType::ICICLE_LAUNCHER, TowerType::SNOWBLOWER, TowerType::SNOWMAN, TowerType::HOUSE};
int avalible_deck_count = 6;
Tower current_hand[5];
int current_hand_count = 0;

Tower to_place;

bool show_ui = true;
bool ui_force_hidden = false;

bool show_card_menu = false;
Tower *card_menu_tower = nullptr;

Upgrade upgrades[1000];
int upgrades_count = 0;

// Keybinds

Keybind move_up = { { ALLEGRO_KEY_W, ALLEGRO_KEY_UP, -1 } };
Keybind move_down = { { ALLEGRO_KEY_S, ALLEGRO_KEY_DOWN, -1 } };
Keybind move_left = { { ALLEGRO_KEY_A, ALLEGRO_KEY_LEFT, -1 } };
Keybind move_right = { { ALLEGRO_KEY_D, ALLEGRO_KEY_RIGHT, -1 } };
Keybind kill_keybind = { { ALLEGRO_KEY_BACKQUOTE, -1 } };
Keybind range_circle_toggle = { { ALLEGRO_KEY_R, -1 } };
Keybind fast_forward = { { ALLEGRO_KEY_F, -1 } };
Keybind next_wave = { { ALLEGRO_KEY_SPACE, -1} };
Keybind toggle_ui = { { ALLEGRO_KEY_Q, -1 } };
Keybind help_menu = { { ALLEGRO_KEY_H, -1 } };
