#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <math.h>

#include "headers/helpers.hpp"

// Converts tile position to pixel position (center of tile)
Vector2i tile_pos_to_pixel_pos(Vector2i tile_pos) {
    Vector2i pixel_pos;
    pixel_pos.x = tile_pos.x * TILE_SIZE + TILE_SIZE / 2;
    pixel_pos.y = tile_pos.y * TILE_SIZE + TILE_SIZE / 2;
    return pixel_pos;
}

// Applies an upgrade to a tower and removes it from the possible upgrades
void apply_upgrade(Tower &tower, int upgrade_index) {
    tower.damage = (int)(tower.damage * tower.possible_upgrades[upgrade_index].damage_multiplier);
    tower.reload_time = (int)(tower.reload_time * tower.possible_upgrades[upgrade_index].fire_rate_multiplier);
    tower.range = (int)(tower.range * tower.possible_upgrades[upgrade_index].range_multiplier);

    remove_object_from_array(tower.possible_upgrades, tower.possible_upgrades_count, upgrade_index);
}

// Draws the range circle of a tower
void draw_range_circle(Tower tower) {
    Vector2i tower_pos = camera_fixed_position(tower.object.position);
    draw_circle_outline(tower_pos, tower.range, BLUE, 2.0f);
}

// Sorts enemies by their path distance (furthest along the path first)
void enemies_by_path_distance(Enemy *sorted_enemies[]) {
    for (int i = 0; i < active_enemies_count; i++) {
        sorted_enemies[i] = &active_enemies[i];
    }

    for (int i = 0; i < active_enemies_count - 1; i++) {
        for (int j = 0; j < active_enemies_count - i - 1; j++) {
            if (sorted_enemies[j]->path_index < sorted_enemies[j + 1]->path_index) {
                Enemy* temp = sorted_enemies[j];
                sorted_enemies[j] = sorted_enemies[j + 1];
                sorted_enemies[j + 1] = temp;
            }
        }
    }
}

// Gets the enemies that towers can shoot at
void current_shots() {
    Enemy *sorted_enemies[100];

    enemies_by_path_distance(sorted_enemies);

    for (int i = 0; i < active_towers_count; i++) {
        active_towers[i].time_since_last_shot += 1.0f / FPS;
        active_towers[i].aimed_this_frame = false;
        for (int j = 0; j < active_enemies_count; j++) {
            if (distance_between(active_towers[i].object.position, sorted_enemies[j]->object.position) <= active_towers[i].range) {
                if (!active_towers[i].aimed_this_frame && sorted_enemies[j]->expected_damage < sorted_enemies[j]->health) {
                    active_towers[i].aimed_this_frame = true;
                    active_towers[i].object.rotation_degrees = (atan2(sorted_enemies[j]->object.position.y - active_towers[i].object.position.y, sorted_enemies[j]->object.position.x - active_towers[i].object.position.x) * (180.0 / ALLEGRO_PI)) + 90;
                }

                if (active_towers[i].time_since_last_shot >= active_towers[i].reload_time && sorted_enemies[j]->expected_damage < sorted_enemies[j]->health) {
                    shoot_projectile(active_towers[i], sorted_enemies[j]);
                    active_towers[i].time_since_last_shot = 0.0f;
                    break;
                }
            }
        }
    }
}

// Shoots a projectile from a tower towards a target enemy
void shoot_projectile(Tower tower, Enemy* target_enemy) {
    Projectile new_projectile;
    new_projectile.object.image = load_image(tower.projectile_image_path);
    new_projectile.object.scale = tower.projectile_scale;
    new_projectile.object.position = tower.object.position;
    new_projectile.object.exists = true;
    new_projectile.speed = PROJECTILE_SPEED;
    new_projectile.damage = tower.damage;

    new_projectile.target = target_enemy;
    target_enemy->expected_damage += new_projectile.damage;

    Vector2 direction = get_direction_to(tower.object.position, target_enemy->object.position);
    Vector2i velocity = vector2_to_vector2i(multiply_vector(direction, new_projectile.speed));
    new_projectile.object.velocity = velocity;

    add_projectile(new_projectile);
}

