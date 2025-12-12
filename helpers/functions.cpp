#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <math.h>

#include "helpers.hpp"

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
        for (int j = 0; j < active_enemies_count; j++) {
            if (distance_between(active_towers[i].object.position, sorted_enemies[j]->object.position) <= active_towers[i].range) {
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
        if (proj == nullptr) {
            continue;
        }
        if (is_colliding(proj->object, proj->target->object) || !proj->target->object.exists) {
            proj->target->health -= proj->damage;
            proj->object.exists = false;
            for (int j = i; j < active_projectiles_count; j++) {
                active_projectiles[j] = active_projectiles[j + 1]; 
            }
            active_projectiles_count--;
            i--;
        }
    }
}

// Load the tile images
int load_tile_images() {
    load_image_with_checks("tiles/grass-0.png", grass_tile_0);
    load_image_with_checks("tiles/grass-1.png", grass_tile_1);
    load_image_with_checks("tiles/grass-2.png", grass_tile_2);

    load_image_with_checks("tiles/path-0.png", path_tile_0);
    load_image_with_checks("tiles/path-1.png", path_tile_1);
    load_image_with_checks("tiles/path-2.png", path_tile_2);
    load_image_with_checks("tiles/path-3.png", path_tile_3);
    load_image_with_checks("tiles/path-4.png", path_tile_4);
    load_image_with_checks("tiles/path-5.png", path_tile_5);
    load_image_with_checks("tiles/path-6.png", path_tile_6);
    load_image_with_checks("tiles/path-7.png", path_tile_7);
    load_image_with_checks("tiles/path-8.png", path_tile_8);
    load_image_with_checks("tiles/path-9.png", path_tile_9);

    load_image_with_checks("tiles/tower_spot.png", tower_spot_tile);
    load_image_with_checks("tiles/enemy_spawn.png", enemy_spawn_tile);
    load_image_with_checks("tiles/enemy_goal.png", enemy_goal_tile);
    return 0;
}

// Function to display the map
void display_map() {
    for (int i = 0; i < active_map.tile_count; i++) {
        MapTile tile = active_map.tiles[i];
        Vector2i tile_position = tile_pos_to_pixel_pos(tile.position);
        Vector2 scale = {1.0f, 1.0f};
        // decide
        switch (tile.type) {
            // Within the grass tile, draw the correct variation
            case GRASS:
                if (tile.variation == 0) {
                    draw(grass_tile_0, tile_position, scale, 0);
                } else if (tile.variation == 1) {
                    draw(grass_tile_1, tile_position, scale, 0);
                } else if (tile.variation == 2) {
                    draw(grass_tile_2, tile_position, scale, 0);
                }
                break;
            // decides the correct path tile
            case PATH:
                switch (tile.variation) {
                        case 0:
                            draw(path_tile_0, tile_position, scale, 0);
                            break;
                        case 1:
                            draw(path_tile_1, tile_position, scale, 0);
                            break;
                        case 2:
                            draw(path_tile_2, tile_position, scale, 0);
                            break;
                        case 3:
                            draw(path_tile_3, tile_position, scale, 0);
                            break;
                        case 4:
                            draw(path_tile_4, tile_position, scale, 0);
                            break;
                        case 5:
                            draw(path_tile_5, tile_position, scale, 0);
                            break;
                        case 6:
                            draw(path_tile_6, tile_position, scale, 0);
                            break;
                        case 7:
                            draw(path_tile_7, tile_position, scale, 0);
                            break;
                        case 8:
                            draw(path_tile_8, tile_position, scale, 0);
                            break;
                        case 9:
                            draw(path_tile_9, tile_position, scale, 0);
                            break;
                        default:
                            printf("Unknown path tile variation %d at position (%d, %d)\n", tile.variation, tile.position.x, tile.position.y);
                            draw(path_tile_0, tile_position, scale, 0);
                            break;
                    }
                break;
            // Draws the tower spot tile
            case TOWER_SPOT:
                draw(tower_spot_tile, tile_position, scale, 0);
                break;
            // Draws the enemy spawn tile
            case ENEMY_SPAWN:
                draw(enemy_spawn_tile, tile_position, scale, 0);
                break;
            // Draws the enemy goal tile
            case ENEMY_GOAL:
                draw(enemy_goal_tile, tile_position, scale, 0);
                break;
            // Incase of an unknown tile type, it defaults to a grass tile and prints an error
            default:
                printf("Unknown tile type %d at position (%d, %d)\n", tile.type, tile.position.x, tile.position.y);
                draw(grass_tile_0, tile_position, scale, 0);
                break;
        }
    }
}

