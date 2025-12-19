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

#include "helpers/headers/helpers.hpp"

// Run every frame
void frame_logic() {
    fill_screen(WHITE);

    al_acknowledge_resize(display);

    if (game_state == IN_GAME) {
        update_camera_position(true, {0, 0}, {(active_map.size.x * TILE_SIZE - get_display_width()) * -1, (active_map.size.y * TILE_SIZE - get_display_height()) * -1});

        display_map();

        run_enemies();

        current_shots();

        recalculate_projectiles();

        draw_all_towers();
        draw_all_enemies();
        draw_all_projectiles();

        check_projectiles();

        do_ui();

        display_hand();
    } else if (game_state == MAIN_MENU) {
        draw_main_menu();
    }
}

// Handling the keyboard input ev is the allegro event
void handle_keyboard_input_down(ALLEGRO_EVENT ev) {
    if (game_state == IN_GAME) {
        if (pressing_keybind(move_up, ev)) {
            camera.velocity.y = 10;
            moved = true;
        }
        if (pressing_keybind(move_down, ev)) {
            camera.velocity.y = -10;
            moved = true;
        }
        if (pressing_keybind(move_left, ev)) {
            camera.velocity.x = 10;
            moved = true;
        }
        if (pressing_keybind(move_right, ev)) {
            camera.velocity.x = -10;
            moved = true;
        }
        if (pressing_keybind(range_circle_toggle, ev)) {
            draw_range_circles = !draw_range_circles;
        }
        if (pressing_keybind(next_wave, ev)) {
            active_map.current_wave_index++;
            buttons[ButtonIndex::START_WAVE_BUTTON].exists = false;
        }
        if (pressing_keybind(toggle_ui, ev)) {
            if (!show_card_menu) {
                show_ui = !show_ui;
            } else {
                show_card_menu = false;
                ui_force_hidden = false;
            }
        }
    }
}

// Run when a key is released
void handle_keyboard_input_up(ALLEGRO_EVENT ev) {
    if (game_state == IN_GAME) {
        if (pressing_keybind(move_up, ev) || pressing_keybind(move_down, ev)) {
            camera.velocity.y = 0;
        }
        if (pressing_keybind(move_left, ev) || pressing_keybind(move_right, ev)) {
            camera.velocity.x = 0;
        }
    }
}

// Run when mouse input is detected
void handle_mouse_input(ALLEGRO_EVENT ev) {
    if (game_state == IN_GAME) {
        tower_menu(ev);
        build_tower_on_click(ev);
        handle_button_clicks(ev);
        handle_buy_card(ev);
    } else if (game_state == MAIN_MENU) {
        do_main_menu_buttons();
    }
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

    if (!load_map_list()) {
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

