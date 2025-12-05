#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "helpers.hpp"

// Renaming drawing functions to use Vector2 and Vector2i and make more sense to me
void draw_rectangle(Vector2i top_left, Vector2i bottom_right, ALLEGRO_COLOR color) {
    al_draw_filled_rectangle(top_left.x, top_left.y, bottom_right.x, bottom_right.y, color);
}

void draw_rectangle_rounded(Vector2i top_left, Vector2i bottom_right, float radius, ALLEGRO_COLOR color) {
    al_draw_filled_rounded_rectangle(top_left.x, top_left.y, bottom_right.x, bottom_right.y, radius, radius, color);
}

void draw_rectangle_rounded_outline(Vector2i top_left, Vector2i bottom_right, float radius, ALLEGRO_COLOR color, float thickness) {
    al_draw_rounded_rectangle(top_left.x, top_left.y, bottom_right.x, bottom_right.y, radius, radius, color, thickness);
}

void draw_circle(Vector2i center, float radius, ALLEGRO_COLOR color) {
    al_draw_filled_circle(center.x, center.y, radius, color);
}

void draw_triangle(Vector2i point1, Vector2i point2, Vector2i point3, ALLEGRO_COLOR color) {
    al_draw_filled_triangle(point1.x, point1.y, point2.x, point2.y, point3.x, point3.y, color);
}

void draw_line(Vector2i start, Vector2i end, ALLEGRO_COLOR color, float thickness) {
    al_draw_line(start.x, start.y, end.x, end.y, color, thickness);
}

void draw_circle_outline(Vector2i center, float radius, ALLEGRO_COLOR color, float thickness) {
    al_draw_circle(center.x, center.y, radius, color, thickness);
}

// Made it so that this function draws the image centered at the position, instead of putting the top left corner there
void draw_image(ALLEGRO_BITMAP *image, Vector2i position) {
    int width = al_get_bitmap_width(image);
    int height = al_get_bitmap_height(image);
    Vector2i position_upper_left = {position.x - width / 2, position.y - height / 2};
    al_draw_bitmap(image, position_upper_left.x, position_upper_left.y, 0);
}

// Draws text centered at the position
void draw_text(Font font, ALLEGRO_COLOR color, Vector2i position, const char *text) {
    al_draw_text(font.font, color, position.x, position.y-font.size/2, ALLEGRO_ALIGN_CENTRE, text);
}

// Gets the window size as a Vector2i
Vector2i get_window_size() {
    Vector2i size;
    size.x = al_get_display_width(display);
    size.y = al_get_display_height(display);
    return size;
}

// Draws a scaled image centered at the position
void draw_scaled_image(ALLEGRO_BITMAP *image, Vector2i position, Vector2 scale, int rotation) {
    al_draw_scaled_rotated_bitmap(image, al_get_bitmap_width(image) / 2, al_get_bitmap_height(image) / 2, position.x, position.y, scale.x, scale.y, rotation * ALLEGRO_PI / 180, 0);
}

// Setup Allegro and its components
bool init_allegro() {
    if(!al_init() || !al_init_image_addon() || !al_init_primitives_addon() || !al_init_font_addon() || !al_init_ttf_addon()) {
        printf("failed to initalize libraries\n");
        return false;
    }

    if (FULLSCREEN) {
        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    }

    // Exit program if program fails to create display
    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if(!display) {
        printf("Display failed\n");
        return false;
    }

    timer = al_create_timer(1.0 / FPS);
   	if (!timer) {
   		printf("timer failed\n");
        return false;
   	}

   	event_queue = al_create_event_queue();
   	if (!event_queue) {
   		printf("queue failed\n");
        return false;
   	}

    // Init keyboard
   	if (!al_install_keyboard()) {
        printf("keyboard failed\n");
        return false;
    }

    // Init mouse
    if (!al_install_mouse()) {
        printf("mouse failed\n");
        return false;
    }

    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));

    al_set_window_title(display, "Summative Game");

    load_font_with_checks("fonts/OpenSans-Font.ttf", 24, 0, default_font);

    return true;
}
