#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <math.h>

#include "helpers.hpp"

// Applies an upgrade to a tower and removes it from the possible upgrades
void apply_upgrade(Tower &tower, int upgrade_index) {
    tower.damage = (int)(tower.damage * tower.possible_upgrades[upgrade_index].damage_multiplier);
    tower.fire_rate = (int)(tower.fire_rate * tower.possible_upgrades[upgrade_index].fire_rate_multiplier);
    tower.range = (int)(tower.range * tower.possible_upgrades[upgrade_index].range_multiplier);

    remove_object_from_array(tower.possible_upgrades, tower.possible_upgrades_count, upgrade_index);
}

// Draws the range circle of a tower
void draw_range_circle(Tower tower) {
    Vector2i tower_pos = camera_fixed_position(tower.object.position);
    draw_circle_outline(tower_pos, tower.range * camera.zoom, BLUE, 2.0f);
}

// Gets the enemies that towers can shoot at
void current_shots() {
    for (int i = 0; i < active_towers_count; i++) {
        active_towers[i]->time_since_last_shot += 1.0f / FPS;
        for (int j = 0; j < active_enemies_count; j++) {
            if (distance_between(active_towers[i]->object.position, active_enemies[j]->object.position) <= active_towers[i]->range) {
                if (active_towers[i]->time_since_last_shot >= active_towers[i]->fire_rate) {
                    shoot_projectile(*active_towers[i], active_enemies[j]);
                    active_towers[i]->time_since_last_shot = 0.0f;
                }
                break;
            }
        }
    }
}

// Shoots a projectile from a tower towards a target enemy
void shoot_projectile(Tower tower, Enemy* target_enemy) {
    Projectile* new_projectile = new Projectile();
    new_projectile->object.image = load_image("images/sun.png");
    new_projectile->object.scale = {0.2f, 0.2f};
    new_projectile->object.position = tower.object.position;
    new_projectile->object.exists = true;

    new_projectile->speed = PROJECTILE_SPEED;
    new_projectile->damage = tower.damage;

    new_projectile->target = target_enemy;

    Vector2 direction = get_direction_to(tower.object.position, target_enemy->object.position);
    Vector2i velocity = vector2_to_vector2i(multiply_vector(direction, new_projectile->speed));
    new_projectile->object.velocity = velocity;

    add_projectile(new_projectile);
}

// Recalculates all projectiles' velocities towards their targets
void recalculate_projectiles() {
    for (int i = 0; i < active_projectiles_count; i++) {
        Projectile* proj = active_projectiles[i];
        if (proj == nullptr) {
            printf("Projectile pointer is null, skipping\n");
            continue;
        }

        Vector2 direction = get_direction_to(proj->object.position, proj->target->object.position);
        Vector2i velocity = vector2_to_vector2i(multiply_vector(direction, proj->speed));
        proj->object.velocity = velocity;
        update_position(proj->object);
    }
}

// Draws all active projectiles
void draw_all_projectiles() {
    for (int i = 0; i < active_projectiles_count; i++) {
        if (active_projectiles[i] != nullptr) {
            draw(*active_projectiles[i]);
        }
    }
}

// Draws all active towers
void draw_all_towers() {
    for (int i = 0; i < active_towers_count; i++) {
        if (active_towers[i] != nullptr) {
            draw(*active_towers[i]);
            if (draw_range_circles) {
                draw_range_circle(*active_towers[i]);
            }
        }
    }
}

// Draws all active enemies
void draw_all_enemies() {
    for (int i = 0; i < active_enemies_count; i++) {
        if (active_enemies[i] != nullptr) {
            draw(*active_enemies[i]);
        }
    }
}

