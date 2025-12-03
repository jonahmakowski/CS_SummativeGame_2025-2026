// to run:
// clang++ ./helpers/*.cpp main.cpp -o ./target/main $(pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 --libs --cflags); ./target/main

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "helpers/helpers.hpp"

// Run every frame
void frame_logic() {
    fill_screen(WHITE);

    run_enemies();

    update_camera_position();

    display_map(active_map);

    current_shots();

    recalculate_projectiles();

    draw_all_towers();
    draw_all_enemies();
    draw_all_projectiles();

    check_projectiles();
}

// Handling the keyboard input ev is the allegro event
void handle_keyboard_input_down(ALLEGRO_EVENT ev) {
    if (pressing_keybind(move_up, ev)) {
        camera.velocity.y = 10;
    }
    if (pressing_keybind(move_down, ev)) {
        camera.velocity.y = -10;
    }
    if (pressing_keybind(move_left, ev)) {
        camera.velocity.x = 10;
    }
    if (pressing_keybind(move_right, ev)) {
        camera.velocity.x = -10;
    }
}

// Run when a key is released
void handle_keyboard_input_up(ALLEGRO_EVENT ev) {
    if (pressing_keybind(move_up, ev) || pressing_keybind(move_down, ev)) {
        camera.velocity.y = 0;
    }
    if (pressing_keybind(move_left, ev) || pressing_keybind(move_right, ev)) {
        camera.velocity.x = 0;
    }
}

// Run when mouse input is detected
void handle_mouse_input(ALLEGRO_EVENT ev) {
    
}

// Run to setup the game before the main loop
// Does things like defining sprites for game objects
bool setup_game() {
    if (!init_allegro()) {
        return false;
    }

    if (load_tile_images() != 0) {
        return false;
    }
    
    return true;
}

int main(int argc, char *argv[]) {    
    srand(time(0));

    if (!setup_game()) {
        printf("Failed to setup game\n");
        return -1;
    }

    active_map = load_map("maps/level1.map");
    //print_map(active_map);

    al_start_timer(timer);
    al_get_mouse_cursor_position(&mouse_pos.x, &mouse_pos.y);
    while(true) {
        ALLEGRO_EVENT ev;

        al_wait_for_event(event_queue, &ev);

        // Handling Input
        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_pos = {ev.mouse.x, ev.mouse.y};
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            handle_mouse_input(ev);
        }

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            handle_keyboard_input_down(ev);
        }

        if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            handle_keyboard_input_up(ev);
        }

        // Run frame logic
        if(ev.type == ALLEGRO_EVENT_TIMER) {
            frame_logic();
            update();
        }

        check_for_exit(ev);
    }

    al_destroy_display(display);
    return 0;
}

