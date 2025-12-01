#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

// Colors

const ALLEGRO_COLOR RED = al_map_rgb(255, 0, 0);
const ALLEGRO_COLOR GREEN = al_map_rgb(0, 255, 0);
const ALLEGRO_COLOR BLUE = al_map_rgb(0, 0, 255);
const ALLEGRO_COLOR WHITE = al_map_rgb(255, 255, 255);
const ALLEGRO_COLOR BLACK = al_map_rgb(0, 0, 0);

// Structs

// Vector with x,y positions as floats
struct Vector2 {
    float x;
    float y;
};

// Vector with x,y positions as integers
struct Vector2i {
    int x;
    int y;
};

// Object struct representing a game object
struct Object {
    ALLEGRO_BITMAP *image;
    Vector2 scale;
    Vector2i velocity;
    Vector2i position;
    bool exists;
};

// Keybind struct representing a set of keycodes
struct Keybind {
    int keycodes[20];
};

// Camera struct representing the camera's position, velocity, and zoom level
struct Camera {
    Vector2i position;
    Vector2i velocity;
    float zoom;
};

// Font struct representing a font and its size
struct Font {
    ALLEGRO_FONT *font;
    int size;
};

// Panel struct representing a UI panel
struct Panel {
    Vector2i top_left;
    Vector2i bottom_right;
    ALLEGRO_COLOR color;

    char text[250];
    ALLEGRO_COLOR text_color = BLACK;
    Font font;

    struct Panel* children[20];
    int child_count = 0;
};

struct Upgrade {
    char name[100];
    char description[250];
    int price;

    float fire_rate_multiplier;
    float range_multiplier;
    float damage_multiplier;
};

// Tower struct representing a tower in the game
struct Tower {
    Object object;
    int level;

    char name[100];

    float fire_rate;
    float range;
    int damage;

    float time_since_last_shot;

    int ice_cube_price;

    Upgrade possible_upgrades[10];
    int possible_upgrades_count;
};

// Enemy struct representing an enemy in the game
struct Enemy {
    Object object;

    int health;
    int max_health;
    float speed;

    int reward;

    int in_levels[20];
    int in_levels_count;

    bool is_boss;
};

struct Projectile {
    Object object;
    
    struct Enemy* target;

    float speed;
    int damage;
};

// Globals

#define MAX_WIDTH 800
#define MAX_HEIGHT 800
#define FPS 30

#define SCREEN_WIDTH 2160
#define SCREEN_HEIGHT 1440
#define FULLSCREEN false

#define PANEL_ROUNDING 10.0f

#define PROJECTILE_SPEED 10.0f

extern ALLEGRO_DISPLAY *display;
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_TIMER *timer;

extern Font default_font;
extern Vector2i mouse_pos;
extern Camera camera;

extern Tower *active_towers[100];
extern Enemy *active_enemies[100];
extern Projectile *active_projectiles[100];

extern int active_towers_count;
extern int active_enemies_count;
extern int active_projectiles_count;

// Keybinds

extern Keybind move_up;
extern Keybind move_down;
extern Keybind move_left;
extern Keybind move_right;
extern Keybind kill_keybind;

// Defines

#define add_enemy(enemy) { \
    active_enemies[active_enemies_count] = &enemy; \
    active_enemies_count++; \
}

#define add_tower(tower) { \
    active_towers[active_towers_count] = &tower; \
    active_towers_count++; \
}

#define add_projectile(projectile) { \
    active_projectiles[active_projectiles_count] = projectile; \
    active_projectiles_count++; \
}

#define update() al_flip_display()
#define sleep(seconds) al_rest(seconds)
#define fill_screen(color) al_clear_to_color(color)
#define load_image(path) al_load_bitmap(path)

// Macro to load image with error checking, it checks that the image is actually there and loaded properly
#define load_image_with_checks(path, ptr) { \
    ALLEGRO_BITMAP *temp = load_image(path); \
    if (!temp) { \
        printf("Failed to load image: %s\n", path); \
        return 1; \
    } \
    ptr = temp; \
}

