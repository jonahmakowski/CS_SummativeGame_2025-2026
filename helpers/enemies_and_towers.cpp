#ifndef ENEMIES_AND_TOWERS_CPP
#define ENEMIES_AND_TOWERS_CPP

#include "structs.cpp"
#include "rewritten_allegro_crap.cpp"

#define define_tower_template(tower_template_name, image_path, l, rate, r, d, cubes) { \
    tower_template_name.object.image = load_image(image_path); \
    tower_template_name.object.scale = {1.0f, 1.0f}; \
    tower_template_name.object.velocity = {0, 0}; \
    tower_template_name.object.position = {0, 0}; \
    tower_template_name.object.exists = false; \
    \
    tower_template_name.level = l; \
    \
    tower_template_name.fire_rate = rate; \
    tower_template_name.range = r; \
    tower_template_name.damage = d; \
    \
    tower_template_name.time_since_last_shot = 0.0f; \
    \
    tower_template_name.ice_cube_price = cubes; \
    tower_template_name.possible_upgrades_count = 0; \
    \
    if (!tower_template_name.object.image) { \
        printf("Failed to load tower image: %s\n", image_path); \
        exit(1); \
    } \
}

Tower snowman_tower_template;

void initialize_enemies_and_towers() {
    define_tower_template(snowman_tower_template, "assets/sprites/towers/snowman_tower.png", 1, 1.0f, 150.0f, 10, 0);
}

#endif
