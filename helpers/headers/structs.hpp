#include "colors.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

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
    ICE_WIZARD,
    HOUSE
} TowerType;

typedef enum ButtonIndex {
    START_WAVE_BUTTON,
    END
} ButtonIndex;

typedef enum GameState {
    MAIN_MENU,
    IN_GAME,
} GameState;

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
    float rotation_degrees = 0;
    bool exists = false;
};

// Keybind struct representing a set of keycodes
struct Keybind {
    int keycodes[20];
};

// Camera struct representing the camera's position, velocity
struct Camera {
    Vector2i position;
    Vector2i velocity;
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

    ALLEGRO_COLOR border_color = BLACK;
    float border_thickness = 3.0f;
    bool has_border = false;

    char text[250];
    ALLEGRO_COLOR text_color = BLACK;
    Font* font = &default_font;

    bool exists = false;
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

    float reload_time;
    int range;
    float damage;

    float time_since_last_shot;

    int ice_cube_price;

    Upgrade possible_upgrades[10];
    int possible_upgrades_count;

    TowerType type;

    char projectile_image_path[100];
    Vector2 projectile_scale;

    int price;

    bool aimed_this_frame = false;
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

    int index = 0;
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
    int variation;
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
    int map_num;

    // In paths, the sequence of tiles that form the enemy path
    // Where index 0 is the spawn point and the last index is the goal
    Vector2i path[1000];
    int path_count;

    TowerSpot tower_spots[1000];
    int tower_spots_count;

    Wave waves[100];
    int wave_count = 0;

    int current_wave_index = -1;

    Vector2i size;
};

// Struct representing a possible map for the main menu
struct PossibleMap {
    char name[200];
    char file_path[200];
    int map_num;
};