// Recalculates all projectiles' velocities towards their targets
void recalculate_projectiles() {
    for (int i = 0; i < active_projectiles_count; i++) {
        Projectile* proj = &active_projectiles[i];
        if (proj == nullptr) {
            printf("Projectile pointer is null, skipping\n");
            continue;
        }

        Vector2 direction = get_direction_to(proj->object.position, proj->target->object.position);
        Vector2i velocity = vector2_to_vector2i(multiply_vector(direction, proj->speed));
        proj->object.velocity = velocity;
        proj->object.rotation_degrees = (atan2(direction.y, direction.x) * (180.0 / ALLEGRO_PI)) + 90;
        proj->object.rotation_degrees = fmod(proj->object.rotation_degrees, 360.0f);
        update_position(proj->object);
    }
}

// Draws all active projectiles
void draw_all_projectiles() {
    for (int i = 0; i < active_projectiles_count; i++) {
        if (active_projectiles[i].object.exists) {
            draw(active_projectiles[i]);
        }
    }
}

// Draws all active towers
void draw_all_towers() {
    for (int i = 0; i < active_towers_count; i++) {
        if (active_towers[i].object.exists) {
            draw(active_towers[i]);
            if (draw_range_circles) {
                draw_range_circle(active_towers[i]);
            }
        }
    }
}

// Draws all active enemies
void draw_all_enemies() {
    for (int i = 0; i < active_enemies_count; i++) {
        if (active_enemies[i].object.exists) {
            draw(active_enemies[i]);
        }
    }
}

// Check if a projectile hit its target
void check_projectiles() {
    for (int i = 0; i < active_projectiles_count; i++) {
        Projectile* proj = &active_projectiles[i];
        if (proj == nullptr || proj->target == nullptr) {
            continue;
        }
        
        if (is_colliding(proj->object, proj->target->object) || !proj->target->object.exists) {
            proj->target->health -= proj->damage;
            proj->target->expected_damage -= proj->damage;
            proj->object.exists = false;
            for (int j = i; j < active_projectiles_count; j++) {
                active_projectiles[j] = active_projectiles[j + 1]; 
            }
            active_projectiles_count--;
            i--;
        }
    }
}