// Function to print map details for debugging
void print_map() {
    printf("Map Name: %s\n", active_map.name);
    for (int i = 0; i < active_map.tile_count; i++) {
        printf("Tile %d: Type %d at Position (%d, %d)\n", i, active_map.tiles[i].type, active_map.tiles[i].position.x, active_map.tiles[i].position.y);
    }

    printf("\n\nPath Points:\n");
    for (int i = 0; i < active_map.path_count; i++) {
        printf("Path Point %d: (%d, %d)\n", i, active_map.path[i].x, active_map.path[i].y);
    }
}

// Adds path points to the map based on the tiles and their connections
void add_path_points_to_map(Map &map) {
    map.path_count = 0;

    MapTile *spawn_point = nullptr;
    MapTile *goal_point = nullptr;

    // Find the goal and spawn tiles
    for (int i = 0; i < map.tile_count; i++) {
        if (map.tiles[i].type == ENEMY_SPAWN) {
            spawn_point = &map.tiles[i];
        } else if (map.tiles[i].type == ENEMY_GOAL) {
            goal_point = &map.tiles[i];
        }
    }

    MapTile *current_tile = spawn_point;
    MapTile *previous_tile = nullptr;
    while (current_tile->position.x != goal_point->position.x || current_tile->position.y != goal_point->position.y) {
        map.path[map.path_count] = current_tile->position;
        map.path_count++;

        bool found_next = false;
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                if (x == 0 && y == 0 || x != 0 && y != 0) {
                    continue;
                }
                
                MapTile *next_tile;
                // If this position is in the array
                if (index_of_in_array({current_tile->position.x + x, current_tile->position.y + y}, map.tiles, map.tile_count) != -1) {
                    // If it's already in the array somehow, shouldn't ever happen
                    if (index_of_in_array({current_tile->position.x + x, current_tile->position.y + y}, map.path, map.path_count) != -1) {
                        continue;
                    }
                    
                    // Get the array at our neighbor position
                    next_tile = &map.tiles[index_of_in_array({current_tile->position.x + x, current_tile->position.y + y}, map.tiles, map.tile_count)];
                    // If it's a path tile or the goal, we found our next tile
                    if (next_tile->type == PATH || next_tile->type == ENEMY_GOAL) {
                        found_next = true;

                        /* diagram to help my (Aaron's) poor brain
                                 x
                           y(-1,-1) (0,-1) (1,-1)
                            (-1, 0) (0, 0) (1, 0)
                            (-1, 1) (0, 1) (1, 1)
                        */

                        // Path direction logic
                        if (previous_tile != nullptr) {
                            if(previous_tile->position.x == next_tile->position.x) {
                                current_tile->variation = rand() % 3 + 8;
                                if (current_tile->variation == 10) {
                                    current_tile->variation = 0;
                                }
                            } else if(previous_tile->position.y == next_tile->position.y) {
                                current_tile->variation = rand() % 3 + 6;
                                if (current_tile->variation == 8) {
                                    current_tile->variation = 1;
                                }
                            } else if ((previous_tile->position.x - current_tile->position.x == 1 || next_tile->position.x - current_tile->position.x == 1) && (previous_tile->position.y - current_tile->position.y == 1 || next_tile->position.y - current_tile->position.y == 1)) {
                                current_tile->variation = 2;
                            } else if ((previous_tile->position.x - current_tile->position.x == 1 || next_tile->position.x - current_tile->position.x == 1) && (previous_tile->position.y - current_tile->position.y == -1 || next_tile->position.y - current_tile->position.y == -1)) {
                                current_tile->variation = 5;
                            } else if ((previous_tile->position.x - current_tile->position.x == -1 || next_tile->position.x - current_tile->position.x == -1) && (previous_tile->position.y - current_tile->position.y == 1 || next_tile->position.y - current_tile->position.y == 1)) {
                                current_tile->variation = 3;
                            } else if ((previous_tile->position.x - current_tile->position.x == -1 || next_tile->position.x - current_tile->position.x == -1) && (previous_tile->position.y - current_tile->position.y == -1 || next_tile->position.y - current_tile->position.y == -1)) {
                                current_tile->variation = 4;
                            } 
                        }
                        
                        // progresses to the next path tile
                        previous_tile = current_tile;
                        current_tile = next_tile;
                        break;
                    }
                }
            }

            if (found_next) {
                break;
            }
        }

        // Error handling if no next tile is found
        if (!found_next) {
            printf("Error: Could not find next path tile from (%d, %d)\n", current_tile->position.x, current_tile->position.y);
            exit(1);
        }
    }

    map.path[map.path_count] = current_tile->position;
    map.path_count++;
}

