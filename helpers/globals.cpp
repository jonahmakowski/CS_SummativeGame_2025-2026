#include "helpers.hpp"

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

// Keybinds

Keybind move_up = { { ALLEGRO_KEY_W, ALLEGRO_KEY_UP, -1 } };
Keybind move_down = { { ALLEGRO_KEY_S, ALLEGRO_KEY_DOWN, -1 } };
Keybind move_left = { { ALLEGRO_KEY_A, ALLEGRO_KEY_LEFT, -1 } };
Keybind move_right = { { ALLEGRO_KEY_D, ALLEGRO_KEY_RIGHT, -1 } };
Keybind kill_keybind = { { ALLEGRO_KEY_ESCAPE, -1 } };