// Function that recaclulates enemies and spawns new ones based on the spawn rate
void run_enemies() {
    // Enemy spawning
    
    // If the current wave is not complete, spawn enemies
    if (!active_map.waves[active_map.current_wave_index].wave_complete || active_map.current_wave_index == -1) {
        bool wave_done = true;

        // For each "sub-wave" in the current wave
        for (int j = 0; j < active_map.waves[active_map.current_wave_index].sub_wave_count; j++) {
            SubWaves *sub_wave = &active_map.waves[active_map.current_wave_index].sub_waves[j];

            sub_wave->time_since_last_spawn += 1.0f / FPS;

            // Handle delay before spawning starts
            if (sub_wave->time_since_last_spawn <= sub_wave->delay && !sub_wave->delay_passed) {
                wave_done = false;
                continue;
            } else if (sub_wave->time_since_last_spawn >= sub_wave->delay && !sub_wave->delay_passed) {
                sub_wave->delay_passed = true;
                sub_wave->time_since_last_spawn = sub_wave->interval + 5;
            }

            // Spawn a new enemy
            if (sub_wave->time_since_last_spawn >= sub_wave->interval && sub_wave->count_spawned < sub_wave->count) {
                sub_wave->time_since_last_spawn = 0;

                Enemy enemy;

                new_enemy(enemy, sub_wave->type);

                enemy.object.position = tile_pos_to_pixel_pos(active_map.path[0]);
                enemy.path_index = 1;
                enemy.index = active_enemies_count;

                add_enemy(enemy);

                sub_wave->count_spawned += 1;
            }

            // If there are still enemies to spawn, the wave is not done
            if (sub_wave->count_spawned < sub_wave->count) {
                wave_done = false;
            }
        }

        active_map.waves[active_map.current_wave_index].wave_complete = wave_done;
    }

    // Update enemies
    
    // For every enemy
    int enemies_killed = 0;
    for (int i = 0; i < active_enemies_count; i++) {
        Enemy* enemy = &active_enemies[i];
        if (enemy == nullptr) {
            continue;
        }
        
        // Move enemy along path
        Vector2i direction = subtract_vector(active_map.path[enemy->path_index], 
                                active_map.path[enemy->path_index - 1]);

        if (direction.x == 0) {
            enemy->object.position.x = tile_pos_to_pixel_pos(active_map.path[enemy->path_index - 1]).x;
        } else if (direction.y == 0) {
            enemy->object.position.y = tile_pos_to_pixel_pos(active_map.path[enemy->path_index - 1]).y;
        }
        
        Vector2i velocity = multiply_vector(direction, enemy->speed);

        enemy->object.velocity = velocity;

        update_position(enemy->object);
        
        // Update path index if reached next tile
        if (direction.x != 0) {
            if (enemy->object.position.x >= tile_pos_to_pixel_pos(active_map.path[enemy->path_index]).x && direction.x > 0) {
                enemy->path_index++;
            } else if (enemy->object.position.x <= tile_pos_to_pixel_pos(active_map.path[enemy->path_index]).x && direction.x < 0) {
                enemy->path_index++;
            }
        } else if (direction.y != 0) {
            if (enemy->object.position.y >= tile_pos_to_pixel_pos(active_map.path[enemy->path_index]).y && direction.y > 0) {
                enemy->path_index++;
            } else if (enemy->object.position.y <= tile_pos_to_pixel_pos(active_map.path[enemy->path_index]).y && direction.y < 0) {
                enemy->path_index++;
            }
        }
        
        // damages or rewards the player based on if the enemy reached the endgoal
        if (enemy->path_index >= active_map.path_count) {
            player_health -= enemy->reward;
        } else if (enemy->health <= 0) {
            player_coins += enemy->reward;
        }
        
        // Remove enemy if it reached the end or died
        if (enemy->path_index >= active_map.path_count || enemy->health <= 0) {
            // Doing some pointer crap
            for (int j = 0; j < active_projectiles_count; j++) {
                if (active_projectiles[j].target->index > enemy->index) {
                    active_projectiles[j].target--;
                }
            }

            enemy->object.exists = false;
            for (int j = i; j < active_enemies_count; j++) {
                active_enemies[j] = active_enemies[j + 1]; 
                active_enemies[j].index = j;
            }
            active_enemies_count--;
            i--;
        }

        // Rotate enemy to face movement direction
        enemy->object.rotation_degrees = (atan2(direction.y, direction.x) * (180.0 / ALLEGRO_PI)) + 90;

        //printf("Enemy %d at position (%d, %d) with health %d (max health %d) and expected damage of %d\n", i, enemy->object.position.x, enemy->object.position.y, enemy->health, enemy->max_health, enemy->expected_damage);
    }
}

