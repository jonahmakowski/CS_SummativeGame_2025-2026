#include <string.h>

#include "helpers.hpp"

// Towers

int define_tower_template(Tower &template_tower, const char* image_path, const char* name, float rate, float range, int damage, int cubes) {
    load_image_with_checks(image_path, template_tower.object.image);
    template_tower.object.scale = {1.0f, 1.0f};
    template_tower.object.velocity = {0, 0};
    template_tower.object.position = {0, 0};
    template_tower.object.exists = false;

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

void new_snowman(Tower &tower) {
    if (define_tower_template(tower, "images/sun.png", "Snowman", 2, 500, 100, 0) != 0) {
        printf("Failed to create new tower");
    }
    add_tower(tower)
}

// Enemies

int define_enemy_template(Enemy &enemy_template, const char* image_path, int in_levels[20], int in_levels_count, int health, int reward, int speed, bool is_boss) {
    enemy_template.health = health;
    
    for (int i = 0; i < in_levels_count; i++) {
        enemy_template.in_levels[i] = in_levels[i];
    }
    
    enemy_template.in_levels_count = in_levels_count;
    enemy_template.health = health;
    enemy_template.reward = reward;
    enemy_template.speed = speed;

    enemy_template.is_boss = is_boss;

    load_image_with_checks(image_path, enemy_template.object.image);

    enemy_template.object.scale = {1.0f, 1.0f};
    enemy_template.object.velocity = {0, 0};
    enemy_template.object.position = {0, 0};
    enemy_template.object.exists = false;

    return 0;
}

void new_penguin(Enemy &enemy) {
    int in_levels[20] = {1};
    if (define_enemy_template(enemy, "images/earth.png", in_levels, 1, 10, 1, 10, false) != 0) {
        printf("Failed to create new tower");
    }
    add_enemy(enemy);
}