// Macro to load font with error checking, it checks that the font is actually there and loaded properly
#define load_font_with_checks(path, size, flags, obj) { \
    ALLEGRO_FONT *temp_font = al_load_ttf_font(path, size, flags); \
    if (!temp_font) { \
        printf("Failed to load font: %s\n", path); \
        return false; \
    } \
    Font result = {temp_font, size}; \
    obj = result; \
}

#define get_display_height() al_get_display_height(display)
#define get_display_width() al_get_display_width(display)

// abs macro, checks if it's greater than 0, if not, makes it positive
#define abs(num) (num < 0) ? -num : num
// max macro, checks if a is greater than b, returns a if true, b if false
#define max(a, b) (a > b) ? a : b
// min macro, checks if a is less than b, returns a if true, b if false
#define min(a, b) (a < b) ? a : b

#define check_for_exit(ev) { \
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN && pressing_keybind(kill_keybind, ev)) { \
        printf("Exiting due to kill keybind\n"); \
        al_destroy_display(display); \
        return 0; \
    } \
}

// Prototypes
// enemies_and_towers.cpp
int define_tower_template(Tower &template_tower, const char* image_path, const char* name, float rate, float range, int damage, int cubes);
void new_snowman(Tower &tower);
int define_enemy_template(Enemy &enemy_template, const char* image_path, int in_levels[20], int in_levels_count, int health, int reward, int speed, bool is_boss);
void new_penguin(Enemy &enemy);

// rewritten_allegro_crap.cpp
void draw_rectangle(Vector2i top_left, Vector2i bottom_right, ALLEGRO_COLOR color);
void draw_rectangle_rounded(Vector2i top_left, Vector2i bottom_right, float radius, ALLEGRO_COLOR color);
void draw_circle(Vector2i center, float radius, ALLEGRO_COLOR color);
void draw_triangle(Vector2i point1, Vector2i point2, Vector2i point3, ALLEGRO_COLOR color);
void draw_line(Vector2i start, Vector2i end, ALLEGRO_COLOR color, float thickness);
void draw_circle_outline(Vector2i center, float radius, ALLEGRO_COLOR color, float thickness);
void draw_image(ALLEGRO_BITMAP *image, Vector2i position);
void draw_text(Font font, ALLEGRO_COLOR color, Vector2i position, const char *text);
Vector2i get_window_size();
void draw_scaled_image(ALLEGRO_BITMAP *image, Vector2i position, Vector2 scale);
bool init_allegro();

// functions.cpp
bool pressing_keybind(Keybind keybind, ALLEGRO_EVENT ev);
Vector2i camera_fixed_position(Vector2i position);
void draw(ALLEGRO_BITMAP *image, Vector2i position, Vector2 scale);
void draw(Object obj);
void draw_child_panel(Panel panel, Vector2i parent_top_left);
void draw(Panel panel);
int add_child_panel(Panel &parent, Panel *child);
void draw(Tower tower);
void draw(Enemy enemy);
void draw(Projectile projectile);
Vector2i get_mouse_pos();
void update_position(Object &obj);
void update_position(Projectile &projectile);
Vector2 normalize(Vector2 vec);
Vector2 get_direction_to(Vector2i from, Vector2i to);
void update_camera_position();
float distance_between(Vector2i a, Vector2i b);
Vector2 get_object_size(Object obj);
Vector2i vector2_to_vector2i(Vector2 vec);
bool is_colliding(Object a, Object b);
bool is_within(Object obj, Vector2i point);
bool is_within(Panel panel, Vector2i point);
bool is_within(Tower tower, Vector2i point);
bool is_within(Enemy enemy, Vector2i point);
bool currently_clicking(Panel panel);
bool currently_clicking(Object obj);
bool currently_clicking(Tower tower);
bool currently_clicking(Enemy enemy);
void remove_object_from_array(Upgrade arr[], int &count, int index);
void apply_upgrade(Tower &tower, int upgrade_index);
void draw_range_circle(Tower tower);
void current_shots();
Vector2i multiply_vector(Vector2i vec, float multiplier);
Vector2 multiply_vector(Vector2 vec, float multiplier);
void shoot_projectile(Tower tower, Enemy* target_enemy);
void recalculate_projectiles();
void draw_all_projectiles();
void draw_all_towers();
void draw_all_enemies();
void check_projectiles();
