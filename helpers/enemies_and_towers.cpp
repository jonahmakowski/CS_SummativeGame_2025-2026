#include <string.h>

#include "helpers.hpp"

// Towers

int define_tower_template(Tower &template_tower, const char* image_path, const char* name, float rate, float range, int damage, int cubes) {
    load_image_with_checks(image_path, template_tower.object.image);
    template_tower.object.scale = {1.0f, 1.0f};
    template_tower.object.velocity = {0, 0};
    template_tower.object.position = {0, 0};
    template_tower.object.exists = true;

    strcpy(template_tower.name, name);

    template_tower.level = 1;
    
    template_tower.fire_rate = rate;
    template_tower.range = range;
    template_tower.damage = damage;
    
    template_tower.time_since_last_shot = rate + 5;
    
    template_tower.ice_cube_price = cubes;
    template_tower.possible_upgrades_count = 0;
    
    return 0;
}

void new_tower(Tower &tower, TowerType type) {
    tower.type = type;
    switch (type) {
        case SNOWMAN:
            if (define_tower_template(tower, "images/sun.png", "Snowman", 2, 500, 100, 0) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case SNOWBALL_THROWER:
            if (define_tower_template(tower, "images/sun.png", "Snowball Thrower", 2, 500, 100, 1) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case WATER_BALLOON:
        if (define_tower_template(tower, "images/sun.png", "Water Balloon Thrower", 2, 500, 100, 0) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case ICICLE_LAUNCHER:
            if (define_tower_template(tower, "images/sun.png", "Icicle Launcher", 2, 500, 100, 0) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case SNOWBLOWER:
            if (define_tower_template(tower, "images/sun.png", "Snowblower", 2, 500, 100, 0) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case MAMMOTH:
            if (define_tower_template(tower, "images/sun.png", "Mammoth", 2, 500, 100, 0) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case ICE_WIZARD:
            if (define_tower_template(tower, "images/sun.png", "Magic Joe", 2, 500, 100, 0) != 0) {
                printf("Failed to create new tower");
            }
            break;
        default:
            printf("Unknown tower type %d\n", type);
            break;
    }
}

// Enemies

int define_enemy_template(Enemy &enemy_template, const char* image_path, int health, int reward, int speed, bool is_boss) {
    enemy_template.health = health;
    
    enemy_template.health = health;
    enemy_template.reward = reward;
    enemy_template.speed = speed;

    enemy_template.is_boss = is_boss;

    load_image_with_checks(image_path, enemy_template.object.image);

    enemy_template.object.scale = {1.0f, 1.0f};
    enemy_template.object.velocity = {0, 0};
    enemy_template.object.position = {0, 0};
    enemy_template.object.exists = true;

    enemy_template.path_index = 0;

    return 0;
}

void new_enemy(Enemy &enemy, EnemyType type) {
    enemy.type = type;
    switch (type) {
        case PENGUIN:
            if (define_enemy_template(enemy, "images/earth.png", 10, 1, 10, false) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case YETI:
            if (define_enemy_template(enemy, "images/sun.png", 100, 10, 2, false) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case ICE_SPRITE:
            if (define_enemy_template(enemy, "images/sun.png", 5, 5, 30, false) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case ICE_GIANT:
            if (define_enemy_template(enemy, "images/sun.png", 50, 10, 15, false) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case YAK:
            if (define_enemy_template(enemy, "images/sun.png", 200, 50, 5, false) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case RABBIT:
            if (define_enemy_template(enemy, "images/sun.png", 20, 10, 45, false) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case FIRE_WIZARD:
            if (define_enemy_template(enemy, "images/sun.png", 75, 15, 20, false) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case CAMPFIRE:
            if (define_enemy_template(enemy, "images/sun.png", 300, 75, 8, false) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case FIRE_SPRITE:
            if (define_enemy_template(enemy, "images/sun.png", 30, 10, 60, false) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case EMBER:
            if (define_enemy_template(enemy, "images/sun.png", 10, 1, 20, false) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case NEIGHBOR_BOSS:
            if (define_enemy_template(enemy, "images/sun.png", 1000, 100, 20, true) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case TODDLER_BOSS:
            if (define_enemy_template(enemy, "images/sun.png", 2000, 200, 20, true) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case SUN_BOSS:
            if (define_enemy_template(enemy, "images/sun.png", 3000, 300, 20, true) != 0) {
                printf("Failed to create new tower");
            }
            break;
        default:
            printf("Unknown enemy type %d\n", type);
            break;
    }
}
