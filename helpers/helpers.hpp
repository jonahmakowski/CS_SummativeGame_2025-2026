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

// Enums

// Enum for different tile types
typedef enum TileType {
    GRASS,
    PATH,
    WATER,
    TOWER_SPOT,
    ENEMY_SPAWN,
    ENEMY_GOAL
} TileType;

typedef enum EnemyType {
    PENGUIN,
    YETI,
    ICE_SPRITE,
    ICE_GIANT,
    YAK,
    RABBIT,
    FIRE_WIZARD,
    CAMPFIRE,
    FIRE_SPRITE,
    EMBER,
    NEIGHBOR_BOSS,
    TODDLER_BOSS,
    SUN_BOSS
} EnemyType;

typedef enum TowerType {
    SNOWMAN,
    SNOWBALL_THROWER,
    WATER_BALLOON,
    ICICLE_LAUNCHER,
    SNOWBLOWER,
    MAMMOTH,
    ICE_WIZARD
} TowerType;

typedef enum ButtonIndex {
    START_WAVE_BUTTON,
    END
} ButtonIndex;

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

extern Font default_font;

// Panel struct representing a UI panel
struct Panel {
    Vector2i top_left;
    Vector2i bottom_right;
    ALLEGRO_COLOR color;

    char text[250];
    ALLEGRO_COLOR text_color = BLACK;
    Font* font = &default_font;

    struct Panel* children[20];
    int child_count = 0;
};

// A Upgrade for a tower
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

    TowerType type;
};

// Enemy struct representing an enemy in the game
struct Enemy {
    Object object;

    int health;
    int max_health;
    float speed;

    int path_index;

    int reward;

    bool is_boss;

    int expected_damage = 0;

    EnemyType type;
};

// Projectile struct representing a projectile in the game
struct Projectile {
    Object object;
    
    struct Enemy* target;

    float speed;
    int damage;
};

// Struct representing a tile on the map
struct MapTile {
    TileType type;
    Vector2i position;
};

// Struct representing a tower spot on the map
struct TowerSpot {
    Vector2i position;
    bool occupied = false;
};

// A struct representing sub-waves of enemies
struct SubWaves {
    int count;
    EnemyType type;
    float interval;

    float delay = 0.0;
    bool delay_passed = false;

    int count_spawned = 0;
    float time_since_last_spawn;
};

// A struct representing a wave of enemies
struct Wave {
    int sub_wave_count = 0;
    bool wave_complete = false;
    SubWaves sub_waves[100];
};

// Map struct representing the game map
struct Map {
    char name[100];
    MapTile tiles[1000];
    int tile_count;

    // In paths, the sequence of tiles that form the enemy path
    // Where index 0 is the spawn point and the last index is the goal
    Vector2i path[1000];
    int path_count;

    TowerSpot tower_spots[1000];
    int tower_spots_count;

    Wave waves[100];
    int wave_count = 0;

    int current_wave_index = 0;

    Vector2i size;
};

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

extern int player_health;
extern int player_coins;
extern bool draw_range_circles;

extern ALLEGRO_DISPLAY *display;
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_TIMER *timer;

extern Vector2i mouse_pos;
extern Camera camera;

extern Tower *active_towers[100];
extern Enemy *active_enemies[100];
extern Projectile *active_projectiles[100];

extern int active_towers_count;
extern int active_enemies_count;
extern int active_projectiles_count;

extern ALLEGRO_BITMAP* grass_tile;
extern ALLEGRO_BITMAP* path_tile;
extern ALLEGRO_BITMAP* tower_spot_tile;
extern ALLEGRO_BITMAP* enemy_spawn_tile;
extern ALLEGRO_BITMAP* enemy_goal_tile;

extern Map active_map;

extern Panel *buttons[1000];

// Keybinds

extern Keybind move_up;
extern Keybind move_down;
extern Keybind move_left;
extern Keybind move_right;
extern Keybind range_circle_toggle;
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
    if ((ev.type == ALLEGRO_EVENT_KEY_DOWN && pressing_keybind(kill_keybind, ev)) || ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { \
        printf("Exiting due to kill keybind or window close\n"); \
        al_destroy_display(display); \
        return 0; \
    } \
}

// Prototypes
// enemies_and_towers.cpp
void new_tower(Tower &tower, TowerType type);
void new_enemy(Enemy &enemy, EnemyType type);

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

// general_functions.cpp
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
void update_camera_position(Vector2i limit_top_left, Vector2i limit_bottom_right);
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
void remove_object_from_array(Projectile arr[], int &count, int index);
void remove_object_from_array(Enemy arr[], int &count, int index);
int index_of_in_array(MapTile tile, MapTile arr[], int count);
int index_of_in_array(Vector2i point, MapTile arr[], int count);
int index_of_in_array(Vector2i point, Vector2i arr[], int count);
Vector2i subtract_vector(Vector2i a, Vector2i b);

// functions.cpp
Vector2i tile_pos_to_pixel_pos(Vector2i tile_pos);
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
int load_tile_images();
void display_map();
void print_map();
bool is_in_array(Vector2i point, Vector2i arr[], int count);
void add_path_points_to_map(Map &map);
Map load_map(const char* file_path);
void run_enemies();
void do_ui();
void handle_button_clicks(ALLEGRO_EVENT ev);
void build_tower_on_click(ALLEGRO_EVENT ev);
