#include "prototypes.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <stdio.h>

// Defines

#define add_enemy(enemy) { \
    active_enemies[active_enemies_count] = enemy; \
    active_enemies_count++; \
}

#define add_tower(tower) { \
    tower.index = active_towers_count; \
    active_towers[active_towers_count] = tower; \
    active_towers_count++; \
}

#define add_projectile(projectile) { \
    active_projectiles[active_projectiles_count] = projectile; \
    active_projectiles_count++; \
}

#define update() al_flip_display()
#define sleep(seconds) al_rest(seconds)
#define fill_screen(color) al_clear_to_color(color)
#define load_image(path) al_load_bitmap(path)

// Macro to load image with error checking, it checks that the image is actually there and loaded properly
#define load_image_with_checks(path, ptr) { \
    ALLEGRO_BITMAP *temp = load_image(path); \
    if (!temp) { \
        printf("Failed to load image: %s\n", path); \
        return 1; \
    } \
    ptr = temp; \
}

// Macro to load font with error checking, it checks that the font is actually there and loaded properly
#define load_font_with_checks(path, size, flags, obj) { \
    ALLEGRO_FONT *temp_font = al_load_ttf_font(path, size, flags); \
    if (!temp_font) { \
        printf("Failed to load font: %s\n", path); \
        return false; \
    } \
    Font result = {temp_font, size}; \
    obj = result; \
}

#define get_display_height() al_get_display_height(display)
#define get_display_width() al_get_display_width(display)

// abs macro, checks if it's greater than 0, if not, makes it positive
#define abs(num) ((num < 0) ? -num : num)

// max macro, checks if a is greater than b, returns a if true, b if false
#define max(a, b) ((a > b) ? a : b)

// min macro, checks if a is less than b, returns a if true, b if false
#define min(a, b) ((a < b) ? a : b)

#define check_for_exit(ev) { \
    if ((ev.type == ALLEGRO_EVENT_KEY_DOWN && pressing_keybind(kill_keybind, ev)) || ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { \
        printf("Exiting due to kill keybind or window close\n"); \
        al_destroy_display(display); \
        return 0; \
    } \
}