// Function to load a map from a file
Map load_map(const char* file_path) {
    // Define variables
    Map map;
    map.tile_count = 0;
    int width, height;

    // Open the file
    FILE* file = fopen(file_path, "r");
    if (!file) {
        printf("Error: Could not open map file %s\n", file_path);
        return map;
    }

    // Read map name, number, and size
    fgets(map.name, sizeof(map.name)-1, file);
    fscanf(file, "%d", &map.map_num);
    fscanf(file, "%d %d", &width, &height);

    map.size = {width, height};

    // Read in all the tiles
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int tile_type;
            fscanf(file, "%d", &tile_type);
            MapTile tile;

            tile.variation = 0;
            tile.type = (TileType)tile_type;
            tile.position = {x, y};

            if (tile.type == GRASS) {
                tile.variation = rand() % 3;
            } else if (tile.type == TOWER_SPOT) {
                map.tower_spots[map.tower_spots_count].position = tile.position;
                map.tower_spots_count++;
            }

            map.tiles[map.tile_count] = tile;
            map.tile_count++;
        }
    }

    // Load wave data
    int last_wave_num = 0;
    int sub_wave_index = 0;

    char line[256];
    fgets(line, sizeof(line) - 1, file);

    map.wave_count = 1;

    while (fgets(line, sizeof(line) - 1, file)) {
        int wave_num, num_enemies, enemy_type;
        float interval, delay;
        sscanf(line, "%d %d %d %f %f", &wave_num, &num_enemies, &enemy_type, &interval, &delay);
        
        if (wave_num != last_wave_num) {
            map.wave_count++;
            map.waves[wave_num].sub_wave_count = 0;
            sub_wave_index = 0;
        }

        SubWaves *sub_wave = &map.waves[wave_num].sub_waves[sub_wave_index];
        sub_wave->count = num_enemies;
        sub_wave->type = (EnemyType)enemy_type;
        sub_wave->interval = interval;
        sub_wave->delay = delay;
        sub_wave_index++;
        last_wave_num = wave_num;
        map.waves[wave_num].sub_wave_count++;
    }

    // Manage paths in another function
    add_path_points_to_map(map);

    fclose(file);
    return map;
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
            enemy->object.exists = false;
            for (int j = i; j < active_enemies_count; j++) {
                active_enemies[j] = active_enemies[j + 1]; 
            }
            active_enemies_count--;
            i--;
        }

        // Rotate enemy to face movement direction
        enemy->object.rotation_degrees = (atan2(direction.y, direction.x) * (180.0 / ALLEGRO_PI)) + 90;
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

    // Next Wave Button
    if ((active_map.waves[active_map.current_wave_index].wave_complete || active_map.current_wave_index == -1) && active_map.current_wave_index < active_map.wave_count - 1) {
        Panel *next_wave_button = new Panel();
        next_wave_button->top_left = {0, get_display_height() / 2 - 40};
        next_wave_button->bottom_right = {300, get_display_height() / 2 + 40};
        next_wave_button->color = BLUE;

        snprintf(next_wave_button->text, sizeof(next_wave_button->text), "Start Next Wave");
        draw(*next_wave_button);

        buttons[ButtonIndex::START_WAVE_BUTTON] = next_wave_button;
    }
}

// Check all the buttons and perform their actions
void handle_button_clicks(ALLEGRO_EVENT ev) {
    if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1) {
        // For each button in the enum
        for (ButtonIndex i = (ButtonIndex)0; i < END; i = (ButtonIndex)(i + 1)) {
            Panel* button = buttons[i];
            // Check if it's being clicked and it exists
            if (button != nullptr && currently_clicking(*button)) {
                // Perform action based on button index
                switch (i) {
                    case START_WAVE_BUTTON:
                        active_map.current_wave_index++;
                        buttons[i] = nullptr;
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
        Vector2i mouse_tile_pos = get_mouse_pos();
        mouse_tile_pos.x /= TILE_SIZE;
        mouse_tile_pos.y /= TILE_SIZE;

        // Find which tile the player clicked on
        for (int i = 0; i < active_map.tower_spots_count; i++) {
            if (active_map.tower_spots[i].position.x == mouse_tile_pos.x && active_map.tower_spots[i].position.y == mouse_tile_pos.y && !active_map.tower_spots[i].occupied) {
                // Build a tower there
                Tower tower;
                new_tower(tower, SNOWBALL_THROWER);
                tower.object.position = tile_pos_to_pixel_pos(active_map.tower_spots[i].position);
                tower.object.scale = {0.5f, 0.5f};
                active_map.tower_spots[i].occupied = true;
                add_tower(tower);
                break;
            }
        }
    }
}