// Function to draw player stats
void do_ui() {
    // Health
    Panel health_panel;
    health_panel.top_left = {0, 0};
    health_panel.bottom_right = {300, 80};
    health_panel.color = GREEN;

    snprintf(health_panel.text, sizeof(health_panel.text), "Health: %d", player_health);
    draw(health_panel);

    // Coins
    Panel coin_panel;
    coin_panel.top_left = {get_display_width() - 300, 0};
    coin_panel.bottom_right = {get_display_width(), 80};
    coin_panel.color = GREEN;

    snprintf(coin_panel.text, sizeof(coin_panel.text), "Coins: %d", player_coins);
    draw(coin_panel);

    Panel discard_button;
    discard_button.top_left = {0, get_display_height() / 2 + 50};
    discard_button.bottom_right = {300, get_display_height() / 2 + 130};
    discard_button.color = RED;
    discard_button.text_color = WHITE;
    discard_button.exists = true;
    snprintf(discard_button.text, sizeof(discard_button.text), "DISCARD HAND - 5 coins");
    draw(discard_button);

    buttons[ButtonIndex::DISCARD_BUTTON] = discard_button;

    // Next Wave Button
    if ((active_map.waves[active_map.current_wave_index].wave_complete || active_map.current_wave_index == -1) && active_map.current_wave_index < active_map.wave_count - 1) {
        Panel next_wave_button;
        next_wave_button.top_left = {0, get_display_height() / 2 - 40};
        next_wave_button.bottom_right = {300, get_display_height() / 2 + 40};
        next_wave_button.color = BLUE;
        next_wave_button.text_color = WHITE;
        next_wave_button.exists = true;

        snprintf(next_wave_button.text, sizeof(next_wave_button.text), "Start Next Wave");
        draw(next_wave_button);

        buttons[ButtonIndex::START_WAVE_BUTTON] = next_wave_button;
    }
}

// Check all the buttons and perform their actions
void handle_button_clicks(ALLEGRO_EVENT ev) {
    if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1) {
        // For each button in the enum
        for (int i = 0; i < (int)ButtonIndex::END; i++) {
            ButtonIndex button_index = (ButtonIndex)i;
            Panel* button = &buttons[i];

            // Check if it's being clicked and it exists
            if (button != nullptr && button->exists) {
                if (!currently_clicking(*button)) {
                    continue;
                }
                
                // Perform action based on button index
                switch (button_index) {
                    case START_WAVE_BUTTON:
                        active_map.current_wave_index++;
                        buttons[i].exists = false;
                        break;
                    case DISCARD_BUTTON:
                        if (player_coins < 5) {
                            printf("Not enough coins to discard hand!\n");
                            break;
                        }
                        current_hand_count = 0;
                        for (int j = 0; j < 5; j++) {
                            draw_card();
                        }
                        player_coins -= 5;
                        break;
                    default:
                        printf("Unknown button index %d\n", i);
                        break;
                }
            }
        }
    }
}

// Function to build a tower on mouse click
void build_tower_on_click(ALLEGRO_EVENT ev) {
    if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1) {
        if (!to_place.object.exists) {
            printf("No tower selected to place\n");
            return;
        }

        Vector2i mouse_tile_pos = get_mouse_pos();
        mouse_tile_pos.x /= TILE_SIZE;
        mouse_tile_pos.y /= TILE_SIZE;

        // Find which tile the player clicked on
        for (int i = 0; i < active_map.tower_spots_count; i++) {
            if (active_map.tower_spots[i].position.x == mouse_tile_pos.x && active_map.tower_spots[i].position.y == mouse_tile_pos.y && !active_map.tower_spots[i].occupied) {
                // Build a tower there
                Tower tower;
                new_tower(tower, to_place.type);
                to_place.object.exists = false;
                
                tower.object.position = tile_pos_to_pixel_pos(active_map.tower_spots[i].position);
                active_map.tower_spots[i].occupied = true;
                
                // Set the tile to grass to get rid of the "for sale" sign
                for (int j = 0; j < active_map.tile_count; j++) {
                    if (active_map.tiles[j].position.x == mouse_tile_pos.x && active_map.tiles[j].position.y == mouse_tile_pos.y) {
                        active_map.tiles[j].variation = 1;
                        break;
                    }
                }

                add_tower(tower);
                break;
            }
        }
    }
}
/*
void tower_menu(ALLEGRO) {
    for (int i = 0; i < active_map.tower_spots_count; i++) {
        if (active_map.tower_spots[i].position.x == mouse_tile_pos.x && active_map.tower_spots[i].position.y == mouse_tile_pos.y && !active_map.tower_spots[i].occupied) {
            
        }
    }
}
*/