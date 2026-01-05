#include <string.h>
#include <stdlib.h>

#include "headers/helpers.hpp"

// Towers
// Return a template tower based on passed parameters
int define_tower_template(Tower &template_tower, const char* image_path, const char* name, float reload_time, float range, int damage, int cubes, const char* projectile_image_path, Vector2 projectile_scale, Vector2 tower_scale, int price, int projectile_area_of_effect, float slowing_amount, float slowing_duration) {
    load_image_with_checks(image_path, template_tower.object.image);
    template_tower.object.scale = tower_scale;
    template_tower.object.velocity = {0, 0};
    template_tower.object.position = {0, 0};
    template_tower.object.exists = true;

    strcpy(template_tower.name, name);

    template_tower.level = 1;
    
    template_tower.reload_time = reload_time;
    template_tower.range = range;
    template_tower.damage = damage;
    
    template_tower.time_since_last_shot = reload_time + 5;
    
    template_tower.ice_cube_price = cubes;
    template_tower.possible_upgrades_count = 0;

    strcpy(template_tower.projectile_image_path, projectile_image_path);

    template_tower.projectile_scale = projectile_scale;

    template_tower.price = price;

    template_tower.projectile_area_of_effect = projectile_area_of_effect;
    template_tower.slowing_amount = slowing_amount;
    template_tower.slowing_duration = slowing_duration;
    
    return 0;
}

// Makes a new instance tower based on type
void new_tower(Tower &tower, TowerType type) {
    tower.type = type;
    switch (type) {
        case SNOWMAN:
            if (define_tower_template(tower, "./images/snowman.png", "Snowman", 1, 300, 0, 0, "./images/snowball.png", {0.1, 0.1}, {1.0f, 1.0f}, 5, 100, 10, 10) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case SNOWBALL_THROWER:
            if (define_tower_template(tower, "./images/child.png", "Snowball Thrower", 1, 500, 100, 1, "./images/snowball.png", {0.1, 0.1}, {1.0f, 1.0f}, 5, 0, 0, 0) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case WATER_BALLOON:
        if (define_tower_template(tower, "./images/WaterBalloonTower.png", "Water Balloon Thrower", 1, 400, 20, 0, "./images/waterBalloon.png", {1, 1}, {1.0f, 1.0f}, 10, 100, 0, 0) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case ICICLE_LAUNCHER:
            if (define_tower_template(tower, "./images/IcicleBallista.png", "Icicle Ballista", 5, 1000, 200, 0, "./images/IcicleProj.png", {0.5, 0.5}, {1.0f, 1.0f}, 30, 0, 0, 0) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case SNOWBLOWER:
            if (define_tower_template(tower, "./images/snowblower.png", "Snowblower", 0.1, 300, 2, 0, "./images/snowball.png", {0.5, 0.5}, {1.0f, 1.0f}, 30, 0, 0, 0) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case MAMMOTH:
            if (define_tower_template(tower, "./images/sun.png", "Mammoth", 5, 300, 500, 0, "./images/sun.png", {0.2, 0.2}, {1.0f, 1.0f}, 50, 0, 0, 0) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case ICE_WIZARD:
            if (define_tower_template(tower, "./images/sun.png", "Ice Wizard (Magic Joe)", 3, 500, 200, 0, "./images/sun.png", {0.2, 0.2}, {1.0f, 1.0f}, 80, 0, 0, 0) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case HOUSE:
            if (define_tower_template(tower, "./images/house.png", "House", 10.0, 0, 20, 0, "./images/sun.png", {0.2, 0.2}, {1.0f, 1.0f}, 100, 0, 0, 0) != 0) {
                printf("Failed to create new tower");
            }
            break;
        default:
            printf("Unknown tower type %d\n", type);
            break;
    }
}

// Enemies
// Return a template enemy based on passed parameters
int define_enemy_template(Enemy &enemy_template, const char* image_path, int health, int reward, int speed, bool is_boss, Vector2 scale) {
    enemy_template.health = health;
    
    enemy_template.health = health;
    enemy_template.max_health = health;
    enemy_template.reward = reward;
    enemy_template.speed = speed;

    enemy_template.is_boss = is_boss;

    load_image_with_checks(image_path, enemy_template.object.image);

    enemy_template.object.scale = scale;
    enemy_template.object.velocity = {0, 0};
    enemy_template.object.position = {0, 0};
    enemy_template.object.exists = true;

    enemy_template.path_index = 0;

    return 0;
}

// Makes a new enemy instance based on the passed type
void new_enemy(Enemy &enemy, EnemyType type) {
    enemy.type = type;
    switch (type) {
        case PENGUIN:
            if (define_enemy_template(enemy, "./images/penguin.png", 10, 1, 5, false, {0.75f, 0.75f}) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case YETI:
            if (define_enemy_template(enemy, "./images/Yeti.png", 50, 10, 2, false, {1.0f, 1.0f}) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case ICE_SPRITE:
            if (define_enemy_template(enemy, "./images/IceSprite.png", 5, 5, 30, false, {0.25f, 0.25f}) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case ICE_GIANT:
            if (define_enemy_template(enemy, "./images/sun.png", 50, 10, 10, false, {0.25f, 0.25f}) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case YAK:
            if (define_enemy_template(enemy, "./images/sun.png", 200, 50, 5, false, {0.25f, 0.25f}) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case RABBIT:
            if (define_enemy_template(enemy, "./images/sun.png", 20, 10, 45, false, {0.25f, 0.25f}) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case FIRE_WIZARD:
            if (define_enemy_template(enemy, "./images/sun.png", 75, 15, 20, false, {0.25f, 0.25f}) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case CAMPFIRE:
            if (define_enemy_template(enemy, "./images/sun.png", 300, 75, 8, false, {0.25f, 0.25f}) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case FIRE_SPRITE:
            if (define_enemy_template(enemy, "./images/sun.png", 30, 10, 60, false, {0.25f, 0.25f}) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case EMBER:
            if (define_enemy_template(enemy, "./images/sun.png", 10, 1, 20, false, {0.25f, 0.25f}) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case NEIGHBOR_BOSS:
            if (define_enemy_template(enemy, "./images/sun.png", 1000, 100, 20, true, {0.25f, 0.25f}) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case TODDLER_BOSS:
            if (define_enemy_template(enemy, "./images/sun.png", 2000, 200, 20, true, {0.25f, 0.25f}) != 0) {
                printf("Failed to create new tower");
            }
            break;
        case SUN_BOSS:
            if (define_enemy_template(enemy, "./images/sun.png", 3000, 300, 20, true, {0.25f, 0.25f}) != 0) {
                printf("Failed to create new tower");
            }
            break;
        default:
            printf("Unknown enemy type %d\n", type);
            break;
    }
}
