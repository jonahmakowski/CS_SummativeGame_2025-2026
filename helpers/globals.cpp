#include "helpers.hpp"

int player_health = 100;
int player_coins = 0;
bool draw_range_circles = false;

ALLEGRO_DISPLAY *display = nullptr;
ALLEGRO_EVENT_QUEUE *event_queue = nullptr;
ALLEGRO_TIMER *timer = nullptr;

Font default_font;
Vector2i mouse_pos = {0, 0};
Camera camera = {{0, 0}, {0, 0}, 1.0f};

Tower *active_towers[100];
Enemy *active_enemies[100];
Projectile *active_projectiles[100];

int active_towers_count = 0;
int active_enemies_count = 0;
int active_projectiles_count = 0;

ALLEGRO_BITMAP* grass_tile;
ALLEGRO_BITMAP* path_tile;
ALLEGRO_BITMAP* tower_spot_tile;
ALLEGRO_BITMAP* enemy_spawn_tile;
ALLEGRO_BITMAP* enemy_goal_tile;

Map active_map;

Panel *buttons[1000];

// Keybinds

Keybind move_up = { { ALLEGRO_KEY_W, ALLEGRO_KEY_UP, -1 } };
Keybind move_down = { { ALLEGRO_KEY_S, ALLEGRO_KEY_DOWN, -1 } };
Keybind move_left = { { ALLEGRO_KEY_A, ALLEGRO_KEY_LEFT, -1 } };
Keybind move_right = { { ALLEGRO_KEY_D, ALLEGRO_KEY_RIGHT, -1 } };
Keybind kill_keybind = { { ALLEGRO_KEY_ESCAPE, -1 } };
Keybind range_circle_toggle = { { ALLEGRO_KEY_R, -1 } };
Keybind fast_forward = { { ALLEGRO_KEY_F, -1 } };