// Check if a projectile hit its target
void check_projectiles() {
    for (int i = 0; i < active_projectiles_count; i++) {
        Projectile* proj = active_projectiles[i];
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
    load_image_with_checks("tiles/grass.png", grass_tile);
    load_image_with_checks("tiles/path.png", path_tile);
    load_image_with_checks("tiles/tower_spot.png", tower_spot_tile);
    load_image_with_checks("tiles/enemy_spawn.png", enemy_spawn_tile);
    load_image_with_checks("tiles/enemy_goal.png", enemy_goal_tile);
    return 0;
}

void display_map(Map map) {
    for (int i = 0; i < map.tile_count; i++) {
        MapTile tile = map.tiles[i];
        Vector2i tile_position = {tile.position.x * TILE_SIZE + TILE_SIZE / 2, tile.position.y * TILE_SIZE + TILE_SIZE / 2};
        Vector2 scale = {1.0f, 1.0f};
        switch (tile.type) {
            case GRASS:
                draw(grass_tile, tile_position, scale);
                break;
            case PATH:
                draw(path_tile, tile_position, scale);
                break;
            case TOWER_SPOT:
                draw(tower_spot_tile, tile_position, scale);
                break;
            case ENEMY_SPAWN:
                draw(enemy_spawn_tile, tile_position, scale);
                break;
            case ENEMY_GOAL:
                draw(enemy_goal_tile, tile_position, scale);
                break;
            default:
                draw(grass_tile, tile_position, scale);
                break;
        }
    }
}

// Function to print map details for debugging
void print_map(Map map) {
    printf("Map Name: %s\n", map.name);
    for (int i = 0; i < map.tile_count; i++) {
        printf("Tile %d: Type %d at Position (%d, %d)\n", i, map.tiles[i].type, map.tiles[i].position.x, map.tiles[i].position.y);
    }

    printf("\n\nPath Points:\n");
    for (int i = 0; i < map.paths_count; i++) {
        printf("Path Point %d: (%d, %d)\n", i, map.paths[i].x, map.paths[i].y);
    }

    printf("Spawn Rate: %d\n", map.spawn_rate);
}

bool is_in_array(Vector2i point, Vector2i arr[], int count) {
    for (int i = 0; i < count; i++) {
        if (arr[i].x == point.x && arr[i].y == point.y) {
            return true;
        }
    }
    return false;
}

// Adds path points to the map based on the tiles
void add_path_points_to_map(Map &map) {
    map.paths_count = 0;

    MapTile *spawn_point = nullptr;
    MapTile *goal_point = nullptr;

    for (int i = 0; i < map.tile_count; i++) {
        if (map.tiles[i].type == ENEMY_SPAWN) {
            spawn_point = &map.tiles[i];
        } else if (map.tiles[i].type == ENEMY_GOAL) {
            goal_point = &map.tiles[i];
        }
    }

    MapTile *current_tile = spawn_point;
    while (current_tile->position.x != goal_point->position.x || current_tile->position.y != goal_point->position.y) {
        map.paths[map.paths_count] = current_tile->position;
        map.paths_count++;

        bool found_next = false;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0 || i != 0 && j != 0) {
                    continue;
                }
                
                MapTile *neighbor_tile;
                if (index_of_in_array({current_tile->position.x + i, current_tile->position.y + j}, map.tiles, map.tile_count) != -1) {
                    if (index_of_in_array({current_tile->position.x + i, current_tile->position.y + j}, map.paths, map.paths_count) != -1) {
                        continue;
                    }
                    neighbor_tile = &map.tiles[index_of_in_array({current_tile->position.x + i, current_tile->position.y + j}, map.tiles, map.tile_count)];
                    if (neighbor_tile->type == PATH || neighbor_tile->type == ENEMY_GOAL) {
                        found_next = true;
                        current_tile = neighbor_tile;
                        break;
                    }
                }
            }

            if (found_next) {
                break;
            }
        }

        if (!found_next) {
            printf("Error: Could not find next path tile from (%d, %d)\n", current_tile->position.x, current_tile->position.y);
            exit(1);
        }
    }

    map.paths[map.paths_count] = current_tile->position;
    map.paths_count++;
}

