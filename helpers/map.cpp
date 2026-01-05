#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <math.h>

#include "headers/helpers.hpp"

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
    load_image_with_checks("tiles/castleOpening.png", enemy_goal_tile);

    load_image_with_checks("tiles/castleWall.png", wall_tile);
    load_image_with_checks("tiles/castleCornerL1.png", cornerl1_wall_tile);
    load_image_with_checks("tiles/castleCornerL2.png", cornerl2_wall_tile);
    load_image_with_checks("tiles/castleCornerR1.png", cornerr1_wall_tile);
    load_image_with_checks("tiles/castleCornerR2.png", cornerr2_wall_tile);
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
                if (tile.variation == 0) {
                    draw(tower_spot_tile, tile_position, scale, 0);
                } else {
                    draw(grass_tile_0, tile_position, scale, 0);
                }
                break;
            // Draws the enemy spawn tile
            case ENEMY_SPAWN:
                draw(enemy_spawn_tile, tile_position, scale, 0);
                break;
            // Draws the enemy goal tile
            case ENEMY_GOAL:
                draw(enemy_goal_tile, tile_position, scale, 0);
                break;
            case WALL:
                draw(wall_tile, tile_position, scale, 0);
                break;
            case CORNERL1_WALL:
                draw(cornerl1_wall_tile, tile_position, scale, 0);
                break;
            case CORNERL2_WALL:
                draw(cornerl2_wall_tile, tile_position, scale, 0);
                break;
            case CORNERR1_WALL:
                draw(cornerr1_wall_tile, tile_position, scale, 0);
                break;
            case CORNERR2_WALL:
                draw(cornerr2_wall_tile, tile_position, scale, 0);
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

    // Load cards
    current_hand_count = 0;
    for (int i= 0; i < 5; i++){
        draw_card();
    }
    
    return map;
}
