#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "headers/helpers.hpp"

struct MainMenuButton {
    Panel panel;
    PossibleMap *map;
};

MainMenuButton main_menu_buttons[100];

// Function that loads a list of maps from the maps directory
bool load_map_list() {
    DIR *dir;
    struct dirent *in_file;

    // Open the maps directory
    dir = opendir(MAP_DIRECTORY);

    // Make sure it opened correctly
    if (dir == NULL) {
        printf("Could not open maps directory: %s\n", MAP_DIRECTORY);
        return false;
    }

    // For file in the maps directory
    while ((in_file = readdir(dir))) {
        char file_path[200], level_name[200];
        // Get the full file path
        snprintf(file_path, sizeof(file_path), "%s%s", MAP_DIRECTORY, in_file->d_name);

        // If it's not a .map file, skip it
        if (strstr(in_file->d_name, ".map") == NULL) {
            continue;
        }

        // Load the map file to get its name and level number
        FILE *file = fopen(file_path, "r");
        if (!file) {
            printf("Could not open map file: %s\n", file_path);
            return false;
        }

        fgets(possible_maps[possible_maps_count].name, sizeof(possible_maps[possible_maps_count].name) - 1, file);
        fscanf(file, "%d", &possible_maps[possible_maps_count].map_num);

        remove_newline_from_string(possible_maps[possible_maps_count].name);
        snprintf(possible_maps[possible_maps_count].file_path, sizeof(possible_maps[possible_maps_count].file_path), "%s", file_path);
        possible_maps_count++;

        fclose(file);
    }

    closedir(dir);
    
    // sorts the maps by level number
    for (int i = 0; i < possible_maps_count - 1; i++) {
        for (int j = 0; j < possible_maps_count - i - 1; j++) {
            if (possible_maps[j].map_num > possible_maps[j + 1].map_num) {
                PossibleMap temp = possible_maps[j];
                possible_maps[j] = possible_maps[j + 1];
                possible_maps[j + 1] = temp;
            }
        }
    }

    // debug stuff to indicate loading a map
    for (int i = 0; i < possible_maps_count; i++) {
        printf("Loaded map: %s (%s)\n", possible_maps[i].name, possible_maps[i].file_path);
    }

    return true;
}

// Draws main menu
void draw_main_menu() {
    fill_screen(LIGHT_GRAY);

    // Draws the title p
    Panel title_panel;
    title_panel.top_left = {get_display_width() / 2 - 300, 100};
    title_panel.bottom_right = {get_display_width() / 2 + 300, 200};
    title_panel.color = GREEN;
    snprintf(title_panel.text, sizeof(title_panel.text), "Tower Defense Game");
    title_panel.has_border = true;
    title_panel.border_color = BLACK;
    title_panel.font = &default_font;
    draw(title_panel);

    // Draws the background for the map list
    Panel map_list_panel;
    map_list_panel.top_left = {get_display_width() / 2 - 300, 250};
    map_list_panel.bottom_right = {get_display_width() / 2 + 300, 250 + possible_maps_count * 70 + 10};
    map_list_panel.color = WHITE;
    map_list_panel.has_border = true;
    map_list_panel.border_color = BLACK;
    draw(map_list_panel);

    // Draws each maps' button
    for (int i = 0; i < possible_maps_count; i++) {
        Panel map_panel;
        map_panel.top_left = {map_list_panel.top_left.x + 10, map_list_panel.top_left.y + 10 + i * 70};
        map_panel.bottom_right = {map_list_panel.bottom_right.x - 10, map_list_panel.top_left.y + 70 + i * 70};
        map_panel.color = LIGHT_GRAY;
        snprintf(map_panel.text, sizeof(map_panel.text), "%s", possible_maps[i].name);
        map_panel.has_border = true;
        map_panel.border_color = BLACK;
        map_panel.font = &default_font;

        main_menu_buttons[i].panel = map_panel;
        main_menu_buttons[i].map = &possible_maps[i];

        draw(main_menu_buttons[i].panel);
    }
}

// Handles main menu button clicks
void do_main_menu_buttons() {
    for (int i = 0; i < possible_maps_count; i++) {
        if (currently_clicking(main_menu_buttons[i].panel)) {
            active_map = load_map(main_menu_buttons[i].map->file_path);
            game_state = IN_GAME;
            return;
        }
    }
}