// Function to load a map from a file
Map load_map(const char* file_path) {
    Map map;
    map.tile_count = 0;
    int width, height;

    FILE* file = fopen(file_path, "r");
    if (!file) {
        printf("Error: Could not open map file %s\n", file_path);
        return map;
    }

    fgets(map.name, sizeof(map.name)-1, file);
    fscanf(file, "%d %d", &width, &height);
    fscanf(file, "%d", &map.spawn_rate);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int tile_type;
            fscanf(file, "%d", &tile_type);
            MapTile tile;
            tile.type = (TileType)tile_type;
            tile.position = {x, y};
            map.tiles[map.tile_count] = tile;
            map.tile_count++;
        }
    }

    map.time_since_last_spawn = map.spawn_rate + 1.0f;

    add_path_points_to_map(map);

    fclose(file);
    return map;
}

// Function that recaclulates enemies and spawns new ones based on the spawn rate
void run_enemies() {
    // Enemy spawning
    active_map.time_since_last_spawn += 1.0f / FPS;
    if (active_map.time_since_last_spawn >= active_map.spawn_rate) {
        active_map.time_since_last_spawn = 0;

        Enemy* new_enemy = new Enemy();
        new_penguin(*new_enemy);

        new_enemy->object.scale = {0.25f, 0.25f};

        Vector2i spawn_position = multiply_vector(active_map.paths[0], TILE_SIZE);
        spawn_position.x += TILE_SIZE / 2;
        spawn_position.y += TILE_SIZE / 2;
        new_enemy->object.position = spawn_position;

        new_enemy->path_index = 1;

        add_enemy(*new_enemy);
    }

    // Update enemies
    for (int i = 0; i < active_enemies_count; i++) {
        Enemy* enemy = active_enemies[i];
        if (enemy == nullptr) {
            continue;
        }

        Vector2i direction = subtract_vector(active_map.paths[enemy->path_index], 
                                active_map.paths[enemy->path_index - 1]);

        if (direction.x == 0) {
            enemy->object.position.x = active_map.paths[enemy->path_index - 1].x * TILE_SIZE + TILE_SIZE / 2;
        } else if (direction.y == 0) {
            enemy->object.position.y = active_map.paths[enemy->path_index - 1].y * TILE_SIZE + TILE_SIZE / 2;
        }
        
        Vector2i velocity = multiply_vector(direction, enemy->speed);

        enemy->object.velocity = velocity;

        update_position(enemy->object);

        if (direction.x != 0) {
            if (enemy->object.position.x >= active_map.paths[enemy->path_index].x * TILE_SIZE + TILE_SIZE / 2 && direction.x > 0) {
                enemy->path_index++;
            } else if (enemy->object.position.x <= active_map.paths[enemy->path_index].x * TILE_SIZE + TILE_SIZE / 2 && direction.x < 0) {
                enemy->path_index++;
            }
        } else if (direction.y != 0) {
            if (enemy->object.position.y >= active_map.paths[enemy->path_index].y * TILE_SIZE + TILE_SIZE / 2 && direction.y > 0) {
                enemy->path_index++;
            } else if (enemy->object.position.y <= active_map.paths[enemy->path_index].y * TILE_SIZE + TILE_SIZE / 2 && direction.y < 0) {
                enemy->path_index++;
            }
        }
        
        if (enemy->path_index >= active_map.paths_count) {
            player_health -= enemy->reward;
        } else if (enemy->health <= 0) {
            player_coins += enemy->reward;
        }
        
        if (enemy->path_index >= active_map.paths_count || enemy->health <= 0) {
            enemy->object.exists = false;
            for (int j = i; j < active_enemies_count; j++) {
                active_enemies[j] = active_enemies[j + 1]; 
            }
            active_enemies_count--;
            i--;
        }
    }
}

//Drawing player stats
void draw_stats() {
    // Health
    Panel health_panel;
    health_panel.top_left = {0, 0};
    health_panel.bottom_right = {300, 80};
    health_panel.color = GREEN;
    health_panel.font = default_font;

    snprintf(health_panel.text, sizeof(health_panel.text), "Health: %d", player_health);

    draw(health_panel);

    // Coins
    Panel coin_panel;
    health_panel.top_left = {get_display_width() - 300, 0};
    health_panel.bottom_right = {get_display_width(), 80};
    health_panel.color = GREEN;
    health_panel.font = default_font;

    snprintf(health_panel.text, sizeof(health_panel.text), "Coins: %d", player_coins);

    draw(health_panel);
}